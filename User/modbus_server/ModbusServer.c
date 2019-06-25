/********************************************************************************
*  版权所有: 	广东利通科技		
*
*  文件名:       ModbusServer.c
*
*
*  当前版本:                       0.1
*  
*  作者:     JERRY                     
*
*  完成日期:  20190613
*                 
*  作者:    无
*
*  完成日期:                       无
*
*
*  描述: 
********************************************************************************/

/*
*********************************************************************************************************
*                                         包含文件
*********************************************************************************************************
*/
#include "include.h"

/********************************************************************************************************
                                  通信定时器启动函数定义
*********************************************************************************************************/
//#define StartCounterT35		(g_T35_num = 0)
//#define StartCounterT15		(g_T15_num = 0)
#define StartCounterT100 		(g_T100_num = 0)
/*不能初始化为-1, 否则CommTimer中g_T100_num++会变成0*/
/*就满足g_T100_num>= 0条件,导致无休止进入CommTimer判断*/
/*至于为什么第一次会进入,很有可能是g_T100_num是全局*/
/*变量，2个中断同时读写导致*/
#define StoptCounterT100			(g_T100_num = -2)

/********************************************************************************************************
                                  通信定时器所使用的变量
*********************************************************************************************************/
//static INT8U g_ENDT15Flag;	/*注意需要赋初值为FALSE*/
//static INT8U g_ENDT35Flag;	/*注意需要赋初值为FALSE*/
//static INT8S g_T35_num;		/*注意需要赋初值为-1*/
//static INT8S g_T15_num;		/*注意需要赋初值为-1*/
static INT16S g_T100_num;	/*发送完成后等待接收计数器，初值为-1*/
//static INT8U g_ComBuf[COMMBUFLEN];  
//static INT8U T15DelayTime[BAUDNUM] = {24,12,8,5,3};
//static INT8U T35DelayTime[BAUDNUM] = {46,23,12,6,4};

INT32U  comm_flag=0;

/********************************************************************************************************
                                  全局变量
*********************************************************************************************************/
//INT8U  g_T15SetNumVal;/*根据选取的通信定时计时器的频率和通信频率赋值*/
//INT8U  g_T35SetNumVal;/*根据选取的通信定时计时器的频率和通信频率赋值*/

INT8U Recive_Flag = 0;			/* 接收标志*/
INT8U WAIT_response_flag = 0;	/* 表示不同通信处理标志*/
INT8U Com_err_counter = 0;		/* 通信错误计数器*/
INT8U Com_err_flag = 0;			/* 通信错误标志*/
//INT8U g_Comm_init = FALSE;		/* 通信初始化标志*/

INT8U g_CommRxFlag = TRUE;          	/*注意需要初使化为TRUE，方便上电时接收使能*/
INT16U g_TxDataCtr;              	/*发送数据发送个数计数*/
SEND_Struct g_SENData;		/* 发送数据的结构体 */
PDU_Struct  g_PDUData;  		/* 接收数据的结构体 */

/**************************************************************************************************/
/************************以下是与应用相关的全局静态函数********************************************/
/**************************************************************************************************/

/************************************************************************************
函数名:	       ReceOneChar

输入参数:	由相关串行硬件通信电路传递来的被接收数据

输出参数:	g_ADUData，存储链路层加应用层的数据及相关标志

返回值:		无

函数说明:	由接收中断调用，每接收一次就调用一次

**************************************************************************************/
void ReceOneChar(INT8U ReceCharacter) 
{
	if (g_CommRxFlag == TRUE)//如果接收允许
	{	  
		StoptCounterT100;		/*开始接收数据，禁止接收超时计数*/

		if (g_PDUData.PDULength < UART_RXBUF_SIZE)/*防止指针操作边界溢出*/
		{
			*(g_PDUData.PDUBuffPtr+g_PDUData.PDULength) = ReceCharacter;
			g_PDUData.PDULength++;
		}
	}
}

/************************************************************************************
函数名:	       data_send_directly

输入参数:	直接发送数据

输出参数:	g_ADUData，存储链路层加应用层的数据及相关标志

返回值:		无

函数说明:	由接收中断调用，每接收一次就调用一次

**************************************************************************************/
void data_send_directly(USART_LIST destUtNo)
{
	uint8_t ch = 0;
	USART_TypeDef *PUSART = USART2;

	if (destUtNo == UART1_COM)
	{
		PUSART = USART1;
	}
#if (BD_USART_NUM >= 2)
	else if (destUtNo == UART2_COM)
	{
		PUSART = USART2;
	}
#endif
#if (BD_USART_NUM >= 3)
	else if (destUtNo == UART4_COM)
	{
		PUSART = UART4;
		rs485FuncSelect(SEND_S);		//485开始发送
		Delay_Ms(1);	// 等待1ms
	}
#endif
#if (BD_USART_NUM >= 4)
	else if (destUtNo == UART5_COM)
	{
		PUSART = UART5;
	}
#endif
	else
	{
		/* 其它是异常情况,直接返回*/
		return;
	}

	g_TxDataCtr = 0;
	LED_Set(LED_COM, ON); 	// 开始通信指示
	while (g_TxDataCtr  <  g_SENData.SENDLength)
	{
		ch= *(g_SENData.SENDBuffPtr+g_TxDataCtr);
		USART_SendData(PUSART, ch);
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(PUSART, USART_FLAG_TXE) == RESET);
		g_TxDataCtr++;
	}
	LED_Set(LED_COM, OFF); 	// 通信完毕

	StartCounterT100;					/*开始等待计数*/
	g_SENData.SENDLength = 0;
	g_TxDataCtr  = 0;

	g_CommRxFlag = TRUE;            	/* 设置为接受状态*/
	if (destUtNo == UART4_COM)
	{
		Delay_Ms(1);	// 等待2ms等最后一个数据发送完毕,否则rs485FuncSelect(RECEIVE_S);会让数据出错
		rs485FuncSelect(RECEIVE_S);		//485默认为
	}
	g_PDUData.PDUBuffPtr = UARTBuf[destUtNo].RxBuf;
	g_PDUData.PDULength = 0;	// 准备接收
}


