#include "cel/net/wmipclient.h"
#include "cel/eventloop.h"
#include "cel/sys/envinfo.h"
#include "cel/multithread.h"
#include "cel/crypto/crypto.h"

#define MAX_FD 10240

static CelEventLoop evt_loop;
//static CelSslContext *sslctx;

static CelWmipContext wmip_ctx;
static CelWmipClient *client;

#ifdef _CEL_UNIX
static CelSignals s_signals[] = 
{
    { SIGPIPE, "sigpipe", SIG_IGN }, 
    { 0, NULL, NULL }
};
static CelResourceLimits s_rlimits = { 0, MAX_FD, MAX_FD };
#endif

void wmipclient_close(CelWmipClient *client, CelAsyncResult *result)
{
    _tprintf(_T("result = %ld\r\n"), result->ret);
    cel_wmipclient_free(client);
}

static int wmipclient_working(void *data)
{
    cel_eventloop_run(&evt_loop);
    /*_tprintf(_T("Event loop thread %d exit.(%s)"), 
       cel_thread_getid(), cel_geterrstr(cel_geterrno()));*/
    cel_thread_exit(0);

    return 0;
}

int wmipclient_test(int argc, TCHAR *argv[])
{
    int i, td_num, work_num;
    void *ret_val;
    CelCpuMask mask;
    CelThread tds[CEL_THDNUM];

    cel_multithread_support();
    cel_cryptomutex_register(NULL, NULL);
    cel_ssllibrary_init();
#ifdef _CEL_UNIX
    cel_signals_init(s_signals);
    cel_resourcelimits_set(&s_rlimits);
#endif
    work_num = (td_num = (int)cel_getnprocs());
    if (work_num > CEL_THDNUM)
        work_num = work_num / 2;
    else if (work_num < 2)
        work_num = 2;
    work_num = 20;
    if (cel_eventloop_init(&evt_loop, td_num, 10240) == -1)
        return 1;
    for (i = 0; i < work_num; i++)
    {
        cel_setcpumask(&mask, i%td_num);
        if (cel_thread_create(&tds[i], NULL, &wmipclient_working, NULL) != 0
            || cel_thread_setaffinity(&tds[i], &mask) != 0)
        {
            _tprintf(_T("Work thread create failed.(%s)\r\n"), cel_geterrstr(cel_geterrno()));
            return 1;
        }
    }

    //if (((sslctx = cel_sslcontext_new(
    //    cel_sslcontext_method(_T("SSLv23")))) == NULL
    //    || cel_sslcontext_set_own_cert(sslctx, "/etc/sunrun/vas_server.crt", 
    //    "/etc/sunrun/vas_server.key", _T("38288446")) == -1
    //    || cel_sslcontext_set_ciphersuites(
    //    sslctx, _T("AES:ALL:!aNULL:!eNULL:+RC4:@STRENGTH")) == -1))
    //{
    //    printf("Ssl context init failed.(%s)\r\n", cel_geterrstr(cel_geterrno()));
    //    return -1;
    //}
    memset(&wmip_ctx, 0 ,sizeof(CelWmipContext));
    client = cel_wmipclient_new(NULL, &wmip_ctx);

    cel_wmipclient_set_nonblock(client, 1);
    cel_eventloop_add_channel(&evt_loop, cel_wmipclient_get_channel(client), NULL);

    cel_wmipclient_set_method(client,CEL_HTTPM_POST);
    cel_wmipclient_set_url(client, "https://192.168.1.230:8443/api/mn/session");
    cel_wmipclient_set_request_header(client,
        CEL_HTTPHDR_CONTENT_TYPE, 
        "application/json", strlen("application/json"));
    cel_wmipclient_request_printf(client, 
        "{\"username\":\"%s\",\"password\":\"%s\",\"action\":\"login\"}", 
        "hujinya", "password");
    cel_wmipclient_request_end(client);
    puts((char *)client->wmip_msg->req.s.buffer);
    cel_wmipclient_async_send_request(client, wmipclient_close);

    for (i = 0; i < work_num; i++)
    {
        //_tprintf(_T("i %d \r\n"), i);
        cel_thread_join(tds[i], &ret_val);
    }

    return 0;
}