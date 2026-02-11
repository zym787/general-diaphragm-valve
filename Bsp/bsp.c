/**
 * @file      : bsp.c
 * @brief     : 板级支持包
 * 
 * @version   : 1.2
 * @author    : Drinkto
 * @date      : Dec 22, 2025
 * 
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Dec 22, 2025 | 1.0 | Drinkto | 首次发布 |
 * | Jan 15, 2026 | 1.1 | Drinkto | 调整初始化顺序,支援各外设,优化初始化 |
 * | Jan 20, 2026 | 1.2 | Drinkto | 支援F0串口1热重载引脚 |
 */

#include "bsp.h"


volatile unsigned long CPU_RunTime = 0UL;
volatile uint32_t idleCounter = 0UL;

/*
********************************************************************************
*   函 数 名: bsp_Init
*   功能说明: bsp层初始化
*   形    参: 无
*   返 回 值: 无
********************************************************************************
*/
void bsp_Init(void)
{
        // bsp_InitDWT(); /* 初始化DWT */
        /* ↓可以使用bsp_DelayMS和bsp_DelayUS函数 */

        /* ↑---------------------------- 注意 ! ----------------------------↑ */
        /// ↑箭头上方禁止使用bsp_timer.c和bsp_tim_pwm.c内函数
        bsp_InitTimer(); /* 初始化硬件及软件定时器 */
        /// ↓可以使用bsp_DelayMS和bsp_DelayUS函数

        bsp_InitUart(COM1, UART1_AF_DISABLE_PA9_PA10); /* 开机后先初始化串口1 调试口 */

        bsp_LogPrintfInfo();    /* 显示系统信息 */

#if DEBUG_MB != 2
        bsp_ModbusInit();   /* 初始化modbus, 485 串口2 */
#endif
        bsp_MotorInit(); /* 初始化电机 */

        bsp_println("bsp Init Completed! ");
}

/*
 *******************************************************************************
 *   函 数 名: Error_Handler
 *   形    参: file : 源代码文件名称。关键字 __FILE__ 表示源代码文件名。
 *             line ：代码行号。关键字 __LINE__ 表示源代码行号
 *   返 回 值: 无
 *       Error_Handler(__FILE__, __LINE__);
 *******************************************************************************
 */
void bsp_ErrorHandler(char *file, uint32_t line)
{
    /*
     用户可以添加自己的代码报告源代码文件名和代码行号，比如将错误文件和行号打印到串口
     printf("Wrong parameters value: file %s on line %d\r\n", file, line)
     */
    bsp_println("Wrong parameters value: file %s on line %d", file, line);

    /* 这是一个死循环，断言失败时程序会在此处死机，以便于用户查错 */
    if (line == 0)
    {
        return;
    }

    while (1)
    {
    }
}

/*
 *******************************************************************************
 *   函 数 名: bsp_RunPer10ms
 *   功能说明: 该函数每隔10ms被Systick中断调用1次。
 *             详见 bsp_timer.c的定时中断服务程序。一些处理时间要求不严格的
 *             任务可以放在此函数。比如：按键扫描、蜂鸣器鸣叫控制等。
 *   形    参: 无
 *   返 回 值: 无
 *******************************************************************************
 */
void bsp_RunPer10ms(void)
{
#if DEBUG_MB != 2
        /* 例如 Modbus 协议，可以插入Modbus轮询函数 */
        bsp_ModbusPoll();
#endif
}

/*
 *******************************************************************************
 *   函 数 名: bsp_RunPer1ms
 *   功能说明: 该函数每隔1ms被Systick中断调用1次。
 *             详见 bsp_timer.c的定时中断服务程序。一些需要周期性处理的事务
 *             可以放在此函数。比如：触摸坐标扫描。
 *   形    参: 无
 *   返 回 值: 无
 *******************************************************************************
 */
void bsp_RunPer1ms(void)
{
    ++CPU_RunTime;
}

/*
 *******************************************************************************
 *   函 数 名: bsp_Idle
 *   功能说明: 空闲时执行的函数。一般主程序在for和while循环程序体中需要
 *             插入 CPU_IDLE() 宏来调用本函数。
 *             本函数缺省为空操作。用户可以添加喂狗、设置CPU进入休眠模式的功能。
 *   形    参: 无
 *   返 回 值: 无
 *******************************************************************************
 */
void bsp_Idle(void)
{

    /* --- 让CPU进入休眠，由Systick定时中断唤醒或者其他中断唤醒 */
    
    
//     #if DEBUG_MB != 2
//     /* 例如 Modbus 协议，可以插入Modbus轮询函数 */
//     if (bsp_GetModbusPollFlag() == FALSE) {
//         return;
//     }
//     bsp_ModbusPoll();
//     #endif
}

void configureTimerForRunTimeStats(void)
{
    CPU_RunTime = 0UL;
}

unsigned long getRunTimeCounterValue(void)
{
    return CPU_RunTime;
}
