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
		rs485FuncSelect(SEND_S);		//485开始发送
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
 * 函数名:	comm_UPSParam_ANALYSE 
 * 描述: 		UPS概况数据解析
 *            -
 * 输入参数: 
 * 输出参数: 
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
 #define UPS_PARAM_RES_LEN		72			 // 0x42返回数据长度72
 #define VOLT_AIN_POS				15			// 15字节开始是A相输入电压数值
 #define VOLT_BIN_POS				(VOLT_AIN_POS+4)			// 19字节开始是B相电压数值
 #define VOLT_CIN_POS				(VOLT_BIN_POS+4)			// 23字节开始是C相电压数值

 #define VOLT_AOUT_POS			(VOLT_CIN_POS+4)			// 27字节开始是A相电压数值
 #define VOLT_BOUT_POS			(VOLT_AOUT_POS+4)		// 31字节开始是B相电压数值
 #define VOLT_COUT_POS			(VOLT_BOUT_POS+4)		// 35字节开始是C相电压数值

 #define AMP_AOUT_POS			(VOLT_COUT_POS+4)			// 27字节开始是A相电压数值
 #define AMP_BOUT_POS			(AMP_AOUT_POS+4)		// 31字节开始是B相电压数值
 #define AMP_COUT_POS			(AMP_BOUT_POS+4)		// 35字节开始是C相电压数值

 #define FREQ_OUT_POS			(AMP_COUT_POS+8)		// 35字节开始是C相电压数值
 
void comm_UPSParam_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.ups_in.volt_Ain;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_PARAM_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_PARAM_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_PARAM_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_AIN_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_BIN_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_CIN_POS);

		pointer = &UPSParams.ups_out.volt_Aout;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_AOUT_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_BOUT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_COUT_POS);

		pointer = &UPSParams.ups_out.amp_Aout;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_AOUT_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_BOUT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_COUT_POS);

		// 输出频率
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FREQ_OUT_POS);
	}
}


/******************************************************************************
 * 函数名:	comm_UPSIn_ANALYSE 
 * 描述: 		UPS输入数据解析
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
 * 修改内容: 对新规约进行解析
 * 修改日期:2012-10-16
 ******************************************************************************/
 #define UPS_IN_RES_LEN			96			 // 0x42返回数据长度72
 #define UPS_PHASE_NUM_IN_POS	19			// 输入相数
 #define AMP_AIN_POS				35			// 35字节开始是A相输入电流数值
 #define AMP_BIN_POS				(AMP_AIN_POS+4)			// 19字节开始是B相电压数值
 #define AMP_CIN_POS				(AMP_BIN_POS+4)			// 23字节开始是C相电压数值
 #define FREQ_IN_POS				(AMP_CIN_POS+4)

 #define FACT_AIN_POS				(FREQ_IN_POS+4)
 #define FACT_BIN_POS				(FACT_AIN_POS+4)
 #define FACT_CIN_POS				(FACT_BIN_POS+4)

 #define BYPASS_AIN_POS			(FACT_CIN_POS+4)
 #define BYPASS_BIN_POS			(BYPASS_AIN_POS+4)
 #define BYPASS_CIN_POS			(BYPASS_BIN_POS+4)
 #define BYPASS_FREQ_POS			(BYPASS_CIN_POS+16)	//跳过了线电压部分
 
void comm_UPSIn_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.ups_in.phase_num;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_IN_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_IN_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_IN_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ UPS_PHASE_NUM_IN_POS);
	
		pointer = &UPSParams.ups_in.amp_Ain;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_AIN_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_BIN_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_CIN_POS);
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FREQ_IN_POS);

		pointer = &UPSParams.ups_in.fact_Ain;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_AIN_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_BIN_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_CIN_POS);

		pointer = &UPSParams.ups_in.bypass_voltA;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_AIN_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_BIN_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_CIN_POS);

		// 旁路频率
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_FREQ_POS);
	}
}


