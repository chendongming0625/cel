/**
 * CEL(C Extension Library)
 * Copyright (C)2008 - 2018 Hu Jinya(hu_jinya@163.com)
 */
#ifndef __CEL_NET_TCPLISTENER_H__
#define __CEL_NET_TCPLISTENER_H__

#include "cel/net/tcpclient.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _CelTcpListenerAsyncArgs CelTcpListenerAsyncArgs;
typedef struct _CelTcpListener CelTcpListener;

typedef void (*CelTcpAcceptCallbackFunc) (
    CelTcpListener *listener, CelTcpClient *new_client, 
    CelAsyncResult *async_result);

struct _CelTcpListenerAsyncArgs
{
    CelSocketAsyncAcceptArgs accept_args;
    char offset_data[ACCEPTEX_RECEIVEDATA_OFFSET];
    CelTcpClient *client;
    CelTcpAcceptCallbackFunc async_callback;
};

struct _CelTcpListener
{
    CelSocket sock;
    CelSockAddr addr;
    CelSslContext *ssl_ctx;
    CelTcpListenerAsyncArgs *async_args;
    void *user_data;
    CelRefCounted ref_counted;
};

int cel_tcplistener_init(CelTcpListener *listener, int family, 
                         CelSslContext *ssl_ctx);
int cel_tcplistener_init_addr(CelTcpListener *listener,
                              CelSockAddr *addr, CelSslContext *ssl_ctx);
/* String format - <ipv4|ipv6|unix>@<ipv4addr|[ipv6addr]>:<port> */
int cel_tcplistener_init_str(CelTcpListener *listener,
                             const TCHAR *str, CelSslContext *ssl_ctx);
int cel_tcplistener_init_socket(CelTcpListener *listener,
                                CelSocket *socket, CelSslContext *ssl_ctx);
void cel_tcplistener_destroy(CelTcpListener *listener);

CelTcpListener *cel_tcplistener_new(int family, CelSslContext *ssl_ctx);
CelTcpListener *cel_tcplistener_new_addr(CelSockAddr *addr, 
                                         CelSslContext *ssl_ctx);
/* String format - <ipv4|ipv6|unix>@<ipv4addr|[ipv6addr]>:<port> */
CelTcpListener *cel_tcplistener_new_str(const TCHAR *str, 
                                        CelSslContext *ssl_ctx);
CelTcpListener *cel_tcplistener_new_socket(CelSocket *socket, 
                                           CelSslContext *ssl_ctx);
void cel_tcplistener_free(CelTcpListener *listener);

#define cel_tcplistener_set_nonblock(listener, nonblock) \
    cel_socket_set_nonblock((CelSocket *)listener, nonblock)
#define cel_tcplistener_set_reuseaddr(listener, reuseaddr) \
    cel_socket_set_reuseaddr((CelSocket *)listener, reuseaddr)

#define cel_tcplistener_get_localaddr(listener) \
    (&(((CelTcpListener *)(listener))->addr))
#define cel_tcplistener_get_localaddrs(listener) \
    cel_sockaddr_ntop(cel_tcplistener_get_localaddr(listener))

#define cel_tcplistener_start(listener, addr) \
    cel_socket_listen(((CelSocket *)listener), addr, 1024)

int cel_tcplistener_accept(CelTcpListener *listener, CelTcpClient *client);

int cel_tcplistener_async_accept(CelTcpListener *listener, 
                                 CelTcpClient *new_client,
                                 CelTcpAcceptCallbackFunc async_callback);

#ifdef __cplusplus
}
#endif

#endif