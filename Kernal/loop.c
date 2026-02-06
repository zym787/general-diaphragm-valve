/**
 * @file      : loop.c
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

#include <stdbool.h>

#include "loop.h"
#include "main.h"
#include "stm32f1xx_hal.h"
#include "tx_api.h"
#include "bsp.h"

/********************************************** 任务优先级,数值越小优先级越高 */
#define APP_CFG_TASK_START_PRIO   2u
#define APP_CFG_TASK_MsgPro_PRIO  3u
#define APP_CFG_TASK_USER_IF_PRIO 4u
#define APP_CFG_TASK_COM_PRIO     5u
#define APP_CFG_TASK_STAT_PRIO    30u
#define APP_CFG_TASK_IDLE_PRIO    31u
#define APP_CFG_TASK_KEEP_PRIO    5u
#define APP_CFG_TASK_DEMO_PRIO    6u

/* 任务抢占阈值,高于此值的任务才允许抢占 如果该值等于任务优先级,则禁用抢占阈值 */
#define APP_CFG_TASK_START_PRE_TH   2u
#define APP_CFG_TASK_MsgPro_PRE_TH  3u
#define APP_CFG_TASK_USER_IF_PRE_TH 4u
#define APP_CFG_TASK_COM_PRE_TH     5u
#define APP_CFG_TASK_STAT_PRE_TH    30u
#define APP_CFG_TASK_IDLE_PRE_TH    31u
#define APP_CFG_TASK_KEEP_PRE_TH    5u
#define APP_CFG_TASK_DEMO_PRE_TH    6u

/******************************************************** 任务栈大小,单位字节 */
#define APP_CFG_TASK_START_STK_SIZE   1024u
#define APP_CFG_TASK_MsgPro_STK_SIZE  1024u
#define APP_CFG_TASK_COM_STK_SIZE     1024u
#define APP_CFG_TASK_USER_IF_STK_SIZE 1024u
#define APP_CFG_TASK_IDLE_STK_SIZE    1024u
#define APP_CFG_TASK_STAT_STK_SIZE    1024u
#define APP_CFG_TASK_KEEP_STK_SIZE    1024u
#define APP_CFG_TASK_DEMO_STK_SIZE    1024u

/*********************************************** 静态变量: 任务启动线程控制块 */
static TX_THREAD AppTaskStartTCB;
static uint64_t AppTaskStartStack[APP_CFG_TASK_START_STK_SIZE / 8];

static TX_THREAD AppTaskMsgProTCB;
static uint64_t AppTaskMsgProStk[APP_CFG_TASK_MsgPro_STK_SIZE / 8];

static TX_THREAD AppTaskCOMTCB;
static uint64_t AppTaskCOMStk[APP_CFG_TASK_COM_STK_SIZE / 8];

static TX_THREAD AppTaskUserIFTCB;
static uint64_t AppTaskUserIFStk[APP_CFG_TASK_USER_IF_STK_SIZE / 8];

static TX_THREAD AppTaskIdleTCB;
static uint64_t AppTaskIdleStk[APP_CFG_TASK_IDLE_STK_SIZE / 8];

static TX_THREAD AppTaskStatTCB;
static uint64_t AppTaskStatStk[APP_CFG_TASK_STAT_STK_SIZE / 8];

static TX_THREAD AppTaskKeepTCB;
static uint64_t AppTaskKeepStk[APP_CFG_TASK_KEEP_STK_SIZE / 8];

static TX_THREAD AppTaskDemoTCB;
static uint64_t AppTaskDemoStk[APP_CFG_TASK_DEMO_STK_SIZE / 8];

/******************************************************************* 函数声明 */
static void AppTaskStart(ULONG thread_input);
static void AppTaskMsgPro(ULONG thread_input);
static void AppTaskUserIF(ULONG thread_input);
static void AppTaskCOM(ULONG thread_input);
static void AppTaskIDLE(ULONG thread_input);
static void AppTaskStat(ULONG thread_input);
static void App_Printf(const char *fmt, ...);
static void AppTaskCreate(void);
static void DispTaskInfo(void);
static void AppObjCreate(void);
static void OSStatInit(void);
static void AppTaskKeep(ULONG thread_input);
static void AppTaskDemo(ULONG thread_input);

/******************************************************************* 变量声明 */
static TX_MUTEX AppPrintfSemp; /* 用于printf互斥 */

