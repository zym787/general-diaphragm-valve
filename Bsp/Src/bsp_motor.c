#include "bsp.h"

#include "tim.h"
#include "stepper_motor.h"

// 定义步进电机实例
StepperMotor_t myStepper;

// 位置到达回调函数
void PositionReachedCallback(void)
{
        bsp_println("目标位置已到达!");
}

// 错误回调函数
void ErrorCallback(uint8_t error_code)
{
       bsp_println("电机错误：%d", error_code);
}


void bsp_MotorInit(void)
{
        // 初始化步进电机
        Stepper_Init(&myStepper, &htim4, TIM_CHANNEL_4, DIR1_GPIO_Port, DIR1_Pin, // 方向引脚
                     ENA1_GPIO_Port, ENA1_Pin);                                   // 使能引脚

        // 配置电机参数
        StepperConfig_t config = {.steps_per_rev = 200,
                                  .max_speed = 800,    // 步/秒
                                  .acceleration = 300, // 步/秒²
                                  .deceleration = 300,
                                  .jerk = 5000,
                                  .min_pulse_width = 5,
                                  .max_pulse_width = 1000,
                                  .enable_active_low = true,
                                  .dir_cw_level = GPIO_PIN_SET};
        Stepper_Config(&myStepper, &config);

        // 设置回调函数
        myStepper.position_reached_cb = PositionReachedCallback;
        myStepper.error_cb = ErrorCallback;
        // 使能电机
        Stepper_Enable(&myStepper);
}

void bsp_MotorTask(void)
{
        // HAL_TIM_PWM_Start(&htim4, TIM_CHANNEL_4);
        // HAL_Delay(1000);
        // HAL_TIM_PWM_Stop(&htim4, TIM_CHANNEL_4);
        // return;
        bsp_MotorControl();
}

void bsp_MotorControl(void)
{
        uint8_t cmd;
        /* 从串口读入一个字符(非阻塞方式) */
        if (comGetChar(COM1, &cmd)) {
                switch (cmd) {
                case '1':
                        BSP_Printf("\r\n【1 - 移动到位置1000步】\r\n");
                        // 示例1：位置控制
                        bsp_println("1 移动到位置1000步\r\n");
                        Stepper_MoveTo(&myStepper, 1000);
                        while (Stepper_IsMoving(&myStepper)) {
                                // 等待移动完成
                        }

                        break;

                case '2':
                        BSP_Printf("\r\n【2 - 反向移动500步】\r\n");
                        // 示例2：相对移动
                        bsp_println("2 反向移动500步\r\n");
                        Stepper_Move(&myStepper, -500);
                        while (Stepper_IsMoving(&myStepper)) {
                                // 显示当前位置
                                bsp_println("当前位置：%ld\r\n", Stepper_GetPosition(&myStepper));
                                HAL_Delay(100);
                        }
                        break;

                case '3':
                        BSP_Printf("\r\n【3 - 速度模式运行】\r\n");
                        // 示例3：速度控制
                        bsp_println("3 速度模式运行\r\n");
                        Stepper_SetSpeed(&myStepper, 300); // 300步/秒
                        Stepper_Run(&myStepper, true);     // 顺时针运行

                        // 运行5秒后停止
                        HAL_Delay(5000);
                        Stepper_Stop(&myStepper);
                        break;

                case '4':
                        BSP_Printf("\r\n【4 - S曲线加减速移动到位置2000】\r\n");
                        // 示例4：设置不同加减速曲线
                        Stepper_SetAccelProfile(&myStepper, S_CURVE);
                        Stepper_SetAcceleration(&myStepper, 200);
                        Stepper_SetDeceleration(&myStepper, 200);

                        bsp_println("4 S曲线加减速移动到位置2000\r\n");
                        Stepper_MoveTo(&myStepper, 2000);
                        while (Stepper_IsMoving(&myStepper)) {
                                bsp_println("速度：%.1f 步/秒\r\n", Stepper_GetSpeed(&myStepper));
                                HAL_Delay(300);
                        }
                        break;

                default:
                        BSP_Printf("\r\n------------------------------------------------\r\n");
                        BSP_Printf("请选择操作命令:\r\n");
                        BSP_Printf("1 - 移动到位置1000步\r\n");
                        BSP_Printf("2 - 反向移动500步\r\n");
                        BSP_Printf("3 - 速度模式运行\r\n");
                        BSP_Printf("4 - S曲线加减速移动到位置2000\r\n");
                        break;
                }
        }
}