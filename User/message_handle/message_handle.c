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

const char VerInfo[]="LC-301-V4.1-190316";

/*定义通信缓冲区*/
PROTOCOL_BUF ProtocolBuf[UART_NUM];

#if 0
static uint8_t SpkBuf[COM_CTRL_LENTH];	// 用来存储上面发来的信息，主要是用于语音报价
/******************************************************************************
 * 函数名:	message_unpack 
 * 描述: 对收到的信息按照规定的协议进行解包,提取。
 *
 * 输入参数: uart_no, 目标串口, buf,源缓存
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
static uint8_t message_unpack(USART_LIST uart_no,PROTOCOL_BUF *buf)
{
	uint8_t *rx_buf;
	uint8_t  i = 0;
	uint8_t err  = ERR_OK;

	rx_buf = buf->pRxBuf;


	/* A类信息: 起始码+通信类+设备地址+控制字节1+金额(4字节)*/
	/* +余额(4字节)+入口站(6字节) +车型+车种+控制字节2+通讯序号*/
	/* +结束码+校验码 (共24个 字节)*/
	
	/* 提取相关信息*/
	for (i = 0; i<COM_CTRL_LENTH; i++)
	{
		SpkBuf[i]= rx_buf[i];
	}

	/*回复之前先更新外设状态*/
	detect_ALG_TTL_working();
	/* 信息正常,先对上位机PC进行回复*/
	//message_pack(uart_no, B_RES_MSG,buf);

	/* 读取有用的控制信息*/
	/* 控制字节1:	*/
	// 0位: 起栏杆--1为抬杆，0为保持
	// 1位: 落栏杆--1为降杆，0为保持
	// 2位: 顶棚灯--1为绿灯，0为红灯
	// 3位: 声报警--1为声报警，0为声报警取消
	// 4位: 光报警--1为光报警，0为光报警取消
	// 5位: 通行灯--1为绿灯，0为红灯
	// 6位: 未定义--备用
	// 7位: 未定义--备用
	device_control_used.control_word[USED]=SpkBuf[CTRL_T];  // 控制字节
	
	/* 控制字节2:	*/
	// 0位: 栏杆控制位  --1:车辆过后自动降杆模式，0:车辆过后手动降杆模式
	// 1位: 费额显示位  --1:费额显示，            0:费额不显示
	// 2位: 语音报价位  --1:报价，                0:不报价
	// 3位: 费额显示保持--1:保持，            0:刷新，当为1时，忽略bit1
	// 4位: 费显选择位  --1:广东版本，无保持功能，0:山西版本有保持功能，本控制需要费显支持
	// 5位: 车控器属性  --1:用于出口，            0:用于入口
	device_control_used.control_word[BACKUP]=SpkBuf[CTRL_B];

	return err;
}


/******************************************************************************
 * 函数名:	message_check 
 * 描述: 对收到的信息进行校验,是否符合规定的协议
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
static uint8_t message_check(PROTOCOL_BUF *buf)
{
	uint8_t *rx_buf;
	uint16_t rx_len;
	uint8_t i,j;
	uint8_t err=0;

	rx_buf = buf->pRxBuf;
	rx_len = buf->RxLen;

	if ((rx_len != COM_CTRL_LENTH) && (rx_len != COM_FX1_LENTH) 
		&& (rx_len != COM_FX2_LENTH) && (rx_len != COM_FX3_LENTH))
	{
		err = LENGTH_ERROR;
	}
	else
	{
		if((rx_buf[BSOF] == MSG_SOF) && (rx_buf[BEOF] == MSG_EOF))
		{
			j=rx_buf[COM_T];	// J用来计算异或值
			for(i=2; i <= (rx_len-3); i++)
			{
				j = rx_buf[i]^j;
			}
			if(j == rx_buf[rx_len-1])
			{
				//err = ERR_OK;
				err = TRANS_REQ;
				if (rx_buf[ADDR] != LOCAL_ADD)
				{
					err = SITEID_ERROR;
				}
			}
			else
			{
				err = CRC_ERROR;
			}
		}
		#if 0
		/*如果是费显信息*/
		else if ((rx_buf[BSOF] == FX_SOF) && (rx_buf[rx_len-2] ==FX_EOF))
		{
			 if (rx_buf[COM_T] == FX_LED)
			 {
			 	if (rx_buf[3] == 0)		// 如果是关闭红绿灯
			 	{
			 		err = TRANS_REQ;	// 直接转发
			 	}
				else
				{
					j=rx_buf[COM_T];	// J用来计算异或值
					for(i=2; i <= (rx_len-3); i++)
					{
						j = rx_buf[i]^j;
					}

					if(j == rx_buf[rx_len-1])
					{
						err = DUMMY_ERROR;	// 直接在这里处理
						/*因为测试软件发来的红绿灯费显有bug,亮度总是为0*/
						if (rx_buf[3] == 0x01)
						{
							message_pack(FEE_UART, FEE_R_MSG,buf);
							message_send_printf(FEE_UART, FALSE,0xFF);
						}
						else
						{
							message_pack(FEE_UART, FEE_G_MSG,buf);
							message_send_printf(FEE_UART, FALSE,0xFF);
						}
					}
				}
			 	
			 }
			 else
			 {
			 	/*只有操作码有问题，除操作码以外的其它命令都直接转发*/
			 	err = TRANS_REQ;	// 直接转发
			 }
		}
		#endif
		else
		{
			err = SOF_ERROR;
		}
	}
	return err;
}

 /******************************************************************************
 * 函数名:	message_process 
 * 描述: 信息处理函数, 对收到的信息不同情况进行不同处理.
 *
 * 输入参数: seq, 表明是第几个串口, buf,串口对应的buf
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
static uint8_t message_process(USART_LIST uart_no)
{
	uint8_t err = 0;
	PROTOCOL_BUF *sbuf = &ProtocolBuf[uart_no]; // source buf

	err = message_check(sbuf);

	if ( err == ERR_OK)
	{
		/*如果不是控制命令就不进入后面的执行函数*/
		err = message_unpack(uart_no, sbuf);
	}
	else if ((err == SITEID_ERROR) ||(err == DUMMY_ERROR)||(err == TRANS_REQ))
	{
		;  // 不是发给本机的,或者留给上一层处理,什么都不做
	}
	else 
	{
		// 如果是错误的信息处理
		message_pack(uart_no, ERR_RES_MSG,sbuf);
	}

	return err;
}