/* 统计任务使用 */
__IO uint8_t OSStatRdy;  /* 统计任务就绪标志 */
__IO uint32_t OSIdleCtr; /* 空闲任务计数 */
__IO float OSCPUUsage;   /* CPU百分比 */
uint32_t OSIdleCtrMax;   /* 1秒内最大的空闲计数 */
uint32_t OSIdleCtrRun;   /* 1秒内空闲任务当前计数 */

uint32_t thread_0_counter = 0;
uint32_t thread_demo_counter = 0;

/**
 * @brief     : 主循环 用于替代main函数,防止CubeMX重新生成时覆盖
 *              c程序入口
 */
void loop(void)
{
        /* 内核开启前关闭HAL的时间基准 */
        HAL_SuspendTick();

        /* 进入ThreadX内核 */
        tx_kernel_enter();

        /* 正常不会运行到这里,运行到这里证明出错了 */
        for (;;) {
                // HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR1_Pin, GPIO_PIN_SET);

                // HAL_GPIO_TogglePin(LED2_R_GPIO_Port, LED2_R_Pin);
                // HAL_Delay(500);

                // HAL_GPIO_WritePin(DIR1_GPIO_Port, DIR1_Pin, GPIO_PIN_RESET);
                // HAL_GPIO_TogglePin(LED1_G_GPIO_Port, LED1_G_Pin);
                // HAL_Delay(500);
        }
}

/*
*******************************************************************************
*	函 数 名: tx_application_define
*	功能说明: ThreadX专用的任务创建，通信组件创建函数
*	形    参: first_unused_memory  未使用的地址空间
*	返 回 值: 无
*******************************************************************************
*/
void tx_application_define(void *first_unused_memory)
{
        /* 将系统定义的相关内容放在这里，例如线程创建信息以及其他各类创建相关信息 */

        /*****************************创建启动任务*****************************/
        tx_thread_create(&AppTaskStartTCB,            /* 任务控制块地址 */
                         "AppTaskStart",              /* 任务名 */
                         AppTaskStart,                /* 启动任务函数地址 */
                         0,                           /* 传递给任务的参数 */
                         &AppTaskStartStack[0],       /* 堆栈基地址 */
                         APP_CFG_TASK_START_STK_SIZE, /* 堆栈空间大小 */
                         APP_CFG_TASK_START_PRIO,     /* 任务优先级 */
                         APP_CFG_TASK_START_PRE_TH,   /* 任务抢占阈值 */
                         TX_NO_TIME_SLICE,            /* 不开启时间片 */
                         TX_AUTO_START);              /* 创建后立即启动 */
}

/*
********************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 启动任务。
*	形    参: thread_input 是在创建该任务时传递的形参
*	返 回 值: 无
*       优 先 级: 2
********************************************************************************
*/
static void AppTaskStart(ULONG thread_input)
{
        (void)thread_input;

        /* 内核开启后，恢复HAL里的时间基准 */
        HAL_ResumeTick();

        /* 外设初始化 */
        bsp_Init();

        /* 创建任务 */
        AppTaskCreate();

        /* 创建任务间通信机制 */
        AppObjCreate();

        for (;;) {
                /* 需要周期性处理的程序，对应裸机工程调用的SysTick_ISR */
                bsp_ProPer1ms();
                tx_thread_sleep(1);
                // HAL_GPIO_TogglePin(LED2_R_GPIO_Port, LED2_R_Pin);
                // // tx_thread_sleep(500);
                // HAL_Delay(2051);
        }
}

/*
********************************************************************************
*	函 数 名: OSStatInit
*	功能说明: 优先执行任务统计
*	形    参: 无
*	返 回 值: 无
*       优 先 级: 30
********************************************************************************
*/
void OSStatInit(void)
{
        OSStatRdy = FALSE;

        tx_thread_sleep(2u); /* 时钟同步 */

        //__disable_irq();
        OSIdleCtr = 0uL; /* 清空闲计数 */
                         //__enable_irq();

        tx_thread_sleep(100); /* 统计100ms内，最大空闲计数 */

        //__disable_irq();
        OSIdleCtrMax = OSIdleCtr; /* 保存最大空闲计数 */
        OSStatRdy = TRUE;
        //__enable_irq();
}