/******************************************************************************
 * 函数名:	CRC_check 
 * 描述: 
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
INT8U CRC_check(void)
{
	INTEGER_UNION CRC_data;

	/*此处理解决在长度小于2时，指针越限引起装置重起问题*/	
	if(g_PDUData.PDULength<7)
	{
		return 0;
	}
	
	CRC_data.b[0] = *(g_PDUData.PDUBuffPtr + g_PDUData.PDULength - 2);
	CRC_data.b[1] = *(g_PDUData.PDUBuffPtr +  g_PDUData.PDULength - 1);
	
	if(get_crc16( g_PDUData.PDUBuffPtr,g_PDUData.PDULength - 2) == CRC_data.i)
	{
		return 1;
	}
	return 0;
}


/******************************************************************************
 * 函数名:	realSum_check 
 * 描述: 
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
INT8U realSum_check(void)
{
	INT16U re_value = 0;
	INT16U i = 0;

	/*此处理解决在长度小于2时，指针越限引起装置重起问题*/	
	if(g_PDUData.PDULength != REAL_DATA_NUM)
	{
		return 0;
	}

	/*对从电压参数寄存器开始求和*/
	for (i = 2; i < g_PDUData.PDULength-1; i++)
	{
		re_value = re_value + *(g_PDUData.PDUBuffPtr+i);
	}

	if ((INT8U)re_value == *(g_PDUData.PDUBuffPtr+g_PDUData.PDULength -1))
	{
		re_value = 1;
	}
	else
	{
		re_value = 0;
	}

	return ((INT8U)re_value);
}


/******************************************************************************
 * 函数名:	comm_DeviceParam_set 
 * 描述: 只设置空调参数，空调是MODBUS协议
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 *------------------------
 * 修改人:	CZH
 * 修改日期:2012-10-10
 ******************************************************************************/
void comm_DeviceParam_set_1(void)
{
	INT8U i=0;
	INT16U* pointer = &DevParams.AirCondSet;
	INTEGER_UNION int_value;
	INT8U reg_num = AIR_ONOFF_SET_NUM;	/*装置参数寄存器的个数*/
	int_value.i=AIR_ONOFF_REG;					/*装置参数寄存器的首地址*/
	g_SENData.SENDBuffPtr = UARTBuf[AIR_COND_UART].RxBuf;;
	*g_SENData.SENDBuffPtr = AIR_STATION_ADDRESS;
	*(g_SENData.SENDBuffPtr+1) = WRITEREG_COMMAND;
	*(g_SENData.SENDBuffPtr+2)=int_value.b[1];
	*(g_SENData.SENDBuffPtr+3)=int_value.b[0];/*寄存器地址*/		
	*(g_SENData.SENDBuffPtr+4) = 0X00;
	*(g_SENData.SENDBuffPtr+5) = reg_num;
	*(g_SENData.SENDBuffPtr+6) = reg_num*2; 
	
	for(i=0;i<1;i++)
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM +i*2 ,	*(pointer+i));
	}
	
	/*CRC校验*/
	int_value.i=get_crc16(g_SENData.SENDBuffPtr,SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2);
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2)= int_value.b[0];
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2+1)= int_value.b[1];
	g_SENData.SENDLength=SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2+2;
	data_send_directly(AIR_COND_UART);
}


/******************************************************************************
 * 函数名:	comm_DeviceParam_set 
 * 描述: 只设置空调参数，空调是MODBUS协议
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 *------------------------
 * 修改人:	CZH
 * 修改日期:2012-10-10
 ******************************************************************************/
void comm_DeviceParam_set_2(void)
{
	INT8U i=0;
	INT16U* pointer = &DevParams.AirColdStartPoint;
	INTEGER_UNION int_value;
	INT8U reg_num = AIR_TEMP_SET_NUM;	/*装置参数寄存器的个数*/
	int_value.i=AIR_TEMP_REG;					/*装置参数寄存器的首地址*/
	g_SENData.SENDBuffPtr = UARTBuf[AIR_COND_UART].RxBuf;;
	*g_SENData.SENDBuffPtr = AIR_STATION_ADDRESS;
	*(g_SENData.SENDBuffPtr+1) = WRITEREG_COMMAND;
	*(g_SENData.SENDBuffPtr+2)=int_value.b[1];
	*(g_SENData.SENDBuffPtr+3)=int_value.b[0];/*寄存器地址*/		
	*(g_SENData.SENDBuffPtr+4) = 0X00;
	*(g_SENData.SENDBuffPtr+5) = reg_num;
	*(g_SENData.SENDBuffPtr+6) = reg_num*2; 
	
	for(i=0;i<4;i++)
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM +i*2 ,	*(pointer+i));
	}
	
	/*CRC校验*/
	int_value.i=get_crc16(g_SENData.SENDBuffPtr,SET_FRAME_HEAD_NUM+AIR_TEMP_SET_NUM*2);
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+AIR_TEMP_SET_NUM*2)= int_value.b[0];
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+AIR_TEMP_SET_NUM*2+1)= int_value.b[1];
	g_SENData.SENDLength=SET_FRAME_HEAD_NUM+AIR_TEMP_SET_NUM*2+2;
	data_send_directly(AIR_COND_UART);
}


/******************************************************************************
 * 函数名:	comm_RealData0_analyse 
 * 描述: 		电压电流传感器0通道数据解析
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改内容: 使用指针对结构体变量赋值
 * 修改日期:
 ******************************************************************************/
