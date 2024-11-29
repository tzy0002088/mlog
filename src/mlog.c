#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "mlog.h"

/*
* +------+---------+------+---------+-----+--------+----------+
* | head | fmt_log | head | fmt_log |.... | .......|
* +------+---------+------+---------+-----+--------+----------+
*/

struct mlog_frame
{
    uint32_t magic : 8;
    uint32_t level : 4;
    uint32_t log_len : 20;
    char *log; // ---> payload
};

struct mlog_async_rb
{
    char buf[MLOG_ASYNC_LOG_BUF];
    uint32_t read_index;
    uint32_t write_index;
};

struct mlog
{
    uint8_t is_init;
    char log_buf_th[MLOG_LINE_MAX_SIZE + 1];
    char log_buf_isr[MLOG_LINE_MAX_SIZE + 1];
    struct
    {
        int level;
        char *tag;
    } filter;
    slist_t backend_list;
#if MLOG_USING_ASYNC_OUTPUT
    struct mlog_async_rb async_rb;
#endif
};

static struct mlog mlog = {0};

/* ANSI Escape Codes */
#define CSI_START     "\033["
#define CSI_END       "\033[0m"

#define RED      "31m"
#define GREEN    "32m"
#define YELLOW   "33m"
#define CYAN     "36m"
#define WHITE    "37m"
#define DEFAULT  (0)

#ifndef MLOG_COLOR_DBG
#define MLOG_COLOR_DBG           DEFAULT
#endif

#ifndef MLOG_COLOR_INFO
#define MLOG_COLOR_INFO          GREEN
#endif

#ifndef MLOG_COLOR_WARNING
#define MLOG_COLOR_WARNING       YELLOW
#endif

#ifndef MLOG_COLOR_ERROR
#define MLOG_COLOR_ERROR         RED
#endif

static const char *level_to_color[] = {
    MLOG_COLOR_ERROR,
    MLOG_COLOR_WARNING,
    MLOG_COLOR_INFO,
    MLOG_COLOR_DBG
};

// [D/main] 11-29 16:36:58.629 main: LOG_D.
static const char *level_to_info[] = {
    "E/",
    "W/",
    "I/",
    "D/",
};

extern int mlog_port_in_isr(void);

static void mlog_lock(void)
{
    extern void mlog_port_lock(void);
    mlog_port_lock();
}

static void mlog_unlock(void)
{
    extern void mlog_port_unlock(void);
    mlog_port_unlock();
}

static char *mlog_get_log_buf(void)
{
    if (!mlog_port_in_isr())
        return mlog.log_buf_th;
    else
        return mlog.log_buf_isr;
}

static int mlog_async_buf_use(void)
{
    return 0;
}

static void *mlog_async_buf_alloc(size_t size)
{
    /* 返回一个地址，并移动写指针 */
    return 0;
}

static uint8_t *mlog_async_buf_read(size_t size)
{
    /* 返回一个地址，并移动读指针 */
    return 0;
}

static void mlog_do_output(uint8_t level, const char *tag, char *log_buf, size_t len)
{
#if MLOG_USING_ASYNC_OUTPUT
    extern void mlog_port_async_notify(void);
    struct mlog_frame *log_frame = mlog_async_buf_alloc(sizeof(struct mlog_frame) + len);
    if (log_frame)
    {
        log_frame->magic = 0x6d;
        log_frame->level = level;
        log_frame->log_len = len;
        log_frame->log = (char *)log_frame + sizeof (*log_frame);
        strncpy(log_frame->log, log_buf,  len);
        mlog_port_async_notify();
    }
#else
    /* 输出到所有的后端 */
    slist_t *node;
    mlog_backend_t *backend;
    for (node = slist_first(&mlog.backend_list); node; node = slist_next(node))
    {
        backend = container_of(node, mlog_backend_t, list);
        if (backend->output)
        {
            if (backend->sup_color)
            {
                backend->output(backend, log_buf, len);
            }
            else
            {
                /* drop color size*/
            }
        }
    }

#endif

}

