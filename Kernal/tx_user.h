/**************************************************************************/
/*                                                                        */
/*       Copyright (c) Microsoft Corporation. All rights reserved.        */
/*                                                                        */
/*       This software is licensed under the Microsoft Software License   */
/*       Terms for Microsoft Azure RTOS. Full text of the license can be  */
/*       found in the LICENSE file at https://aka.ms/AzureRTOS_EULA       */
/*       and in the root directory of this software.                      */
/*                                                                        */
/**************************************************************************/

/**************************************************************************/
/**************************************************************************/
/**                                                                       */
/** ThreadX Component                                                     */
/**                                                                       */
/**   User Specific                                                       */
/**                                                                       */
/**************************************************************************/
/**************************************************************************/

/**************************************************************************/
/*                                                                        */
/*  PORT SPECIFIC C INFORMATION                            RELEASE        */
/*                                                                        */
/*    tx_user.h                                           PORTABLE C      */
/*                                                           6.0          */
/*                                                                        */
/*  AUTHOR                                                                */
/*                                                                        */
/*    William E. Lamie, Microsoft Corporation                             */
/*                                                                        */
/*  DESCRIPTION                                                           */
/*                                                                        */
/*    This file contains user defines for configuring ThreadX in specific */
/*    ways. This file will have an effect only if the application and     */
/*    ThreadX library are built with TX_INCLUDE_USER_DEFINE_FILE defined. */
/*    Note that all the defines in this file may also be made on the      */
/*    command line when building ThreadX library and application objects. */
/*                                                                        */
/*  RELEASE HISTORY                                                       */
/*                                                                        */
/*    DATE              NAME                      DESCRIPTION             */
/*                                                                        */
/*  05-19-2020     William E. Lamie         Initial Version 6.0           */
/*                                                                        */
/**************************************************************************/

#ifndef TX_USER_H
#define TX_USER_H

/* Define various build options for the ThreadX port.  The application should either make changes
   here by commenting or un-commenting the conditional compilation defined OR supply the defines
   though the compiler's equivalent of the -D option.

   最快速度优化需要开启的选项:

        TX_MAX_PRIORITIES                       32
        TX_DISABLE_PREEMPTION_THRESHOLD
        TX_DISABLE_REDUNDANT_CLEARING
        TX_DISABLE_NOTIFY_CALLBACKS
        TX_NOT_INTERRUPTABLE
        TX_TIMER_PROCESS_IN_ISR
        TX_REACTIVATE_INLINE
        TX_DISABLE_STACK_FILLING
        TX_INLINE_THREAD_RESUME_SUSPEND

   最小代码优化需要开启的选项:

        TX_MAX_PRIORITIES                       32
        TX_DISABLE_PREEMPTION_THRESHOLD
        TX_DISABLE_REDUNDANT_CLEARING
        TX_DISABLE_NOTIFY_CALLBACKS
        TX_NOT_INTERRUPTABLE
        TX_TIMER_PROCESS_IN_ISR

   Of course, many of these defines reduce functionality and/or change the behavior of the
   system in ways that may not be worth the trade-off. For example, the TX_TIMER_PROCESS_IN_ISR
   results in faster and smaller code, however, it increases the amount of processing in the ISR.
   In addition, some services that are available in timers are not available from ISRs and will
   therefore return an error if this option is used. This may or may not be desirable for a
   given application.  */


/* 覆盖tx_port.h 里面的宏定义  */
/*#define TX_MAX_PRIORITIES                32*/
/*#define TX_THREAD_USER_EXTENSION                ????*/
/*#define TX_TIMER_THREAD_STACK_SIZE                1024*/
/*#define TX_TIMER_THREAD_PRIORITY                0*/

/*#define TX_MINIMUM_STACK                200*/


/*
   确定定时器是否到期的处理，比如应用定时器，溢出时间和函数tx_thread_sleep调用等，是在系统定时器任务里面还是在定时器中断里面调用。
   默认是在定时任务里面，当定义了下面函数后，将直接在定时器中断里面处理，可以去掉定时器任务所消耗资源。 */
// #define TX_TIMER_PROCESS_IN_ISR


/* 用于设置定时器激活是否采用内联方式，默认此功能是关闭的。如果使能后，内联方式的执行速度快，但增加代码量 */
// #define TX_REACTIVATE_INLINE


