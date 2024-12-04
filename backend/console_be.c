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

int mlog_console_be_init(void)
{
    static mlog_backend_t console_be = {0};
    mlog_init();
    console_be.output = console_output;
    mlog_backend_register(&console_be, "console", 1);
    return 0;
}
