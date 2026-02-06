#include "app_data.h"
#include "bsp.h"

// 全局实际业务数据初始化
App_Data_T g_AppData = {.mb_addr = 0x01, /* 设备Modbus地址 */
                        .mb_baud = 0x04, /* 设备波特率 */
                        /* 电缸1参数 */
                        .motor1 =
                            {
                                .motor_enable = 0,   /* 电机使能（0：禁用，1：启用） */
                                .motor_dir = 0,      /* 电机方向（0：正转，1：反转） */
                                .motor_speed = 0,    /* 电机转速（单位：rpm） */
                                .motor_acc = 0,      /* 电机加速度（单位：rpm/s） */
                                .motor_position = 0, /* 电机位置（单位：step，32位） */
                            },
                        /* 电缸2参数 */
                        .motor2 =
                            {
                                .motor_enable = 0,   /* 电机使能（0：禁用，1：启用） */
                                .motor_dir = 0,      /* 电机方向（0：正转，1：反转） */
                                .motor_speed = 0,    /* 电机转速（单位：rpm） */
                                .motor_acc = 0,      /* 电机加速度（单位：rpm/s） */
                                .motor_position = 0, /* 电机位置（单位：step，32位） */
                            },
                        /* 电缸3参数 */
                        .motor3 =
                            {
                                .motor_enable = 0,   /* 电机使能（0：禁用，1：启用） */
                                .motor_dir = 0,      /* 电机方向（0：正转，1：反转） */
                                .motor_speed = 0,    /* 电机转速（单位：rpm） */
                                .motor_acc = 0,      /* 电机加速度（单位：rpm/s） */
                                .motor_position = 0, /* 电机位置（单位：step，32位） */
                            },
                        /* 电缸4参数 */
                        .motor4 = {
                            .motor_enable = 0,   /* 电机使能（0：禁用，1：启用） */
                            .motor_dir = 0,      /* 电机方向（0：正转，1：反转） */
                            .motor_speed = 0,    /* 电机转速（单位：rpm） */
                            .motor_acc = 0,      /* 电机加速度（单位：rpm/s） */
                            .motor_position = 0, /* 电机位置（单位：step，32位） */
                        }};

// 实际值更新（主循环中调用，如100ms一次）
void App_Data_Update(void)
{
        /* 更新Modbus地址 */
        g_AppData.mb_addr = bsp_GetAddress();
}

// 实际值初始化
void App_Data_Init(void)
{
}