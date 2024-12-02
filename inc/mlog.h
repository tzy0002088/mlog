#ifndef __MLOG_H__
#define __MLOG_H__

#include "mlog_cfg.h"
#include "mlog_def.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif

#define LOG_W(...)          mlog_w(LOG_TAG, __VA_ARGS__)
#define LOG_I(...)          mlog_i(LOG_TAG, __VA_ARGS__)
#define LOG_E(...)          mlog_e(LOG_TAG, __VA_ARGS__)
#define LOG_D(...)          mlog_d(LOG_TAG, __VA_ARGS__)
#define LOG_RAW(...)        mlog_raw(__VA_ARGS__)

int mlog_init(void);
void mlog_output(uint8_t level, const char *tag, const char *format, ...);
void mlog_raw(const char *format, ...);

/* async */
void mlog_async_output(const char *name);
int mlog_async_loop(void);
void mlog_flush(void);

/* backend */
int mlog_backend_register(mlog_backend_t *backend, const char *name, int sup_color);
int mlog_backend_unregister(mlog_backend_t *backend);
mlog_backend_t *mlog_backend_find(const char *name);

#ifdef __cplusplus
}
#endif

#endif //__MLOG_H__