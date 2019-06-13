 /**
  ******************************************************************************
  * @file    message_handle.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   串口信息应用层处理,包括打包和解包函数.
  *
  ******************************************************************************
  */
  
#include "include.h"

/*定义通信缓冲区*/
PROTOCOL_BUF ProtocolBuf[UART_NUM];

/******************************************************************************
 * 函数名:	Comm1_Init 
 * 描述: 串口1的初始化
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Comm1_Init(uint32_t baudrate)
{
	// 物理层初始化
	USART1_Init(baudrate);
	/*初始化协议层缓冲区数据*/
	ProtocolBuf[UART1_COM].pTxBuf = UARTBuf[UART1_COM].TxBuf;
	ProtocolBuf[UART1_COM].pRxBuf = UARTBuf[UART1_COM].RxBuf;
	ProtocolBuf[UART1_COM].RxLen = 0;
	ProtocolBuf[UART1_COM].TxLen = 0;
}
#if (BD_USART_NUM >= 2)
/******************************************************************************
 * 函数名:	Comm2_Init 
 * 描述: 串口2的初始化
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
 void Comm2_Init(uint32_t baudrate)
{
	// 物理层初始化
	USART2_Init(baudrate);
	/*初始化协议层缓冲区数据*/
	ProtocolBuf[UART2_COM].pTxBuf = UARTBuf[UART2_COM].TxBuf;
	ProtocolBuf[UART2_COM].pRxBuf = UARTBuf[UART2_COM].RxBuf;
	ProtocolBuf[UART2_COM].RxLen = 0;
	ProtocolBuf[UART2_COM].TxLen = 0;
}
#endif

#if (BD_USART_NUM >= 3)
/******************************************************************************
 * 函数名:	Comm2_Init 
 * 描述: 串口2的初始化
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
 void Comm4_Init(uint32_t baudrate)
{
	// 物理层初始化
	USART4_Init(baudrate);
	/*初始化协议层缓冲区数据*/
	ProtocolBuf[UART4_COM].pTxBuf = UARTBuf[UART4_COM].TxBuf;
	ProtocolBuf[UART4_COM].pRxBuf = UARTBuf[UART4_COM].RxBuf;
	ProtocolBuf[UART4_COM].RxLen = 0;
	ProtocolBuf[UART4_COM].TxLen = 0;
	rs485FuncSelect(RECEIVE_S);
}
#endif

#if (BD_USART_NUM >= 4)
/******************************************************************************
 * 函数名:	Comm2_Init 
 * 描述: 串口2的初始化
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
 void Comm5_Init(uint32_t baudrate)
{
	// 物理层初始化
	USART5_Init(baudrate);
	/*初始化协议层缓冲区数据*/
	ProtocolBuf[UART5_COM].pTxBuf = UARTBuf[UART5_COM].TxBuf;
	ProtocolBuf[UART5_COM].pRxBuf = UARTBuf[UART5_COM].RxBuf;
	ProtocolBuf[UART5_COM].RxLen = 0;
	ProtocolBuf[UART5_COM].TxLen = 0;
}
#endif


/******************************************************************************
 * 函数名:	message_pack 
 * 描述: 信息的打包,形成协议格式数据
 *         		
 * 输入参数: uart_no, 目标输出串口, buf，数据处理串口
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void message_pack(USART_LIST destUtNo, uint8_t msg_type,PROTOCOL_BUF *buf)
{
	/*取得目标串口对应的发送缓存*/
	uint8_t *pbuf = ProtocolBuf[destUtNo].pTxBuf;	//buf->pTxBuf;
	//uint8_t *prx_buf = buf->pRxBuf;
	uint8_t len = 0;
	
	switch (msg_type)
	{
	/* 透传,直接拷贝*/
	case TRANS_MSG:
		for (len = 0; len < buf->RxLen; len++)
		{
			pbuf[len] = buf->pRxBuf[len];
			len++;
		}
		break;
		
	case NOT_USED_MSG:
		break;
	default:
		break;
	}

	/*更新发送长度*/
	//pbuf[len++] = '\0';
	ProtocolBuf[destUtNo].TxLen = len;
}
 
