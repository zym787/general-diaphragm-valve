#include "stepper_motor.h"
#include <math.h>
#include "main.h"

// 内部函数声明
static void CalculateSpeedProfile(StepperMotor_t *motor);
static void UpdateTimerFrequency(StepperMotor_t *motor, float speed);
static float CalculateStepDelay(float speed);
static void GeneratePulse(StepperMotor_t *motor);
static void HandleAcceleration(StepperMotor_t *motor);
static void HandleDeceleration(StepperMotor_t *motor);
static void ApplyScurveAcceleration(StepperMotor_t *motor);

// 全局电机实例指针（支持多电机）
static StepperMotor_t *motor_instances[4] = {NULL};
static uint8_t motor_count = 0;

/**
 * @brief  初始化步进电机
 * @param  motor: 电机结构体指针
 * @param  timer: 定时器句柄
 * @param  channel: 定时器通道
 * @param  dir_port: 方向端口
 * @param  dir_pin: 方向引脚
 * @param  en_port: 使能端口
 * @param  en_pin: 使能引脚
 */
void Stepper_Init(StepperMotor_t *motor, TIM_HandleTypeDef *timer, uint32_t channel, GPIO_TypeDef *dir_port,
                  uint16_t dir_pin, GPIO_TypeDef *en_port, uint16_t en_pin)
{
        // 初始化硬件参数
        motor->pulse_timer = timer;
        motor->pulse_channel = channel;
        motor->dir_port = dir_port;
        motor->dir_pin = dir_pin;
        motor->en_port = en_port;
        motor->en_pin = en_pin;

        // 初始化默认参数
        motor->current_pos = 0;
        motor->target_pos = 0;
        motor->current_speed = 0;
        motor->target_speed = 0;
        motor->set_speed = 100; // 默认100步/秒

        // 初始化状态
        motor->state = STEPPER_STOPPED;
        motor->mode = POSITION_MODE;
        motor->is_enabled = false;
        motor->is_moving = false;

        // 初始化默认配置
        motor->config.steps_per_rev = 200;    // 1.8度步进角
        motor->config.max_speed = 1000;       // 最大1000步/秒
        motor->config.acceleration = 500;     // 加速度500步/秒²
        motor->config.deceleration = 500;     // 减速度500步/秒²
        motor->config.jerk = 10000;           // 加加速度
        motor->config.min_pulse_width = 5;    // 5us最小脉宽
        motor->config.max_pulse_width = 1000; // 1ms最大脉宽
        motor->config.enable_active_low = true;
        motor->config.dir_cw_level = GPIO_PIN_SET;

        motor->accel_profile = LINEAR_RAMP;

        // 初始化回调函数
        motor->position_reached_cb = NULL;
        motor->error_cb = NULL;

        // 添加到全局实例数组
        if (motor_count < 4) {
                motor_instances[motor_count] = motor;
                motor_count++;
        }

        // 禁用电机
        Stepper_Disable(motor);

        // 配置方向引脚
        HAL_GPIO_WritePin(dir_port, dir_pin, GPIO_PIN_RESET);
}

/**
 * @brief  配置电机参数
 */
void Stepper_Config(StepperMotor_t *motor, StepperConfig_t *config)
{
        motor->config = *config;
}

/**
 * @brief  使能电机
 */
void Stepper_Enable(StepperMotor_t *motor)
{
        if (motor->config.enable_active_low) {
                HAL_GPIO_WritePin(motor->en_port, motor->en_pin, GPIO_PIN_RESET);
        } else {
                HAL_GPIO_WritePin(motor->en_port, motor->en_pin, GPIO_PIN_SET);
        }
        motor->is_enabled = true;

        // 等待电机稳定
        HAL_Delay(100);
}

/**
 * @brief  禁用电机
 */
void Stepper_Disable(StepperMotor_t *motor)
{
        if (motor->config.enable_active_low) {
                HAL_GPIO_WritePin(motor->en_port, motor->en_pin, GPIO_PIN_SET);
        } else {
                HAL_GPIO_WritePin(motor->en_port, motor->en_pin, GPIO_PIN_RESET);
        }
        motor->is_enabled = false;

        // 停止定时器
        HAL_TIM_Base_Stop_IT(motor->pulse_timer);
        HAL_TIM_PWM_Stop(motor->pulse_timer, motor->pulse_channel);
        motor->is_moving = false;
        motor->state = STEPPER_STOPPED;
}

