/********************************************************************************
*  版权所有: 深圳市中电电力技术有限公司				
*
*  文件名:       ModbusServer.c
*
*
*  当前版本:                       0.1
*  
*  作者:                                 
*
*  完成日期:  
*
*
*  取代版本:                       无
*                 
*  作者:    无
*
*  完成日期:                       无
*
*
*  描述: 
*                           
*                           一类(与应用相关):MB_GetRegValue,MB_SetRegValue,FReadHook，FwriteHook，SecrateRegPreDeal
                                                                       write_tou_quarter, write_tou_date，DoPowerCal,CheckFrameBorad
*                           
*                          二类(与物理层相关):CommCfgPort，UARTInit，SendResponse,UART0_ISR(Comm.c),UART1_ISR(Comm.c)
*
*                           三类:与功能相关函数。其他函数
*
*
*  其它: 
*
*  历史: 
*                1. 
*                2. 
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
#define StoptCounterT100			(g_T100_num = -1)

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

INT8U g_CommRxFlag;          	/*注意需要初使化为TRUE，方便上电时接收使能*/
INT16U g_TxDataCtr;              	/*发送数据发送个数计数*/
//LONG32U CommAppFlag = 0;
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
 * 函数名:	comm_ProtectData_analyse 
 * 描述: 		保护参数的解析
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 *------------------------
 * 修改人:Amy Wen	
 * 修改内容: 使用指针对结构体变量赋值
 * 修改日期:2012-2-13
 * 修改人:CZH
 * 修改内容: 对新规约进行解析
 * 修改日期:2012-10-09
 ******************************************************************************/
void comm_RealData_analyse(void)			 
{
	INT8U i;
	INT16U * pointer = &RSUParams.phase[0].vln;/*第0相*/

	if(CRC_check() && (g_PDUData.PDULength == (REAL_DATA_NUM*2+5)))
	{		
		for (i=0;i<2;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}

		/*第1相*/
		pointer = &RSUParams.phase[1].vln;
		for(i = 7;i <= 8;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i-7));
		}

		/*第2相*/
		pointer = &RSUParams.phase[2].vln;
		for(i = 14;i <= 15;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i-14));
		}

		/*第3相*/
		pointer = &RSUParams.phase[3].vln;
		for(i = 21;i <= 22;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i-21));
		}

		/*第4相*/
		pointer = &RSUParams.phase[4].vln;
		for(i = 27;i <= 28;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i-27));
		}

		/*第5相*/
		pointer = &RSUParams.phase[5].vln;
		for(i = 35;i <= 36;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i-35));
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
 * 函数名:	comm_ProtectData_set 
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
 * 修改人:	CZH
 * 修改内容: 使用指针对结构体变量赋值
 * 修改日期:2012-10-10
 ******************************************************************************/
void comm_ProtectData_set(void)					/* 保护参数设置帧 */
{
	#if 0
	INTEGER_UNION int_value;
	INT16U *pointer = &protect_param.trip_revert_mode;
	INT8U i;
	
	int_value.i = PROTECT_REG;
	INT8U reg_num = PROTECT_PARAM_NUM;
	g_SENData.SENDBuffPtr = g_ComBuf;
	*g_SENData.SENDBuffPtr = STATION_ADDRESS;
	*(g_SENData.SENDBuffPtr + 1) = WRITEREG_COMMAND;
	*(g_SENData.SENDBuffPtr + 2) = int_value.b[1];
	*(g_SENData.SENDBuffPtr + 3) = int_value.b[0];		/* 寄存器地址 */
	*(g_SENData.SENDBuffPtr + 4) = 0X00;
	*(g_SENData.SENDBuffPtr + 5) = reg_num;
	*(g_SENData.SENDBuffPtr + 6) = reg_num*2;  
	
	for(i =0;i<3;i++)	/*保护跳闸告警复归方式,控制功能投退字*/
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM+ i*2,*(pointer+i));
	}
	/*保护功能跳闸告警投退字*/
	long_to_char(g_SENData.SENDBuffPtr+ SET_FRAME_HEAD_NUM + 3*2, protect_param.trip_key_word);
	long_to_char(g_SENData.SENDBuffPtr+ SET_FRAME_HEAD_NUM + 5*2, protect_param.alarm_key_word);	
	/*保护控制功能相关参数*/
	protect_param.black1 = 0;		/*保留位置0*/
	protect_param.black2 = 0;
	protect_param.black3 = 0;
	protect_param.black4 = 0;
	pointer = &protect_param.short_I_rate;
	for(i =1;i<=50;i++)
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM+ (6+i)*2,*(pointer+(i-1)));
	}
	/*R1出口控制字*/
	for(i=0;i<3;i++)
	{
		long_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM+ 57*2+4*i, protect_param.protect_link_switch[i]);
	}   
	/*R1出口展宽及新增保护控制参数等*/
	pointer = &protect_param.link_action_return_time1;
	for(i =0;i<30;i++)
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM+ (63+i)*2,*(pointer+ i));
	}
	/*CRC校验码*/
	int_value.i=get_crc16(g_SENData.SENDBuffPtr,SET_FRAME_HEAD_NUM+PROTECT_PARAM_NUM*2);
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+PROTECT_PARAM_NUM*2)= int_value.b[0];
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+PROTECT_PARAM_NUM*2+1)= int_value.b[1];
	g_SENData.SENDLength=SET_FRAME_HEAD_NUM+PROTECT_PARAM_NUM*2+2;
	SendResponse();
	#endif
}


