/**
 * @file      : loop.h
 * @brief     :
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Jan 27, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Jan 27, 2026 | 1.0 | Drinkto | xxx |
 */

#ifndef __LOOP_H__
#define __LOOP_H__

/* 任务编号 */
typedef enum {
        TASK_INDEX_1 = 0, /* 任务1 */
        TASK_INDEX_2 = 1, /* 任务2 */
        TASK_INDEX_3 = 2, /* 任务3 */
        TASK_INDEX_4 = 3, /* 任务4 */
        TASK_INDEX_5 = 4, /* 任务5 */
        TASK_INDEX_6 = 5, /* 任务6 */
        TASK_MAX,         /* 任务数 */
} TASK_INDEX_E;

extern void loop(void);

#endif