bool comm_RealData_analyse(INT8U ch)			 
{
	INT32U * pointer = &RSUParams.phase[ch].param_v;	/*第0相参数*/
	INT32U temp = 0;

	//USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);	//禁止中断,防止频繁收到无效数据
	USART_Cmd(UART5, DISABLE);
	if(realSum_check())
	{		
		char3_to_int(g_PDUData.PDUBuffPtr + 2, pointer);
		char3_to_int(g_PDUData.PDUBuffPtr + 5, &temp);
		// 浮点运算，扩大100倍,因为kv扩大了100倍
		RSUParams.phase[ch].vln = ((float)RSUParams.phase[ch].param_v/temp)*RSUParams.phase[ch].k_v;

		pointer = &RSUParams.phase[ch].param_a;
		char3_to_int(g_PDUData.PDUBuffPtr + 8, pointer);
		char3_to_int(g_PDUData.PDUBuffPtr + 11, &temp);
		// 浮点运算，扩大100倍
		RSUParams.phase[ch].amp = ((float)RSUParams.phase[ch].param_a/temp)*RSUParams.phase[ch].k_a;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/******************************************************************************
 * 函数名:	comm_DeviceParam_1_analyse 
 * 描述: 		设备参数的解析,空调开关机状态
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 *------------------------
 ******************************************************************************/
void comm_DeviceParam_1_analyse(void)			
{
	INT8U i=0;
	INT16U* pointer = &DevParams.AirCondSet;

	if(CRC_check() && (g_PDUData.PDULength == (AIR_ONOFF_SET_NUM*2+5)))
	{
		for (i=0;i<AIR_ONOFF_SET_NUM;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
	}
}

/******************************************************************************
 * 函数名:	comm_DeviceParam_1_analyse 
 * 描述: 		设备参数的解析,空调开关机状态
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 *------------------------
 ******************************************************************************/
void comm_DeviceParam_2_analyse(void)			
{
	INT8U i=0;
	INT16U* pointer = &DevParams.AirColdStartPoint;

	if(CRC_check() && (g_PDUData.PDULength == (AIR_TEMP_SET_NUM*2+5)))
	{
		for (i=0;i<AIR_TEMP_SET_NUM;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
	}
}


/******************************************************************************
 * 函数名:	comm_SPDStatus_analyse 
 * 描述: 		防雷的输入状态解析
 *            -
 * 输入参数:  05 02 00 00 00 11 B9 82
 * 输出参数:  05 02 03 8C 00 00 B8 21		// 返回03是长度,8C 00 00都是DI状态
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:CZH
 * 修改内容: 对新规约进行解析
 * 修改日期:2012-10-16
 ******************************************************************************/
void comm_SPDStatus_analyse(void)	
{
	INT32U temp = 0;
	INT16U* pointer = &SPDParams.status;

	if(CRC_check() && (g_PDUData.PDULength == (3+5)))	// 返回只有3个字节
	{
		// 电源状态
		*pointer = (*(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM))&0x01;	//第一个数值的第0位

		/*取出第1位~16位,分别是收到数据的第1个字节的1~7位*/
		/*第2个字节和第3个字节低1位*/
		temp = (*(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM+2));
		temp = (temp << 8) +(*(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM+1));
		temp = (temp << 8) +(*(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM));

		pointer = &SPDParams.DI_status;	// DI状态1~16
		*pointer = (temp>>1)&0xFF;
		
	}
}

/******************************************************************************
 * 函数名:	comm_SPDData_analyse 
 * 描述: 		防雷的数据状态解析
 *            -
 * 输入参数:  05 04 00 0E 00 03 D0 4C			// 发送14开始读3个
 * 输出参数:  05 04 06 00 00 00 00 00 00 52 53   // 接收6个数据
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:CZH
 * 修改内容: 对新规约进行解析
 * 修改日期:2012-10-16
 ******************************************************************************/
void comm_SPDTimes_analyse(void)	
{
	INT16U* pointer = &SPDParams.struck_times;

	if(CRC_check() && (g_PDUData.PDULength == (SPD_TIMES_NUM*2+5)))
	{
		// 只需要总雷击次数
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + 1*2, pointer);
	}
}

/******************************************************************************
 * 函数名:	comm_config_analyse 
 * 描述: 		读取温湿度
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 ******************************************************************************/
void comm_EnviTemp_analyse(void)				 
{
	INT8U i=0;
	INT16U* pointer = &ENVIParms.moist;

	if(CRC_check() && (g_PDUData.PDULength == (ENVI_TEMP_NUM*2+5)))
	{
		for (i=0;i<ENVI_TEMP_NUM;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
	}
}


/*****************************************************************************
 * 函数名:	comm_config_analyse 
 * 描述: 		读取温湿度
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 ******************************************************************************/
void comm_water_in_analyse(void)				 
{
	INT8U i=0;
	INT16U* pointer = &ENVIParms.water_flag;

	if(CRC_check() && (g_PDUData.PDULength == (WATER_IN_NUM*2+5)))
	{
		for (i=0;i<WATER_IN_NUM;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
	}
}

/******************************************************************************
 * 函数名:	comm_config_analyse 
 * 描述: 		读取温湿度
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 ******************************************************************************/
void comm_EnviAirOnOff_analyse(void)				 
{
	INT8U i=0;
	INT16U* pointer = &ENVIParms.air_cond_status;

	if(CRC_check() && (g_PDUData.PDULength == (ENVI_AIRCOND_ONOFF_NUM*2+5)))
	{
		for (i=0;i<ENVI_AIRCOND_ONOFF_NUM;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
	}
}


/******************************************************************************
 * 函数名:	comm_config_analyse 
 * 描述: 		读取温湿度
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 ******************************************************************************/
void comm_EnviAirTemp_analyse(void)				 
{
	INT8U i=0;
	INT16U* pointer = &ENVIParms.air_cond_temp_out;// 读外温

	if(CRC_check() && (g_PDUData.PDULength == (ENVI_AIRCOND_TEMP_NUM*2+5)))
	{
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, pointer);
		/*调试发现外温总是测不出来,总是为2000,此时判断为无效*/
		if (ENVIParms.air_cond_temp_out > 1000)
		{
			ENVIParms.air_cond_temp_out = 0xFFFF;
		}

		/*读内温*/
		pointer = &ENVIParms.air_cond_temp_in;
		i = 2;	// 室内温度跳了1个寄存器
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, pointer);
		if (ENVIParms.air_cond_temp_in > 1000)
		{
			ENVIParms.air_cond_temp_in = 0xFFFF;
		}

		/*读电流值*/
		pointer = &ENVIParms.air_cond_amp;
		i = 5;	// 电流值多了5个寄存器
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, pointer);

		/*读电压值*/
		pointer = &ENVIParms.air_cond_volt;
		i = 6;	// 电压值多了6个寄存器
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, pointer);
	}
}

/******************************************************************************
 * 函数名:	comm_config_analyse 
 * 描述: 		读取温湿度
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 ******************************************************************************/
void comm_EnviAirAlarm_analyse(void)				 
{
	INT8U i=0;
	INT16U* pointer = &ENVIParms.air_cond_hightemp_alarm;

	if(CRC_check() && (g_PDUData.PDULength == (ENVI_AIRCOND_ALARM_NUM*2+5)))
	{
		for (i=0;i<4;i++)		// 高温、低温、高湿、低湿报警
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
		/*读电压值*/
		pointer = &ENVIParms.air_cond_infan_alarm;
		for (i=0;i<5;i++)		// 内外风机,压缩机,电加热,应急风机
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + (i+12)*2, (pointer+i));
		}
	}

}

