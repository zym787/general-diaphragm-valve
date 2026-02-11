/*
 * 四路步进电机驱动库
 * 基于STM32F103C8 HAL库的CMake项目
 * 使用单一定时器(TIM2)驱动4路步进电机
 */

#include "main.h"
#include "tim.h"
#include "gpio.h"

#define MAX_MOTORS 4

// 步进电机控制结构体
typedef struct {
        GPIO_TypeDef *step_port;
        uint16_t step_pin;
        GPIO_TypeDef *dir_port;
        uint16_t dir_pin;
        int32_t target_pos;      // 目标位置
        int32_t current_pos;     // 当前位置
        int32_t remaining_steps; // 剩余步数
        uint8_t direction;       // 方向: 0=正转, 1=反转
        uint32_t delay_us;       // 当前脉冲间隔(微秒)
        uint32_t min_delay_us;   // 最小脉冲间隔(对应最高速度)
        uint32_t max_delay_us;   // 最大脉冲间隔(对应最低速度)
        uint8_t enabled;         // 使能状态
} stepper_motor_t;

// 电机实例数组
static stepper_motor_t motors[MAX_MOTORS];

// 全局变量
static TIM_HandleTypeDef *htim_step = NULL;
static uint8_t timer_initialized = 0;
static uint8_t active_motors = 0; // 正在运动的电机数量

// 内部函数声明
static void update_timer_period(void);
static void motor_step_handler(stepper_motor_t *motor);

/**
 * @brief 初始化步进电机驱动库
 * @param htim 定时器句柄，用于产生步进脉冲
 */
void stepper_init(TIM_HandleTypeDef *htim)
{
        htim_step = htim;
        timer_initialized = 1;

        // 初始化所有电机实例
        for (int i = 0; i < MAX_MOTORS; i++) {
                motors[i].step_port = NULL;
                motors[i].step_pin = 0;
                motors[i].dir_port = NULL;
                motors[i].dir_pin = 0;
                motors[i].target_pos = 0;
                motors[i].current_pos = 0;
                motors[i].remaining_steps = 0;
                motors[i].direction = 0;
                motors[i].delay_us = 5000;      // 默认5ms
                motors[i].min_delay_us = 100;   // 最高速度(100us间隔)
                motors[i].max_delay_us = 20000; // 最低速度(20ms间隔)
                motors[i].enabled = 0;
        }
}

/**
 * @brief 添加步进电机
 * @param index 电机索引(0-3)
 * @param step_port 步进信号GPIO端口
 * @param step_pin 步进信号GPIO引脚
 * @param dir_port 方向信号GPIO端口
 * @param dir_pin 方向信号GPIO引脚
 * @return 0成功，-1失败
 */
int8_t stepper_add_motor(uint8_t index, GPIO_TypeDef *step_port, uint16_t step_pin, GPIO_TypeDef *dir_port,
                         uint16_t dir_pin)
{
        if (index >= MAX_MOTORS || !timer_initialized) {
                return -1;
        }

        motors[index].step_port = step_port;
        motors[index].step_pin = step_pin;
        motors[index].dir_port = dir_port;
        motors[index].dir_pin = dir_pin;
        motors[index].enabled = 1;

        return 0;
}

/**
 * @brief 设置电机目标位置
 * @param index 电机索引
 * @param position 目标位置(步数)
 * @param speed 最大速度(步/秒)
 */
void stepper_set_target_position(uint8_t index, int32_t position, float speed)
{
        if (index >= MAX_MOTORS || !motors[index].enabled) {
                return;
        }

        motors[index].target_pos = position;
        motors[index].remaining_steps = position - motors[index].current_pos;

        // 根据速度计算脉冲间隔
        if (speed > 0) {
                float period = 1.0f / speed;                       // 秒/步
                uint32_t new_delay = (uint32_t)(period * 1000000); // 转换为微秒

                // 限制在最小最大延迟范围内
                if (new_delay < motors[index].min_delay_us) {
                        new_delay = motors[index].min_delay_us;
                } else if (new_delay > motors[index].max_delay_us) {
                        new_delay = motors[index].max_delay_us;
                }

                motors[index].delay_us = new_delay;
        }

        // 设置方向
        if (motors[index].remaining_steps >= 0) {
                motors[index].direction = 0; // 正转
                HAL_GPIO_WritePin(motors[index].dir_port, motors[index].dir_pin, GPIO_PIN_RESET);
        } else {
                motors[index].direction = 1; // 反转
                HAL_GPIO_WritePin(motors[index].dir_port, motors[index].dir_pin, GPIO_PIN_SET);
                motors[index].remaining_steps = -motors[index].remaining_steps;
        }

        // 启动定时器
        if (motors[index].remaining_steps > 0 && htim_step != NULL) {
                update_timer_period();
                if (HAL_TIM_Base_GetState(htim_step) == HAL_TIM_STATE_READY) {
                        HAL_TIM_Base_Start_IT(htim_step);
                }
        }
}

