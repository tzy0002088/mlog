/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-27      tzy          first implementation
 */

#ifndef __MLOG_H__
#define __MLOG_H__

#include <stdint.h>
#include "mlog_def.h"

#ifdef __cplusplus
extern "C"{
#endif

#define LOG_W(...)          mlog_w(LOG_TAG, __VA_ARGS__)
#define LOG_I(...)          mlog_i(LOG_TAG, __VA_ARGS__)
#define LOG_E(...)          mlog_e(LOG_TAG, __VA_ARGS__)
#define LOG_D(...)          mlog_d(LOG_TAG, __VA_ARGS__)
#define LOG_RAW(...)        mlog_raw(__VA_ARGS__)
#define LOG_HEX(...)        mlog_hex(LOG_TAG, __VA_ARGS__)

/* init */
int mlog_init(void);
void mlog_output(uint32_t level, const char *tag, const char *format, ...);
void mlog_raw(const char *format, ...);
/* async */
void mlog_async_output(const char *name);
int mlog_async_loop(void);
void mlog_flush(void);
/* backend */
int mlog_backend_register(mlog_backend_t *backend, const char *name, int sup_color);
int mlog_backend_unregister(mlog_backend_t *backend);
mlog_backend_t *mlog_backend_find(const char *name);
/* filter */
void mlog_global_filter_level_set(uint32_t level);
void mlog_global_filter_tag_set(const char *tag);

/* port */
int mlog_port_in_isr(void);
void mlog_port_lock(void);
void mlog_port_unlock(void);
/* Asynchronous buffer lock to avoid using the same lock as the print thread */
void mlog_port_async_lock(void);
void mlog_port_async_unlock(void);
const char *mlog_port_thread_name(void);
void mlog_port_async_notify(void);
int mlog_port_async_wait(int time);
void mlog_port_init(void);
const char *mlog_port_time(void);

#ifdef __cplusplus
}
#endif

#endif //__MLOG_H__