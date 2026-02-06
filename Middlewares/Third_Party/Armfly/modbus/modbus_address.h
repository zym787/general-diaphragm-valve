/**
 * @file      : modbus_address.h
 * @brief     : modbus地址定义
 *
 * @version   : 1.0
 * @author    : Drinkto
 * @date      : Jan 7, 2026
 *
 * @changelog:
 * | Date | version | Author | Description |
 * | --- | --- | --- | --- |
 * | Jan 7, 2026 | 1.0 | Drinkto | 首次发布,兼容Modbus_Lin |
 */

#ifndef __MODBUS_ADDRESS_H
#define __MODBUS_ADDRESS_H

/******************************* 保 持 寄 存 器 *******************************/
/* 03H 读保持寄存器 */
/* 06H 写保持寄存器 */
/* 10H 写多个保存寄存器 */
#define SLAVE_REG_P00       0ul   //初始化标志
#define SLAVE_REG_P01       1ul   //通道方向
#define SLAVE_REG_P02       2ul   //转速
#define SLAVE_REG_P03       3ul   //恢复出厂参数
#define SLAVE_REG_P04       4ul   //当前次数
#define SLAVE_REG_P05       5ul   //当前次数(万)
#define SLAVE_REG_P06       6ul   //启停
#define SLAVE_REG_P07       7ul   //
#define SLAVE_REG_P08       8ul
#define SLAVE_REG_P09       9ul   //恢复出厂参数
#define SLAVE_REG_P10       10ul  //当前状态
#define SLAVE_REG_P11       11ul  //当前次数
#define SLAVE_REG_P12       12ul  //当前次数(万)
#define SLAVE_REG_P13       13ul  //
#define SLAVE_REG_P14       14ul  //
#define SLAVE_REG_P15       15ul  //
#define SLAVE_REG_P16       16ul  //
#define SLAVE_REG_P17       17ul  //
#define SLAVE_REG_P18       18ul  //
#define SLAVE_REG_P19       19ul  //版本号
#if 0
#define SLAVE_REG_P20       20ul  //用户私有数据1
#define SLAVE_REG_P21       21ul  //用户私有数据2
#define SLAVE_REG_P22       22ul  //用户私有数据3
#define SLAVE_REG_P23       23ul  //用户私有数据4
#define SLAVE_REG_P24       24ul  //用户私有数据5
#define SLAVE_REG_P25       25ul  //用户私有数据6
#define SLAVE_REG_P26       26ul  //用户私有数据7
#define SLAVE_REG_P27       27ul  //用户私有数据8
#define SLAVE_REG_P28       28ul  //用户私有数据9
#define SLAVE_REG_P29       29ul  //用户私有数据10
#define SLAVE_REG_P30       30ul  //
#define SLAVE_REG_P31       31ul  //
#define SLAVE_REG_P32       32ul  //
#define SLAVE_REG_P33       33ul  //
#define SLAVE_REG_P34       34ul  //
#define SLAVE_REG_P35       35ul  //
#define SLAVE_REG_P36       36ul  //
#define SLAVE_REG_P37       37ul  //
#define SLAVE_REG_P38       38ul  //
#define SLAVE_REG_P39       39ul  //
#define SLAVE_REG_P40       40ul  //
#define SLAVE_REG_P41       41ul  //
#define SLAVE_REG_P42       42ul  //
#define SLAVE_REG_P43       43ul  //
#define SLAVE_REG_P44       44ul  //
#define SLAVE_REG_P45       45ul  //
#define SLAVE_REG_P46       46ul  //
#define SLAVE_REG_P47       47ul  //
#define SLAVE_REG_P48       48ul  //
#define SLAVE_REG_P49       49ul  //
#define SLAVE_REG_P50       50ul  //设置波特率
#define SLAVE_REG_P51       51ul  //设置设备地址
#define SLAVE_REG_P52       52ul  //开机切换半通道
#define SLAVE_REG_P53       53ul  //就近切换使能
#define SLAVE_REG_P54       54ul  //
#define SLAVE_REG_P55       55ul  //
#define SLAVE_REG_P56       56ul  //切换通道停留时间
#define SLAVE_REG_P57       57ul  //
#define SLAVE_REG_P58       58ul  //老化测试
#define SLAVE_REG_P59       59ul  //
#define SLAVE_REG_P60       60ul  //
#define SLAVE_REG_P61       61ul  //
#define SLAVE_REG_P62       62ul  //
#define SLAVE_REG_P63       63ul  //
#define SLAVE_REG_P64       64ul  //
#define SLAVE_REG_P65       65ul  //
#define SLAVE_REG_P66       66ul  //
#define SLAVE_REG_P67       67ul  //
#define SLAVE_REG_P68       68ul  //
#define SLAVE_REG_P69       69ul  //
#define SLAVE_REG_P70       70ul  //
#define SLAVE_REG_P71       71ul  //
#define SLAVE_REG_P72       72ul  //
#define SLAVE_REG_P73       73ul  //
#define SLAVE_REG_P74       74ul  //
#define SLAVE_REG_P75       75ul  //
#define SLAVE_REG_P76       76ul  //
#define SLAVE_REG_P77       77ul  //
#define SLAVE_REG_P78       78ul  //
#define SLAVE_REG_P79       79ul  //
#define SLAVE_REG_P80       80ul  //
#define SLAVE_REG_P81       81ul  //
#define SLAVE_REG_P82       82ul  //
#define SLAVE_REG_P83       83ul  //
#define SLAVE_REG_P84       84ul  //
#define SLAVE_REG_P85       85ul  //
#define SLAVE_REG_P86       86ul  //
#define SLAVE_REG_P87       87ul  //
#define SLAVE_REG_P88       88ul  //
#define SLAVE_REG_P89       89ul  //
#define SLAVE_REG_P90       90ul  //
#define SLAVE_REG_P91       91ul  //转速
#define SLAVE_REG_P92       92ul  //加速度
#define SLAVE_REG_P93       93ul  //减速度
#define SLAVE_REG_P94       94ul  //工作电流
#define SLAVE_REG_P95       95ul  //校准电流
#define SLAVE_REG_P96       96ul  //设置通道数
#define SLAVE_REG_P97       97ul  //设置阀类型
#define SLAVE_REG_P98       98ul  //通道补偿
#define SLAVE_REG_P99       99ul  //减速比
#endif

//保持寄存器数量
#define SLAVE_REG_P_MAX     SLAVE_REG_P06   //根据最后一个保持寄存器地址来确定个数
#ifdef SLAVE_REG_P00
#define SLAVE_REG_PXX       (SLAVE_REG_P_MAX + 1)   //保持寄存器从0开始
#else
#define SLAVE_REG_PXX       (SLAVE_REG_P_MAX)       //保持寄存器从1开始
#endif

/******************************* 输 入 寄 存 器 *******************************/
/* 04H 读取输入寄存器(模拟信号) */
#define REG_A01		0x0401
#define REG_AXX		REG_A01


#endif // !__MODBUS_ADDRESS_H
