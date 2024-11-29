#include <stdio.h>

#define LOG_LVL     LOG_LVL_DBG
#define LOG_TAG     "main"
#include "tslog.h"

int main(void)
{
    extern int mlog_console_be_init(void);
    mlog_init();
    mlog_console_be_init();

    LOG_I("LOG_I.\n");
    LOG_W("LOG_D.\n");
    LOG_E("LOG_D.\n");
    LOG_D("LOG_I.\n");
    LOG_RAW("hello wolrd.\n");

    return 0;
}