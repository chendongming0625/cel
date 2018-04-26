#include "cel/crypto/jwt.h"

int jwt_test(int argc, TCHAR *argv[])
{
    CelJwt jwt;
    char token[256];
    int token_len = 256;

    cel_jwt_init(&jwt);
    cel_jwt_alg_set(&jwt, CEL_JWT_ALG_HS256);
    cel_jwt_payload_add_int(&jwt, "user_id", 116);
    cel_jwt_payload_add_string(&jwt, "user", "user1");
    cel_jwt_payload_add_string(&jwt, "domain", "domain1");
    cel_jwt_encode(&jwt, token, &token_len, "123456789", 9);
    puts(token);
    if (cel_jwt_decode(&jwt, token, (size_t)token_len, "123456789", 9) == 0)
        puts("OK");

    return 0;
}