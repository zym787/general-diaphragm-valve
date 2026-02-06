/*
*********************************************************************************************************
*
*	模块名称 : MODS通信模块. 从站模式【原创】
*	文件名称 : modbus_slave.c
*	版    本 : V1.5
*	说    明 : 头文件
*
*	Copyright (C), 2020-2030, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "modbus_slave.h"
#include "app_data.h"


/*
*********************************************************************************************************
*	                                   函数声明
*********************************************************************************************************
*/
static void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen);
static void MODS_SendAckOk(void);
static void MODS_SendAckErr(uint8_t _ucErrCode);

static void MODS_AnalyzeApp(void);

static void MODS_RxTimeOut(void);


static void MODS_03H(void);
static void MODS_06H(void);


static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value);
static uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value);

void MODS_ReciveNew(uint8_t _byte);

/*
*********************************************************************************************************
*	                                   变量
*********************************************************************************************************
*/
/*
Baud rate	Bit rate	 Bit time	 Character time	  3.5 character times
  2400	    2400 bits/s	  417 us	      4.6 ms	      16 ms
  4800	    4800 bits/s	  208 us	      2.3 ms	      8.0 ms
  9600	    9600 bits/s	  104 us	      1.2 ms	      4.0 ms
 19200	   19200 bits/s    52 us	      573 us	      2.0 ms
 38400	   38400 bits/s	   26 us	      286 us	      1.75 ms(1.0 ms)
 115200	   115200 bit/s	  8.7 us	       95 us	      1.75 ms(0.33 ms) 后面固定都为1750us
*/
typedef struct
{
	uint32_t Bps;
	uint32_t usTimeOut;
}MODBUSBPS_T;

const MODBUSBPS_T ModbusBaudRate[] =
{	
    {2400,	16000},
	{4800,	 8000}, 
	{9600,	 4000},
	{19200,	 2000},
	{38400,	 1750},
	{115200, 1750},
	{128000, 1750},
	{230400, 1750},
};

static uint8_t g_mods_timeout = 0;
MODS_T g_tModS = {0};
VAR_T g_tVar;

/*
*********************************************************************************************************
*	函 数 名: MODS_Poll
*	功能说明: 解析数据包. 在主程序中轮流调用。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MODS_Poll(void)
{
	uint16_t addr;
	uint16_t crc1;
	
	/* 超过3.5个字符时间后执行MODH_RxTimeOut()函数。全局变量 g_rtu_timeout = 1; 通知主程序开始解码 */
	if (g_mods_timeout == 0)	
	{
		return;								/* 没有超时，继续接收。不要清零 g_tModS.RxCount */
	}
	
	g_mods_timeout = 0;	 					/* 清标志 */

	if (g_tModS.RxCount < 4)				/* 接收到的数据小于4个字节就认为错误，地址（8bit）+指令（8bit）+操作寄存器（16bit） */
	{
		goto err_ret;
	}

	/* 计算CRC校验和，这里是将接收到的数据包含CRC16值一起做CRC16，结果是0，表示正确接收 */
	crc1 = CRC16_Modbus(g_tModS.RxBuf, g_tModS.RxCount);
	if (crc1 != 0)
	{
		goto err_ret;
	}

	/* 站地址 (1字节） */
	addr = g_tModS.RxBuf[0];				/* 第1字节 站号 */
	if (addr != g_AppData.mb_addr)		 			/* 判断主机发送的命令地址是否符合 */
	{
		goto err_ret;
	}

	/* 分析应用层协议 */
	MODS_AnalyzeApp();
    
#if DEBUG_MB == 1
    bsp_printf("Modbus Rece:|");
    for (uint16_t i = 0; i < g_tModS.RxCount; ++i) {
        BSP_Printf( " %02X", g_tModS.RxBuf[i]);
    }
    BSP_Printf(" |\r\n");
#endif
    
	
err_ret:
	g_tModS.RxCount = 0;					/* 必须清零计数器，方便下次帧同步 */
}

