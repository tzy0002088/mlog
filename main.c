#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define LOG_LVL     LOG_LVL_DBG
#define LOG_TAG     "main"
#include "tslog.h"

static pthread_t tid_1;
static pthread_t tid_2;
static pthread_t tid_3;

static void *test_thead(void *prma)
{
    while (1)
    {
        LOG_I("LOG_I %s.\n", (char *)prma);
        LOG_W("LOG_W %s.\n", (char *)prma);
        LOG_E("LOG_E %s.\n", (char *)prma);
        LOG_D("LOG_D %s.\n", (char *)prma);
        LOG_RAW("LOG_RAW %s.\n", (char *)prma);
        usleep(1000);
        //sleep(1);
    }
    return NULL;
}

int main(void)
{
    int count = 0;
    extern int mlog_console_be_init(void);
    mlog_init();
    mlog_console_be_init();

    LOG_I("LOG_I.\n");
    LOG_W("LOG_W.\n");
    LOG_E("LOG_E.\n");
    LOG_D("LOG_D.\n");

    pthread_create(&tid_1, NULL, test_thead, "tid_1");
    pthread_create(&tid_2, NULL, test_thead, "tid_2");
    pthread_create(&tid_3, NULL, test_thead, "tid_3");

    while(1)
    {
        LOG_I("LOG_I %d.\n", count++);
        LOG_W("LOG_W %d.\n", count++);
        LOG_E("LOG_E %d.\n", count++);
        LOG_D("LOG_D %d.\n", count++);
        LOG_RAW("LOG_RAW %d.\n", count++);
        usleep(1000);
    }

    return 0;
}