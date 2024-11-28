#include <bits/stdint-uintn.h>
#include <stdint.h>
#include "mlog.h"

struct mlog_frame
{
    uint32_t magic : 8;
    uint32_t level : 4;
    uint32_t log_len : 20;
    uint8_t *log_buf;
};

struct mlog_async_rb
{
    uint8_t buf[MLOG_ASYNC_LOG_BUF];
    uint32_t read_index;
    uint32_t write_index;
};

struct mlog
{
    bool is_init;
    char log_buf_th[MLOG_LINE_MAX_SIZE + 1];
    char log_buf_isr[MLOG_LINE_MAX_SIZE + 1];
    struct
    {
        int level; // 全局 level 过滤
        char *tag; // 全局 tag 过滤
    } filter;
    slist_t backend_list;
#if MLOG_USING_ASYNC_OUTPUT
    struct mlog_async_rb async_rb;
#endif
};

static struct mlog mlog = {0};

extern void mlog_port_lock(void);
extern void mlog_port_unlock(void);
extern void mlog_port_init(void);
extern int mlog_port_in_isr(void);
extern const char *mlog_port_thread_name(void);

static int mlog_async_buf_use(void)
{
    
}

void mlog_output(uint8_t level, const char *tag, const char *format, ...)
{
    /* 获取一个指定尺寸的 buffer */
}

int mlog_init(void)
{
    mlog_port_init();
    return 0;
}

static int mlog_async_buf_space(void)
{

}

static int mlog_async_log_put(void *log, size_t len)
{
    


}

static int mlog_async_log_get(uint8_t **ptr)
{

}

void mlog_async_output(const char *name)
{
    while (1)
    {

    }
}

int mlog_async_loop(void)
{
    extern int mlog_port_async_wait(int time);
    extern void mlog_port_async_notify(void);
    /* 外面开线程，来调用这个函数 */
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