/******************************************************************************
 * 函数名:	comm_DeviceParam_analyse 
 * 描述: 		可编程逻辑参数解析
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 *------------------------
 * 修改人:CZH
 * 修改内容: 对新规约进行解析
 * 修改日期:2012-10-16
 ******************************************************************************/
void comm_UPSData_analyse(void)	
{
	#if  0
	INT8U i=0;
	INT16U* pointer = &logic_param.element[0].on_off;

	if(CRC_check() && (g_PDUData.PDULength == (LONGIC_PARAM_NUM*2+5)))
	{			
		for(i=0;i<73;i++)		
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}				
	}
	#endif
}

/******************************************************************************
 * 函数名:	comm_DeviceParam_analyse 
 * 描述: 		可编程逻辑参数解析
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2009.1.6
 * 
 *------------------------
 * 修改人:CZH
 * 修改内容: 对新规约进行解析
 * 修改日期:2012-10-16
 ******************************************************************************/
void comm_SPDData_analyse(void)	
{
	#if 0
	INT8U i=0;
	INT16U* pointer = &logic_param.element[0].on_off;

	if(CRC_check() && (g_PDUData.PDULength == (LONGIC_PARAM_NUM*2+5)))
	{			
		for(i=0;i<73;i++)		
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}				
	}
	#endif
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
	INT16U* pointer = &ENVIParms.temp;

	if(CRC_check() && (g_PDUData.PDULength == (ENVI_TEMP_NUM*2+5)))
	{
		for (i=0;i<ENVI_TEMP_NUM;i++)
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
		for (i=0;i<ENVI_AIRCOND_ALARM_NUM;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
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

#if 0
/***********************************************************************
函数名:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

输入参数:		start_reg:	读取的寄存器开始地址
              reg_num:		读取的寄存器个数

输出参数:		无.

返回值:		无.

功能说明:              读取寄存器处理
***********************************************************************/
void comm_ask_ups(INT16U start_reg,INT8U reg_num,INT8U reg_type)
{
	INTEGER_UNION reg;
	
	reg.i = start_reg;	
	g_SENData.SENDBuffPtr = UARTBuf[UPS_UART].RxBuf;
	*g_SENData.SENDBuffPtr = RSU_STATION_ADDRESS;

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
	//SendResponse();
} 


/***********************************************************************
函数名:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

输入参数:		start_reg:	读取的寄存器开始地址
              reg_num:		读取的寄存器个数

输出参数:		无.

返回值:		无.

功能说明:              读取寄存器处理
***********************************************************************/
void comm_ask_spd(INT16U start_reg,INT8U reg_num,INT8U reg_type)
{
	INTEGER_UNION reg;
	
	reg.i = start_reg;	
	g_SENData.SENDBuffPtr = UARTBuf[UPS_UART].RxBuf;
	*g_SENData.SENDBuffPtr = RSU_STATION_ADDRESS;

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
	//SendResponse();
} 
#endif
/***********************************************************************
函数名:			void comm_process(void)

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
		//comm_flag |= PROTECT_INIT_FLAG;			/*初始化保护参数*/
		//comm_flag &= ~RANDOM_SEND_FLAG;		/*在初始化装置参数前紧急刷新实时数据*/
	}

	/********************************************************************************************************************
	***此处对通信查询和设置指令冲突情况进行处理，SOE查询优先级最高，其次为参数 ***
	***设置，最后为参数查询，如在接收到正确的参数解析指令后，还未对装置参数解  ***
	***析，此时又接收到设置参数指令，这种情况下取消对	参数解析，直接进行设置	    ***
	***若参数设置指令与SOE查询指令冲突时，SOE查询指令优先,因Recive_Flag是在中断中置     ***
	***标志，所以此种方法在参数设置时并不能完全屏蔽参数解析，还需要在参数解析  ***
	***时做一定过滤，如装置参数过滤条件"if(!(comm_flag & PARAM_SET_FLAG))"，需要十分注意 !!!!    ***
	********************************************************************************************************************/
	 /*如果发现参数设置帧，则将接收标志以及发送标志都清零*/
	if (comm_flag & (DEV_PARAM_SET_FLAG_1 +DEV_PARAM_SET_FLAG_2 + DOOR_OPEN_SET_FLAG + DOOR_CLOSE_SET_FLAG) )		
	{
		// 轮询也取消掉
		comm_flag &= ~(REAL_DATA_SEND_FLAG +UPS_PARAM_SEND_FLAG +SPD_STATUS_SEND_FLAG +SPD_TIMES_SEND_FLAG \
					     +ENVI_TEMP_SEND_FLAG +ENVI_AIRCOND_ONOFF_FLAG+ENVI_AIRCOND_TEMP_FLAG \
					     +ENVI_AIRCOND_ALARM_FLAG + DEV_PARAM_SEND_FLAG_1+ DEV_PARAM_SEND_FLAG_2);
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
			case REAL_DATA_ANALYSE:					/*接收到实时数据*/
				comm_RealData_analyse();
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

	#if 0
			case UPS_DATA_ANALYSE:	
				comm_UPSData_analyse();
			break;
			
			case SPD_DATA_ANALYSE:
				comm_SPDData_analyse();
			break;
	#endif
			
			default:
			break;
		}
		
		Recive_Flag = 0;
		WAIT_response_flag = 0;						/*将等待标志以及接收标志都清零*/
		g_PDUData.PDULength = 0;
	}
	/**************************************发送过程处理*********************************************/
	// 2个同时处理有bug
	if((!WAIT_response_flag) && (g_CommRxFlag == FALSE))	
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
		
		// RSU数据，这个是MODBUS协议
		else if (comm_flag & REAL_DATA_SEND_FLAG)
		{
			comm_flag &= ~REAL_DATA_SEND_FLAG;
			/*读取实时测量数据,电能数据等*/  
			comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			WAIT_response_flag = REAL_DATA_ANALYSE;
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
			#if 0
			comm_flag &= ~UPS_PARAM_SEND_FLAG;
			//ups走的是电总协议
			comm_ask_ups(UPS_REG, UPS_DATA_NUM, READREG_COMMAND);	/*读取UPS参数数据*/  
			WAIT_response_flag = UPS_DATA_ANALYSE;
			#endif
		}

		else if (comm_flag & SPD_STATUS_SEND_FLAG)
		{
			#if 0
			comm_flag &= ~SPD_STATUS_SEND_FLAG;
			comm_ask_spd(SPD_STATUS_REG, SPD_STATUS_NUM, 0x02);	/*读取SPD参数数据*/  
			WAIT_response_flag = SPD_STATUS_ANALYSE;
			#endif
		}

		else if (comm_flag & SPD_TIMES_SEND_FLAG)
		{
			#if 0
			comm_flag &= ~SPD_TIMES_SEND_FLAG;
			comm_ask_spd(SPD_TIMES_REG, SPD_STATUS_NUM, 0x04);		/*读取SPD参数数据*/  
			WAIT_response_flag = SPD_TIMES_ANALYSE;
			#endif
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
	}    
	
}