/******************************************************************************
 * 函数名:	comm_UPSOut_ANALYSE 
 * 描述: 		UPS输出数据解析0xE1
 *            -
 * 输入参数: 
 * 输出参数: 
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
 #define UPS_OUT_RES_LEN			88			 // 0xE1返回数据长度72
 #define UPS_PHASE_NUM_OUT_POS	19			// 输入相数
 #define FACT_AOUT_POS			(UPS_PHASE_NUM_OUT_POS+4)			// 35字节开始是A相输入电流数值
 #define FACT_BOUT_POS			(FACT_AOUT_POS+4)			// 19字节开始是B相电压数值
 #define FACT_COUT_POS			(FACT_BOUT_POS+4)			// 23字节开始是C相电压数值

 #define KWH_AOUT_POS			(FACT_COUT_POS+16)
 #define KWH_BOUT_POS			(KWH_AOUT_POS+4)
 #define KWH_COUT_POS			(KWH_BOUT_POS+4)

 #define VAH_AOUT_POS			(KWH_COUT_POS+4)
 #define VAH_BOUT_POS			(VAH_AOUT_POS+4)
 #define VAH_COUT_POS			(VAH_BOUT_POS+4)

 #define LOAD_AOUT_POS			(VAH_COUT_POS+4)
 #define LOAD_BOUT_POS			(LOAD_AOUT_POS+4)
 #define LOAD_COUT_POS			(LOAD_BOUT_POS+4)
 
void comm_UPSOut_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.ups_out.phase_num;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_OUT_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_OUT_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_OUT_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ UPS_PHASE_NUM_OUT_POS);

		pointer = &UPSParams.ups_out.fact_Aout;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_AOUT_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_BOUT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_COUT_POS);

		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ KWH_AOUT_POS);
		*(pointer+4) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ KWH_BOUT_POS);
		*(pointer+5) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ KWH_COUT_POS);

		*(pointer+6) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VAH_AOUT_POS);
		*(pointer+7) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VAH_BOUT_POS);
		*(pointer+8) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VAH_COUT_POS);

		*(pointer+9) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ LOAD_AOUT_POS);
		*(pointer+10) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ LOAD_BOUT_POS);
		*(pointer+11) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ LOAD_COUT_POS);
	}
}


/******************************************************************************
 * 函数名:	comm_UPSBat_ANALYSE 
 * 描述: 		UPS电池数据解析0xE3
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
 * 修改内容: 对新规约进行解析
 * 修改日期:2012-10-16
 ******************************************************************************/
#define UPS_BAT_RES_LEN			72			 // 0x42返回数据长度72
#define BAT_RUNNING_DAY_POS		19			// 35字节开始是A相输入电流数值
#define BAT_VOLT_POS				(BAT_RUNNING_DAY_POS+4)			// 19字节开始是电池运行天数
#define BAT_CHARGE_AMP_POS		(BAT_VOLT_POS+4)
#define BAT_DISCHG_AMP_POS		(BAT_CHARGE_AMP_POS+4)
#define BAT_BACKUP_TIME_POS		(BAT_DISCHG_AMP_POS+16)
#define BAT_TMP_POS				(BAT_BACKUP_TIME_POS+4)	//电池温度,不支持
#define BAT_ENVI_TMP_POS			(BAT_TMP_POS+4)
#define BAT_VOLUMN_POS			(BAT_ENVI_TMP_POS+4)
#define BAT_DISCHG_TIMES_POS	(BAT_VOLUMN_POS+4)		//放电次数
#define BAT_CHKSUM_POS			(BAT_DISCHG_TIMES_POS+4)


void comm_UPSBat_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.battery.running_day;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_BAT_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_BAT_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_BAT_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_RUNNING_DAY_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_VOLT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_CHARGE_AMP_POS);
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_DISCHG_AMP_POS);
		*(pointer+4) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_BACKUP_TIME_POS);
		*(pointer+5) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_ENVI_TMP_POS);	// 环境温度
		*(pointer+6) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_VOLUMN_POS);		// 电池容量
		*(pointer+7) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_DISCHG_TIMES_POS);		// 放电次数
	}
}

#if 0
/******************************************************************************
 * 函数名:	comm_UPSTime_ANALYSE 
 * 描述: 		UPS电池运行时间解析0xE4
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
 * 修改内容: 对新规约进行解析
 * 修改日期:2012-10-16
 ******************************************************************************/