/*
*********************************************************************************************************
*	函 数 名: MODS_ReciveNew
*	功能说明: 串口接收中断服务程序会调用本函数。当收到一个字节时，执行一次本函数。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
void MODS_ReciveNew(uint8_t _byte)
{
	/*
		3.5个字符的时间间隔，只是用在RTU模式下面，因为RTU模式没有开始符和结束符，
		两个数据包之间只能靠时间间隔来区分，Modbus定义在不同的波特率下，间隔时间是不一样的，
		详情看此C文件开头
	*/
	uint8_t i;
	
	/* 根据波特率，获取需要延迟的时间 */
	for(i = 0; i < (sizeof(ModbusBaudRate)/sizeof(ModbusBaudRate[0])); i++)
	{
		if(g_MBBaudRateTable[g_AppData.mb_baud] == ModbusBaudRate[i].Bps)
		{
			break;
		}	
	}

	g_mods_timeout = 0;

        // bsp_LedOff(LED_GREEN);
        bsp_LedToggleFlag();
        /* 硬件定时中断，定时精度us 硬件定时器1用于MODBUS从机, 定时器2用于MODBUS主机*/
        // bsp_StartHardTimer(1, 1730 * 2, (void *)MODS_RxTimeOut);
        bsp_StartHardTimer(1, ModbusBaudRate[i].usTimeOut, (void *)MODS_RxTimeOut);

        if (g_tModS.RxCount < S_RX_BUF_SIZE)
	{
		g_tModS.RxBuf[g_tModS.RxCount++] = _byte;
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_RxTimeOut
*	功能说明: 超过3.5个字符时间后执行本函数。 设置全局变量 g_mods_timeout = 1，通知主程序开始解码。
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_RxTimeOut(void)
{
        g_mods_timeout = 1;
        bsp_println("Receive Timeout!");
        // bsp_LedOn(LED_GREEN);
        bsp_LedToggleFlag();
        // bsp_LedToggle(LED_RED);
        // bsp_println("Toggle LED!");
}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendWithCRC
*	功能说明: 发送一串数据, 自动追加2字节CRC
*	形    参: _pBuf 数据；
*			  _ucLen 数据长度（不带CRC）
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen)
{
	uint16_t crc;
	uint8_t buf[S_TX_BUF_SIZE];

	memcpy(buf, _pBuf, _ucLen);
	crc = CRC16_Modbus(_pBuf, _ucLen);
	buf[_ucLen++] = crc >> 8;
	buf[_ucLen++] = crc;

	RS485_SendBuf(buf, _ucLen);
    
#if DEBUG_MB == 1
    bsp_printf("COM%d&%d Send:|", COM2 + 1, COM2 + 1);
    for (uint16_t i = 0; i < _ucLen; ++i) {
        BSP_Printf( " %02X", buf[i]);
    }
    BSP_Printf(" |\r\n");
#endif
}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckErr
*	功能说明: 发送错误应答
*	形    参: _ucErrCode : 错误代码
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendAckErr(uint8_t _ucErrCode)
{
	uint8_t txbuf[3];

	txbuf[0] = g_tModS.RxBuf[0];					/* 485地址 */
	txbuf[1] = g_tModS.RxBuf[1] | 0x80;				/* 异常的功能码 */
	txbuf[2] = _ucErrCode;							/* 错误代码(01,02,03,04) */

	MODS_SendWithCRC(txbuf, 3);
}

/*
*********************************************************************************************************
*	函 数 名: MODS_SendAckOk
*	功能说明: 发送正确的应答.
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_SendAckOk(void)
{
	uint8_t txbuf[6];
	uint8_t i;

	for (i = 0; i < 6; i++)
	{
		txbuf[i] = g_tModS.RxBuf[i];
	}
	MODS_SendWithCRC(txbuf, 6);
}

/*
*********************************************************************************************************
*	函 数 名: MODS_AnalyzeApp
*	功能说明: 分析应用层协议
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_AnalyzeApp(void)
{
    uint16_t reg;
	switch (g_tModS.RxBuf[1])				/* 第2个字节 功能码 */
	{
		case 0x03:							/* 读取保持寄存器（此例程存在g_tVar中）*/
            ///[2] [3] 起始地址  [4] [5] 要读取的数量
            // reg = BEBufToUint16(&g_tModS.RxBuf[2]); 			/* 寄存器号 */
			MODS_03H();
			bsp_PutMsg(MSG_MODS_03H, 0);
			break;

		case 0x06:							/* 写单个保存寄存器（此例程改写g_tVar中的参数）*/
            reg = BEBufToUint16(&g_tModS.RxBuf[2]); 			/* 寄存器号 */
			MODS_06H();
			bsp_PutMsg(MSG_MODS_06H, reg);
			break;

		default:
			g_tModS.RspCode = RSP_ERR_CMD;
			MODS_SendAckErr(g_tModS.RspCode);	/* 告诉主机命令错误 */
			break;
	}
}


