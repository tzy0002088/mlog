#include <stdint.h>
#include "mlog.h"

struct mlog
{
    char log_buf_th[MLOG_LINE_MAX_SIZE + 1];
    char log_buf_isr[MLOG_LINE_MAX_SIZE + 1];
    struct
    {
        int level; // 全局 level 过滤
        char *tag; // 全局 tag 过滤
    } filter;
    slist_t backend_list;
};

extern void mlog_port_lock(void);
extern void mlog_port_unlock(void);
extern void mlog_port_init(void);
extern int mlog_port_in_isr(void);
extern const char *mlog_port_thread_name(void);
extern void mlog_port_async_wait(int time);
extern void mlog_port_async_notify(void);

void mlog_output(uint8_t level, const char *tag, const char *format, ...)
{
    
}

int mlog_init(void)
{
    mlog_port_init();
    return 0;
}

int mlog_async_loop(void)
{
    /* 外面开线程，来调用这个函数 */
    while (1)
    {
        /* wait thread */
        
    }

    return -1;
}
