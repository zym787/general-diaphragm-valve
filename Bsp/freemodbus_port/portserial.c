/*
 * FreeModbus Libary: BARE Port
 * Copyright (C) 2006 Christian Walter <wolti@sil.at>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * File: $Id$
 */

#include "port.h"
#include "usart.h"
#include "bsp_uart_fifo.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
// static void prvvUARTTxReadyISR( void );
// static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
        /* If xRXEnable enable serial receive interrupts. If xTxENable enable
         * transmitter empty interrupts.
         */
        // 关键：切换前等待当前发送彻底完成（含停止位）
        if (!xTxEnable) {
                while ((huart2.Instance->SR & USART_SR_TC) == 0) {
                        __NOP();
                }
        }
        if (xRxEnable & (!xTxEnable)) {
                /* Enable the receive interrupt. */
                __HAL_UART_DISABLE_IT(&huart2, UART_IT_TXE);
                __HAL_UART_ENABLE_IT(&huart2, UART_IT_RXNE);
                RS485_RX_EN();
        } else if (xTxEnable & (!xRxEnable)) {
                /* Enable the transmitter empty interrupt. */
                __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE);
                __HAL_UART_ENABLE_IT(&huart2, UART_IT_TXE);
                RS485_TX_EN();
        } else {
                __HAL_UART_DISABLE_IT(&huart2, UART_IT_RXNE | UART_IT_TXE);
                RS485_RX_EN(); /* 安全默认状态 */
        }
}

BOOL xMBPortSerialInit(UCHAR ucPort, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity, UCHAR ucStopBits)
{
        MX_USART2_UART_Init();
        RS485_InitTXE();
        mb_println("xMBPortSerialInit: Protocol stack ready");
        // HAL_UART_Receive_IT(&huart2, NULL, 1);
        return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
        /* Put a byte in the UARTs transmit buffer. This function is called
         * by the protocol stack if pxMBFrameCBTransmitterEmpty( ) has been
         * called. */
        RS485_TX_EN();
#if 0
        huart2.Instance->DR = (ucByte & (uint16_t)0x01FF); // 发送一个字节
        while (READ_BIT(huart2.Instance->SR, USART_SR_TC) == RESET) {
                __NOP(); // 等待发送完成
        }
#else
        HAL_UART_Transmit_IT(&huart2, (uint8_t *)&ucByte, 1);
#endif
        mb_println("xMBPortSerialPutByte: 0x%02X", ucByte);
        return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
        /* Return the byte in the UARTs receive buffer. This function is called
         * by the protocol stack after pxMBFrameCBByteReceived( ) has been called.
         */
        RS485_RX_EN();
        *pucByte = (uint16_t)(huart2.Instance->DR & (uint16_t)0x01FF); // 接收一个字节
        mb_println("xMBPortSerialGetByte: 0x%02X", pucByte[0]);
        return TRUE;
}

#if 1
/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
// static void prvvUARTTxReadyISR( void )
// {
//     pxMBFrameCBTransmitterEmpty(  );
// }

// /* Create an interrupt handler for the receive interrupt for your target
//  * processor. This function should then call pxMBFrameCBByteReceived( ). The
//  * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
//  * character.
//  */
// static void prvvUARTRxISR( void )
// {
//     pxMBFrameCBByteReceived(  );
// }
#endif

/**
 * @brief This function handles USART2 global interrupt.
 * @note  Comment out the definition of this function in stm32f1xx_it.c
 *        Move this function from stm32f1xx_it.c to here
 */
void USART2_IRQHandler(void)
{
        HAL_UART_IRQHandler(&huart2);

#if 1
        /* Receive interrupt */
        if (READ_BIT(huart2.Instance->SR, USART_SR_RXNE) != RESET) {

                xMBRTUReceiveFSM();
                CLEAR_BIT(huart2.Instance->SR, USART_SR_RXNE);
        }
        /* Send interrupt */
        else if (READ_BIT(huart2.Instance->SR, USART_SR_TXE) != RESET) {

                xMBRTUTransmitFSM();
                CLEAR_BIT(huart2.Instance->SR, USART_SR_TXE);
        }
#else
        /* Receive interrupt */
        if (__HAL_UART_GET_FLAG(&huart2, USART_SR_RXNE)) {
                __HAL_UART_CLEAR_FLAG(&huart2, USART_SR_RXNE);
                prvvUARTRxISR();
        }
        /* Send interrupt */
        else if (__HAL_UART_GET_FLAG(&huart2, USART_SR_TXE)) {
                __HAL_UART_CLEAR_FLAG(&huart2, USART_SR_TXE);
                prvvUARTTxReadyISR();
        }
#endif
}

// 添加HAL库回调函数（在stm32f1xx_hal_uart.c外实现）
// void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
// {
//         if (huart->Instance == USART2) {
//                 prvvUARTRxISR();
//                 bsp_LedToggle(LED_RED);
//         }
// }

// void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
// {
//         if (huart->Instance == USART2) {
//                 prvvUARTTxReadyISR();
//                 bsp_LedToggle(LED_GREEN);
//         }
// }
