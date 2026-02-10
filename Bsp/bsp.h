/*!
 * @file     : bsp.h
 * @brief    : Borad Surport Package 板级支持包
 *
 * @version   : 1.1
 * @author    : Drinkto
 * @date      : Dec 22, 2025
 * 
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Dec 22, 2025 | 1.0 | Drinkto | 首次发布 |
 * | Jan 15, 2026 | 1.1 | Drinkto | 调整文件内容 |
 */

#ifndef __BSP_H
#define __BSP_H

#ifdef __cplusplus
extern "C" {
#endif

///编译配置等
// #include "config.h" /* 配置 */

/* Includes ------------------------------------------------------------------*/
/* HAL Drivers */
#include "stm32f1xx_hal.h"
// #include "main.h"
// #include "gpio.h"
#include "usart.h"
// #include "tim.h"
// #include "iwdg.h"

/* Stander Lib */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* Component */

/* Board Support Package */
/* 通过取消注释或者添加注释的方式控制是否包含底层驱动模块 */
#include "bsp_port.h"   /* 硬件信息 */

// #include "bsp_iwdg.h"
#include "bsp_dwt.h"
#include "bsp_timer.h"
#include "bsp_led.h"
#include "bsp_addr.h"

#include "bsp_msg.h"
#include "bsp_user_lib.h"
#include "bsp_uart_fifo.h"
#include "bsp_log.h"

#include "bsp_modbus.h"
#include "bsp_motor.h"
#include "bsp_addr.h"

/* 定义 BSP 版本号及修订日期 */
#define __STM32F1_BSP_VERSION       "v1.3"
#define __STM32F1_BSP_REV_DATE      "260203"

/* CPU空闲时执行的函数 */
//#define CPU_IDLE()        bsp_Idle()
/* 开关全局中断的宏 */
#define ENABLE_INT()    __set_PRIMASK(0)    /* 使能全局中断 */
#define DISABLE_INT()   __set_PRIMASK(1)    /* 禁止全局中断 */

/* 这个宏仅用于调试阶段排错 */
#if DEBUG_BSP == 1
#define BSP_Printf      printf
#else
#define BSP_Printf(...)
#endif

/* 方便RTOS里面使用 */
extern void SysTick_ISR(void);
#define bsp_ProPer1ms SysTick_ISR

#define ERROR_HANDLER()     Error_Handler(__FILE__, __LINE__);

#ifndef TRUE
    #define TRUE                    1
#endif
#ifndef FALSE
    #define FALSE                   0
#endif

/* 提供给其他C文件调用的函数 */
extern void bsp_Init(void);
extern void bsp_Idle(void);

void bsp_GetCpuID(uint32_t *_id);
void bsp_ErrorHandler(char *file, uint32_t line);

extern void configureTimerForRunTimeStats(void);
extern unsigned long getRunTimeCounterValue(void);

#ifdef __cplusplus
}
#endif
#endif //__BSP_H