/***********************************************************************
函数名:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

输入参数:		start_reg:	读取的寄存器开始地址
              reg_num:		读取的寄存器个数

输出参数:		无.

返回值:		无.

功能说明:              读取寄存器处理
***********************************************************************/
void comm_ask(INT16U station,USART_LIST buf_no,INT16U start_reg,INT8U reg_num,INT8U reg_type)
{
	INTEGER_UNION reg;
	
	reg.i = start_reg;	
	g_SENData.SENDBuffPtr = UARTBuf[buf_no].RxBuf;
	*g_SENData.SENDBuffPtr = station;

	if(reg_type!=READREG_COMMAND )
	{
		return;
	}
	else 
	{		
		*(g_SENData.SENDBuffPtr + 1) = reg_type;
	}	
	
	*(g_SENData.SENDBuffPtr + 2) = reg.b[1];				/*寄存器地址*/
	*(g_SENData.SENDBuffPtr + 3) = reg.b[0];
	
	*(g_SENData.SENDBuffPtr + 4) = 0x00;					/*寄存器数量*/
	*(g_SENData.SENDBuffPtr + 5) = reg_num;
			
	 reg.i = get_crc16(g_SENData.SENDBuffPtr,6);			/*加入CRC校验*/
	*(g_SENData.SENDBuffPtr + 6) = reg.b[0];
	*(g_SENData.SENDBuffPtr + 7) = reg.b[1];
	
	g_SENData.SENDLength= 8;
	g_TxDataCtr = 0;
	data_send_directly(buf_no);
} 


/***********************************************************************
函数名:		comm_wait(INT16U start_reg,INT8U reg_num,INT8U reg_type)

输入参数:		start_reg:	读取的寄存器开始地址
              reg_num:		读取的寄存器个数

输出参数:		无.

返回值:		无.

功能说明:              读取寄存器处理
***********************************************************************/
void comm_wait(USART_LIST destUtNo, UINT16 seq)
{	
	// 开始接收
	realDataChannelSelect(seq);
	USART_Cmd(UART5, ENABLE);
	// 不发送,直接准备接收
	StartCounterT100;					/*开始等待计数*/
	//g_SENData.SENDLength = 0;
	g_CommRxFlag = TRUE;            	/* 设置为接受状态*/
	if (destUtNo == UART4_COM)
	{
		rs485FuncSelect(RECEIVE_S);		//485默认为
	}
	g_PDUData.PDUBuffPtr = UARTBuf[destUtNo].RxBuf;
	g_PDUData.PDULength = 0;			// 准备接收
	UARTBuf[destUtNo].RecFlag = FALSE;	// 把上次的接收标志清掉
	//USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);		// 直接使用UART5的
} 


/***********************************************************************
函数名:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

输入参数:		start_reg:	读取的寄存器开始地址
              reg_num:		读取的寄存器个数

输出参数:		无.

返回值:		无.

功能说明:              读取寄存器处理
***********************************************************************/
void comm_ask_ups(INT16U station,USART_LIST buf_no,INT16U cid2,INT8U info_len,INT8U command)
{
	INT8U c_value[4] = {0,0,};
	USP_LENGTH_BITS uspLenthData;
	INT16U temp = 0;
	INT16U send_len = 0;
		
	g_SENData.SENDBuffPtr = UARTBuf[buf_no].RxBuf;
	
	*g_SENData.SENDBuffPtr = UPS_SOI;	// 电总协议0x7E开始
	send_len++;
	
	hex2_to_ascii(UPS_VER, c_value);
	*(g_SENData.SENDBuffPtr + 1) = c_value[1]; // 通讯版本号0x21
	*(g_SENData.SENDBuffPtr + 2) = c_value[0]; 
	send_len += 2;
	
	hex2_to_ascii(station, c_value);			// 设备地址
	*(g_SENData.SENDBuffPtr + 3) = c_value[1]; 
	*(g_SENData.SENDBuffPtr + 4) = c_value[0]; 
	send_len += 2;

	hex2_to_ascii(UPS_CID1, c_value);			// 设备地址
	*(g_SENData.SENDBuffPtr + 5) = c_value[1]; 
	*(g_SENData.SENDBuffPtr + 6) = c_value[0];
	send_len += 2;

	hex2_to_ascii(cid2, c_value);			// 设备地址
	*(g_SENData.SENDBuffPtr + 7) = c_value[1]; 
	*(g_SENData.SENDBuffPtr + 8) = c_value[0];
	send_len += 2;

	/*LENTH的计算*/
	uspLenthData.lenth_bits.lenid = info_len;
	uspLenthData.lenth_bits.lchksum = lchkSumCalc(uspLenthData.lenth_bits.lenid) &0x0F;
	hex4_to_ascii(uspLenthData.lenth_word, c_value);
	*(g_SENData.SENDBuffPtr + 9) = c_value[3]; 
	*(g_SENData.SENDBuffPtr + 10) = c_value[2];
	*(g_SENData.SENDBuffPtr + 11) = c_value[1]; 
	*(g_SENData.SENDBuffPtr + 12) = c_value[0];
	send_len += 4;

	/*g_SENData.SENDBuffPtr都已经转成了ASCII码*/
	temp = checkSumCalc(g_SENData.SENDBuffPtr+1, 12);	//SOI不算在内
	hex4_to_ascii(temp, c_value);
	*(g_SENData.SENDBuffPtr + 13) = c_value[3]; 
	*(g_SENData.SENDBuffPtr + 14) = c_value[2];
	*(g_SENData.SENDBuffPtr + 15) = c_value[1]; 
	*(g_SENData.SENDBuffPtr + 16) = c_value[0];
	send_len += 4;

	*(g_SENData.SENDBuffPtr + 17) = UPS_EOI;
	send_len++;

	g_SENData.SENDLength= send_len;
	g_TxDataCtr = 0;
	data_send_directly(buf_no);
} 

