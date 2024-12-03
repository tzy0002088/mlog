#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "mlog.h"
#include "mlog_list.h"

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
    const char *tag;
    char *log;
};

struct mlog_async_buf
{
    char buf[MLOG_ASYNC_LOG_BUF];
    uint16_t read_index;
    uint16_t write_index;
};

struct mlog
{
    uint8_t is_init;
    char log_buf_th[MLOG_LINE_MAX_SIZE + 1];
    char log_buf_isr[MLOG_LINE_MAX_SIZE + 1];
    struct
    {
        uint32_t level;
        char tag[MLOG_FILTER_TAG_MAX + 1];
    } filter;
    slist_t backend_list;
#if MLOG_USING_ASYNC_OUTPUT
    struct mlog_async_buf async_buf;
#endif
};

#define MLOG_FRAME_MAGIC                  (0x68)
#define MLOG_NEWLINE_SIGN                 ("\n")
#define MLOG_ALIGN(size, align)           (((size) + (align) - 1) & ~((align) - 1))

#define MLOG_FORMAT_STR(dst, fmt, ...)          \
do {                                                  \
    int len = snprintf(dst, MLOG_LINE_MAX_SIZE - fmt_len, fmt, __VA_ARGS__);  \
    if (len < 0) {                                    \
        return -1;                                    \
    }                                                 \
    fmt_len += len;                                   \
} while(0)

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

static const char *level_to_info[] = {
    "E/",
    "W/",
    "I/",
    "D/",
};

static void mlog_lock(void)
{
    mlog_port_lock();
}

static void mlog_unlock(void)
{
    mlog_port_unlock();
}

static char *mlog_get_log_buf(void)
{
    if (!mlog_port_in_isr())
    {
        return mlog.log_buf_th;
    }
    else
    {
#if MLOG_USING_ISR_LOG
        return mlog.log_buf_isr;
#else
        return NULL;
#endif
    }
}

static void mlog_output_to_all_backend(uint8_t level, const char *tag, char *log_buf, size_t log_len)
{
    size_t drop_len;
    slist_t *node;
    mlog_backend_t *backend;
    for (node = slist_first(&mlog.backend_list); node; node = slist_next(node))
    {
        backend = container_of(node, mlog_backend_t, list);
        if (backend->output)
        {
            if (backend->filter && !backend->filter(backend, tag, level))
                continue;

            if (backend->sup_color)
            {
                backend->output(backend, log_buf, log_len);
            }
            else
            {
                if (level_to_color[level])
                {
                    drop_len = strlen(level_to_color[level]) + strlen(CSI_START);
                    log_buf += drop_len;
                    log_len -= (drop_len + strlen(CSI_END) + 1);
                    log_buf[log_len] = '\0';
                }
                backend->output(backend, log_buf, log_len);
            }
        }
    }
}

#if MLOG_USING_ASYNC_OUTPUT
static void mlog_async_lock(void)
{
    mlog_port_async_lock();
}

static void mlog_async_unlock(void)
{
    mlog_port_async_unlock();
}

static inline int mlog_async_buffer_data_size(void)
{
    return mlog.async_buf.write_index - mlog.async_buf.read_index;
}

static inline int mlog_async_buffer_space(void)
{
    return MLOG_ASYNC_LOG_BUF - mlog.async_buf.write_index;
}

static int mlog_async_put_frame(size_t size, struct mlog_frame *frame)
{
    int ret = -1;
    struct mlog_frame *log_frame = NULL;
    if (mlog_async_buffer_space() >= size)
    {
        mlog_async_lock();
        log_frame = (struct mlog_frame *)&mlog.async_buf.buf[mlog.async_buf.write_index];
        *log_frame = *frame;
        log_frame->log = (char *)log_frame + sizeof (struct mlog_frame);
        strncpy(log_frame->log, frame->log, frame->log_len);
        mlog.async_buf.write_index += size;
        mlog_async_unlock();
        ret = 0;
    }
    return ret;
}

static void *mlog_async_get_frame(size_t size)
{
    char *buf = NULL;
    if (mlog_async_buffer_data_size() >= size)
    {
        buf = &mlog.async_buf.buf[mlog.async_buf.read_index];
        mlog.async_buf.read_index += size;
    }
    else
    {
        mlog.async_buf.read_index = 0;
        mlog_async_lock();
        mlog.async_buf.write_index = 0;
        mlog_async_unlock();
    }
    return buf;
}

void mlog_async_output(const char *name)
{
    struct mlog_frame *frame;
    mlog_backend_t *backend = name ? mlog_backend_find(name) : NULL;
    while ((frame = mlog_async_get_frame(MLOG_ALIGN(sizeof(struct mlog_frame), 4))))
    {
        if (frame->magic == MLOG_FRAME_MAGIC)
        {
            if (!backend)
                mlog_output_to_all_backend(frame->level, frame->tag, frame->log, frame->log_len);
            else if (backend->output)
                backend->output(backend, frame->log, frame->log_len);
            /* release log buffer */
            mlog_async_get_frame(MLOG_ALIGN(frame->log_len, 4));
        }
    }
}
#endif

static int mlog_head_formater(uint8_t level, const char *tag, char *log_buf)
{
    int fmt_len = 0;

    if (level_to_color[level])
    {
        MLOG_FORMAT_STR(log_buf, "%s", CSI_START);
        MLOG_FORMAT_STR(log_buf + fmt_len, "%s", level_to_color[level]);
    }
    MLOG_FORMAT_STR(log_buf + fmt_len, "[%s%s]", level_to_info[level], tag);

#if MLOG_OUTPUT_THREAD_NAME
    MLOG_FORMAT_STR(log_buf + fmt_len, " %s", mlog_port_thread_name());
#endif

#if MLOG_OUTPUT_TIME
    MLOG_FORMAT_STR(log_buf + fmt_len, " %s", mlog_port_time());
#endif

    MLOG_FORMAT_STR(log_buf + fmt_len, "%s", ": ");
    return fmt_len;
}