/**
 * @brief  移动到绝对位置
 */
void Stepper_MoveTo(StepperMotor_t *motor, int32_t position)
{
        if (!motor->is_enabled)
                return;

        motor->target_pos = position;
        motor->mode = POSITION_MODE;

        // 确定方向
        bool direction = (position >= motor->current_pos);
        HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin,
                          direction ? motor->config.dir_cw_level : !motor->config.dir_cw_level);

        // 计算加减速参数
        CalculateSpeedProfile(motor);

        // 启动运动
        motor->state = STEPPER_ACCELERATING;
        motor->is_moving = true;

        // 设置初始速度
        motor->current_speed = 0;
        UpdateTimerFrequency(motor, motor->current_speed);

        // 启动定时器
        // HAL_TIM_Base_Start_IT(motor->pulse_timer);
        HAL_TIM_PWM_Start(motor->pulse_timer, motor->pulse_channel);
}

/**
 * @brief  相对移动
 */
void Stepper_Move(StepperMotor_t *motor, int32_t steps)
{
        Stepper_MoveTo(motor, motor->current_pos + steps);
}

/**
 * @brief  设置当前位置
 */
void Stepper_SetPosition(StepperMotor_t *motor, int32_t position)
{
        motor->current_pos = position;
}

/**
 * @brief  设置运行速度
 */
void Stepper_SetSpeed(StepperMotor_t *motor, float speed)
{
        if (speed > motor->config.max_speed) {
                speed = motor->config.max_speed;
        }

        motor->set_speed = speed;

        // 如果正在运行，更新速度
        if (motor->is_moving) {
                CalculateSpeedProfile(motor);
        }
}

/**
 * @brief  速度模式运行
 */
void Stepper_Run(StepperMotor_t *motor, bool direction)
{
        if (!motor->is_enabled)
                return;

        motor->mode = SPEED_MODE;
        motor->target_speed = motor->set_speed;

        // 设置方向
        HAL_GPIO_WritePin(motor->dir_port, motor->dir_pin,
                          direction ? motor->config.dir_cw_level : !motor->config.dir_cw_level);

        // 启动加速
        motor->state = STEPPER_ACCELERATING;
        motor->is_moving = true;
        motor->current_speed = 0;

        UpdateTimerFrequency(motor, motor->current_speed);
        // HAL_TIM_Base_Start_IT(motor->pulse_timer);
        HAL_TIM_PWM_Start(motor->pulse_timer, motor->pulse_channel);
}

/**
 * @brief  平滑停止
 */
void Stepper_Stop(StepperMotor_t *motor)
{
        if (!motor->is_moving || motor->state == STEPPER_STOPPED)
                return;

        if (motor->mode == POSITION_MODE) {
                // 位置模式：直接跳到减速阶段
                if (motor->state == STEPPER_ACCELERATING || motor->state == STEPPER_RUNNING) {
                        motor->state = STEPPER_DECELERATING;
                        motor->decel_after = motor->current_pos;
                }
        } else {
                // 速度模式：减速到停止
                motor->mode = POSITION_MODE;
                motor->target_pos = motor->current_pos;
                motor->state = STEPPER_DECELERATING;
                CalculateSpeedProfile(motor);
        }
}

/**
 * @brief  立即停止
 */
void Stepper_StopImmediately(StepperMotor_t *motor)
{
        HAL_TIM_Base_Stop_IT(motor->pulse_timer);
        HAL_TIM_PWM_Stop(motor->pulse_timer, motor->pulse_channel);
        motor->is_moving = false;
        motor->state = STEPPER_STOPPED;
        motor->current_speed = 0;
}

/**
 * @brief  设置加速度
 */
void Stepper_SetAcceleration(StepperMotor_t *motor, float acceleration)
{
        motor->config.acceleration = acceleration;
        if (motor->is_moving) {
                CalculateSpeedProfile(motor);
        }
}

/**
 * @brief  设置减速度
 */
void Stepper_SetDeceleration(StepperMotor_t *motor, float deceleration)
{
        motor->config.deceleration = deceleration;
        if (motor->is_moving) {
                CalculateSpeedProfile(motor);
        }
}

