/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-27      tzy          first implementation
 */

#ifndef __MLOG_DEF_H__
#define __MLOG_DEF_H__

#include <stddef.h>
#include "mlog_cfg.h"
#include "mlog_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/* log level */
#define LOG_LVL_DBG         3
#define LOG_LVL_INFO        2
#define LOG_LVL_WARNING     1
#define LOG_LVL_ERROR       0

#define LOG_FILTER_LVL_ALL          LOG_LVL_DBG
#define LOG_ASYNC_WAITING_FOREVER   -1

#ifndef LOG_LVL
#define LOG_LVL       LOG_LVL_WARNING
#endif

#ifndef LOG_TAG
#define LOG_TAG       "DBG"
#endif

#if LOG_LVL >= LOG_LVL_DBG
#define mlog_d(TAG, ...)         mlog_output(LOG_LVL_DBG, TAG, __VA_ARGS__)
#else
#define mlog_d(TAG, ...)
#endif

#if LOG_LVL >= LOG_LVL_INFO
#define mlog_i(TAG, ...)         mlog_output(LOG_LVL_INFO, TAG, __VA_ARGS__)
#else
#define mlog_i(TAG, ...)
#endif

#if LOG_LVL >= LOG_LVL_WARNING
#define mlog_w(TAG, ...)         mlog_output(LOG_LVL_WARNING, TAG, __VA_ARGS__)
#else
#define mlog_w(TAG, ...)
#endif

#if LOG_LVL >= LOG_LVL_ERROR
#define mlog_e(TAG, ...)         mlog_output(LOG_LVL_ERROR, TAG, __VA_ARGS__)
#else
#define mlog_e(TAG, ...)
#endif

#define mlog_hex(TAG, buf, len)      mlog_hexdump(TAG, buf, len)

typedef struct mlog_backend
{
    char name[MLOG_BACKEND_NAME_MAX];
    int sup_color;
    int (*init)(struct mlog_backend *backend);
    int (*output)(struct mlog_backend *backend, const char *log, size_t length);
    /* Backend filter, If 0 is returned, it is considered necessary to filter the log */
    int (*filter)(struct mlog_backend *backend, const char *tag, uint8_t level);
    int (*flush)(struct mlog_backend *backend);
    int (*deinit)(struct mlog_backend *backend);
    slist_t list;
} mlog_backend_t;

#ifdef __cplusplus
}
#endif

#endif //__MLOG_DEF_H__