/******************************************************************************
 * 函数名:	message_send_printf 
 * 描述: 信息的发送,把缓存区的数据发送至串口
 * 		这是阻塞型的发送,发送时不能干其它任务.
 *
 * 输入参数:  把scUtNo对应的发送缓存数据发送到destUtNo口中
 *				只有透传才用到scUtNo, pScbuf.其它的无意义
 *	pack_en: 是否还需要打包? 
 *		TRUE: 是, 
 *		FALSE: 不需要打包，直接发送,此时msg_type没有意义
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void message_send_printf(USART_LIST destUtNo,USART_LIST scUtNo,bool pack_en, uint8_t msg_type)
{
	uint16_t i = 0;
	uint8_t ch = 0;
	PROTOCOL_BUF *pScbuf,*pDestbuf;
	USART_TypeDef *PUSART = USART1;

	pScbuf  = &ProtocolBuf[scUtNo];
	pDestbuf  = &ProtocolBuf[destUtNo];

	if (destUtNo == UART1_COM)
	{
		PUSART = USART1;
		LED_Set(LED_COM, ON);
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

	// 是否在这里打包数据?
	if (pack_en)
	{
		message_pack(destUtNo, msg_type, pScbuf);
	}

	//LED_Set(LED_COM, ON); 	// 开始通信指示
	/*和printf一样,是阻塞型的发送*/
	for (i = 0; i < pDestbuf->TxLen; i++)
	{
		ch = pDestbuf->pTxBuf[i];
		USART_SendData(PUSART, ch);
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(PUSART, USART_FLAG_TXE) == RESET);	
		
	}
	
	pDestbuf->TxLen = 0;
	LED_Set(LED_COM, OFF); 	// 通信完毕
	if (destUtNo == UART4_COM)
	{
		rs485FuncSelect(RECEIVE_S);		//485默认为
	}
}

/******************************************************************************
 * 函数名:	params_modify_deal 
 * 描述: 
 *            -MODBUS client接收数据后的处理,要在主循环里面调用
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:20190613
 * 
 ******************************************************************************/
void params_modify_deal(void)
{
	if((UARTBuf[PC_UART].TxLen == 0)&& (system_flag&COMM1_MODIFIED) )	/*修改了通信参数*/
	{
		Comm1_Init(Baud[DevParams.BaudRate_1]);
		system_flag&=~COMM1_MODIFIED;
	}
	
	if((UARTBuf[PC_UART].TxLen == 0)&& (system_flag&COMM2_MODIFIED) )	/*修改了通信参数*/
	{
		Comm2_Init(Baud[DevParams.BaudRate_2]);
		system_flag&=~COMM2_MODIFIED;
	}

	if((UARTBuf[PC_UART].TxLen == 0)&& (system_flag&COMM3_MODIFIED) )	/*修改了通信参数*/
	{
		Comm4_Init(Baud[DevParams.BaudRate_3]);
		system_flag&=~COMM3_MODIFIED;
	}

	if((UARTBuf[PC_UART].TxLen == 0)&& (system_flag&COMM4_MODIFIED) )	/*修改了通信参数*/
	{
		Comm5_Init(Baud[DevParams.BaudRate_4]);
		system_flag&=~COMM4_MODIFIED;
	}
	
	if( System_Reset &&(UARTBuf[PC_UART].TxLen == 0))/*装置复位放在最后,等待所有操作完成后进行重启*/
	{
		System_Reset = 0;
		INT_DISABLE();		// 防止复位命令被打断
		NVIC_SystemReset();	// 这个地方需要测试一下
	}
}

/******************************************************************************
 * 函数名:	comm_rec_proc 
 * 描述: -通讯接收数据处理函数,主程序中调用,处理RS232口数据,
 *				   若是本装置的数据则进入解包处理
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void comm_rec_proc(void)
{
	USART_LIST i = BD1_UART;

	if (UARTBuf[PC_UART].RecFlag)		                      //client的RS485口有数据
	{
		UARTBuf[i].RecFlag = 0;		//接收数据已处理，清除相关标志
		ProtocolBuf[i].pTxBuf = UARTBuf[i].TxBuf;         //地址置换
		ProtocolBuf[i].pRxBuf = UARTBuf[i].RxBuf;
		ProtocolBuf[i].RxLen = UARTBuf[i].RxLen;
		ProtocolBuf[i].TxLen = 0;

		modbus_rtu_process(&ProtocolBuf[i], DevParams.Address);	/*MODBUS通信协议处理*/

		UARTBuf[i].TxLen = ProtocolBuf[i].TxLen;  /*置换回来，处理物理层数据*/
		if(UARTBuf[i].TxLen >0)
		{
			message_send_printf(i, i, FALSE, NOT_USED_MSG);
			UARTBuf[i].TxLen = 0;
		}
		Delay_clk(50);
		UARTBuf[i].RxLen = 0;	        /*接收数据已处理，清除相关标志*/
	}
	params_modify_deal();	//后续的数据改变处理,注意它的调用在大循环里面

	#if 0
			err = message_process(i);		//通信协议处理
			if (err == TRANS_REQ)							// 需要透传的
			{
				message_send_printf(TRANS_UART, i, TRUE, TRANS_MSG);
			}

			UARTBuf[i].TxLen = ProtocolBuf[i].TxLen;  //置换回来，处理物理层数据
			if(UARTBuf[i].TxLen >0)
			{
				/*回复B/C信息给上位机*/
				message_send_printf(i, i,FALSE, 0xFF);
			}
			Delay_Ms(5);				// 稍微有点延时,可以不要

			/*放在括号内,只有收到新的信息才操作*/
			if (err == ERR_OK)
			{
				params_modify_deal();		//后续的数据改变处理
			}
	#endif
}
/*********************************************END OF FILE**********************/
