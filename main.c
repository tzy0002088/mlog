
/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-27      tzy          first implementation
 */

#include <stddef.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

#define LOG_LVL     LOG_LVL_DBG
#define LOG_TAG     "main"
#include "mlog.h"

static pthread_t tid_1;
static pthread_t tid_2;
static pthread_t tid_3;
static pthread_t tid_4;

static void *test_thead(void *prma)
{
    uint8_t hex_buf[40];
    for (int i = 0; i < 40; i++)
    {
        hex_buf[i] = i + 'A';
    }
    int len = snprintf((char *)hex_buf, 1, "1");

    while (1)
    {
        LOG_HEX("ble_rx", hex_buf, 40);
        LOG_I("LOG_I %s.", (char *)prma);
        LOG_W("LOG_W %s.", (char *)prma);
        LOG_E("LOG_E %s.", (char *)prma);
        LOG_D("LOG_D %s.", (char *)prma);
        LOG_RAW("LOG_RAW %s.\n", (char *)prma);
        usleep(1000);
    }
    return NULL;
}

int main(void)
{
    int count = 0;
    extern int mlog_console_be_init(void);
    mlog_init();
    mlog_console_be_init();

    pthread_create(&tid_1, NULL, test_thead, "tid_1");
    pthread_create(&tid_2, NULL, test_thead, "tid_2");
    pthread_create(&tid_3, NULL, test_thead, "tid_3");
    pthread_create(&tid_4, NULL, test_thead, "tid_4");

    while(1)
    {
        LOG_I("LOG_I %d.", count++);
        LOG_W("LOG_W %d.", count++);
        LOG_E("LOG_E %d.", count++);
        LOG_D("LOG_D %d.", count++);
        LOG_RAW("LOG_RAW %d.\n", count++);
        usleep(1000);
    }

    return 0;
}