#endif
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
 * 函数名:	Bits_Swap 
 * 描述: 交换字节中的2个位
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2019.03.10
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
 uint8_t Bits_Swap(uint8_t vbyte, uint8_t num1, uint8_t num2)
{
	uint8_t tmp_v1 = 0;
	uint8_t tmp_re = 0;
	uint8_t i = 0;

	tmp_v1 = vbyte;
	for (i = 0; i< 8; i++)
	{
		
		if (i == num1)
		{	
			tmp_re |= ((tmp_v1 >> i)&0x01)<<num2;
		}
		else if (i == num2)
		{
			tmp_re |= ((tmp_v1 >> i)&0x01)<<num1;
		}
		else
		{
			/*取剩下位*/
			tmp_re |= ((tmp_v1 >> i)&0x01)<<i;
		}
	}
	return tmp_re;
}


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
	//uint8_t xor_t = 0;
	uint8_t i = 0;
	//uint8_t temp_v = 0;

	switch (msg_type)
	{
	/* 透传,直接拷贝*/
	case TRANS_MSG:
		for (len = 0; len < buf->RxLen; i++)
		{
			pbuf[len] = buf->pRxBuf[len];
			len++;
		}
		break;
		
#ifdef TEST
	case TEST_MSG:
		pbuf[len++] = 0x80;
		pbuf[len++] = 0x3f;
		pbuf[len++] = 0x30;
		pbuf[len++] = 0x5b;
		pbuf[len++] = 0x4f;
		pbuf[len++] = 0x66;
		pbuf[len++] = 0x6d;
		pbuf[len++] = 0x7d;
		pbuf[len++] = 0x07;
		pbuf[len++] = 0x7f;
		pbuf[len++] = 0x01;
		break;
#endif

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

	// 是否在这里打包数据?
	if (pack_en)
	{
		message_pack(destUtNo, msg_type, pScbuf);
	}

	LED_Set(LED_COM, ON); 	// 开始通信指示
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
}

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
 * 函数名:	Comm_Proc 
 * 描述: -通讯处理函数,主程序中调用,主要处理RS 485口数据,
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
void Comm_Proc(void)
{
	//uint8_t err = ERR_OK;
	USART_LIST i = BD1_UART;

	/*注意不能改成i < pc_com[PC_USART_NUM], 数组溢出*/
	for (i = bd_com[0]; i <= bd_com[BD_USART_NUM-1]; i++)
	{
		if (UARTBuf[i].RecFlag)		                      //RS485口有数据
		{
			// 上位机过来的是主站轮询信息
			if (i == PC_UART)
			{
				UARTBuf[i].RecFlag = 0;		//接收数据已处理，清除相关标志
				ProtocolBuf[i].pTxBuf = UARTBuf[i].TxBuf;         //地址置换
				ProtocolBuf[i].pRxBuf = UARTBuf[i].RxBuf;
				ProtocolBuf[i].RxLen = UARTBuf[i].RxLen;
				ProtocolBuf[i].TxLen = 0;
				//UARTBuf[i].RxLen = 0;		//已经被读取到ProtocolBuf0.RxLen, 尽快清0

				modbus_rtu_process(&ProtocolBuf[i], DevParams.Address);	/*MODBUS通信协议处理*/

				UARTBuf[i].TxLen = ProtocolBuf[i].TxLen;  /*置换回来，处理物理层数据*/
				if(UARTBuf[i].TxLen >0)
				{
					message_send_printf(i, i, FALSE, NOT_USED_MSG);
					UARTBuf[i].TxLen = 0;
					//Rs485StartSend();
				}
				Delay_clk(50);
				UARTBuf[i].RxLen = 0;	        /*接收数据已处理，清除相关标志*/
			}

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
			else
			{
				// 轮询从站的回复
			}
		}
		params_modify_deal();	//后续的数据改变处理,注意它的调用在大循环里面
	}
}
/*********************************************END OF FILE**********************/
