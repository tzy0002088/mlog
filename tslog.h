#ifndef __TSLOG_H__
#define __TSLOG_H__

#ifdef __cplusplus
extern "C" {
#endif

#define TS_LOG_ENABLE
#define TS_LOG_COLOR
#define PKG_USING_MLOG

#if defined(TS_LOG_ENABLE) && defined(PKG_USING_MLOG)
#include "mlog.h"
#else

#if defined(TS_LOG_ENABLE)

#define TS_LOG_PRINT       printf

/* log level */
#define LOG_LVL_DBG         3
#define LOG_LVL_INFO        2
#define LOG_LVL_WARNING     1
#define LOG_LVL_ERROR       0

#ifndef LOG_LVL
#define LOG_LVL       LOG_LVL_WARNING
#endif

#ifndef LOG_TAG
#define LOG_TAG       "DBG"
#endif

/* ANSI Escape Codes */
#define BLACK    "30"
#define RED      "31"
#define GREEN    "32"
#define YELLOW   "33"
#define BLUE     "34"
#define PURPLE   "35"
#define CYAN     "36"
#define WHITE    "37"
#define DEFAULT  "0"

#ifdef TS_LOG_COLOR
#define LOG_OUTPUT(color, lvl, ...)                         \
do                                                          \
{                                                           \
    TS_LOG_PRINT("\033["color"m[" lvl "/" LOG_TAG "] ");  \
    TS_LOG_PRINT(__VA_ARGS__);                                 \
    TS_LOG_PRINT("\033[0m");                                   \
}while(0)                                   
#define LOG_OUTPOT_RAW(...)         TS_LOG_PRINT(__VA_ARGS__)
#else
#define LOG_OUTPUT(n, lvl, ...)                             \
do                                                          \
{                                                           \
    TS_LOG_PRINT("[" lvl "/" LOG_TAG "] ");               \
    TS_LOG_PRINT(__VA_ARGS__);                                 \
}while(0)                                                   
#endif
#else
#define LOG_OUTPUT(color, lvl, ...)
#define LOG_OUTPOT_RAW(...)
#endif /* TS_LOG_ENABLE */

#if LOG_LVL >= LOG_LVL_WARNING
#define LOG_W(...)         LOG_OUTPUT(YELLOW, "W", __VA_ARGS__)
#else
#define LOG_W(...)
#endif

#if LOG_LVL >= LOG_LVL_INFO
#define LOG_I(...)         LOG_OUTPUT(GREEN, "I", __VA_ARGS__)
#else
#define LOG_I(...)
#endif

#if LOG_LVL >= LOG_LVL_ERROR
#define LOG_E(...)         LOG_OUTPUT(RED, "E", __VA_ARGS__)
#else
#define LOG_E(...)
#endif

#if LOG_LVL >= LOG_LVL_DBG
#define LOG_D(...)         LOG_OUTPUT(DEFAULT, "D", __VA_ARGS__)
#else
#define LOG_D(...)
#endif

#define LOG_RAW(...)         LOG_OUTPOT_RAW(__VA_ARGS__)

#endif /* PKG_USING_MLOG */

#ifdef __cplusplus
}
#endif

#endif //__TSLOG_H__