/* 用于设置是否关闭栈填充，默认情况下是使能的，所有任务的栈空间全部填充为0xEF，
 * 带有ThreadX调试组件或者运行时栈检测会用到。
 */
// #define TX_DISABLE_STACK_FILLING


/* 用于使能栈检测，默认是关闭的。此选项使能后，而TX_DISABLE_STACK_FILLING没使能时，栈填充将开启，方便栈检测 */
// #define TX_ENABLE_STACK_CHECKING


/* 用于设置是否关闭抢占阀值，默认是开启的。如果应用程序不需要此功能，关闭后可以降低代码需求，提升性能 */
#define TX_DISABLE_PREEMPTION_THRESHOLD


/* 用于设置是否清零ThreadX全局变量，如果编译器启动代码在ThreadX运行前清除了.bss段，那么可以关闭不必要的清零 */
// #define TX_DISABLE_REDUNDANT_CLEARING


/* 确定是否不需要定时器组，禁止后需要用户注释掉tx_initialize_low_level文件里面tx_timer_interrupt的调用。
   另外，禁止后，必须使能TX_TIMER_PROCESS_IN_ISR */
/*
#define TX_NO_TIMER
#ifndef TX_TIMER_PROCESS_IN_ISR
#define TX_TIMER_PROCESS_IN_ISR
#endif
*/


/* 用于设置是否关闭通知回调，默认是使能的。如果应用程序没有用到消息回调，关闭掉后可以减小代码，并且可以提升性能。 */
#define TX_DISABLE_NOTIFY_CALLBACKS


/* 使能tx_thread_resume和tx_thread_suspend使用内联代码，优势是提升这两个函数的执行性能，劣势是增加代码量 */
// #define TX_INLINE_THREAD_RESUME_SUSPEND


/* 设置TreadX内核不可中断，好处是降低处理负担，并且产生的代码小。但增加锁时间 */
// #define TX_NOT_INTERRUPTABLE


/* 使能事件Trace，会稍微增加点代码 */
// #define TX_ENABLE_EVENT_TRACE


/* 使能BLOCK_POOL信息获取 */
// #define TX_BLOCK_POOL_ENABLE_PERFORMANCE_INFO


/* 使能BYTE_POOL信息获取 */
// #define TX_BYTE_POOL_ENABLE_PERFORMANCE_INFO


/* 使能事件标志信息获取 */
// #define TX_EVENT_FLAGS_ENABLE_PERFORMANCE_INFO


/* 使能互斥信号量信息获取 */
// #define TX_MUTEX_ENABLE_PERFORMANCE_INFO


/* 使能消息对象信息获取 */
// #define TX_QUEUE_ENABLE_PERFORMANCE_INFO

/* 使能信号量信息获取  */
// #define TX_SEMAPHORE_ENABLE_PERFORMANCE_INFO


/* 使能任务性能信息获取 */
// #define TX_THREAD_ENABLE_PERFORMANCE_INFO


/* 使能定时器性能信息获取 */
// #define TX_TIMER_ENABLE_PERFORMANCE_INFO


/* 使能执行变更通知 */
// #define TX_ENABLE_EXECUTION_CHANGE_NOTIFY


/* 定义获取系统状态的宏。*/
/*#define TX_THREAD_GET_SYSTEM_STATE() _tx_thread_system_state */


/* 定义用于判断是否调用 _tx_thread_system_return 函数的条件
                                        (TX_THREAD_SYSTEM_RETURN_CHECK(c))*/
/*#define #define TX_THREAD_SYSTEM_RETURN_CHECK (c)  ((ULONG) _tx_thread_preempt_disable)*/


/* 定义通用的定时器滴答时长基准值，以供其他中间件组件使用。*/
/*#define TX_TIMER_TICKS_PER_SECOND                100*/


/* 确定基本对齐类型是否已定义。*/
// #define ALIGN_TYPE_DEFINED


/* 定义在块和字节池操作中使用的基本对齐类型 */
// #define ALIGN_TYPE  ULONG


/* 将 TX_MEMSET 宏定义为标准库函数。*/
// #define TX_MEMSET  memset((a),(b),(c))


#ifdef __IAR_SYSTEMS_ASM__
/* Define if the IAR library is supported. */
/*#define TX_ENABLE_IAR_LIBRARY_SUPPORT*/
#endif


/* 使能安全关键型配置 */
// #define TX_SAFETY_CRITICAL

#endif

