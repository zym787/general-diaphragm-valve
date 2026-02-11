/*
 * stepper_lib.h
 * 步进电机驱动库头文件
 */

#ifndef STEPPER_LIB_H
#define STEPPER_LIB_H

#include "main.h"
#include <stdint.h>
#include <stdlib.h>

// 最大电机数量
#define MAX_MOTORS 4

// 电机控制结构体
typedef struct {
        GPIO_TypeDef *step_port;   // 步进脉冲GPIO端口
        uint16_t step_pin;         // 步进脉冲GPIO引脚
        GPIO_TypeDef *dir_port;    // 方向控制GPIO端口
        uint16_t dir_pin;          // 方向控制GPIO引脚
        GPIO_TypeDef *enable_port; // 使能控制GPIO端口
        uint16_t enable_pin;       // 使能控制GPIO引脚
        int32_t target_pos;        // 目标位置
        int32_t current_pos;       // 当前位置
        int32_t remaining_steps;   // 剩余步数
        uint8_t direction;         // 方向: 0=正转, 1=反转
        uint32_t delay_us;         // 当前脉冲间隔(微秒)
        uint32_t min_delay_us;     // 最小脉冲间隔(对应最高速度)
        uint32_t max_delay_us;     // 最大脉冲间隔(对应最低速度)
        uint8_t enabled;           // 使能状态
        uint8_t en_active_high;    // 使能信号有效电平: 1=高电平有效, 0=低电平有效
} stepper_motor_t;

// 函数声明
void stepper_init(TIM_HandleTypeDef *htim);
int8_t stepper_add_motor(uint8_t index, GPIO_TypeDef *step_port, uint16_t step_pin, GPIO_TypeDef *dir_port,
                         uint16_t dir_pin, GPIO_TypeDef *enable_port, uint16_t enable_pin, uint8_t en_active_high);
void stepper_set_target_position(uint8_t index, int32_t position, float speed);
void stepper_set_speed_range(uint8_t index, float min_speed, float max_speed);
int32_t stepper_get_position(uint8_t index);
uint8_t stepper_is_finished(uint8_t index);
void stepper_enable_motor(uint8_t index, uint8_t enable);
void stepper_set_direction(uint8_t index, uint8_t direction);
void stepper_stop_all(void);
void stepper_timer_callback(void);

#endif /* STEPPER_LIB_H */