/**
 * @brief  设置加减速曲线
 */
void Stepper_SetAccelProfile(StepperMotor_t *motor, AccelProfile_t profile)
{
        motor->accel_profile = profile;
}

/**
 * @brief  获取当前位置
 */
int32_t Stepper_GetPosition(StepperMotor_t *motor)
{
        return motor->current_pos;
}

/**
 * @brief  获取当前速度
 */
float Stepper_GetSpeed(StepperMotor_t *motor)
{
        return motor->current_speed;
}

/**
 * @brief  检查是否在运动
 */
bool Stepper_IsMoving(StepperMotor_t *motor)
{
        return motor->is_moving;
}

/**
 * @brief  获取电机状态
 */
StepperState_t Stepper_GetState(StepperMotor_t *motor)
{
        return motor->state;
}

/**
 * @brief  计算速度曲线（梯形加减速）
 */
static void CalculateSpeedProfile(StepperMotor_t *motor)
{
        int32_t distance = abs(motor->target_pos - motor->current_pos);

        // 计算加速到设定速度所需的距离
        float accel_distance = (motor->set_speed * motor->set_speed) / (2.0f * motor->config.acceleration);
        float decel_distance = (motor->set_speed * motor->set_speed) / (2.0f * motor->config.deceleration);

        // 总加减速距离
        float total_accel_decel_distance = accel_distance + decel_distance;

        if (distance <= total_accel_decel_distance) {
                // 距离不足以加速到设定速度
                motor->nominal_speed = sqrt((distance * motor->config.acceleration * motor->config.deceleration) /
                                            (motor->config.acceleration + motor->config.deceleration));

                motor->accel_until =
                    distance * motor->config.deceleration / (motor->config.acceleration + motor->config.deceleration);
                motor->decel_after = distance - motor->accel_until;
        } else {
                // 可以加速到设定速度
                motor->nominal_speed = motor->set_speed;
                motor->accel_until = accel_distance;
                motor->decel_after = distance - decel_distance;
        }

        // 计算加速度增量
        motor->accel_increment = motor->config.acceleration / motor->config.max_speed;
}

/**
 * @brief  更新定时器频率
 */
static void UpdateTimerFrequency(StepperMotor_t *motor, float speed)
{
        if (speed < 1.0f)
                speed = 1.0f;
        if (speed > motor->config.max_speed)
                speed = motor->config.max_speed;

        // 计算步进延迟（微秒）
        float delay_us = 1000000.0f / speed;

        // 限制脉冲宽度
        if (delay_us < motor->config.min_pulse_width) {
                delay_us = motor->config.min_pulse_width;
        } else if (delay_us > motor->config.max_pulse_width) {
                delay_us = motor->config.max_pulse_width;
        }

        // 计算定时器参数
        uint32_t timer_freq = HAL_RCC_GetPCLK1Freq() * 2; // TIMx时钟频率
        // if (motor->pulse_timer->Instance == TIM1 || motor->pulse_timer->Instance == TIM8) {
        if (motor->pulse_timer->Instance == TIM1) {
                timer_freq = HAL_RCC_GetPCLK2Freq() * 2;
        }

        // 计算ARR和PSC
        uint32_t cycles = (uint32_t)(delay_us * timer_freq / 1000000.0f);

        // 确保最小周期
        if (cycles < 10)
                cycles = 10;

        // 简单处理：设置PSC=0，ARR=cycles
        motor->pulse_timer->Instance->PSC = 0;
        motor->pulse_timer->Instance->ARR = cycles;
        motor->pulse_timer->Instance->CCR1 = cycles / 2; // 50%占空比
}

/**
 * @brief  定时器更新中断回调
 * @note   需要在stm32f1xx_it.c的TIMx_IRQHandler中调用
 */
void Stepper_TIM_UpdateCallback(TIM_HandleTypeDef *htim)
{
        for (int i = 0; i < motor_count; i++) {
                if (motor_instances[i]->pulse_timer->Instance == htim->Instance) {
                        Stepper_Process(motor_instances[i]);
                        break;
                }
        }
}

/**
 * @brief  步进电机处理函数（在定时器中断中调用）
 */