static int mlog_tail_formater(uint8_t level, char *log_buf, size_t fmt_len)
{
    int old_fmt = fmt_len;
    if (level_to_color[level])
    {
        MLOG_FORMAT_STR(log_buf + fmt_len, "%s", CSI_END);
    }

#if MLOG_OUTPUT_NEWLINE
    MLOG_FORMAT_STR(log_buf + fmt_len, "%s", MLOG_NEWLINE_SIGN);
#endif

    log_buf[fmt_len] = '\0';
    return fmt_len - old_fmt;
}

static int mlog_formater(uint32_t level, const char *tag, char *log_buf, const char *format, va_list args)
{
    int head_len = -1, log_len = -1, tail_len = -1;

    head_len = mlog_head_formater(level, tag, log_buf);
    if (head_len > 0)
        log_len = vsnprintf(log_buf + head_len, MLOG_LINE_MAX_SIZE - head_len, format, args);
    if (log_len > 0)
        tail_len =  mlog_tail_formater(level, log_buf, head_len + log_len);

    return tail_len >= 0 ? head_len + log_len + tail_len : -1;
}

static void mlog_do_output(uint32_t level, const char *tag, char *log_buf, size_t log_len)
{
#if MLOG_USING_ASYNC_OUTPUT
    struct mlog_frame log_frame;

    log_frame.magic = MLOG_FRAME_MAGIC;
    log_frame.level = level;
    log_frame.log_len = log_len;
    log_frame.tag = tag;
    log_frame.log = log_buf;

    if (!mlog_async_put_frame(MLOG_ALIGN(sizeof(struct mlog_frame) + log_len, 4), &log_frame))
    {
        mlog_port_async_notify();
    }
#else
    mlog_output_to_all_backend(level, tag, log_buf, log_len);
#endif
}

void mlog_output(uint32_t level, const char *tag, const char *format, ...)
{
    va_list args;
    int fmt_len = 0;
    char *log_buf = NULL;

    if (!mlog.is_init)
        return;
#if MLOG_USING_FILTER
    if (level > mlog.filter.level)
        return;
    else if (strstr(mlog.filter.tag, tag))
        return;
#endif
    log_buf = mlog_get_log_buf();
    if (log_buf)
    {
        va_start(args, format);
        mlog_lock();
        fmt_len = mlog_formater(level, tag, log_buf, format, args);
        if (fmt_len > 0 && fmt_len <= MLOG_LINE_MAX_SIZE)
            mlog_do_output(level, tag, log_buf, fmt_len + 1); // Allocate an extra byte terminator
        mlog_unlock();
        va_end(args);
    }
}

void mlog_raw(const char *format, ...)
{
    va_list args;
    int fmt_len = 0;
    char *log_buf = NULL;

    log_buf = mlog_get_log_buf();
    if (log_buf)
    {
        va_start(args, format);
        mlog_lock();
        fmt_len = vsnprintf(log_buf, MLOG_LINE_MAX_SIZE, format, args);
        if (fmt_len > 0 && fmt_len <= MLOG_LINE_MAX_SIZE)
            mlog_do_output(LOG_LVL_DBG, "", log_buf, fmt_len + 1); // Allocate an extra byte terminator
        mlog_unlock();
        va_end(args);
    }
}

void mlog_flush(void)
{
    slist_t *node;
    mlog_backend_t *backend;
    for (node = slist_first(&mlog.backend_list); node; node = slist_next(node))
    {
        backend = container_of(node, mlog_backend_t, list);
        if (backend->flush)
            backend->flush(backend);
    }
}

int mlog_backend_register(struct mlog_backend *backend, const char *name, int sup_color)
{
    if (backend->init)
        backend->init(backend);
    strncpy(backend->name, name, MLOG_BACKEND_NAME_MAX);
    backend->sup_color = sup_color;
    slist_init(&backend->list);
    slist_append(&mlog.backend_list, &backend->list);

    return 0;
}

int mlog_backend_unregister(struct mlog_backend *backend)
{
    if (backend->deinit)
        backend->deinit(backend);
    slist_remove(&mlog.backend_list, &backend->list);
    return 0;
}

mlog_backend_t *mlog_backend_find(const char *name)
{
    slist_t *node;
    mlog_backend_t *backend;
    for (node = slist_first(&mlog.backend_list); node; node = slist_next(node))
    {
        backend = container_of(node, mlog_backend_t, list);
        if (!strcmp(backend->name, name))
            return backend;
    }
    return NULL;
}

int mlog_async_loop(void)
{
#if MLOG_USING_ASYNC_OUTPUT
    mlog_async_output(NULL);
    while (1)
    {
        mlog_port_async_wait(LOG_ASYNC_WAITING_FOREVER);
        while (1)
        {
            mlog_async_output(NULL);
            if (!mlog_port_async_wait(2000))
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
#endif
    return -1;
}

void mlog_global_filter_level_set(uint32_t level)
{
    mlog.filter.level = level;
}

void mlog_global_filter_tag_set(const char *tag)
{
    strncpy(mlog.filter.tag, tag, MLOG_FILTER_TAG_MAX);
}

int mlog_init(void)
{
    if (mlog.is_init)
        return 0;

    mlog_port_init();
    slist_init(&mlog.backend_list);
    mlog.filter.level = LOG_FILTER_LVL_ALL;
    mlog.is_init = 1;

    return 0;
}
