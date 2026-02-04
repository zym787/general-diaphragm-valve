/**
 * @file      : bsp_timer.h
 * @brief     : 定时器模块
 *              实现了多个软件定时器供主程序使用(精度1ms),可通过修改 TMR_COUNT 增加软定时器个数
 *              实现了ms级别延时函数(精度1ms) 和us级别延时函数
 *              实现了系统运行时间函数(1ms单位)
 * 
 * @version   : 1.8
 * @author    : Drinkto
 * @date      : Dec 23, 2025
 * 
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Feb 01, 2013 | 1.0 | armfly | 正式发布 |
 * | Jun 21, 2013 | 1.1 | Drinkto | 增加us级延迟函数 bsp_DelayUS |
 * | Sep 07, 2014 | 1.2 | Drinkto | 增加TIM4 硬件定时中断，实现us级别定时.20us - 16秒 |
 * | May 04, 2015 | 1.3 | Drinkto | 增加 bsp_CheckRunTime(int32_t _LastTime) 用来计算时间差值 |
 * | May 22, 2015 | 1.4 | Drinkto | 完善 bsp_InitHardTimer() ，增加条件编译选择TIM2-5 |
 * | Nov 26, 2018 | 1.5 | Drinkto | s_tTmr赋初值0; 增加g_ucEnableSystickISR变量避免HAL提前打开systick中断引起的异常 |
 * | Jan 05, 2026 | 1.6 | Drinkto | 支援TIM1，以适应stm32f030 |
 * | Jan 07, 2026 | 1.7 | Drinkto | 支援stm32f103,添加管理bsp库滴答定时器中断接口 |
 * | Jan 10, 2026 | 1.8 | Drinkto | 添加检测定时器是否开启接口 |
 */

#ifndef __BSP_TIMER_H
#define __BSP_TIMER_H

#include <stdint.h>

///移植选项：使用CubeMX生成的工程时,建议关闭0,函数在stm32f1xx_it.c内已定义
///          仅需自行实现bsp库的滴答定时中断服务程序,调用SysTick_ISR()即可
#define USE_BSP_SYSTICK_ISR     0   ///是否启用bsp层系统嘀嗒定时器中断服务管理

/*
 在此定义若干个软件定时器全局变量
 注意，必须增加__IO 即 volatile，因为这个变量在中断和主程序中同时被访问，有可能造成编译器错误优化。
 */
#define TMR_COUNT   6       /* 软件定时器的个数 （定时器ID范围 0 - TMR_COUNT - 1) */

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef enum
{
    TMR_ONCE_MODE = 0, /* 一次工作模式 */
    TMR_AUTO_MODE = 1 /* 自动定时工作模式 */
} TMR_MODE_E;

/* 定时器结构体，成员变量必须是 volatile, 否则C编译器优化时可能有问题 */
typedef struct
{
    volatile uint8_t Mode; /* 计数器模式，1次性 */
    volatile uint8_t Flag; /* 定时到达标志  */
    volatile uint32_t Count; /* 计数器 */
    volatile uint32_t PreLoad; /* 计数器预装值 */
} SOFT_TMR;

/* 提供给其他C文件调用的函数 */
void bsp_InitTimer(void);
#ifndef THREADX_ENABLED
extern uint8_t bsp_GetSystickISRFlag(void);
extern void bsp_SetSystickISRFlag(uint8_t _switch);
#endif
void SysTick_ISR(void);
extern void bsp_DelayMS(uint32_t n);
extern void bsp_DelayUS(uint32_t n);
void bsp_StartTimer(uint8_t _id, uint32_t _period);
void bsp_StartAutoTimer(uint8_t _id, uint32_t _period);
void bsp_StopTimer(uint8_t _id);
uint8_t bsp_CheckTimer(uint8_t _id);
uint8_t bsp_IsTimerON(uint8_t _id);
int32_t bsp_GetRunTime(void);
int32_t bsp_CheckRunTime(int32_t _LastTime);

void bsp_InitHardTimer(void);
void bsp_StartHardTimer(uint8_t _CC, uint32_t _uiTimeOut, void *_pCallBack);

#endif //__BSP_TIMER_H
