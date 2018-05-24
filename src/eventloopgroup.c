/**
 * CEL(C Extension Library)
 * Copyright (C)2008 - 2016 Hu Jinya(hu_jinya@163.com) 
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
#include "cel/eventloopgroup.h"
#include "cel/allocator.h"
#include "cel/error.h"
#include "cel/sys/envinfo.h"
#include "cel/multithread.h"

CelEventLoopThreadId *_cel_eventloopthread_id()
{
    CelEventLoopThreadId *ptr;
    
    if ((ptr = (CelEventLoopThreadId *)
        cel_multithread_get_keyvalue(CEL_MT_KEY_EVENTLOOPTHREADID)) == NULL)
    {
        if ((ptr = (CelEventLoopThreadId *)
            _cel_sys_malloc(sizeof(CelEventLoopThreadId))) != NULL)
        {
            ptr->i = 0;
            if (cel_multithread_set_keyvalue(
                CEL_MT_KEY_EVENTLOOPTHREADID, ptr) != -1
                && cel_multithread_set_keydestructor(
                CEL_MT_KEY_EVENTLOOPTHREADID, _cel_sys_free) != -1)
                return ptr;
            _cel_sys_free(ptr);
        }
        return NULL;
    }
    return ptr;
}

static int cel_eventloopgroup_start(CelEventLoopThread *evt_loop_thread)
{
    CelEventLoopThreadId *thread_id = _cel_eventloopthread_id();

    thread_id->i = evt_loop_thread->i;
    //printf("thread id %d\r\n", thread_id->i);
    cel_eventloop_run(&(evt_loop_thread->evt_loop));
    _tprintf(_T("Event loop thread %d exit.(%s)"), 
       (int)cel_thread_getid(), cel_geterrstr(cel_geterrno()));
    cel_thread_exit(0);

    return 0;
}

int cel_eventloopgroup_init(CelEventLoopGroup *group, int n_threads)
{
    int i;
    CelCpuMask mask;
    CelEventLoopThread *evt_loop_thread;

    if (n_threads <= 0)
        n_threads = (int)cel_getnprocs();
    if (n_threads > CEL_THDNUM)
        n_threads = n_threads / 2;
    else if (n_threads < 2)
        n_threads = 2;
    if ((group->evt_loop_threads = (CelEventLoopThread *)
        cel_malloc(sizeof(CelEventLoopThread) * n_threads)) != NULL)
    {
        group->n_threads = n_threads;
        for (i  = 0; i < group->n_threads; i++)
        {
            evt_loop_thread = &(group->evt_loop_threads[i]);
            evt_loop_thread->i = i;
            cel_eventloop_init(&(evt_loop_thread->evt_loop), 1, 10240);
            cel_thread_create(&(evt_loop_thread->thread), NULL, 
                cel_eventloopgroup_start, evt_loop_thread);
            cel_setcpumask(&mask, i % group->n_threads);
            cel_thread_setaffinity(&(evt_loop_thread->thread), &mask);
        }
    }
    return 0;
}

void cel_eventloopgroup_destroy(CelEventLoopGroup *group)
{
    int i;
    void *ret_val;
    CelEventLoopThread *evt_loop_threads;

    for (i = 0; i < group->n_threads; i++)
    {
        evt_loop_threads = &(group->evt_loop_threads[i]);
        cel_thread_join(evt_loop_threads->thread, &ret_val);
    }
    cel_free(group->evt_loop_threads);
}

CelEventLoopGroup *cel_eventloopgroup_new(int n_threads)
{
    CelEventLoopGroup *group;

    if ((group = (CelEventLoopGroup *)
        cel_malloc(sizeof(CelEventLoopGroup))) != NULL)
    {
        if (cel_eventloopgroup_init(group, n_threads) == 0)
            return group;
        cel_free(group);
    }
    return NULL;
}

void cel_eventloopgroup_free(CelEventLoopGroup *group)
{
    cel_eventloopgroup_destroy(group); 
    cel_free(group);
}