/*
*********************************************************************************************************
*	函 数 名: MODS_ReadRegValue
*	功能说明: 读取保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			  reg_value 存放寄存器结果
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
static uint8_t MODS_ReadRegValue(uint16_t reg_addr, uint8_t *reg_value)
{
    uint16_t value = 0;
    
	switch (reg_addr)									/* 判断寄存器地址 */
	{
		case SLAVE_REG_P00:
			value =	g_tVar.P00_INIT;	
			break;
        
        case SLAVE_REG_P01:
			value =	g_tVar.P01_DIR;	
			break;

		case SLAVE_REG_P02:
			value =	g_tVar.P02_SPD;							/* 将寄存器值读出 */
			break;
        
        case SLAVE_REG_P03:
			value =	g_tVar.P03_FACY;	
			break;
        
        case SLAVE_REG_P04:
			value =	g_tVar.P04_RCNT;	
			break;

		case SLAVE_REG_P05:
			value =	g_tVar.P05_RCNTK;							/* 将寄存器值读出 */
			break;
	
        case SLAVE_REG_P06:
			value =	g_tVar.P06_STAR_STOP;							/* 将寄存器值读出 */
			break;
        
		default:
			return 0;									/* 参数异常，返回 0 */
	}

    reg_value[0] = value >> 8;                  /* 注意数据是大端  */
    reg_value[1] = value;

    return 1;       /* 读取成功 */
}

/*
*********************************************************************************************************
*	函 数 名: MODS_WriteRegValue
*	功能说明: 写保持寄存器的值
*	形    参: reg_addr 寄存器地址
*			  reg_value 寄存器值
*	返 回 值: 1表示OK 0表示错误
*********************************************************************************************************
*/
static uint8_t MODS_WriteRegValue(uint16_t reg_addr, uint16_t reg_value)
{
    switch (reg_addr)									/* 判断寄存器地址 */
        {
        case SLAVE_REG_P00:
                g_tVar.P00_INIT = reg_value;	
                break;

        case SLAVE_REG_P01:
                g_tVar.P01_DIR = reg_value;	
                break;

        case SLAVE_REG_P02:
                g_tVar.P02_SPD = reg_value;							/* 将寄存器值读出 */
                break;

        case SLAVE_REG_P03:
                g_tVar.P03_FACY = reg_value;
                break;

        case SLAVE_REG_P04:
                g_tVar.P04_RCNT = reg_value;
                break;

        case SLAVE_REG_P05:
                g_tVar.P05_RCNTK = reg_value;							/* 将寄存器值读出 */
                break;

        case SLAVE_REG_P06:
                g_tVar.P06_STAR_STOP = reg_value;							/* 将寄存器值读出 */
                break;

        default:
                return 0;									/* 参数异常，返回 0 */
        }

    return 0;       /* 除写入读写寄存器范围内值外,其余操作均无效非法, 返回 0 */
}

/*
*********************************************************************************************************
*	函 数 名: MODS_03H
*	功能说明: 读取保持寄存器 在一个或多个保持寄存器中取得当前的二进制值
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_03H(void)
{
	/*
		从机地址为11H。保持寄存器的起始地址为006BH，结束地址为006DH。该次查询总共访问3个保持寄存器。

		主机发送:
			11 从机地址
			03 功能码
			00 寄存器地址高字节
			6B 寄存器地址低字节
			00 寄存器数量高字节
			03 寄存器数量低字节
			76 CRC高字节
			87 CRC低字节

		从机应答: 	保持寄存器的长度为2个字节。对于单个保持寄存器而言，寄存器高字节数据先被传输，
					低字节数据后被传输。保持寄存器之间，低地址寄存器先被传输，高地址寄存器后被传输。
			11 从机地址
			03 功能码
			06 字节数
			00 数据1高字节(006BH)
			6B 数据1低字节(006BH)
			00 数据2高字节(006CH)
			13 数据2 低字节(006CH)
			00 数据3高字节(006DH)
			00 数据3低字节(006DH)
			38 CRC高字节
			B9 CRC低字节

		例子:
			01 03 30 06 00 01  6B0B      ---- 读 3006H, 触发电流
			01 03 4000 0010 51C6         ---- 读 4000H 倒数第1条浪涌记录 32字节
			01 03 4001 0010 0006         ---- 读 4001H 倒数第1条浪涌记录 32字节

			01 03 F000 0008 770C         ---- 读 F000H 倒数第1条告警记录 16字节
			01 03 F001 0008 26CC         ---- 读 F001H 倒数第2条告警记录 16字节

			01 03 7000 0020 5ED2         ---- 读 7000H 倒数第1条波形记录第1段 64字节
			01 03 7001 0020 0F12         ---- 读 7001H 倒数第1条波形记录第2段 64字节

			01 03 7040 0020 5F06         ---- 读 7040H 倒数第2条波形记录第1段 64字节
	*/
	uint16_t reg;
	uint16_t num;
	uint16_t i;
	uint8_t reg_value[2 * 32];

	g_tModS.RspCode = RSP_OK;

    /** 第1步： 判断接到指定个数数据 ===============================================================*/
	/* 地址（8bit）+指令（8bit）+寄存器起始地址高低字节（16bit）+寄存器个数（16bit）+ CRC16 */
	if (g_tModS.RxCount != 8)								/* 03H命令必须是8个字节 */
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* 数据值域错误 */
		goto err_ret;
	}

	/** 第2步： 数据解析 ===========================================================================*/
	/* 数据是大端，要转换为小端 */
	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 				/* 寄存器号 */
	num = BEBufToUint16(&g_tModS.RxBuf[4]);					/* 寄存器个数 */
	
	/* 读取的数据个数要在范围内 */
	if (num > sizeof(reg_value) / 2)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;					/* 数据值域错误 */
		goto err_ret;
	}

	/* 读取的数据存入到reg_value里面 */
	for (i = 0; i < num; i++)
	{
		if (MODS_ReadRegValue(reg, &reg_value[2 * i]) == 0)	/* 读出寄存器值放入reg_value，此函数已经做了大端转小端处理 */
		{
			g_tModS.RspCode = RSP_ERR_REG_ADDR;				/* 寄存器地址错误 */
			break;
		}
		reg++;
	}

	/** 第3步： 应答回复 =========================================================================*/
