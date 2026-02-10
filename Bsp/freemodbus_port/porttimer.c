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

/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "tim.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
        // MX_TIM3_Init();
        // HAL_TIM_Base_DeInit(&htim3);
        // HAL_TIM_Base_MspDeInit(&htim3);

        TIM_ClockConfigTypeDef sClockSourceConfig = {0};
        TIM_MasterConfigTypeDef sMasterConfig = {0};

        htim3.Instance = TIM3;
        htim3.Init.Prescaler = 72 - 1;
        htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
        // htim3.Init.Period = (usTim1Timerout50us * 50) / 100 - 1;
        htim3.Init.Period = 1750 - 1;
        htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
        if (HAL_TIM_Base_Init(&htim3) != HAL_OK) {
                Error_Handler();
        }
        sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
        if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK) {
                Error_Handler();
        }
        sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
        sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
        if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK) {
                Error_Handler();
        }
        return TRUE;
}


inline void
vMBPortTimersEnable(  )
{
        /* Enable the timer with the timeout passed to xMBPortTimersInit( ) */
        /* 清零定时器用于下一次计数 */
        __HAL_TIM_SET_COUNTER(&htim3, 0);
        /* 清中断标志,防止定时器已启动立即进入中断 */
        __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
        /* 启动定时器 */
        HAL_TIM_Base_Start_IT(&htim3);
}

inline void
vMBPortTimersDisable(  )
{
        /* Disable any pending timers. */
        HAL_TIM_Base_Stop_IT(&htim3);
}

#if 1
/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}

void TIM3_IRQHandler(void)
{
        // HAL_TIM_IRQHandler(&htim3);

        if (__HAL_TIM_GET_FLAG(&htim3, TIM_FLAG_UPDATE)) {
                /* 清除中断标志位 */
                __HAL_TIM_CLEAR_FLAG(&htim3, TIM_FLAG_UPDATE);
                /* 通知协议栈3.5字符超时 */
                prvvTIMERExpiredISR();
        }
}
#endif
