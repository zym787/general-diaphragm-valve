/**
 * @file      : bsp_msg.h
 * @brief     : 消息处理模块，建立消息处理机制
 * 
 * @version   : 1.1
 * @author    : Drinkto
 * @date      : Dec 23, 2025
 * 
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Mar 27, 2015 | 1.0 | armfly | 正式发布 |
 * | Jan 20, 2026 | 1.1 | Drinkto | 将信号量置为全局变量 |
 */

#ifndef __BSP_MSG_H
#define __BSP_MSG_H

#include <stdint.h>

#define MSG_FIFO_SIZE    40	   		/* 消息个数 */

enum 
{
	MSG_NONE = 0,
	
	MSG_MODS_01H,
	MSG_MODS_02H,
	MSG_MODS_03H,
	MSG_MODS_04H,
	MSG_MODS_05H,
	MSG_MODS_06H,
	MSG_MODS_10H,
    
    MSG_KEYV_PRESS,     /* 有建按下,待执行按键状态机 */
    
    MSG_MODS_READ,
    MSG_MODS_WRITE,
    
    MSG_MODS_DISPLAY,   /* 显示信息 */
};

/* 按键FIFO用到变量 */
typedef struct
{
	uint16_t MsgCode;		/* 消息代码 */
	uint32_t MsgParam;		/* 消息的数据体, 也可以是指针（强制转化） */
}MSG_T;

extern MSG_T ucMsg;    ///信号量

/* 按键FIFO用到变量 */
typedef struct
{
	MSG_T Buf[MSG_FIFO_SIZE];	/* 消息缓冲区 */
	uint8_t Read;					/* 缓冲区读指针1 */
	uint8_t Write;					/* 缓冲区写指针 */
	uint8_t Read2;					/* 缓冲区读指针2 */
}MSG_FIFO_T;

/* 供外部调用的函数声明 */
void bsp_InitMsg(void);
void bsp_PutMsg(uint16_t _MsgCode, uint32_t _MsgParam);
uint8_t bsp_GetMsg(MSG_T *_pMsg);
uint8_t bsp_GetMsg2(MSG_T *_pMsg);
void bsp_ClearMsg(void);

#endif

/***************************** 安富莱电子 www.armfly.com (END OF FILE) *********************************/
