/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-12-1      tzy          first implementation
 */

#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <time.h>
#include "mlog.h"

static pthread_t tid;
static pthread_mutex_t line_lock;
static pthread_mutex_t async_lock;
static sem_t notify;

void mlog_port_lock(void)
{
    pthread_mutex_lock(&line_lock);
}

void mlog_port_unlock(void)
{
    pthread_mutex_unlock(&line_lock);
}

void mlog_port_async_lock(void)
{
    pthread_mutex_lock(&async_lock);
}

void mlog_port_async_unlock(void)
{
    pthread_mutex_unlock(&async_lock);
}

static void *mlog_async_entry(void *prma)
{
    mlog_async_loop();
    return "byby";
}

void mlog_port_init(void)
{
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);    
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&line_lock, &attr);
    pthread_mutex_init(&async_lock, &attr);
    sem_init(&notify, 0, 0);
    pthread_create(&tid, NULL, mlog_async_entry, NULL);
}

int mlog_port_in_isr(void)
{
    return 0;
}

const char *mlog_port_thread_name(void)
{
    static char buffer[24];
    snprintf(buffer, 24, "tid: %u", (unsigned int)pthread_self());
    return buffer;
}

const char *mlog_port_time(void)
{
    static char time_buf[20];
    struct tm tm;
    time_t cur_time = time(NULL);
    localtime_r(&cur_time, &tm);
    snprintf(time_buf, 40, "%02d-%02d %02d:%02d:%02d", tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
    return time_buf;
}

static void sem_reset(sem_t* sem)
{
    int sval;
    sem_getvalue(sem, &sval);
    while(sval--)
    {
        sem_trywait(sem);
    }
}

int mlog_port_async_wait(int timeout_ms)
{
    int ret = -1;
    struct timespec ts;
    if (timeout_ms > 0)
    {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += timeout_ms / 1000;             
        ts.tv_nsec += (timeout_ms % 1000) * 1000000;
        if (ts.tv_nsec >= 1000000000)
        {             
            ts.tv_sec++;
            ts.tv_nsec -= 1000000000;
        }
        ret = sem_timedwait(&notify, &ts);
    }
    else if (timeout_ms == 0)
    {
        ret = sem_trywait(&notify);
    }
    else
    {
        ret = sem_wait(&notify);
    }
    if (ret == 0)
    {
        sem_reset(&notify);
    }

    return ret;
}

void mlog_port_async_notify(void)
{
    sem_post(&notify);
}