void ModbusServer_init(void)
{
	//g_ENDT15Flag=FALSE;
	//g_ENDT35Flag=FALSE;
	//g_T35_num  = -1;
	//g_T15_num= -1;
	StoptCounterT100;
	g_CommRxFlag = FALSE; 	/*主站使能发送*/
	g_TxDataCtr = 0;
		
	g_PDUData.PDUBuffPtr = UARTBuf[MEAS_UART].RxBuf;
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
 * 作者:		CZH
 * 创建日期:	2012.10.10
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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
	if ((MEAS_UART == uartNo) ||(AIR_COND_UART == uartNo)||(UPS_UART == uartNo) ||(TEMP_UART== uartNo))
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
		if(g_T100_num > Wait_max_time)				//接收响应超时
		{
			StoptCounterT100;
			Com_err_counter++;						//通讯错误次数

			comm_overtime_deal();
			g_CommRxFlag = FALSE;					/*设置为发送状态*/  
																							
			if(Com_err_counter > 4)		
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
 * 描述: 		界面1s刷新，前500ms刷新数据，后500ms刷新SOE纪录
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
void start_comm(void)				
{
	static INT16U polling_counter = 0;	// 每5s进行一个类型参数的轮询

	polling_counter++;

	// 查询RSU数据
	if (polling_counter == 1)
	{
		comm_flag |= REAL_DATA_SEND_FLAG;
	}
	// 查询参数数据1
	else if (polling_counter == 2)
	{
		comm_flag |= DEV_PARAM_SEND_FLAG_1;
	}
	// 查询参数数据2
	else if (polling_counter == 3)
	{
		comm_flag |= DEV_PARAM_SEND_FLAG_2;
	}
	// 查询温湿度数据
	else if (polling_counter ==4)
	{
		comm_flag |= ENVI_TEMP_SEND_FLAG;
	}
	// 查询空调状态数据
	else if (polling_counter ==5)
	{
		comm_flag |= ENVI_AIRCOND_ONOFF_FLAG;
	}
	// 查询空调温度数据
	else if (polling_counter ==6)
	{
		comm_flag |= ENVI_AIRCOND_TEMP_FLAG;
	}
	// 查询空调报警数据
	else if (polling_counter ==7)
	{
		comm_flag |= ENVI_AIRCOND_ALARM_FLAG;
		polling_counter = 0;
	}
}


