/**
 * CEL(C Extension Library)
 * Copyright (C)2008 - 2018 Hu Jinya(hu_jinya@163.com) 
 *
 * This program is free software; you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License 
 * as published by the Free Software Foundation; either version 2 
 * of the License, or (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details.
 */
#ifndef __CEL_NET_TELNET_H__
#define __CEL_NET_TELNET_H__

#include "cel/types.h"
#include "cel/net/tcpclient.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _CEL_UNIX
#include <arpa/telnet.h>
#else
/*
 * Definitions for the TELNET protocol.
 */
#define IAC     255  /* interpret as command: */
#define DONT    254  /* you are not to use option */
#define DO      253  /* please, you use option */
#define WONT    252  /* I won't use option */
#define WILL    251  /* I will use option */
#define SB      250  /* interpret as subnegotiation */
#define GA      249  /* you may reverse the line */
#define EL      248  /* erase the current line */
#define EC      247  /* erase the current character */
#define AYT     246  /* are you there */
#define AO      245  /* abort output--but let prog finish */
#define IP      244  /* interrupt process--permanently */
#define BREAK   243  /* break */
#define DM      242  /* data mark--for connect. cleaning */
#define NOP     241  /* nop */
#define SE      240  /* end sub negotiation */
#define EOR     239  /* end of record (transparent mode) */
#define ABORT   238  /* Abort process */
#define SUSP    237  /* Suspend process */
#define xEOF    236  /* End of file: EOF is already used... */

#define SYNCH   242  /* for telfunc calls */


/* telnet options */
#define TELOPT_BINARY          0 /* 8-bit data path */
#define TELOPT_ECHO            1 /* echo */
#define TELOPT_RCP             2 /* prepare to reconnect */
#define TELOPT_SGA             3 /* suppress go ahead */
#define TELOPT_NAMS            4 /* approximate message size */
#define TELOPT_STATUS          5 /* give status */
#define TELOPT_TM              6 /* timing mark */
#define TELOPT_RCTE            7 /* remote controlled transmission and echo */
#define TELOPT_NAOL            8 /* negotiate about output line width */
#define TELOPT_NAOP            9 /* negotiate about output page size */
#define TELOPT_NAOCRD         10 /* negotiate about CR disposition */
#define TELOPT_NAOHTS         11 /* negotiate about horizontal tabstops */
#define TELOPT_NAOHTD         12 /* negotiate about horizontal tab disposition */
#define TELOPT_NAOFFD         13 /* negotiate about formfeed disposition */
#define TELOPT_NAOVTS         14 /* negotiate about vertical tab stops */
#define TELOPT_NAOVTD         15 /* negotiate about vertical tab disposition */
#define TELOPT_NAOLFD         16 /* negotiate about output LF disposition */
#define TELOPT_XASCII         17 /* extended ascic character set */
#define TELOPT_LOGOUT         18 /* force logout */
#define TELOPT_BM             19 /* byte macro */
#define TELOPT_DET            20 /* data entry terminal */
#define TELOPT_SUPDUP         21 /* supdup protocol */
#define TELOPT_SUPDUPOUTPUT   22 /* supdup output */
#define TELOPT_SNDLOC         23 /* send location */
#define TELOPT_TTYPE          24 /* terminal type */
#define TELOPT_EOR            25 /* end or record */
#define TELOPT_TUID           26 /* TACACS user identification */
#define TELOPT_OUTMRK         27 /* output marking */
#define TELOPT_TTYLOC         28 /* terminal location number */
#define TELOPT_3270REGIME     29 /* 3270 regime */
#define TELOPT_X3PAD          30 /* X.3 PAD */
#define TELOPT_NAWS           31 /* window size */
#define TELOPT_TSPEED         32 /* terminal speed */
#define TELOPT_LFLOW          33 /* remote flow control */
#define TELOPT_LINEMODE       34 /* Linemode option */
#define TELOPT_XDISPLOC       35 /* X Display Location */
#define TELOPT_OLD_ENVIRON    36 /* Old - Environment variables */
#define TELOPT_AUTHENTICATION 37 /* Authenticate */
#define TELOPT_ENCRYPT        38 /* Encryption option */
#define TELOPT_NEW_ENVIRON    39 /* New - Environment variables */
#define TELOPT_TN3270E        40 /* RFC2355 - TN3270 Enhancements */
#define TELOPT_CHARSET        42 /* RFC2066 - Charset */
#define TELOPT_COMPORT        44 /* RFC2217 - Com Port Control */
#define TELOPT_KERMIT         47 /* RFC2840 - Kermit */
#endif

typedef struct _CelTelnet
{
    CelTcpClient tcp_client;
}CelTelent;



#ifdef __cplusplus
}
#endif

#endif
