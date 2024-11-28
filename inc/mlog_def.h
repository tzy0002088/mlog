#ifndef __MLOG_DEF_H__
#define __MLOG_DEF_H__

#include <stddef.h>
#include "mlog_list.h"

#ifdef __cplusplus
extern "C" {
#endif

/* log level */
#define LOG_LVL_DBG         3
#define LOG_LVL_INFO        2
#define LOG_LVL_WARNING     1
#define LOG_LVL_ERROR       0

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

typedef struct mlog_backend
{
    char name[8];
    int sup_color;
    int (*init)(struct mlog_backend *backend);
    int (*output)(struct mlog_backend *backend, const char *log, size_t length);
    int (*flush)(struct mlog_backend *backend);
    int (*deinit)(struct mlog_backend *backend);
    slist_t node;
} mlog_backend_t;

#ifdef __cplusplus
}
#endif

#endif //__MLOG_DEF_H__