/*
********************************************************************************
*	函 数 名: AppTaskStatistic
*	功能说明: 统计任务，用于实现CPU利用率的统计。为了测试更加准确，可以开启注释调用的全局中断开关
*	形    参: thread_input 创建该任务时传递的形参
*	返 回 值: 无
*       优 先 级: 30
********************************************************************************
*/
static void AppTaskStat(ULONG thread_input)
{
        (void)thread_input;

        while (OSStatRdy == FALSE) {
                tx_thread_sleep(200); /* 等待统计任务就绪 */
        }

        OSIdleCtrMax /= 100uL;
        if (OSIdleCtrMax == 0uL) {
                OSCPUUsage = 0u;
        }

        //__disable_irq();
        OSIdleCtr = OSIdleCtrMax * 100uL; /* 设置初始CPU利用率 0% */
                                          //__enable_irq();

        for (;;) {
                // __disable_irq();
                OSIdleCtrRun = OSIdleCtr; /* 获得100ms内空闲计数 */
                OSIdleCtr = 0uL;          /* 复位空闲计数 */
                                          //	__enable_irq();            /* 计算100ms内的CPU利用率 */
                OSCPUUsage = (100uL - (float)OSIdleCtrRun / OSIdleCtrMax);
                tx_thread_sleep(100); /* 每100ms统计一次 */
        }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskIDLE
*	功能说明: 空闲任务
*	形    参: thread_input 创建该任务时传递的形参
*	返 回 值: 无
        优 先 级: 31
*********************************************************************************************************
*/
static void AppTaskIDLE(ULONG thread_input)
{
        TX_INTERRUPT_SAVE_AREA

            (void)
            thread_input;

        while (1) {
                TX_DISABLE
                OSIdleCtr++;
                TX_RESTORE
        }
}

/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate(void)
{
        // /**************创建MsgPro任务*********************/
        // tx_thread_create(&AppTaskMsgProTCB,            /* 任务控制块地址 */
        //                  "App Msp Pro",                /* 任务名 */
        //                  AppTaskMsgPro,                /* 启动任务函数地址 */
        //                  0,                            /* 传递给任务的参数 */
        //                  &AppTaskMsgProStk[0],         /* 堆栈基地址 */
        //                  APP_CFG_TASK_MsgPro_STK_SIZE, /* 堆栈空间大小 */
        //                  APP_CFG_TASK_MsgPro_PRIO,     /* 任务优先级*/
        //                  APP_CFG_TASK_MsgPro_PRIO,     /* 任务抢占阀值 */
        //                  TX_NO_TIME_SLICE,             /* 不开启时间片 */
        //                  TX_AUTO_START);               /* 创建后立即启动 */

        // /**************创建USER IF任务*********************/
        // tx_thread_create(&AppTaskUserIFTCB,             /* 任务控制块地址 */
        //                  "App Task UserIF",             /* 任务名 */
        //                  AppTaskUserIF,                 /* 启动任务函数地址 */
        //                  0,                             /* 传递给任务的参数 */
        //                  &AppTaskUserIFStk[0],          /* 堆栈基地址 */
        //                  APP_CFG_TASK_USER_IF_STK_SIZE, /* 堆栈空间大小 */
        //                  APP_CFG_TASK_USER_IF_PRIO,     /* 任务优先级*/
        //                  APP_CFG_TASK_USER_IF_PRIO,     /* 任务抢占阀值 */
        //                  TX_NO_TIME_SLICE,              /* 不开启时间片 */
        //                  TX_AUTO_START);                /* 创建后立即启动 */

        // /**************创建COM任务*********************/
        // tx_thread_create(&AppTaskCOMTCB,            /* 任务控制块地址 */
        //                  "App Task COM",            /* 任务名 */
        //                  AppTaskCOM,                /* 启动任务函数地址 */
        //                  0,                         /* 传递给任务的参数 */
        //                  &AppTaskCOMStk[0],         /* 堆栈基地址 */
        //                  APP_CFG_TASK_COM_STK_SIZE, /* 堆栈空间大小 */
        //                  APP_CFG_TASK_COM_PRIO,     /* 任务优先级*/
        //                  APP_CFG_TASK_COM_PRIO,     /* 任务抢占阀值 */
        //                  TX_NO_TIME_SLICE,          /* 不开启时间片 */
        //                  TX_AUTO_START);            /* 创建后立即启动 */
        /**************创建KEEP任务*********************/
        tx_thread_create(&AppTaskKeepTCB,            /* 任务控制块地址 */
                         "App Task Keep",            /* 任务名 */
                         AppTaskKeep,                /* 启动任务函数地址 */
                         0,                          /* 传递给任务的参数 */
                         &AppTaskKeepStk[0],         /* 堆栈基地址 */
                         APP_CFG_TASK_KEEP_STK_SIZE, /* 堆栈空间大小 */
                         APP_CFG_TASK_KEEP_PRIO,     /* 任务优先级*/
                         APP_CFG_TASK_KEEP_PRE_TH,   /* 任务抢占阀值 */
                         TX_NO_TIME_SLICE,           /* 不开启时间片 */
                         TX_AUTO_START);             /* 创建后立即启动 */
        /**************创建DEMO任务*********************/
        tx_thread_create(&AppTaskDemoTCB,            /* 任务控制块地址 */
                         "App Task Demo",            /* 任务名 */
                         AppTaskDemo,                /* 启动任务函数地址 */
                         0,                          /* 传递给任务的参数 */
                         &AppTaskDemoStk[0],         /* 堆栈基地址 */
                         APP_CFG_TASK_DEMO_STK_SIZE, /* 堆栈空间大小 */
                         APP_CFG_TASK_DEMO_PRIO,     /* 任务优先级*/
                         APP_CFG_TASK_DEMO_PRE_TH,   /* 任务抢占阀值 */
                         TX_NO_TIME_SLICE,           /* 不开启时间片 */
                         TX_AUTO_START);             /* 创建后立即启动 */
}

/* 以下定义测试线程  */
/**
 * @brief     : 线程0 简单的while-forever-sleep循环
 * @param     : thread_input
 */
static void AppTaskKeep(ULONG thread_input)
{
        UINT status;

        /* 这个线程只是简单的处于while-forever-sleep循环,用于测试线程的基本功能 */
        while (1) {
                /* Increment the thread counter.  */
                thread_0_counter++;

                /* Sleep for 10 ticks.  */
                // HAL_Delay(1000);
                // bsp_LedToggle(LED_GREEN);
                // uint32_t uSec = 1750;
                // bsp_println("Toggle LED in %d us !", uSec);
                // bsp_LedToggleFlag();
                // bsp_StartHardTimer(2, uSec, (void *)bsp_LedToggleFlag);

                bsp_LedToggle(LED_GREEN);
                _tx_thread_sleep(1000);

                /* Check status.  */
                if (status != TX_SUCCESS)
                        break;
        }
}

/**
 * @brief     : 线程1 简单的while-forever-sleep循环
 * @param     : thread_input
 */
static void AppTaskDemo(ULONG thread_input)
{
        UINT status;

        bsp_MotorInit();

            while (1)
        {
                /* Increment the thread counter.  */
                thread_demo_counter++;

                bsp_MotorTask();

                _tx_thread_sleep(50);

                /* Check status.  */
                if (status != TX_SUCCESS)
                        break;
        }
}

/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通讯
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppObjCreate(void)
{
        /* 创建互斥信号量 */
        tx_mutex_create(&AppPrintfSemp, "AppPrintfSemp", TX_NO_INHERIT);
}

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式
*	形    参: 同printf的参数。
*             在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
static void App_Printf(const char *fmt, ...)
{
        char buf_str[200 + 1]; /* 特别注意，如果printf的变量较多，注意此局部变量的大小是否够用 */
        va_list v_args;

        va_start(v_args, fmt);
        (void)vsnprintf((char *)&buf_str[0], (size_t)sizeof(buf_str), (char const *)fmt, v_args);
        va_end(v_args);

        /* 互斥操作 */
        tx_mutex_get(&AppPrintfSemp, TX_WAIT_FOREVER);

        printf("%s", buf_str);

        tx_mutex_put(&AppPrintfSemp);
}
#endif

/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将ThreadX任务信息通过串口打印出来
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void DispTaskInfo(void)
{
        TX_THREAD *p_tcb; /* 定义一个任务控制块指针 */

        p_tcb = &AppTaskStartTCB;

        /* 打印标题 */
        App_Printf("===============================================================\r\n");
        App_Printf("OS CPU Usage = %5.2f%%\r\n", OSCPUUsage);
        App_Printf("===============================================================\r\n");
        App_Printf(" 任务优先级 任务栈大小 当前使用栈  最大栈使用   任务名\r\n");
        App_Printf("   Prio     StackSize   CurStack    MaxStack   Taskname\r\n");

        /* 遍历任务控制列表TCB list)，打印所有的任务的优先级和名称 */
        while (p_tcb != (TX_THREAD *)0) {
                App_Printf(
                    "   %2d        %5d      %5d       %5d      %s\r\n", p_tcb->tx_thread_priority,
                    p_tcb->tx_thread_stack_size, (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_ptr,
                    (int)p_tcb->tx_thread_stack_end - (int)p_tcb->tx_thread_stack_highest_ptr, p_tcb->tx_thread_name);

                p_tcb = p_tcb->tx_thread_created_next;

                if (p_tcb == &AppTaskStartTCB)
                        break;
        }
}