void Stepper_Process(StepperMotor_t *motor)
{
        if (!motor->is_moving)
                return;

        // 生成脉冲
        GeneratePulse(motor);

        // 更新位置
        bool direction = (HAL_GPIO_ReadPin(motor->dir_port, motor->dir_pin) == motor->config.dir_cw_level);
        motor->current_pos += direction ? 1 : -1;

        // 检查位置模式是否到达目标
        if (motor->mode == POSITION_MODE) {
                int32_t distance = motor->target_pos - motor->current_pos;

                if (distance == 0) {
                        // 到达目标位置
                        motor->is_moving = false;
                        motor->state = STEPPER_STOPPED;
                        motor->current_speed = 0;

                        // 停止定时器
                        HAL_TIM_Base_Stop_IT(motor->pulse_timer);
                        HAL_TIM_PWM_Stop(motor->pulse_timer, motor->pulse_channel);

                        // 调用回调函数
                        if (motor->position_reached_cb != NULL) {
                                motor->position_reached_cb();
                        }
                        return;
                }
        }

        // 处理加减速
        switch (motor->state) {
        case STEPPER_ACCELERATING:
                HandleAcceleration(motor);
                break;

        case STEPPER_RUNNING:
                // 匀速运行，无需处理
                break;

        case STEPPER_DECELERATING:
                HandleDeceleration(motor);
                break;

        default:
                break;
        }
}

/**
 * @brief  处理加速阶段
 */
static void HandleAcceleration(StepperMotor_t *motor)
{
        // 根据加减速曲线类型处理
        switch (motor->accel_profile) {
        case LINEAR_RAMP:
                motor->current_speed += motor->config.acceleration / motor->config.max_speed;
                break;

        case S_CURVE:
                ApplyScurveAcceleration(motor);
                break;

        case EXPONENTIAL:
                // 指数加速
                motor->current_speed *= 1.1f;
                break;
        }

        // 限制最大速度
        if (motor->current_speed > motor->nominal_speed) {
                motor->current_speed = motor->nominal_speed;
        }

        // 检查是否进入匀速阶段
        if (motor->mode == POSITION_MODE) {
                float distance_traveled = fabs(motor->current_pos - (motor->target_pos > motor->current_pos
                                                                         ? motor->target_pos - motor->accel_until
                                                                         : motor->target_pos + motor->accel_until));

                if (distance_traveled >= motor->accel_until ||
                    fabs(motor->current_speed - motor->nominal_speed) < 0.1f) {
                        motor->state = STEPPER_RUNNING;
                }
        } else if (fabs(motor->current_speed - motor->target_speed) < 0.1f) {
                motor->state = STEPPER_RUNNING;
        }

        // 更新定时器频率
        UpdateTimerFrequency(motor, motor->current_speed);
}

/**
 * @brief  处理减速阶段
 */
static void HandleDeceleration(StepperMotor_t *motor)
{
        // 根据加减速曲线类型处理
        switch (motor->accel_profile) {
        case LINEAR_RAMP:
                motor->current_speed -= motor->config.deceleration / motor->config.max_speed;
                break;

        case S_CURVE:
                // S曲线减速
                break;

        case EXPONENTIAL:
                // 指数减速
                motor->current_speed *= 0.9f;
                break;
        }

        // 确保速度不为负
        if (motor->current_speed < 1.0f) {
                motor->current_speed = 0;
        }

        // 更新定时器频率
        UpdateTimerFrequency(motor, motor->current_speed);
}

/**
 * @brief  应用S曲线加减速
 */
static void ApplyScurveAcceleration(StepperMotor_t *motor)
{
        // 简化的S曲线实现
        static float jerk_integral = 0;

        // 根据加加速度调整加速度
        float accel_change = motor->config.jerk / motor->config.max_speed;

        // 更新加速度
        motor->accel_increment += accel_change;

        // 限制加速度变化率
        if (motor->accel_increment > motor->config.acceleration) {
                motor->accel_increment = motor->config.acceleration;
        }

        // 更新速度
        motor->current_speed += motor->accel_increment;
}

/**
 * @brief  生成脉冲
 */
static void GeneratePulse(StepperMotor_t *motor)
{
        // 使用PWM模式生成脉冲，由硬件自动处理
        // 这里只需要确保定时器运行即可
        HAL_GPIO_TogglePin(LED2_R_GPIO_Port, LED2_R_Pin);
}