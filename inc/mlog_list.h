/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-11-28      tzy          first implementation
 */

#ifndef __MLOG_LIST_H__
#define __MLOG_LIST_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct slist_node
{
    struct slist_node *next;
};
typedef struct slist_node slist_t;

static inline void slist_init(slist_t *l)
{
    l->next = NULL;
}

static inline void slist_append(slist_t *l, slist_t *n)
{
    slist_t *node = l;
    while(node->next) node = node->next;
    node->next = n;
    n->next = NULL;
}

static inline void slist_remove(slist_t *l, slist_t *n)
{
    slist_t *node = l;
    while (node->next && node->next != n) node = node->next;
    if (node->next != NULL) node->next = node->next->next;
}

static inline slist_t *slist_first(slist_t *l)
{
    return l->next;
}

static inline slist_t *slist_next(slist_t *n)
{
    return n->next;
}

#define container_of(ptr, type, member) \
        ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))

#ifdef __cplusplus
}
#endif

#endif //__MLOG_LIST_H__