/**
 * @file      : bsp_user_lib.h
 * @brief    : 字符串操作/数值转换
 *              提供一些常用的sting、mem操作函数以及Modbus CRC16函数
 * 
 * @version   : 1.4
 * @author    : Drinkto
 * @date      : Dec 23, 2025
 * 
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Dec 05, 2013 | 1.0 | armfly | 正式发布 |
 * | Jun 20, 2014 | 1.1 | armfly |  |
 * | Apr 06, 2015 | 1.2 | armfly |  |
 * | Dec 23, 2025 | 1.3 | Drinkto | 添加日期宏 |
 * | Jan 19, 2026 | 1.4 | Drinkto | 添加文件名宏 |
 */

#ifndef __BSP_USER_LIB_H
#define __BSP_USER_LIB_H

#include "string.h"

/* Double expansion needed for stringification of macro values. */
#define STR(s)          #s
#define NUM2STR(s)      STR(s)

/* Transform the Date format from Aug 14 2025 to 2025-08-14 */
#define _YEAR_ ((((__DATE__ [7] - '0') * 10 + (__DATE__ [8] - '0')) * 10 \
+ (__DATE__ [9] - '0')) * 10 + (__DATE__ [10] - '0'))

#define _YEAR_SHORT_ ((__DATE__ [9] - '0') * 10 + (__DATE__ [10] - '0'))

#define _MONTH_ ((__DATE__ [0] == 'J' && __DATE__ [1] == 'a' && __DATE__ [2] == 'n') ? 1 \
: (__DATE__ [0] == 'F' && __DATE__ [1] == 'e' && __DATE__ [2] == 'b') ? 2 \
: (__DATE__ [0] == 'M' && __DATE__ [1] == 'a' && __DATE__ [2] == 'r') ? 3 \
: (__DATE__ [0] == 'A' && __DATE__ [1] == 'p' && __DATE__ [2] == 'r') ? 4 \
: (__DATE__ [0] == 'M' && __DATE__ [1] == 'a' && __DATE__ [2] == 'y') ? 5 \
: (__DATE__ [0] == 'J' && __DATE__ [1] == 'u' && __DATE__ [2] == 'n') ? 6 \
: (__DATE__ [0] == 'J' && __DATE__ [1] == 'u' && __DATE__ [2] == 'l') ? 7 \
: (__DATE__ [0] == 'A' && __DATE__ [1] == 'u' && __DATE__ [2] == 'g') ? 8 \
: (__DATE__ [0] == 'S' && __DATE__ [1] == 'e' && __DATE__ [2] == 'p') ? 9 \
: (__DATE__ [0] == 'O' && __DATE__ [1] == 'c' && __DATE__ [2] == 't') ? 10 \
: (__DATE__ [0] == 'N' && __DATE__ [1] == 'o' && __DATE__ [2] == 'v') ? 11 : 12)

#define _DAY_ ((__DATE__ [4] == ' ' ? 0 : __DATE__ [4] - '0') * 10 \
+ (__DATE__ [5] - '0'))

#define _HOUR_    ((__TIME__ [0] - '0') * 10 + (__TIME__ [1] - '0'))
#define _MIN_     ((__TIME__ [3] - '0') * 10 + (__TIME__ [4] - '0'))
#define _SEC_     ((__TIME__ [6] - '0') * 10 + (__TIME__ [7] - '0'))

/* For debug print FILE_NAME 处理掉获取的文件名目录 */
#define FILE_NAME ( strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__ )
#define FILE_NAME2 ( strrchr(FILE_NAME, '\\') ? strrchr(FILE_NAME, '\\') + 1 : FILE_NAME )

extern uint32_t ASCIITOHEX(uint8_t *buf, uint8_t len);
extern uint16_t sys_crc16(unsigned char *puchMsg, unsigned int usDataLen);

int str_len(char *_str);
void str_cpy(char *_tar, char *_src);
int str_cmp(char * s1, char * s2);
void mem_set(char *_tar, char _data, int _len);

void int_to_str(int _iNumber, char *_pBuf, unsigned char _len);
int str_to_int(char *_pStr);

uint16_t BEBufToUint16(uint8_t *_pBuf);
uint16_t LEBufToUint16(uint8_t *_pBuf);

uint32_t BEBufToUint32(uint8_t *_pBuf);
uint32_t LEBufToUint32(uint8_t *_pBuf);

uint16_t CRC16_Modbus(uint8_t *_pBuf, uint16_t _usLen) ;
int32_t  CaculTwoPoint(int32_t x1, int32_t y1, int32_t x2, int32_t y2, int32_t x);

char BcdToChar(uint8_t _bcd);
void HexToAscll(uint8_t * _pHex, char *_pAscii, uint16_t _BinBytes);
uint32_t AsciiToUint32(char *pAscii);

#endif //__BSP_USER_LIB_H