/***********************************************************************
函数名:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

输入参数:		start_reg:	读取的寄存器开始地址
              reg_num:		读取的寄存器个数

输出参数:		无.

返回值:		无.

功能说明:              读取寄存器处理
***********************************************************************/
void comm_ask_spd(INT16U station,USART_LIST buf_no,INT16U start_reg,INT8U reg_num,INT8U reg_type)
{
	INTEGER_UNION reg;
	
	reg.i = start_reg;	
	g_SENData.SENDBuffPtr = UARTBuf[buf_no].RxBuf;
	*g_SENData.SENDBuffPtr = station;
		
	*(g_SENData.SENDBuffPtr + 1) = reg_type;
	
	*(g_SENData.SENDBuffPtr + 2) = reg.b[1];				/*寄存器地址*/
	*(g_SENData.SENDBuffPtr + 3) = reg.b[0];
	
	*(g_SENData.SENDBuffPtr + 4) = 0x00;					/*寄存器数量*/
	*(g_SENData.SENDBuffPtr + 5) = reg_num;
			
	 reg.i = get_crc16(g_SENData.SENDBuffPtr,6);			/*加入CRC校验*/
	*(g_SENData.SENDBuffPtr + 6) = reg.b[0];
	*(g_SENData.SENDBuffPtr + 7) = reg.b[1];
	
	g_SENData.SENDLength= 8;
	g_TxDataCtr = 0;
	data_send_directly(buf_no);
} 


//用软件计算CRC4函数
void CalulateCRCbySoft(INT8U *pucData,INT8U wLength,INT8U *pOutData)
{
	INT8U ucTemp;
	INT16U wValue;
	INT16U crc_tbl[16]={0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef};  //四位余式表

	wValue=0;

	//本字节的CRC余式等于上一字节的CRC余式的低12位左移4位后，
	//再加上上一字节CRC余式右移4位（也既取高4位）和本字节之和后所求得的CRC码
	while(wLength--!=0)
	{
		//根据四位CRC余式表，先计算高四位CRC余式
		ucTemp=((wValue>>8))>>4;
		wValue<<=4;
		wValue^=crc_tbl[ucTemp^((*pucData)>>4)];
		//再计算低四位余式
		ucTemp=((wValue>>8))>>4;
		wValue<<=4;
		wValue^=crc_tbl[ucTemp^((*pucData)&0x0f)];
		pucData++;
	}
	pOutData[0]=wValue;	
	pOutData[1]=(wValue>>8);
    
}

/***********************************************************************
函数名:		comm_ask_locker(INT16U start_reg,INT8U reg_num,INT8U reg_type)

输入参数:		info_len:	净荷长度
              msg:		消息参数

输出参数:		无.

返回值:		无.

功能说明:       生久锁命令函数
***********************************************************************/
void comm_ask_locker(INT16U station,USART_LIST buf_no,INT16U signal,INT16U info_len,INT16U msg)
{
	INT8U lock_chksum[2] = {0,0};
	INT16U send_len = 0;
		
	g_SENData.SENDBuffPtr = UARTBuf[buf_no].RxBuf;
	
	*(g_SENData.SENDBuffPtr+LOCK_SQU_SOI) = LOCK_SOI;
	send_len++;
	*(g_SENData.SENDBuffPtr+LOCK_SQU_DES_STA) = station;
	send_len++;
	*(g_SENData.SENDBuffPtr+LOCK_SQU_SRC_STA) = station;
	send_len++;
	*(g_SENData.SENDBuffPtr+LOCK_SQU_DES_THRD) = LOCK_DES_THREAD;
	send_len++;
	*(g_SENData.SENDBuffPtr+LOCK_SQU_SRC_THRD) = LOCK_SRC_THREAD;
	send_len++;
	*(g_SENData.SENDBuffPtr+LOCK_SQU_LENTH_HIGH) = info_len>>8;
	send_len++;
	*(g_SENData.SENDBuffPtr+LOCK_SQU_LENTH_LOW) = (info_len&0xFF);
	send_len++;
	*(g_SENData.SENDBuffPtr+LOCK_SQU_SIGNAL) =  signal>>8;
	send_len++;
	*(g_SENData.SENDBuffPtr+LOCK_SQU_SIGNAL+1) = (signal&0xFF);
	send_len++;

	if (msg != NULL)
	{
		*(g_SENData.SENDBuffPtr+send_len) = msg>>8;
		send_len++;
		*(g_SENData.SENDBuffPtr+send_len) = (msg&0xFF);
		send_len++;
	}
	
	CalulateCRCbySoft(g_SENData.SENDBuffPtr+1, send_len-1,lock_chksum);
	

	*(g_SENData.SENDBuffPtr+send_len) = lock_chksum[1];
	send_len++;
	*(g_SENData.SENDBuffPtr+send_len) = lock_chksum[0];
	send_len++;
	*(g_SENData.SENDBuffPtr+send_len) = LOCK_EOI1;
	send_len++;
	*(g_SENData.SENDBuffPtr+send_len) = LOCK_EOI2;
	send_len++;
	
	g_SENData.SENDLength= send_len;
	g_TxDataCtr = 0;
	data_send_directly(buf_no);
}


