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

#ifndef _PORT_H
#define _PORT_H

#include <assert.h>
#include <inttypes.h>
#include "bsp.h"

#define	INLINE                      inline
#define PR_BEGIN_EXTERN_C           extern "C" {
#define	PR_END_EXTERN_C             }

#define MB_RTU_DEBUG 1 /* 使能RTU调试打印 */
// #define MB_TIMER_DEBUG

#define ENTER_CRITICAL_SECTION() DISABLE_INT()    /* 禁止全局中断 */
#define EXIT_CRITICAL_SECTION()  ENABLE_INT()     /* 使能全局中断 */

typedef uint8_t BOOL;

typedef unsigned char UCHAR;
typedef char CHAR;

typedef uint16_t USHORT;
typedef int16_t SHORT;

typedef uint32_t ULONG;
typedef int32_t LONG;

#ifndef TRUE
#define TRUE            1
#endif

#ifndef FALSE
#define FALSE           0
#endif

/* ----------------------- Type definitions ---------------------------------*/
typedef enum { MB_LOG_ERROR = 0, MB_LOG_WARN = 1, MB_LOG_INFO = 2, MB_LOG_DEBUG = 3 } eMBPortLogLevel;

#if MB_RTU_DEBUG == 1
#define mb_println(format, ...)                                                                                        \
        printf("[%20s:%4d]  MB(%6d) " format "\r\n", FILE_NAME2, __LINE__, HAL_GetTick(), ##__VA_ARGS__)
#define mb_printf(format, ...) printf("[%20s:%4d]  MB(%6d) " format, FILE_NAME2, __LINE__, HAL_GetTick(), ##__VA_ARGS__)
#else
#define mb_println(format, ...)
#define mb_printf(format, ...)
#endif

// extern void vMBPortLog(eMBPortLogLevel eLevel, const CHAR *szModule, const CHAR *szFmt, ...);

#endif
