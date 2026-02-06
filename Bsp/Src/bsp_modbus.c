/**
 * @file      : bsp_modbus.c
 * @brief     : modbus
 *
 * @version   : 1.2
 * @author    : Drinkto
 * @date      : Jan 4, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Jan 04, 2026 | 1.0 | Drinkto | 首次发布 |
 * | Jan 10, 2026 | 1.1 | Drinkto | 分离解析函数,支援读03写06保持寄存器,支持波特率可设,提供bsp层接口 |
 * | Jan 20, 2026 | 1.2 | Drinkto | 支援modbus主机 |
 */

#include "bsp.h"
// #include "app.h"
#include "app_data.h"
#include "modbus_slave.h"

MSG_T ucMsg; /// 信号量

uint32_t g_MBBaudRateTable[MB_BAUDRATE_SETTING_NUM] = {4800ul, 9600ul, 19200ul, 38400ul, 115200ul};

static void bsp_MB_GetHoldingReg(void);
static void bsp_MB_SetHoldingReg(uint32_t _reg);

// ---------------------- 第一步：定义寄存器表（核心，易修改）----------------------
// 说明：新增/修改寄存器，只需在此添加/修改项，无需改动其他逻辑
const Modbus_Reg_Map_Item_T g_Modbus_Reg_Map[] = {
    // ------------ 保持寄存器（Holding，0x0000~0x000F，读/写，16位）------------
    {0x0000, &g_AppData.mb_addr, 1, NULL, "设备Modbus地址"},
    {0x0001, &g_AppData.mb_baud, 1, NULL, "Modbus波特率"},
    {0x0002, &g_AppData.motor1.motor_enable, 1, NULL, "电机1使能"},
    {0x0003, &g_AppData.motor1.motor_dir, 1, NULL, "电机1方向"},
    {0x0004, &g_AppData.motor1.motor_speed, 1, NULL, "电机1转速（rpm）"},
    {0x0005, &g_AppData.motor1.motor_acc, 1, NULL, "电机1加速度（rpm/s）"},
    {0x0006, &g_AppData.motor1.motor_position, 1, NULL, "电机1位置（step）"},
};
// 寄存器表项数量（自动计算，避免手动修改）
const uint16_t g_Modbus_Reg_Map_Count = sizeof(g_Modbus_Reg_Map) / sizeof(Modbus_Reg_Map_Item_T);

// ---------------------- 第二步：实现数据转换函数（示例）----------------------
// 0.1精度换算：实际值（如25.5℃）×10 = 寄存器值（255）
uint32_t Modbus_Conv_0_1(uint32_t data_in, bool is_reg_to_app)
{
        if (is_reg_to_app) {
                // 寄存器值 → 实际值（如255 → 25.5）
                return data_in / 10;
        } else {
                // 实际值 → 寄存器值（如25.5 → 255）
                return data_in * 10;
        }
}

// ---------------------- 第三步：实现寄存器读写接口（Modbus核心调用）----------------------
// 查找寄存器映射项（内部辅助函数）
static const Modbus_Reg_Map_Item_T *Modbus_Reg_Find_Item(uint16_t reg_addr)
{
        for (uint16_t i = 0; i < g_Modbus_Reg_Map_Count; i++) {
                if (g_Modbus_Reg_Map[i].reg_addr == reg_addr) {
                        return &g_Modbus_Reg_Map[i];
                }
        }
        return NULL; // 未找到对应寄存器
}

// 读取寄存器（映射到实际值）
bool Modbus_Reg_Read(uint16_t reg_addr, uint16_t *reg_data, uint16_t reg_len)
{
        const Modbus_Reg_Map_Item_T *p_item = Modbus_Reg_Find_Item(reg_addr);
        if (p_item == NULL) {
                return false; // 未找到或无读取权限
        }

        // 临界区保护（避免实际值被中断修改，数据错乱）
        DISABLE_INT();

        if (p_item->reg_count == 1) {
                // 16位数据直接读取
                if (p_item->conv_fun != NULL) {
                        // 有转换函数（如实际值→寄存器值）
                        *reg_data = (uint16_t)p_item->conv_fun(*(uint16_t *)p_item->app_data_ptr, false);
                } else {
                        // 无转换函数，直接赋值
                        *reg_data = *(uint16_t *)p_item->app_data_ptr;
                }
        } else if (p_item->reg_count == 2) {
                // 32位数据（占用2个16位寄存器，大端/小端可在此调整）
                uint32_t app_data_32 = *(uint32_t *)p_item->app_data_ptr;
                reg_data[0] = (uint16_t)(app_data_32 & 0xFFFF);
                reg_data[1] = (uint16_t)(app_data_32 >> 16);
        }

        ENABLE_INT();
        return true;
}

