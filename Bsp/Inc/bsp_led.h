/**
 * @file      : bsp_led.h
 * @brief     : LED驱动
 *
 * @version   : 1.3
 * @author    : Drinkto
 * @date      : Dec 24, 2025
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Dec 23, 2025 | 1.0 | Drinkto | 初始化LED |
 * | Dec 24, 2025 | 1.1 | Drinkto | 添加数码管驱动 |
 * | Jan 06, 2026 | 1.2 | Drinkto | 支持901电路板 |
 * | Jan 07, 2026 | 1.3 | Drinkto | 支持909电路板,添加测试引脚 |
 */

#ifndef __BSP_LED_H
#define __BSP_LED_H

// 引用bsp端口定义
#include "bsp_port.h"

#if PCB_VERSION == A12_901_E2
/// A12_901_E2
#define LED_GREEN           LED1_Pin /* Green绿色 运行指示灯 */
#define LED_GREEN_PIN       LED1_Pin
#define LED_GREEN_PORT      LED1_GPIO_Port
/// A12_909
#define BO_909           BO_909_Pin /* 909电路板BO PB13 */
#define BO_909_Pin       GPIO_PIN_13
#define BO_909_GPIO_Port GPIOB
#elif PCB_VERSION == A12_929_A
/// A12_929_A HHH-001-A
#define LED_RED        LED1_R_Pin /* Red红色   暂停指示灯 */
#define LED_RED_PIN    LED1_R_Pin
#define LED_RED_PORT   LED1_R_GPIO_Port
#define LED_GREEN      LED2_G_Pin /* Green绿色 运行指示灯 */
#define LED_GREEN_PIN  LED2_G_Pin
#define LED_GREEN_PORT LED2_G_GPIO_Port
#elif PCB_VERSION == A12_927_A
#define LED_RED        LED2_R_Pin /* Red红色   暂停指示灯 */
#define LED_RED_PIN    LED2_R_Pin
#define LED_RED_PORT   LED2_R_GPIO_Port
#define LED_GREEN      LED1_G_Pin /* Green绿色 运行指示灯 */
#define LED_GREEN_PIN  LED1_G_Pin
#define LED_GREEN_PORT LED1_G_GPIO_Port
#endif

/// LED状态
#define LED_ON    GPIO_PIN_RESET /* LED开启状态 */
#define LED_OFF   GPIO_PIN_SET   /* LED关闭状态 */
#define LED_ERROR 0xFF

typedef enum { STATUS_OFF = 0U, STATUS_ON } STATUS_E;

/// LED操作函数
extern void bsp_LedInit(void);
extern void bsp_LedOn(uint32_t _index);
extern void bsp_LedOff(uint32_t _index);
extern uint8_t bsp_IsLedOn(uint32_t _index);
extern void bsp_LedToggle(uint32_t _index);
extern void bsp_LedToggleFlag(void);

extern void bsp_LedTask(void);

#endif  //__BSP_LED_H