#define UPS_BATTIME_RES_LEN			56			 // 0xE4返回数据长度72



void comm_UPSTime_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.battery.running_day;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_BAT_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_BAT_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_BAT_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_RUNNING_DAY_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_VOLT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_CHARGE_AMP_POS);
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_DISCHG_AMP_POS);
		*(pointer+4) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_BACKUP_TIME_POS);
		*(pointer+5) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_ENVI_TMP_POS);	// 环境温度
		*(pointer+6) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_VOLUMN_POS);		// 电池容量
		*(pointer+7) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_DISCHG_TIMES_POS);		// 放电次数
	}
}
#endif


/******************************************************************************
 * 函数名:	comm_UPSStatus_ANALYSE 
 * 描述: 		UPS电池数据解析0x43
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
 * 修改内容: 对新规约进行解析
 * 修改日期:2012-10-16
 ******************************************************************************/
#define UPS_STATUS_RES_LEN		40			 // 0x42返回数据长度40
#define BAT_SUPPLY_OUT_POS		15			// 15字节开始是UPS输出供电方式
#define BAT_SUPPLY_IN_POS		(BAT_SUPPLY_OUT_POS+4)		// 19字节开始是UPS输入供电方式
#define BAT_STATUS_POS			(BAT_SUPPLY_IN_POS+2)		// 19字节开始是UPS输入供电方式

void comm_UPSStatus_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.status.supply_out_status;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_STATUS_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_STATUS_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_STATUS_RES_LEN))
	{
		*pointer = ascii_to_hex2(g_PDUData.PDUBuffPtr+ BAT_SUPPLY_OUT_POS);
		*(pointer+1) = 	ascii_to_hex2(g_PDUData.PDUBuffPtr+ BAT_SUPPLY_IN_POS);
		*(pointer+2) = 	ascii_to_hex2(g_PDUData.PDUBuffPtr+ BAT_STATUS_POS);
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
} 


/*根据lenth计算电总协议的LCHKSUM*/
 UINT16 lchkSumCalc(UINT16 len_value)
{
	INT16U re_value = 0;

	/*先对D0D1D2D3+D4D5D6D7+D8D9D10D11求和*/
	re_value = ((len_value&0x0F) + ((len_value>>4)&0x0F) + ((len_value>>8)&0x0F));
	re_value = (~(re_value&0x0F))+1;	// 再模16余数取反加1
	re_value = re_value&0x0F;

	return re_value;
}


/*根据lenth计算电总协议的CHECKSUM*/
 UINT16 checkSumCalc(UINT8 *buffer, UINT8 len)
{
	INT16U re_value = 0;
	INT8U i = 0;

	/*先对所有除SIO之外的求和*/
	for (i = 0; i<len; i++)
	{
		re_value = re_value+buffer[i];
	}
	// 模65535的余数+1
	re_value = (~(re_value&0xFFFF))+1;	// 再模65536余数取反加1

	return re_value;
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
	//static INT8U last_rec_flag = 0;
	//bool try_again = FALSE;
	
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
		comm_flag &= ~(REAL_DATA0_SEND_FLAG +REAL_DATA1_SEND_FLAG+REAL_DATA2_SEND_FLAG +REAL_DATA3_SEND_FLAG \
					     +UPS_PARAM_SEND_FLAG +SPD_STATUS_SEND_FLAG +SPD_TIMES_SEND_FLAG \
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

			case UPS_STATUS_ANALYSE:	
				comm_UPSStatus_ANALYSE();
			break;

			case SPD_STATUS_ANALYSE:
				comm_SPDStatus_analyse();
			break;
			
			case SPD_TIMES_ANALYSE:
				comm_SPDTimes_analyse();
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
		#if 0
		// 协议不支持
		else if (comm_flag & UPS_TIME_SEND_FLAG)
		{
			comm_flag &= ~UPS_TIME_SEND_FLAG;
			//ups走的是电总协议
			/*读取UPS参数数据*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_TIME, 0, NULL);
			WAIT_response_flag = UPS_TIME_ANALYSE;
		}
		#endif
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
		;
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
		polling_counter = 0;
	}
}


