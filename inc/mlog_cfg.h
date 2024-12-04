/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-27      tzy          first implementation
 */
 
#ifndef __MLOG_CFG_H__
#define __MLOG_CFG_H__

#ifndef MLOG_BACKEND_NAME_MAX
#define MLOG_BACKEND_NAME_MAX       (8U)
#endif

#ifndef MLOG_LINE_MAX_SIZE
#define MLOG_LINE_MAX_SIZE          (256U)
#endif

#ifndef MLOG_ASYNC_LOG_BUF
#define MLOG_ASYNC_LOG_BUF          (2048U)
#endif

#ifndef MLOG_FILTER_TAG_MAX
#define MLOG_FILTER_TAG_MAX         (15U)
#endif

#ifndef MLOG_USING_FILTER
#define MLOG_USING_FILTER           (1)
#endif

#ifndef MLOG_USING_ASYNC_OUTPUT
#define MLOG_USING_ASYNC_OUTPUT     (1)
#endif

#ifndef MLOG_OUTPUT_THREAD_NAME
#define MLOG_OUTPUT_THREAD_NAME     (1)
#endif

#ifndef MLOG_OUTPUT_CURRENT_TIME
#define MLOG_OUTPUT_CURRENT_TIME    (1)
#endif

#ifndef MLOG_USING_ISR_LOG
#define MLOG_USING_ISR_LOG          (1)
#endif

#ifndef MLOG_OUTPUT_TIME
#define MLOG_OUTPUT_TIME            (1)
#endif

#ifndef MLOG_OUTPUT_NEWLINE
#define MLOG_OUTPUT_NEWLINE           (1)
#endif

#endif //__MLOG_CFG_H__