/***********************************************************************
函数名:			void comm_polling_process(void)

输入参数:		无.

输出参数:		无.

返回值:			无.

功能说明:           通信处理函数,由模块的使用者根据通信负荷和频率的大小调用
***********************************************************************/
void comm_polling_process(void)
{
	static INT8U Init_sign = FALSE;
	static INT8U Init_send = FALSE;
	
	if( Init_sign == FALSE)	/*初始化装置参数*/
	{
		Init_sign = TRUE;
		comm_flag |= DEV_PARAM_SEND_FLAG_2;		/*先读取装置参数*/
	}

	/********************************************************************************************************************
	***此处对通信查询和设置指令冲突情况进行处理，SOE查询优先级最高，其次为参数 ***
	***设置，最后为参数查询，如在接收到正确的参数解析指令后，还未对装置参数解  ***
	***标志，所以此种方法在参数设置时并不能完全屏蔽参数解析，还需要在参数解析  ***
	***时做一定过滤，如装置参数过滤条件"if(!(comm_flag & PARAM_SET_FLAG))"，需要十分注意 !!!!    ***
	********************************************************************************************************************/
	 /*如果发现参数设置帧，则将接收标志以及发送标志都清零*/
	if (comm_flag & (DEV_PARAM_SET_FLAG_1 +DEV_PARAM_SET_FLAG_2 + DOOR_OPEN_SET_FLAG + DOOR_CLOSE_SET_FLAG) )		
	{
		// 轮询也取消掉
		comm_flag &= ~(REAL_DATA0_SEND_FLAG +REAL_DATA1_SEND_FLAG+REAL_DATA2_SEND_FLAG +REAL_DATA3_SEND_FLAG \
					     +UPS_PARAM_SEND_FLAG + UPS_IN_SEND_FLAG + UPS_OUT_SEND_FLAG + UPS_BAT_SEND_FLAG \
					     + UPS_STATUS_SEND_FLAG +UPS_ALARM_SEND_FLAG + SPD_STATUS_SEND_FLAG +SPD_TIMES_SEND_FLAG \
					     +ENVI_TEMP_SEND_FLAG +ENVI_AIRCOND_ONOFF_FLAG+ENVI_AIRCOND_TEMP_FLAG \
					     +ENVI_AIRCOND_ALARM_FLAG + DEV_PARAM_SEND_FLAG_1+ DEV_PARAM_SEND_FLAG_2\
					     +WATER_IN_FLAG);
		if(Recive_Flag>0)
		{
			WAIT_response_flag = 0;
			Recive_Flag = 0;  
			g_PDUData.PDULength = 0;				/*将接收帧长度置零会比较好*/
		}
		Com_err_counter = 0;
	}

	/**********************************接收到正确数据后参数解析******************************************/
	if(Recive_Flag)									/*如果接收到数据*/
	{
		switch(Recive_Flag)
		{
			case REAL_DATA0_ANALYSE:
				comm_RealData_analyse(CHANNEL_0);		/*接收到实时数据*/
				#if 0		// 在串口接收的时候已经校验了长度
				if (!comm_RealData_analyse(CHANNEL_0))		// 数据不对，再接收一次
				{
					if (last_rec_flag != Recive_Flag)		// 如果是第一次接收
					{
						try_again = TRUE;				//再接收一次
					}
				}
				last_rec_flag = Recive_Flag;
				#endif
			break;

			case REAL_DATA1_ANALYSE:					/*接收到实时数据*/
				comm_RealData_analyse(CHANNEL_1);
			break;

			case REAL_DATA2_ANALYSE:					/*接收到实时数据*/
				comm_RealData_analyse(CHANNEL_2);
			break;

			case REAL_DATA3_ANALYSE:					/*接收到实时数据*/
				comm_RealData_analyse(CHANNEL_3);
			break;
			
			case DEVICE_DATA_1_ANALYSE:				/*接收到装置参数*/
				if(!(comm_flag & DEV_PARAM_SET_FLAG_1))
				{
					// 装置参数
					comm_DeviceParam_1_analyse();
				}
			break;

			case DEVICE_DATA_2_ANALYSE:				/*接收到装置参数*/
				if(!(comm_flag & DEV_PARAM_SET_FLAG_2))
				{
					// 装置参数
					comm_DeviceParam_2_analyse();
				}
			break;
			
			case ENVI_TEMP_ANALYSE:
				comm_EnviTemp_analyse();
			break;

			case ENVI_AIR_ONOFF_ANALYSE:
				comm_EnviAirOnOff_analyse();
			break;

			case ENVI_AIR_TEMP_ANALYSE:
				comm_EnviAirTemp_analyse();
			break;

			case ENVI_AIR_ALARM_ANALYSE:
				comm_EnviAirAlarm_analyse();
			break;

			case UPS_PARAM_ANALYSE:	
				comm_UPSParam_ANALYSE();
			break;
			
			case UPS_IN_ANALYSE:	
				comm_UPSIn_ANALYSE();
			break;

			case UPS_OUT_ANALYSE:	
				comm_UPSOut_ANALYSE();
			break;

			case UPS_BAT_ANALYSE:	
				comm_UPSBat_ANALYSE();
			break;

			case UPS_ALARM_ANALYSE:	
				comm_UPSAlarm_ANALYSE();
			break;

			case UPS_STATUS_ANALYSE:	
				comm_UPSStatus_ANALYSE();
			break;

			case SPD_STATUS_ANALYSE:
				comm_SPDStatus_analyse();
			break;
			
			case SPD_TIMES_ANALYSE:
				comm_SPDTimes_analyse();
			break;

			case WATER_IN_ANALYSE:
				comm_water_in_analyse();
			break;
			
			default:
			break;
		}

		Recive_Flag = 0;
		WAIT_response_flag = 0;						/*将等待标志以及接收标志都清零*/
		g_PDUData.PDULength = 0;
	}
	/**************************************发送过程处理*********************************************/
	// 2个同时处理有bug
	if ((!WAIT_response_flag) && (g_CommRxFlag == FALSE))	
	{
		/********************************发送参数设置帧处理*************************************/
		/*空调参数设置为MODBUS通信*/
		if(comm_flag & DEV_PARAM_SET_FLAG_1)			
		{
			comm_flag &= ~DEV_PARAM_SET_FLAG_1;
			comm_DeviceParam_set_1();
			WAIT_response_flag = WAIT_PARAM_SET_1; 
		}

		else if(comm_flag & DEV_PARAM_SET_FLAG_2)			
		{
			comm_flag &= ~DEV_PARAM_SET_FLAG_2;
			comm_DeviceParam_set_2();
			WAIT_response_flag = WAIT_PARAM_SET_2; 
		}

		else if(comm_flag & DOOR_OPEN_SET_FLAG)			
		{
			comm_flag &= ~DOOR_OPEN_SET_FLAG;
			// 开锁
			comm_ask_locker(LOCK_ADDR_1, LOCKER_UART, LOCK_OPEN, 0x02, NULL);
			WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(comm_flag & DOOR_CLOSE_SET_FLAG)			
		{
			comm_flag &= ~DOOR_CLOSE_SET_FLAG;
			// 开锁
			comm_ask_locker(LOCK_ADDR_1, LOCKER_UART, LOCK_CLOSE, 0x02, NULL);
			WAIT_response_flag = WAIT_DOOR_CLOSE; 
		}
		
		// RSU数据
		else if (comm_flag & REAL_DATA0_SEND_FLAG)
		{
			comm_flag &= ~REAL_DATA0_SEND_FLAG;
			/*读取实时测量数据,电能数据等*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_0);
			WAIT_response_flag = REAL_DATA0_ANALYSE;
		}
		// RSU数据
		else if (comm_flag & REAL_DATA1_SEND_FLAG)
		{
			comm_flag &= ~REAL_DATA1_SEND_FLAG;
			/*读取实时测量数据,电能数据等*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_1);
			WAIT_response_flag = REAL_DATA1_ANALYSE;
		}
		// RSU数据
		else if (comm_flag & REAL_DATA2_SEND_FLAG)
		{
			comm_flag &= ~REAL_DATA2_SEND_FLAG;
			/*读取实时测量数据,电能数据等*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_2);
			WAIT_response_flag = REAL_DATA2_ANALYSE;
		}
		// RSU数据
		else if (comm_flag & REAL_DATA3_SEND_FLAG)
		{
			comm_flag &= ~REAL_DATA3_SEND_FLAG;
			/*读取实时测量数据,电能数据等*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_3);
			WAIT_response_flag = REAL_DATA3_ANALYSE;
		}

		else if (comm_flag & DEV_PARAM_SEND_FLAG_1)
		{
			comm_flag &= ~DEV_PARAM_SEND_FLAG_1;
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,AIR_ONOFF_REG, AIR_ONOFF_SET_NUM, READREG_COMMAND);	/*读取装置参数数据,电能数据等*/  
			WAIT_response_flag = DEVICE_DATA_1_ANALYSE;
		}

		else if (comm_flag & DEV_PARAM_SEND_FLAG_2)
		{
			comm_flag &= ~DEV_PARAM_SEND_FLAG_2;
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,AIR_TEMP_REG, AIR_TEMP_SET_NUM, READREG_COMMAND);	/*读取装置参数数据,电能数据等*/  
			if( Init_send == FALSE)	/*初始化装置参数*/
			{
				Init_send = TRUE;
				comm_flag |= DEV_PARAM_SEND_FLAG_1;
			}
			WAIT_response_flag = DEVICE_DATA_2_ANALYSE;
		}

		else if (comm_flag & UPS_PARAM_SEND_FLAG)
		{
			comm_flag &= ~UPS_PARAM_SEND_FLAG;
			//ups走的是电总协议
			/*读取UPS参数数据*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_ALL, 0, NULL);
			WAIT_response_flag = UPS_PARAM_ANALYSE;
		}
		else if (comm_flag & UPS_IN_SEND_FLAG)
		{
			comm_flag &= ~UPS_IN_SEND_FLAG;
			//ups走的是电总协议
			/*读取UPS参数数据*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_IN, 0, NULL);
			WAIT_response_flag = UPS_IN_ANALYSE;
		}
		else if (comm_flag & UPS_OUT_SEND_FLAG)
		{
			comm_flag &= ~UPS_OUT_SEND_FLAG;
			//ups走的是电总协议
			/*读取UPS参数数据*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_OUT, 0, NULL);
			WAIT_response_flag = UPS_OUT_ANALYSE;
		}
		else if (comm_flag & UPS_BAT_SEND_FLAG)
		{
			comm_flag &= ~UPS_BAT_SEND_FLAG;
			//ups走的是电总协议
			/*读取UPS参数数据*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_BAT, 0, NULL);
			WAIT_response_flag = UPS_BAT_ANALYSE;
		}
		// 报警信息轮询
		else if (comm_flag & UPS_ALARM_SEND_FLAG)
		{
			comm_flag &= ~UPS_ALARM_SEND_FLAG;
			//ups走的是电总协议
			/*读取UPS参数数据*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_ALARM, 0, NULL);
			WAIT_response_flag = UPS_ALARM_ANALYSE;
		}
		else if (comm_flag & UPS_STATUS_SEND_FLAG)
		{
			comm_flag &= ~UPS_STATUS_SEND_FLAG;
			//ups走的是电总协议
			/*读取UPS参数数据*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_STATUS, 0, NULL);
			WAIT_response_flag = UPS_STATUS_ANALYSE;
		}
		else if (comm_flag & SPD_STATUS_SEND_FLAG)
		{
			comm_flag &= ~SPD_STATUS_SEND_FLAG;
			comm_ask_spd(SPD_STATION_ADDRESS, SPD_UART,SPD_STATUS_REG, SPD_STATUS_NUM, SPD_DIREAD_COMMAND);	/*读取SPD参数数据*/  
			WAIT_response_flag = SPD_STATUS_ANALYSE;
		}

		else if (comm_flag & SPD_TIMES_SEND_FLAG)
		{
			comm_flag &= ~SPD_TIMES_SEND_FLAG;
			comm_ask_spd(SPD_STATION_ADDRESS, SPD_UART,SPD_TIMES_REG, SPD_TIMES_NUM, SPD_TIMESREAD_COMMAND);		/*读取SPD参数数据*/  
			WAIT_response_flag = SPD_TIMES_ANALYSE;
		}

		else if (comm_flag & ENVI_TEMP_SEND_FLAG)
		{
			comm_flag &= ~ENVI_TEMP_SEND_FLAG;
			comm_ask(TEMP_STATION_ADDRESS,TEMP_UART,ENVI_TEMP_REG, ENVI_TEMP_NUM, READREG_COMMAND);	/*读取温湿度数据*/  
			WAIT_response_flag = ENVI_TEMP_ANALYSE;
		}

		else if (comm_flag & ENVI_AIRCOND_ONOFF_FLAG)
		{
			comm_flag &= ~ENVI_AIRCOND_ONOFF_FLAG;
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,ENVI_AIRCOND_ONOFF_REG, ENVI_AIRCOND_ONOFF_NUM, READREG_COMMAND);	/*读取空调状态数据*/  
			WAIT_response_flag = ENVI_AIR_ONOFF_ANALYSE;
		}

		else if (comm_flag & ENVI_AIRCOND_TEMP_FLAG)
		{
			comm_flag &= ~ENVI_AIRCOND_TEMP_FLAG;
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,ENVI_AIRCOND_TEMP_REG, ENVI_AIRCOND_TEMP_NUM, READREG_COMMAND);	/*读取空调温度数据*/  
			WAIT_response_flag = ENVI_AIR_TEMP_ANALYSE;
		}

		else if (comm_flag & ENVI_AIRCOND_ALARM_FLAG)
		{
			comm_flag &= ~ENVI_AIRCOND_ALARM_FLAG;
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,ENVI_AIRCOND_ALARM_REG, ENVI_AIRCOND_ALARM_NUM, READREG_COMMAND);	/*读取空调报警数据*/  
			WAIT_response_flag = ENVI_AIR_ALARM_ANALYSE;
		}

		else if (comm_flag & WATER_IN_FLAG)
		{
			comm_flag &= ~WATER_IN_FLAG;
			comm_ask(WATERIN_STATION_ADDRESS, WATER_UART,WATER_IN_REG, WATER_IN_NUM, READREG_COMMAND);	/*读取空调报警数据*/  
			WAIT_response_flag = WATER_IN_ANALYSE;
		}
	}    
	
}

