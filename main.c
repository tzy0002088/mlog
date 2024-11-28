#include <stdio.h>

#define LOG_LVL     LOG_LVL_DBG
#define LOG_TAG     "main"
#include "tslog.h"

int main(void)
{
    LOG_I("hello.\n");

    //mlog_init();

    printf("xxx.\n");
    return 0;
}