// 写入寄存器（映射到实际值）
bool Modbus_Reg_Write(uint16_t reg_addr, const uint16_t *reg_data, uint16_t reg_len)
{
        const Modbus_Reg_Map_Item_T *p_item = Modbus_Reg_Find_Item(reg_addr);
        if (p_item == NULL) {
                return false; // 未找到或无写入权限
        }

        // 临界区保护（避免实际值被中断修改，数据错乱）
        DISABLE_INT();

        if (p_item->reg_count == 1) {
                // 16位数据直接写入
                if (p_item->conv_fun != NULL) {
                        // 有转换函数（如寄存器值→实际值）
                        *(uint16_t *)p_item->app_data_ptr = (uint16_t)p_item->conv_fun(*reg_data, true);
                } else {
                        // 无转换函数，直接赋值
                        *(uint16_t *)p_item->app_data_ptr = *reg_data;
                }
        } else if (p_item->reg_count == 2) {
                // 32位数据（占用2个16位寄存器，大端/小端可在此调整）
                uint32_t app_data_32 = ((uint32_t)reg_data[1] << 16) | reg_data[0];
                *(uint32_t *)p_item->app_data_ptr = app_data_32;
        }

        ENABLE_INT();
        return true;
}

// ---------------------- 第四步：实现调试接口（打印寄存器表）----------------------
void Modbus_Reg_Print_Table(void)
{
        const char *flag_str = "R/W H";

        // 串口打印寄存器表（方便调试，查看所有寄存器配置）
        BSP_Printf("================== Modbus Register Table ==================\r\n");
        BSP_Printf(" |   Addr   |  Type  |  Value   |  Desc                  | \r\n");

        for (uint16_t i = 0; i < g_Modbus_Reg_Map_Count; i++) {
                BSP_Printf("|  0x%04X  |  %4s  |  %5s   |  %20s  |\r\n", g_Modbus_Reg_Map[i].reg_addr, flag_str,
                           g_Modbus_Reg_Map[i].app_data_ptr, g_Modbus_Reg_Map[i].desc);
        }
}

void bsp_ModbusInit(void)
{
        /// 拉取配置
        //    for (uint8_t i = 0; i < EE_NUM; ++i) {
        //        SetHoldingReg(i);
        //    }

        /* 485 串口2配置 */
        bsp_InitUart(COM2, UART2_AF_DISABLE_PA2_PA3); /* 初始化串口参数 */

        bsp_DelayMS(10);
        RS485_SetBaud(g_MBBaudRateTable[g_AppData.mb_baud]); // 更新波特率
        bsp_println(" 485 Initialize! Baud: %dbps (%d) ", g_MBBaudRateTable[g_AppData.mb_baud], g_AppData.mb_baud);
        bsp_DelayMS(10);

        /// 消息通知复位
        bsp_InitMsg();

        /// 初始化协议栈
        bsp_SetModbusPollFlag(TRUE); /* 初始化完成后开始Modbus Poll */
}

/// 是否执行bsp库的modbus轮询服务
static __IO uint8_t g_ucEnableModbusPoll = 0; /* 等待变量初始化 */

/*
 *******************************************************************************
 *   函 数 名: bsp_GetModbusPollFlag
 *   功能说明: 返回g_ucEnableModbusPoll的值
 *             以判断是否执行bsp库的滴答定时中断服务程序
 *   形    参:  无
 *   返 回 值: g_ucEnableModbusPoll的值
 *******************************************************************************
 */
uint8_t bsp_GetModbusPollFlag(void)
{
        return g_ucEnableModbusPoll;
}
/*
 *******************************************************************************
 *   函 数 名: bsp_SetModbusPollFlag
 *   功能说明: 设置g_ucEnableModbusPoll的值
 *   形    参:  _switch 0 不执行bsp库的滴答定时中断服务程序
 *                      1 执行bsp库的滴答定时中断服务程序
 *   返 回 值: 无
 *******************************************************************************
 */
void bsp_SetModbusPollFlag(uint8_t _switch)
{
        g_ucEnableModbusPoll = ((_switch == TRUE) ? TRUE : FALSE);
}

/*
********************************************************************************
*	函 数 名: bsp_ModbusPoll
*	功能说明: 解析数据包. 在主程序中轮流调用。
*	形    参: 无
*	返 回 值: 无
********************************************************************************
*/
void bsp_ModbusPoll(void)
{
        if (bsp_GetModbusPollFlag() == FALSE) {
                return;
        }
        MODS_Poll();
}

/*
********************************************************************************
*	函 数 名: bsp_ModbusTask
*	功能说明: 解析寄存器
*	形    参: 无
*	返 回 值: 无
********************************************************************************
*/
void bsp_ModbusTask(void)
{
        if (bsp_GetMsg(&ucMsg)) {
                switch (ucMsg.MsgCode) {

                /// 03H指令 读保持寄存器
                case MSG_MODS_03H:
                        bsp_MB_GetHoldingReg();
                        break;

                /// 06H指令 写保持寄存器
                case MSG_MODS_06H:
                        bsp_MB_SetHoldingReg(ucMsg.MsgParam);
                        break;

                default:
                        break;
                }
        }
}

/*
********************************************************************************
*	函 数 名: bsp_MB_GetHoldingReg
*	功能说明: 更新保持寄存器值(处理03H指令)
*	形    参：无
*	返 回 值: 无
********************************************************************************
*/
static void bsp_MB_GetHoldingReg(void)
{
}

/*
********************************************************************************
*	函 数 名: bsp_MB_SetHoldingReg
*	功能说明: 设置保持寄存器值(处理06H指令)
*	形    参：无
*	返 回 值: 无
********************************************************************************
*/
static void bsp_MB_SetHoldingReg(uint32_t _reg)
{
}
