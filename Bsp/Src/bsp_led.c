/**
 * @file      : bsp_led.c
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

#include "bsp.h"

#define LED_NUM 1 /* led个数 */

/* 使能GPIO时钟 */
#define ALL_LED_GPIO_CLK_ENABLE()             \
        {                                     \
                __HAL_RCC_GPIOB_CLK_ENABLE(); \
                __HAL_RCC_GPIOC_CLK_ENABLE(); \
        };

/* 依次定义GPIO */
typedef struct {
        GPIO_TypeDef* gpio;
        uint16_t pin;
        uint8_t ActiveLevel; /* 激活电平 */
} X_GPIO_T;

/* GPIO和PIN定义 */
static const X_GPIO_T s_gpio_list[LED_NUM] = {
#if PCB_VERSION == A12_909_A3
    {BO_909_GPIO_Port, BO_909_Pin, 0}, /* BO PB13 */
#endif

    {0, 0, 0}};

/*
*********************************************************************************************************
*	函 数 名: bsp_LedInit
*	功能说明: 配置LED对应的GPIO
*	形    参:  无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_LedInit(void)
{
        GPIO_InitTypeDef gpio_init;
        uint8_t i;

        /* 第1步：打开GPIO时钟 */
        ALL_LED_GPIO_CLK_ENABLE();

        /* 第2步：配置所有的按键GPIO为推挽输出模式 */
        gpio_init.Mode = GPIO_MODE_OUTPUT_PP;  /* 设置推挽输出 */
        gpio_init.Pull = GPIO_NOPULL;          /* 上下拉电阻不使能 */
        gpio_init.Speed = GPIO_SPEED_FREQ_LOW; /* GPIO速度等级 */

        for (i = 0; i < LED_NUM; i++) {
                gpio_init.Pin = s_gpio_list[i].pin;
                HAL_GPIO_WritePin(s_gpio_list[i].gpio, s_gpio_list[i].pin, s_gpio_list[i].ActiveLevel);
                HAL_GPIO_Init(s_gpio_list[i].gpio, &gpio_init);
        }
}

/**
 * @brief     : 打开指定LED
 * @param     : _index LED索引
 */
void bsp_LedOn(const uint32_t _index)
{
#if PCB_VERSION == A12_901_E2
        if (_index == LED_GREEN) {
                HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, LED_ON);
        } else if (_index == BO_909) {
                HAL_GPIO_WritePin(BO_909_GPIO_Port, BO_909_Pin, LED_ON);
        }
#elif PCB_VERSION == A12_929_A || PCB_VERSION == A12_927_A
        if (_index == LED_RED) {
                HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, LED_ON);
        } else if (_index == LED_GREEN) {
                HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, LED_ON);
        }
#endif
        else {
                return;
        }
}

/**
 * @brief     : 关闭指定LED
 * @param     : _index LED索引
 */
void bsp_LedOff(const uint32_t _index)
{
#if PCB_VERSION == A12_901_E2
        if (_index == LED_GREEN) {
                HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, LED_OFF);
        } else if (_index == BO_909) {
                HAL_GPIO_WritePin(BO_909_GPIO_Port, BO_909_Pin, LED_OFF);
        }
#elif PCB_VERSION == A12_929_A || PCB_VERSION == A12_927_A
        if (_index == LED_RED) {
                HAL_GPIO_WritePin(LED_RED_PORT, LED_RED_PIN, LED_OFF);
        } else if (_index == LED_GREEN) {
                HAL_GPIO_WritePin(LED_GREEN_PORT, LED_GREEN_PIN, LED_OFF);
        }
#endif
        else {
                return;
        }
}

/**
 * @brief     : 查询指定LED是否开启
 * @param     : _index LED索引
 * @return    : 1 开启
 *              0 关闭
 */
uint8_t bsp_IsLedOn(const uint32_t _index)
{
#if PCB_VERSION == A12_901_E2
        if (_index == LED_GREEN) {
                return HAL_GPIO_ReadPin(LED_GREEN_PORT, LED_GREEN_PIN);
        } else if (_index == BO_909) {
                return HAL_GPIO_ReadPin(BO_909_GPIO_Port, BO_909_Pin);
        }
#elif PCB_VERSION == A12_929_A || PCB_VERSION == A12_927_A
        if (_index == LED_RED) {
                return HAL_GPIO_ReadPin(LED_RED_PORT, LED_RED_PIN);
        } else if (_index == LED_GREEN) {
                return HAL_GPIO_ReadPin(LED_GREEN_PORT, LED_GREEN_PIN);
        }
#endif
        else {
                return LED_ERROR;
        }
}

/**
 * @brief     : 切换指定LED状态
 * @param     : _index LED索引
 */
void bsp_LedToggle(const uint32_t _index)
{
        if (LED_ON == bsp_IsLedOn(_index)) {
                bsp_LedOff(_index);
        } else if (LED_OFF == bsp_IsLedOn(_index)) {
                bsp_LedOn(_index);
        } else {
                return;
        }
}

#if PCB_VERSION == A12_901_E2
void bsp_LedToggleFlag(void) { bsp_LedToggle(LED2_G); }
#endif

/**
 * @brief     : LED任务
 * @details   : 在loop.c里周期执行,用于闪烁LED
 */
void bsp_LedTask(void) {}
