
#include "core_riscv32.h"
#include "mlog.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include <assert.h>
#include <stddef.h>

static SemaphoreHandle_t line_buffer_lock;
static SemaphoreHandle_t async_lock;
static SemaphoreHandle_t async_signal;
static int irq_lock_level = 0;
static int async_irq_lock_level = 0;

int mlog_port_in_isr(void);

void mlog_port_lock(void)
{
    if (!mlog_port_in_isr())
    {
        xSemaphoreTake(line_buffer_lock, portMAX_DELAY);
    }
    else
    {
#if MLOG_USING_ISR_LOG
        irq_lock_level = vPortSetInterruptMask();
#endif
    }
}

void mlog_port_unlock(void)
{
    if (!mlog_port_in_isr())
    {
        xSemaphoreGive(line_buffer_lock);
    }
    else
    {
#if MLOG_USING_ISR_LOG
        vPortClearInterruptMask(irq_lock_level);
#endif
    }
}

void mlog_port_async_lock(void)
{
#if MLOG_USING_ISR_LOG
    async_irq_lock_level = vPortSetInterruptMask();
#else
    xSemaphoreTake(async_lock, portMAX_DELAY);
#endif
}

void mlog_port_async_unlock(void)
{
#if MLOG_USING_ISR_LOG
    vPortClearInterruptMask(async_irq_lock_level);
#else
    xSemaphoreGive(async_lock);
#endif
}

static void mlog_async_entry(void *prma)
{
    mlog_async_loop();
    vTaskDelete(NULL);
}

int mlog_port_init(void)
{
    BaseType_t xTask;

    line_buffer_lock = xSemaphoreCreateMutex();
#if MLOG_USING_ISR_LOG
    async_lock = xSemaphoreCreateMutex();
    assert(async_lock);
#endif
    async_signal = xSemaphoreCreateBinary();
    assert(line_buffer_lock && async_signal);

    xTask = xTaskCreate(mlog_async_entry, "mlog_async", configMINIMAL_STACK_SIZE * 2,
                        NULL, tskIDLE_PRIORITY + 1, NULL);
    assert(xTask == pdPASS);
    return 0;
}

int mlog_port_in_isr(void)
{
    return !(read_csr(mstatus) & 0x8);
}

const char *mlog_port_thread_name(void)
{
    const char *name = "ISR";

    if (mlog_port_in_isr())
    {
        if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
            name = "N/A";
    }
    else
    {
        if (xTaskGetCurrentTaskHandle())
            name = pcTaskGetName(NULL);
        else
            name = "N/A";;
    }
    return name;
}

/* async thread contex */
int mlog_port_async_wait(int timeout_ms)
{
    BaseType_t ret;

    if (timeout_ms >= 0)
        ret = xSemaphoreTake(async_signal, pdMS_TO_TICKS(timeout_ms));
    else
        ret = xSemaphoreTake(async_signal, portMAX_DELAY);
    return ret == pdPASS ? 0 : -1;
}

/* thread or irq contex */
void mlog_port_async_notify(void)
{
    if (!mlog_port_in_isr())
    {
        xSemaphoreGive(async_signal);
    }
    else
    {
        BaseType_t taskAwake = pdFALSE;
        xSemaphoreGiveFromISR(async_signal, &taskAwake);
        if (taskAwake == pdPASS)
        {
            portYIELD_FROM_ISR(taskAwake);
        }
    }
}