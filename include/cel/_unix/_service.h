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
#ifndef __OS_SYS_SERVICE_UNIX_H__
#define __OS_SYS_SERVICE_UNIX_H__

#include "cel/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _OsServiceEntry
{
    CelMainFunc on_start;
    CelVoidFunc on_stop;
}OsServiceEntry;

OsServiceEntry *os_service_entry_create(TCHAR *name, CelMainFunc on_start, 
                                        CelVoidFunc on_stop);

int _os_service_entry_dispatch(OsServiceEntry *sc_entry, 
                               int argc, char **argv);
#define os_service_entry_dispatch(sc_entry) \
    _os_service_entry_dispatch(sc_entry, argc, argv)

BOOL os_service_is_running(TCHAR *name);
BOOL os_service_stop(TCHAR *name);
BOOL os_service_reload(TCHAR *name);

#ifdef __cplusplus
}
#endif

#endif
