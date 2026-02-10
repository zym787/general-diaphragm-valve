/*
 * FreeModbus Libary: Linux Port
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

/* ----------------------- Standard includes --------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>

#include "port.h"

/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "mbconfig.h"

/* ----------------------- Defines ------------------------------------------*/
#define NELEMS( x ) ( sizeof( ( x ) )/sizeof( ( x )[0] ) )

/* ----------------------- Static variables ---------------------------------*/
static FILE    *fLogFile = NULL;
static eMBPortLogLevel eLevelMax = MB_LOG_DEBUG;

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortLogLevel( eMBPortLogLevel eNewLevelMax )
{
    eLevelMax = eNewLevelMax;
}

void
vMBPortLogFile( FILE * fNewLogFile )
{
    fLogFile = fNewLogFile;
}

void
vMBPortLog( eMBPortLogLevel eLevel, const CHAR * szModule, const CHAR * szFmt, ... )
{
    CHAR            szBuf[512];
    int             i;
    va_list         args;
//     FILE           *fOutput = fLogFile == NULL ? stderr : fLogFile;

    static const char *arszLevel2Str[] = {"ERROR", "WARN", "INFO", "DEBUG"};

    // 级别过滤（提升性能）
    if (eLevel > eLevelMax)
            return;

    // 添加时间戳前缀（增强调试价值）
    uint32_t tick = HAL_GetTick();
    i = snprintf(szBuf, sizeof(szBuf), "[%lu] %s: %s: ", tick, arszLevel2Str[eLevel], szModule);
    // 安全处理可变参数
    if (i > 0 && i < sizeof(szBuf)) {
            va_start(args, szFmt);
            i += vsnprintf(&szBuf[i], sizeof(szBuf) - i, szFmt, args);
            va_end(args);
    }

    // 添加终止符确保安全
    if (i >= sizeof(szBuf) - 1)
            szBuf[sizeof(szBuf) - 1] = '\0';
    else
            szBuf[i] = '\0';

    // 通过重定向的printf输出
    BSP_Printf("%s\r\n", szBuf); // 添加回车换行符
}

// void
// vMBPortEnterCritical( void )
// {
//     if( pthread_mutex_lock( &xLock ) != 0 )
//     {
//         vMBPortLog( MB_LOG_ERROR, "OTHER", "Locking primitive failed: %s\n", strerror( errno ) );
//     }
// }

// void
// vMBPortExitCritical( void )
// {
//     if( pthread_mutex_unlock( &xLock ) != 0 )
//     {
//         vMBPortLog( MB_LOG_ERROR, "OTHER", "Locking primitive failed: %s\n", strerror( errno ) );
//     }
// }
