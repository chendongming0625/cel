#include "cel/net/httpmultipart.h"
#include "cel/log.h"
#include "cel/error.h"
#include "cel/allocator.h"
#include "cel/file.h"
#include "cel/net/http.h"

CelHttpMultipartEntity *cel_httpmultipart_entity_new()
{
    CelHttpMultipartEntity *entity;

    if ((entity = (CelHttpMultipartEntity *)cel_malloc(
        sizeof(CelHttpMultipartEntity))) != NULL)
    {
        entity->is_file = FALSE;
        cel_vstring_init_a(&(entity->param_name));
        cel_vstring_init_a(&(entity->file_name));
        cel_httpbodycache_init(&(entity->cache), CEL_HTTPBODY_BUF_LEN_MAX);
        return entity;
    }
    return NULL;
}

void cel_httpmultipart_entity_free(CelHttpMultipartEntity *entity)
{
    //puts("cel_httpmultipart_entity_free");
    cel_vstring_destroy_a(&(entity->param_name));
    cel_vstring_destroy_a(&(entity->file_name));
    cel_httpbodycache_destroy(&(entity->cache));
    cel_free(entity);
}

int cel_httpmultipart_init(CelHttpMultipart *multipart)
{
    cel_vstring_init_a(&(multipart->boundary));
    multipart->reading_state = CEL_HTTPMULTIPART_ENTITY_BOUNDARY;
    multipart->reading_entity = NULL;
    cel_list_init(&(multipart->entity_list), 
        (CelFreeFunc)cel_httpmultipart_entity_free);
    return 0;
}

void cel_httpmultipart_destroy(CelHttpMultipart *multipart)
{
    cel_vstring_destroy_a(&(multipart->boundary));
    multipart->reading_entity = NULL;
    cel_list_foreach(&(multipart->entity_list), 
        (CelEachFunc)cel_httpmultipart_entity_free, NULL);
}

CelHttpMultipart *cel_httpmultipart_new(void)
{
    CelHttpMultipart *multipart;

    if ((multipart = 
        (CelHttpMultipart *)cel_malloc(sizeof(CelHttpMultipart))) != NULL)
    {
        if (cel_httpmultipart_init(multipart) == 0)
            return multipart;
        cel_free(multipart);
    }

    return NULL;
}

void cel_httpmultipart_free(CelHttpMultipart *multipart)
{
    cel_httpmultipart_destroy(multipart);
    cel_free(multipart);
}

int cel_httpmultipart_reading_boundary(CelHttpMultipart *multipart,
                                       CelStream *s, size_t len)
{
    size_t offset = 0;
    BYTE ch = '\0', ch1;
    CelVStringA *boundary = &(multipart->boundary);

    if (len < cel_vstring_len(boundary) + 4)
    {
        //puts("cel_httpmultipart_reading_boundary error,length");
        return -1;
    }
    if (memcmp(cel_stream_get_pointer(s), 
        cel_vstring_str_a(boundary), cel_vstring_len(boundary)) != 0)
    {
        //puts(cel_vstring_str_a(boundary));
        //puts(cel_stream_get_pointer(s));
        //printf("cel_httpmultipart_reading_boundary error.\r\n");
        return -1;
    }
    cel_stream_seek(s, cel_vstring_len(boundary)); 
    cel_stream_read_u8(s, ch);
    ch1 = ch;
    cel_stream_read_u8(s, ch);
    offset += cel_vstring_len(boundary) + 4;
    if (ch1 == '-' && ch == '-')
    {
        //puts("HTTPMULTIPART end");
        cel_stream_seek(s, 2);
        multipart->reading_entity = NULL;
        return (int)offset;
    }
    if ((multipart->reading_entity = cel_httpmultipart_entity_new()) == NULL)
        return -1;
    cel_list_push_back(&(multipart->entity_list),
        &(multipart->reading_entity->item));
    multipart->reading_state = CEL_HTTPMULTIPART_ENTITY_HEADER;
    //puts("cel_httpmultipart_reading_boundary ok");

    return (int)offset;
}

int cel_httpmultipart_reading_header(CelHttpMultipart *multipart,
                                     CelStream *s, size_t len)
{
    BYTE *ch, ch1 = '\0', ch2 = '\0', ch3 = '\0';
    size_t _size = 0;

    while (_size < len)
    {
        ch = cel_stream_get_pointer(s) + _size;
        _size++;
        if (*ch == '\n' && ch1 == '\r'&& ch2 == '\n' && ch3 == '\r')
            break;
        ch3 = ch2;
        ch2 = ch1;
        ch1 = *ch;
    }
    cel_stream_seek(s, _size);
    multipart->reading_state = CEL_HTTPMULTIPART_ENTITY_VALUE;
    //puts("cel_httpmultipart_reading_header ok");

    return _size;
}

int cel_httpmultipart_reading_value(CelHttpMultipart *multipart,
                                    CelStream *s, size_t len)
{
    CelHttpMultipartEntity *reading_entity = multipart->reading_entity;
    BYTE *start = cel_stream_get_pointer(s);
    BYTE ch, ch1 = '\0';
    size_t _size = 0, size = 0;

    while (_size < len)
    {
        cel_stream_read_u8(s, ch);
        _size++;
        if (ch == '-' && ch1 == '-'
            && (size = cel_httpmultipart_reading_boundary(
            multipart, s, len - _size)) != -1)
        {
            //puts("cel_httpmultipart_reading_value ok");
            _size -= 2;
            if (cel_httpbodycache_reading(
                &(reading_entity->cache), (char *)start, _size) != _size)
                return -1;
            return (int)size + _size + 2;
        }
        ch1 = ch;
    }
    //printf("cel_httpmultipart_reading_value size %d\r\n", _size);
    if (cel_httpbodycache_reading(
        &(reading_entity->cache), (char *)start, _size) != (int)_size)
        return -1;
    return (int)_size;
}

int cel_httpmultipart_reading(CelHttpMultipart *multipart, 
                              CelStream *s, size_t len)
{
    int _offset;
    size_t offset = 0;

    while (len - offset > 0)
    {
        switch (multipart->reading_state)
        {
        case CEL_HTTPMULTIPART_ENTITY_BOUNDARY:
            cel_stream_seek(s, 2);
            if ((_offset = cel_httpmultipart_reading_boundary(
                multipart, s, len - offset)) == -1)
                return -1;
            offset += _offset;
            if (multipart->reading_entity == NULL)
                return offset;
            break;
        case CEL_HTTPMULTIPART_ENTITY_HEADER:
            if ((_offset = cel_httpmultipart_reading_header(
                multipart, s, len - offset)) == -1)
                return -1;
            offset += _offset;
            break;
        case CEL_HTTPMULTIPART_ENTITY_VALUE:
            if ((_offset = cel_httpmultipart_reading_value(
                multipart, s, len - offset)) == -1)
                return -1;
            offset += _offset;
            break;
        default:
            return -1;
        }
    }
    //puts(cel_vstring_str_a(&(multipart->boundary)));
    //printf("cel_httpmultipart_reading len %d\r\n", len);
    return offset;
}