/**
 * @brief 设置电机速度范围
 * @param index 电机索引
 * @param min_speed 最小速度(步/秒)
 * @param max_speed 最大速度(步/秒)
 */
void stepper_set_speed_range(uint8_t index, float min_speed, float max_speed)
{
        if (index >= MAX_MOTORS || !motors[index].enabled) {
                return;
        }

        if (min_speed > 0) {
                motors[index].max_delay_us = (uint32_t)(1000000.0f / min_speed);
        }

        if (max_speed > 0) {
                motors[index].min_delay_us = (uint32_t)(1000000.0f / max_speed);
        }
}

/**
 * @brief 获取电机当前位置
 * @param index 电机索引
 * @return 当前位置(步数)
 */
int32_t stepper_get_position(uint8_t index)
{
        if (index >= MAX_MOTORS) {
                return 0;
        }
        return motors[index].current_pos;
}

/**
 * @brief 检查电机是否到达目标位置
 * @param index 电机索引
 * @return 1表示到达，0表示未到达
 */
uint8_t stepper_is_finished(uint8_t index)
{
        if (index >= MAX_MOTORS || !motors[index].enabled) {
                return 1;
        }
        return (motors[index].remaining_steps == 0);
}

/**
 * @brief 更新定时器周期以匹配最快电机的脉冲频率
 */
static void update_timer_period(void)
{
        uint32_t min_delay = UINT32_MAX;
        uint8_t active_count = 0;

        // 找出所有正在移动的电机中最快的脉冲间隔
        for (int i = 0; i < MAX_MOTORS; i++) {
                if (motors[i].enabled && motors[i].remaining_steps > 0) {
                        if (motors[i].delay_us < min_delay) {
                                min_delay = motors[i].delay_us;
                        }
                        active_count++;
                }
        }

        active_motors = active_count;

        if (active_count > 0 && min_delay != UINT32_MAX && htim_step != NULL) {
                // 将微秒转换为定时器计数值
                // 假设定时器时钟为72MHz，预分频器为71，则定时器频率为1MHz
                // 计数值 = 微秒数
                __HAL_TIM_SET_AUTORELOAD(htim_step, min_delay);

                // 设置预分频器使定时器频率为1MHz
                // 这样可以直接使用微秒值作为计数值
                if (htim_step->Instance == TIM2) {
                        htim_step->Init.Prescaler = 71; // 72MHz/72 = 1MHz
                        htim_step->Init.CounterMode = TIM_COUNTERMODE_UP;
                        htim_step->Init.Period = min_delay;
                        htim_step->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
                        htim_step->Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;

                        HAL_TIM_Base_Init(htim_step);
                }
        }
}

/**
 * @brief 单个电机步进处理
 * @param motor 电机指针
 */
static void motor_step_handler(stepper_motor_t *motor)
{
        if (motor->remaining_steps > 0) {
                // 产生步进脉冲
                HAL_GPIO_WritePin(motor->step_port, motor->step_pin, GPIO_PIN_SET);

                // 简单延时模拟脉冲宽度
                volatile uint32_t pulse_width = 2; // 约2us脉冲宽度
                while (pulse_width--)
                        ;

                HAL_GPIO_WritePin(motor->step_port, motor->step_pin, GPIO_PIN_RESET);

                // 更新位置
                if (motor->direction == 0) {
                        motor->current_pos++;
                } else {
                        motor->current_pos--;
                }

                motor->remaining_steps--;
        }
}

/**
 * @brief 定时器中断回调函数
 * 这个函数需要在stm32f1xx_it.c中调用
 */
void stepper_timer_callback(void)
{
        uint8_t any_active = 0;

        // 为每个活动电机执行一步
        for (int i = 0; i < MAX_MOTORS; i++) {
                if (motors[i].enabled && motors[i].remaining_steps > 0) {
                        motor_step_handler(&motors[i]);
                        any_active = 1;
                }
        }

        // 如果没有活动电机，停止定时器
        if (!any_active && htim_step != NULL) {
                HAL_TIM_Base_Stop_IT(htim_step);
        } else if (any_active) {
                // 更新定时器周期以适应剩余活动电机
                update_timer_period();
        }
}

/**
 * @brief 停止所有电机
 */
void stepper_stop_all(void)
{
        for (int i = 0; i < MAX_MOTORS; i++) {
                motors[i].remaining_steps = 0;
        }

        if (htim_step != NULL) {
                HAL_TIM_Base_Stop_IT(htim_step);
        }
}