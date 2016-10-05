/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2016 Treasure Data Inc.
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef FLB_LOG_H
#define FLB_LOG_H

#include <fluent-bit/flb_info.h>
#include <fluent-bit/flb_macros.h>
#include <fluent-bit/flb_thread_storage.h>

#include <inttypes.h>
#include <pthread.h>
#include <errno.h>

/* FIXME: this extern should be auto-populated from flb_thread_storage.h */
extern FLB_TLS_DEFINE(struct flb_log, flb_log_ctx)

/* Message types */
#define FLB_LOG_OFF     0
#define FLB_LOG_ERROR   1
#define FLB_LOG_WARN    2
#define FLB_LOG_INFO    3  /* default */
#define FLB_LOG_DEBUG   4
#define FLB_LOG_TRACE   5

/* Logging outputs */
#define FLB_LOG_STDERR   0  /* send logs to STDERR         */
#define FLB_LOG_FILE     1  /* write logs to a file        */
#define FLB_LOG_SOCKET   2  /* write logs to a unix socket */

#define FLB_LOG_EVENT    MK_EVENT_NOTIFICATION

/* Logging main context */
struct flb_log {
    uint16_t type;             /* log type */
    uint16_t level;            /* level    */
    char *out;                 /* FLB_LOG_FILE or FLB_LOG_SOCKET */
    pthread_t tid;             /* thread ID   */
    struct mk_event_loop *evl;
};

/*
int flb_log_check(int level) {
    struct flb_log *lc = FLB_TLS_GET(flb_log_ctx);
    if (lc->level < level)
        return FLB_FALSE;
    else
        return FLB_TRUE;
}
*/

#ifdef FLB_HAVE_C_TLS
#define flb_log_check(l)                                            \
    (FLB_TLS_GET(flb_log_ctx)->level < l) ? FLB_FALSE: FLB_TRUE
#else
int flb_log_check(int level);
#endif

struct flb_log *flb_log_init(int type, int level, char *out);
int flb_log_stop(struct flb_log *log);
void flb_log_print(int type, const char *file, int line, const char *fmt, ...);

/* Logging macros */
#define flb_error(fmt, ...)                                          \
        flb_log_print(FLB_LOG_ERROR, NULL, 0, fmt, ##__VA_ARGS__)

#define flb_warn(fmt, ...)                                           \
        flb_log_print(FLB_LOG_WARN, NULL, 0, fmt, ##__VA_ARGS__)

#define flb_info(fmt, ...)                                           \
        flb_log_print(FLB_LOG_INFO, NULL, 0, fmt, ##__VA_ARGS__)

#define flb_debug(fmt, ...)                                         \
        flb_log_print(FLB_LOG_DEBUG, NULL, 0, fmt, ##__VA_ARGS__)

#ifdef FLB_HAVE_TRACE
#define flb_trace(fmt, ...)                                             \
        flb_log_print(FLB_LOG_TRACE, __FILE__, __LINE__,                \
                      fmt, ##__VA_ARGS__)
#else
#define flb_trace(fmt, ...)  do {} while(0)
#endif

int flb_log_worker_init(void *data);
int flb_errno_print(int errnum, const char *file, int line);

#ifdef __FILENAME__
#define flb_errno() flb_errno_print(errno, __FILENAME__, __LINE__)
#else
#define flb_errno() flb_errno_print(errno, __FILE__, __LINE__)
#endif

#endif
