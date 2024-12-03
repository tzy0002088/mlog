#include <stdio.h>
#include <string.h>
#include "mlog.h"


static int console_output(struct mlog_backend *backend, const char *log, size_t length)
{
    while (length--)
    {
        putchar(*log++);
    }
    return 0;
}

static int console_filter(struct mlog_backend *backend, const char *tag, unsigned int level)
{
    /* nothing filter */
    if (!strcmp("xxx_tag", tag))
        return 0;
    return -1;
}

int mlog_console_be_init(void)
{
    static mlog_backend_t console_be = {0};
    mlog_init();
    console_be.output = console_output;
    console_be.filter = console_filter;
    mlog_backend_register(&console_be, "uart0", 1);
    return 0;
}