err_ret:
	if (g_tModS.RspCode == RSP_OK)							 /* 正确应答 */
	{
		g_tModS.TxCount = 0;
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[0]; /* 返回从机地址 */
		g_tModS.TxBuf[g_tModS.TxCount++] = g_tModS.RxBuf[1]; /* 返回从机指令 */
		g_tModS.TxBuf[g_tModS.TxCount++] = num * 2;			 /* 返回字节数 */

		for (i = 0; i < num; i++)                            /* 返回数据*/ 
		{
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i];
			g_tModS.TxBuf[g_tModS.TxCount++] = reg_value[2*i+1];
		}
		MODS_SendWithCRC(g_tModS.TxBuf, g_tModS.TxCount);	/* 发送正确应答 */
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);					/* 发送错误应答 */
	}
}

/*
*********************************************************************************************************
*	函 数 名: MODS_06H
*	功能说明: 写单个寄存器
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void MODS_06H(void)
{

	/*
		写保持寄存器。注意06指令只能操作单个保持寄存器，16指令可以设置单个或多个保持寄存器

		主机发送:
			11 从机地址
			06 功能码
			00 寄存器地址高字节
			01 寄存器地址低字节
			00 数据1高字节
			01 数据1低字节
			9A CRC校验高字节
			9B CRC校验低字节

		从机响应:
			11 从机地址
			06 功能码
			00 寄存器地址高字节
			01 寄存器地址低字节
			00 数据1高字节
			01 数据1低字节
			1B CRC校验高字节
			5A	CRC校验低字节

		例子:
			01 06 30 06 00 25  A710    ---- 触发电流设置为 2.5
			01 06 30 06 00 10  6707    ---- 触发电流设置为 1.0


			01 06 30 1B 00 00  F6CD    ---- SMA 滤波系数 = 0 关闭滤波
			01 06 30 1B 00 01  370D    ---- SMA 滤波系数 = 1
			01 06 30 1B 00 02  770C    ---- SMA 滤波系数 = 2
			01 06 30 1B 00 05  36CE    ---- SMA 滤波系数 = 5

			01 06 30 07 00 01  F6CB    ---- 测试模式修改为 T1
			01 06 30 07 00 02  B6CA    ---- 测试模式修改为 T2

			01 06 31 00 00 00  8736    ---- 擦除浪涌记录区
			01 06 31 01 00 00  D6F6    ---- 擦除告警记录区

*/

	uint16_t reg;
	uint16_t value;

	g_tModS.RspCode = RSP_OK;

    /** 第1步： 判断接到指定个数数据 ===============================================================*/
	/* 地址（8bit）+指令（8bit）+寄存器起始地址高低字节（16bit）+寄存器个数（16bit）+ CRC16 */
	if (g_tModS.RxCount != 8)
	{
		g_tModS.RspCode = RSP_ERR_VALUE;		/* 数据值域错误 */
		goto err_ret;
	}

	/** 第2步： 数据解析 ===========================================================================*/
	/* 数据是大端，要转换为小端 */
	reg = BEBufToUint16(&g_tModS.RxBuf[2]); 	/* 寄存器号 */
	value = BEBufToUint16(&g_tModS.RxBuf[4]);	/* 寄存器值 */

	if (MODS_WriteRegValue(reg, value) == 1)	/* 该函数会把写入的值存入寄存器 */
	{
		;
	}
	else
	{
		g_tModS.RspCode = RSP_ERR_REG_ADDR;		/* 寄存器地址错误 */
	}

	/** 第3步： 应答回复 =========================================================================*/
err_ret:
	if (g_tModS.RspCode == RSP_OK)				/* 正确应答 */
	{
		MODS_SendAckOk();
	}
	else
	{
		MODS_SendAckErr(g_tModS.RspCode);		/* 告诉主机命令错误 */
	}
}
