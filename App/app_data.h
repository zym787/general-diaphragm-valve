#ifndef APP_DATA_H
#define APP_DATA_H

#include <stdbool.h>
#include <stdint.h>

/* 电缸参数 */
typedef struct {
        uint8_t motor_enable;     // 电机使能（0：禁用，1：启用）
        uint8_t motor_dir;        // 电机方向（0：正转，1：反转）
        uint32_t motor_speed;     // 电机转速（单位：rpm）
        uint32_t motor_acc;       // 电机加速度（单位：rpm/s）
        uint32_t motor_position;  // 电机位置（单位：step，32位）
} Motor_Status_T;

// 实际业务数据结构体（统一管理所有实际值，便于维护）
typedef struct {
        /* 通信参数 */
        uint8_t mb_baud;
        uint8_t mb_addr;

        /* 电缸参数 */
        Motor_Status_T motor1;
        Motor_Status_T motor2;
        Motor_Status_T motor3;
        Motor_Status_T motor4;
        // 模拟量输入（对应Modbus输入寄存器）
        // uint32_t adc_voltage;  // ADC采集电压（单位：mV，32位）
} App_Data_T;

// 全局实际业务数据（外部可通过接口访问，避免直接全局变量暴露）
extern App_Data_T g_AppData;

// 实际值更新接口（如传感器采集、状态刷新）
void App_Data_Update(void);

// 实际值初始化接口
void App_Data_Init(void);

#endif /* APP_DATA_H */