void ModbusServer_init(void)
{
	StoptCounterT100;
	g_CommRxFlag = FALSE; 	/*主站使能发送*/
	g_TxDataCtr = 0;
		
	g_PDUData.PDUBuffPtr = UARTBuf[REAL_DATA_UART].RxBuf;
	g_PDUData.PDULength =0;
}

/******************************************************************************
 * 函数名:	comm_soeinfo_analyse 
 * 描述: 		设置参数通信重发处理
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:		
 * 创建日期:
 * 
 ******************************************************************************/
 void comm_overtime_deal(void)
{
	switch(WAIT_response_flag)
	{
		case WAIT_PARAM_SET_1:	/*装置参数设置回复超时处理*/
			comm_flag |= DEV_PARAM_SET_FLAG_1;
		break;

		case WAIT_PARAM_SET_2:	/*装置参数设置回复超时处理*/
			comm_flag |= DEV_PARAM_SET_FLAG_2;
		break;
		
		default:
		break;
	}
	/*超时已处理，置无等待标志*/
	WAIT_response_flag = WAIT_NONE;
}

/**************************************************************************
函数名:	            data_received_handle

输入参数:		无

输出参数:		无

返回值:			无

函数说明:	      通信定时器计时函数，由该函数来
                                  维持MODBUS RTU规约中的T15和T35标志
***************************************************************************/
void data_received_handle(USART_LIST uartNo)
{
	if ((REAL_DATA_UART == uartNo) ||(AIR_COND_UART == uartNo)||(UPS_UART == uartNo) ||(TEMP_UART== uartNo))
	{
		/*一次有效的接收后，置该位为FALSE，防止在数据处理之前
		又来数据冲掉已保存在数据接收缓冲区中的数据*/
		g_CommRxFlag = FALSE; 
		Com_err_counter = 0;
			
		if(Com_err_flag == TRUE)
		{
			Com_err_flag = FALSE;					/*清除通信出错标志*/
		}
		Recive_Flag = WAIT_response_flag;			/*表示接收到数据*/
	}
}

