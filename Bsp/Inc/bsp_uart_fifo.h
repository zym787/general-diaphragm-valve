/**
 * @file      : bsp_uart_fifo.h
 * @brief     : 串口中断+FIFO驱动模块
 *              采用串口中断+FIFO模式实现多个串口的同时访问
 *
 * @version   : 2.2
 * @author    : Drinkto
 * @date      : Jan 06, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Feb 01, 2013 | 1.0 | armfly | 正式发布 |
 * | Jan 04, 2026 | 2.0 | Drinkto | 移植到F1系列,支援901电路板 |
 * | Jan 14, 2026 | 2.1 | Drinkto | 移植到F0 |
 * | Jan 20, 2026 | 2.2 | Drinkto | 针对F030做小体积优化 |
 */

#ifndef _BSP_USART_FIFO_H_
#define _BSP_USART_FIFO_H_

#include <stdint.h>
// #include "core_cm3.h"
// #include "modbus_host.h"

/*
        A12_927 串口分配：
        【串口1】 TTL 调试
                PA9/USART1_TX	  --- 打印调试口
                P10/USART1_RX

        【串口2】 RS485 通信
                PA2/USART2_TX
                PA3/USART2_RX
*/

/// 是否使用bsp层中断管理
#define USE_BSP_IRQ   1
#define UART1_FIFO_EN 1  /// 调试串口
#define UART2_FIFO_EN 1  /// 485串口
#define UART3_FIFO_EN 0  /// 串口

/* PB2 控制RS485芯片的发送使能 */
#define RS485_TXEN_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()
#define RS485_TXEN_GPIO_PORT         RS485_CTR_GPIO_Port
#define RS485_TXEN_PIN               RS485_CTR_Pin

#define RS485_RX_EN() RS485_TXEN_GPIO_PORT->BSRR = (RS485_TXEN_PIN << 16)
#define RS485_TX_EN() RS485_TXEN_GPIO_PORT->BSRR = RS485_TXEN_PIN

/* 定义端口号 */
typedef enum {
        COM1 = 0, /* USART1 */
        COM2 = 1, /* USART2 */
        COM3 = 2, /* USART3 */
} COM_PORT_E;

/* 串口端口重映射 */
typedef enum {
        UART1_AF_ENABLE_PB6_PB7 = 0,    /* USART1  AF Enable Tx映射到PB6 Rx映射到PB7 */
        UART1_AF_DISABLE_PA9_PA10 = 1,  /* USART1  AF Disable Tx映射到PA9 Rx映射到PA10 */
        UART2_AF_ENABLE_PD5_PD6 = 2,    /* USART2  AF Enable Tx映射到PD5 Rx映射到PD6 */
        UART2_AF_DISABLE_PA2_PA3 = 3,   /* USART2  AF Disable Tx映射到PA2 Rx映射到PA3 */
        UART3_AF_ENABLE_PD8_PD9 = 4,    /* USART3  AF Enable Tx映射到PD8 Rx映射到PD9 */
        UART3_AF_PARTIAL_PC10_PC11 = 5, /* USART3  AF Partial Tx映射到PC10 Rx映射到PC11 */
        UART3_AF_DISABLE_PB10_PB11 = 6, /* USART3  AF Disable Tx映射到PB10 Rx映射到PB11 */
} USART_ALTERNATE_E;

/* 定义串口波特率和FIFO缓冲区大小，分为发送缓冲区和接收缓冲区, 支持全双工 */
#if UART1_FIFO_EN == 1
#define UART1_BAUD        115200
#define UART1_TX_BUF_SIZE 1 * 1024
#define UART1_RX_BUF_SIZE 1 * 1024
#endif

#if UART2_FIFO_EN == 1
#define UART2_BAUD        115200
#define UART2_TX_BUF_SIZE 1 * 1024
#define UART2_RX_BUF_SIZE 2 * 1024
#endif

#if UART3_FIFO_EN == 1
#define UART3_BAUD        115200
#define UART3_TX_BUF_SIZE 1 * 1024
#define UART3_RX_BUF_SIZE 1 * 1024
#endif

/* 串口设备结构体 */
typedef struct {
        USART_TypeDef *uart;     /* STM32内部串口设备指针 */
        uint8_t *pTxBuf;         /* 发送缓冲区 */
        uint8_t *pRxBuf;         /* 接收缓冲区 */
        uint16_t usTxBufSize;    /* 发送缓冲区大小 */
        uint16_t usRxBufSize;    /* 接收缓冲区大小 */
        __IO uint16_t usTxWrite; /* 发送缓冲区写指针 */
        __IO uint16_t usTxRead;  /* 发送缓冲区读指针 */
        __IO uint16_t usTxCount; /* 等待发送的数据个数 */

        __IO uint16_t usRxWrite; /* 接收缓冲区写指针 */
        __IO uint16_t usRxRead;  /* 接收缓冲区读指针 */
        __IO uint16_t usRxCount; /* 还未读取的新数据个数 */

        void (*SendBefor)(void);          /* 开始发送之前的回调函数指针（主要用于RS485切换到发送模式） */
        void (*SendOver)(void);           /* 发送完毕的回调函数指针（主要用于RS485将发送模式切换为接收模式） */
        void (*ReciveNew)(uint8_t _byte); /* 串口收到数据的回调函数指针 */
        uint8_t Sending;                  /* 正在发送中 */
} UART_T;

uint8_t bsp_GetUsartSkipGpioInitFlag(void);
void bsp_SetUsartSkipGpioInitFlag(uint8_t _switch);

void bsp_InitUart(COM_PORT_E _ucPort, USART_ALTERNATE_E _ucAF);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comSendChar(COM_PORT_E _ucPort, uint8_t _ucByte);
uint8_t comGetChar(COM_PORT_E _ucPort, uint8_t *_pByte);
void comSendBuf(COM_PORT_E _ucPort, uint8_t *_ucaBuf, uint16_t _usLen);
void comClearTxFifo(COM_PORT_E _ucPort);
void comClearRxFifo(COM_PORT_E _ucPort);
void comSetBaud(COM_PORT_E _ucPort, uint32_t _BaudRate);

void USART_SetBaudRate(USART_TypeDef *USARTx, uint32_t BaudRate);
void bsp_SetUartParam(USART_TypeDef *Instance, uint32_t BaudRate, uint32_t Parity, uint32_t Mode);

void RS232_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void RS232_SendStr(char *_pBuf);
void RS232_SetBaud(uint32_t _baud);

void RS485_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
void RS485_SendStr(char *_pBuf);
void RS485_SetBaud(uint32_t _baud);
uint8_t UartTxEmpty(COM_PORT_E _ucPort);

extern void bsp_UartEnterFIFO(uint8_t _date, UART_T *_pUart);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
