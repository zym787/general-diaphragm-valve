#ifndef __STEPPER_MOTOR_H
#define __STEPPER_MOTOR_H

#include "stm32f1xx_hal.h"
#include <stdbool.h>
#include <stdint.h>

// 电机状态枚举
typedef enum {
        STEPPER_STOPPED,
        STEPPER_ACCELERATING,
        STEPPER_RUNNING,
        STEPPER_DECELERATING,
        STEPPER_HOMING
} StepperState_t;

// 运动模式枚举
typedef enum {
        SPEED_MODE,    // 速度模式
        POSITION_MODE, // 位置模式
        HOMING_MODE    // 回零模式
} MotionMode_t;

// 加减速曲线类型
typedef enum {
        LINEAR_RAMP, // 线性加减速
        S_CURVE,     // S型曲线
        EXPONENTIAL  // 指数曲线
} AccelProfile_t;

// 电机配置结构体
typedef struct {
        uint32_t steps_per_rev;   // 每转步数
        float max_speed;          // 最大速度 (步/秒)
        float acceleration;       // 加速度 (步/秒²)
        float deceleration;       // 减速度 (步/秒²)
        float jerk;               // 加加速度 (用于S曲线)
        uint32_t min_pulse_width; // 最小脉冲宽度 (us)
        uint32_t max_pulse_width; // 最大脉冲宽度 (us)
        bool enable_active_low;   // 使能信号低有效
        bool dir_cw_level;        // 顺时针方向电平
} StepperConfig_t;

// 电机控制结构体
typedef struct {
        // 硬件相关
        TIM_HandleTypeDef *pulse_timer; // 脉冲定时器
        uint32_t pulse_channel;         // 脉冲通道
        GPIO_TypeDef *dir_port;         // 方向端口
        uint16_t dir_pin;               // 方向引脚
        GPIO_TypeDef *en_port;          // 使能端口
        uint16_t en_pin;                // 使能引脚

        // 运动参数
        volatile int32_t current_pos; // 当前位置 (步)
        volatile int32_t target_pos;  // 目标位置 (步)
        volatile float current_speed; // 当前速度 (步/秒)
        volatile float target_speed;  // 目标速度 (步/秒)
        volatile float set_speed;     // 设定速度 (步/秒)

        // 状态变量
        volatile StepperState_t state;
        volatile MotionMode_t mode;
        volatile bool is_enabled;
        volatile bool is_moving;

        // 配置参数
        StepperConfig_t config;
        AccelProfile_t accel_profile;

        // 计算变量
        float step_delay_us;         // 步进延迟 (微秒)
        uint32_t timer_arr;          // 定时器ARR值
        uint32_t timer_psc;          // 定时器预分频
        uint32_t step_counter;       // 步进计数器
        uint32_t accel_step_counter; // 加减速步数计数器

        // 加减速计算
        float accel_increment; // 加速度增量
        float decel_distance;  // 减速距离
        float accel_until;     // 加速到何时
        float decel_after;     // 何时开始减速
        float nominal_speed;   // 名义速度

        // 回调函数
        void (*position_reached_cb)(void);    // 位置到达回调
        void (*error_cb)(uint8_t error_code); // 错误回调
} StepperMotor_t;

// 错误代码定义
#define STEPPER_ERROR_NONE         0
#define STEPPER_ERROR_OVERRUN      1
#define STEPPER_ERROR_OVERHEAT     2
#define STEPPER_ERROR_STALL        3
#define STEPPER_ERROR_LIMIT_SWITCH 4

// 函数声明
void Stepper_Init(StepperMotor_t *motor, TIM_HandleTypeDef *timer, uint32_t channel, GPIO_TypeDef *dir_port,
                  uint16_t dir_pin, GPIO_TypeDef *en_port, uint16_t en_pin);

void Stepper_Config(StepperMotor_t *motor, StepperConfig_t *config);
void Stepper_Enable(StepperMotor_t *motor);
void Stepper_Disable(StepperMotor_t *motor);

// 位置控制
void Stepper_MoveTo(StepperMotor_t *motor, int32_t position);
void Stepper_Move(StepperMotor_t *motor, int32_t steps);
void Stepper_SetPosition(StepperMotor_t *motor, int32_t position);

// 速度控制
void Stepper_SetSpeed(StepperMotor_t *motor, float speed);
void Stepper_Run(StepperMotor_t *motor, bool direction);
void Stepper_Stop(StepperMotor_t *motor);
void Stepper_StopImmediately(StepperMotor_t *motor);

// 运动控制
void Stepper_SetAcceleration(StepperMotor_t *motor, float acceleration);
void Stepper_SetDeceleration(StepperMotor_t *motor, float deceleration);
void Stepper_SetAccelProfile(StepperMotor_t *motor, AccelProfile_t profile);

// 状态获取
int32_t Stepper_GetPosition(StepperMotor_t *motor);
float Stepper_GetSpeed(StepperMotor_t *motor);
bool Stepper_IsMoving(StepperMotor_t *motor);
StepperState_t Stepper_GetState(StepperMotor_t *motor);

// 中断处理
void Stepper_TIM_UpdateCallback(TIM_HandleTypeDef *htim);
void Stepper_Process(StepperMotor_t *motor);

#endif /* __STEPPER_MOTOR_H */