/**************************************************************************
函数名:	            CommTimer

输入参数:		无

输出参数:		无

返回值:			无

函数说明:	      通信定时器计时函数，由该函数来
                                  维持MODBUS RTU规约中的T15和T35标志
***************************************************************************/
void CommTimer(void)
{
	if (g_T100_num >= 0)
	{
		if(g_T100_num > Wait_max_time)			//接收响应超时
		{
			StoptCounterT100;
			Com_err_counter++;				//通讯错误次数

			comm_overtime_deal();
			g_CommRxFlag = FALSE;			/*设置为发送状态*/  
																							
			if(Com_err_counter > 3)	// 重复轮询3次后还是没反应，错误状态		
			{
				Com_err_flag = TRUE;
			} 
		}
		else
		{
			g_T100_num++;
		}
	}
}

/******************************************************************************
 * 函数名:	start_comm 
 * 描述: 	设备作为服务器开始周期性读取外设数据
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 ******************************************************************************/
void start_comm(void)				
{
	static INT16U polling_counter = 0;	// 每5s进行一个类型参数的轮询

	polling_counter++;

	// 查询RSU数据
	if (polling_counter == 1)
	{
		comm_flag |= REAL_DATA0_SEND_FLAG;
	}
	else if (polling_counter ==2)
	{
		comm_flag |= REAL_DATA1_SEND_FLAG;
	}
	else if (polling_counter ==3)
	{
		comm_flag |= REAL_DATA2_SEND_FLAG;
	}
	else if (polling_counter ==4)
	{
		comm_flag |= REAL_DATA3_SEND_FLAG;
	}
	
	// 查询UPS数据
	else if (polling_counter ==5)
	{
		comm_flag |= UPS_PARAM_SEND_FLAG;
	}
	// 查询UPS数据
	else if (polling_counter ==6)
	{
		comm_flag |= UPS_IN_SEND_FLAG;
	}
	// 查询UPS数据
	else if (polling_counter ==7)
	{
		comm_flag |= UPS_OUT_SEND_FLAG;
	}
	// 查询UPS数据
	else if (polling_counter ==8)
	{
		comm_flag |= UPS_BAT_SEND_FLAG;
	}
	// 查询UPS数据,不支持查询
	else if (polling_counter ==9)
	{
		comm_flag |= UPS_ALARM_SEND_FLAG;
	}
	// 查询UPS数据
	else if (polling_counter ==10)
	{
		comm_flag |= UPS_STATUS_SEND_FLAG;
	}
	
	// 查询参数数据1
	else if (polling_counter == 11)
	{
		comm_flag |= DEV_PARAM_SEND_FLAG_1;
	}
	// 查询参数数据2
	else if (polling_counter == 12)
	{
		comm_flag |= DEV_PARAM_SEND_FLAG_2;
	}
	// 查询温湿度数据
	else if (polling_counter ==13)
	{
		comm_flag |= ENVI_TEMP_SEND_FLAG;
	}
	// 查询空调状态数据
	else if (polling_counter ==14)
	{
		comm_flag |= ENVI_AIRCOND_ONOFF_FLAG;
	}
	// 查询空调温度数据
	else if (polling_counter ==15)
	{
		comm_flag |= ENVI_AIRCOND_TEMP_FLAG;
	}
	// 查询空调报警数据
	else if (polling_counter ==16)
	{
		comm_flag |= ENVI_AIRCOND_ALARM_FLAG;
	}
	// 查询SPD状态数据
	else if (polling_counter ==17)
	{
		comm_flag |= SPD_STATUS_SEND_FLAG;
	}
	// 查询SPD次数数据
	else if (polling_counter ==18)
	{
		comm_flag |= SPD_TIMES_SEND_FLAG;
	}
	else if (polling_counter ==19)
	{
		comm_flag |= WATER_IN_FLAG;
		polling_counter = 0;
	}
}