static int mlog_head_formater(uint8_t level, const char *tag, char *log_buf)
{
    uint8_t fmt_len = 0;
    extern const char *mlog_port_thread_name(void);

    if (level_to_color[level])
    {
        fmt_len = snprintf(log_buf, MLOG_LINE_MAX_SIZE, "%s", CSI_START);
        fmt_len += snprintf(log_buf + fmt_len, MLOG_LINE_MAX_SIZE, "%s", level_to_color[level]);
    }
    fmt_len += snprintf(log_buf + fmt_len, MLOG_LINE_MAX_SIZE, "[%s%s]: ", level_to_info[level], tag);
    return fmt_len;
}

static int mlog_tail_formater(uint8_t level, char *log_buf, size_t log_len)
{
    int fmt_len = 0;
    if (level_to_color[level])
    {
        fmt_len = snprintf(log_buf + log_len, MLOG_LINE_MAX_SIZE, "%s", CSI_END);
    }
    return fmt_len;
}

static int mlog_formater(uint8_t level, const char *tag, char *log_buf, const char *format, va_list args)
{
    int fmt_len = mlog_head_formater(level, tag, log_buf);
    fmt_len += vsnprintf(log_buf + fmt_len, MLOG_LINE_MAX_SIZE, format, args);
    fmt_len +=  mlog_tail_formater(level, log_buf, fmt_len);
    return fmt_len;
}

void mlog_output(uint8_t level, const char *tag, const char *format, ...)
{
    va_list args;
    int log_len = 0;
    char *log_buf = NULL;

    if (!mlog.is_init)
        return;
#if MLOG_USING_FILTER
    if (level > mlog.filter.level)
        return;
    else if (mlog.filter.tag && !strcmp(tag, mlog.filter.tag))
        return;
#endif
    log_buf = mlog_get_log_buf();

    mlog_lock();
    va_start(args, format);
    log_len = mlog_formater(level, tag, log_buf, format, args);
    va_end(args);
    mlog_do_output(level, tag, log_buf, log_len);
    mlog_unlock();
}

void mlog_raw(const char *format, ...)
{
    char *log_buf = NULL;
    int fmt_len = 0;
    va_list args;
    log_buf = mlog_get_log_buf();

    mlog_lock();
    va_start(args, format);
    fmt_len = vsnprintf(log_buf, MLOG_LINE_MAX_SIZE, format, args);
    va_end(args);
    if (fmt_len > 0 && fmt_len <= MLOG_LINE_MAX_SIZE)
        mlog_do_output(LOG_LVL_DBG, "", log_buf, fmt_len);
    mlog_unlock();
}

int mlog_init(void)
{
    extern void mlog_port_init(void);
    mlog_port_init();
    mlog.is_init = 1;
    slist_init(&mlog.backend_list);
    mlog.filter.level = LOG_FILTER_LVL_ALL;

    return 0;
}

void mlog_async_output(const char *name)
{
    
}

void mlog_flush(void)
{

}

int mlog_backend_register(struct mlog_backend *backend, const char *name, int sup_color)
{
    if (backend->init)
        backend->init(backend);
    strncpy(backend->name, name, MLOG_BACKEND_NAME_MAX);
    backend->sup_color = sup_color;
    slist_init(&backend->list);
    /* lock */
    slist_append(&mlog.backend_list, &backend->list);

    return 0;
}

int mlog_backend_unregister(struct mlog_backend *backend)
{
    return 0;
}

int mlog_async_loop(void)
{
    extern int mlog_port_async_wait(int time);
    extern void mlog_port_async_notify(void);
    mlog_async_output(NULL);
    while (1)
    {
        mlog_port_async_wait(-1);
        while (1)
        {
            mlog_async_output(NULL);
            if (mlog_port_async_wait(2) == 0)
            {
                continue;
            }
            else
            {
                mlog_flush();
                break;
            }
        }
    }

    return -1;
}
