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
#define LOG_RAW(...)

int mlog_init(void);
int mlog_async_loop(void);

/* output */
/* 如果是异步输出，格式化完事的 log 存放到哪里？ */
void mlog_output(uint8_t level, const char *tag, const char *format, ...);
void mlog_flush(void);

/* 后端注册 */
int mlog_backend_register(struct mlog_backend *backend, const char *name, int sup_color);
int mlog_backend_unregister(struct mlog_backend *backend);

#ifdef __cplusplus
}
#endif

#endif //__MLOG_H__