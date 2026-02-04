/**
 * @file      : bsp_port.c
 * @brief     : bsp端口定义文件
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Jan 15, 2025
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Jan 15, 2025 | 1.0 | Drinkto | 适配929电路板 |
 */

#ifndef __BSP_PORT_H
#define __BSP_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "main.h"

/*
 *******************************************************************************
 *                                 电路板型号                                  *
 *******************************************************************************
 */
#define A12_901_E2 (0xA12901E2)
#define A12_906_B4 (0xA12906B4)
#define A12_909_A3 (0xA12909A3)
#define A12_929_A  (0xA129290A)
#define A12_927_A  (0xA129270A)

#define PCB_VERSION A12_927_A

#if PCB_VERSION == A12_901_E2
#define BOARD_VERSION_CHAR AIGIS_BASIC_BOARD_VR
/* PCB描述 */
#define BOARD_DESCRIPTION  AIGIS_BASIC_BOARD_DESCRIPTION
// 引用板级宏
#include "AIGIS_Basic_A12_901.h"
#elif PCB_VERSION == A12_929_A
#define BOARD_VERSION_CHAR AIGIS_BASIC_BOARD_VR
/* PCB描述 */
#define BOARD_DESCRIPTION  AIGIS_BASIC_BOARD_DESCRIPTION
// 引用板级宏
#include "AIGIS_Basic_A12_929.h"
#elif PCB_VERSION == A12_927_A
#define BOARD_VERSION_CHAR AIGIS_BASIC_BOARD_VR
/* PCB描述 */
#define BOARD_DESCRIPTION  AIGIS_BASIC_BOARD_DESCRIPTION
// 引用板级宏
#include "AIGIS_Basic_A12_927.h"
#else
#error "Undefined PCB_VERSION !!!"
#endif

/*
 *******************************************************************************
 *                                 软件 信息                                  *
 *******************************************************************************
 */
/// 软件名称
#define SOFTWARE_NAME            "ZF-AGS"
#define SOFTWARE_NAME_NUMBER     0xC23F
/* 软件版本描述 */
#define SOFTWARE_DESCRIPTION     "Column Valve"
#define CUSTOMIZE_DESCRIPTION    "AIGIS General"
/* 控制描述 */
#define CONTROL_DESCRIPTION      "485 Control by Modbus"
/// 软件版本
/* Major-Minor-Revision v4.0.1-r6 */
/* Major version */
#define PROJECT_VERSION_MAJOR    6 /**< Major version number (X.x.x) */
/* Minor version */
#define PROJECT_VERSION_MINOR    0 /**< Minor version number (x.X.x) */
/* Revise version */
#define PROJECT_VERSION_REVISION 0 /**< Revision version number (x.x.X) */
/* Path version */
#define PROJECT_VERSION_PATH     1 /**< Patch version number (x.x.x-rX) */

/* 注意GNU 与Keil/IAR字符串拼接时使用符号不一样 */
#ifdef __GNUC__
#define VERSION_CHAR(a, b, c, d) SOFTWARE_NAME " v" STR(a) "." STR(b) "." STR(c) "-r" STR(d)
#define SHORT_DATA               _YEAR_SHORT_ "." _MONTH_ "." _DAY_
/* version string */
#define VERSION_STRING          SOFTWARE_NAME " v"\
                NUM2STR(PROJECT_VERSION_MAJOR) "." NUM2STR(PROJECT_VERSION_MINOR) "." NUM2STR( PROJECT_VERSION_REVISION) "." NUM2STR(PROJECT_VERSION_PATH)
#else
#define VERSION_CHAR(a, b, c, d) SOFTWARE_NAME##" v"##STR(a)##"."##STR(b)##"."##STR(c)##"."##STR(d)
#define SHORT_DATA               _YEAR_SHORT_##"."##_MONTH_##"."##_DAY_
/* version string */
#define VERSION_STRING                                                                                          \
        SOFTWARE_NAME##" v"##NUM2STR(PROJECT_VERSION_MAJOR)##"."##NUM2STR(PROJECT_VERSION_MINOR)##"."##NUM2STR( \
            PROJECT_VERSION_REVISION)##"-"##"r"##NUM2STR(PROJECT_VERSION_PATH)
#endif

/* version data number */
#define VERSION_DATA_NUMBER       ((_YEAR_) * 10000U + (_MONTH_) * 100U + (_DAY_) * 1U)
/* version number */
#define VERSION_CHECK(a, b, c, d) (uint32_t)((a * 1000U) + (b * 100U) + (c * 10U) + (d * 1U))
#define VERSION_NUMBER \
        VERSION_CHECK(PROJECT_VERSION_MAJOR, PROJECT_VERSION_MINOR, PROJECT_VERSION_REVISION, PROJECT_VERSION_PATH)

#ifdef __cplusplus
}
#endif
#endif  //__BSP_PORT_H
