 /**
  ******************************************************************************
  * @file    message_handle.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   ������ϢӦ�ò㴦��,��������ͽ������.
  *
  ******************************************************************************
  */
  
#include "include.h"

/*����ͨ�Ż�����*/
PROTOCOL_BUF ProtocolBuf[UART_NUM];
volatile uint16_t testVar = 0x01;

/******************************************************************************
 * ������:	Comm1_Init 
 * ����: ����1�ĳ�ʼ��
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Comm1_Init(uint32_t baudrate)
{
	// ������ʼ��
	USART1_Init(baudrate);
	/*��ʼ��Э��㻺��������*/
	ProtocolBuf[UART1_COM].pTxBuf = UARTBuf[UART1_COM].TxBuf;
	ProtocolBuf[UART1_COM].pRxBuf = UARTBuf[UART1_COM].RxBuf;
	ProtocolBuf[UART1_COM].RxLen = 0;
	ProtocolBuf[UART1_COM].TxLen = 0;
}
#if (BD_USART_NUM >= 2)
/******************************************************************************
 * ������:	Comm2_Init 
 * ����: ����2�ĳ�ʼ��
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
 void Comm2_Init(uint32_t baudrate)
{
	// ������ʼ��
	USART2_Init(baudrate);
	/*��ʼ��Э��㻺��������*/
	ProtocolBuf[UART2_COM].pTxBuf = UARTBuf[UART2_COM].TxBuf;
	ProtocolBuf[UART2_COM].pRxBuf = UARTBuf[UART2_COM].RxBuf;
	ProtocolBuf[UART2_COM].RxLen = 0;
	ProtocolBuf[UART2_COM].TxLen = 0;
}
#endif

#if (BD_USART_NUM >= 3)
/******************************************************************************
 * ������:	Comm2_Init 
 * ����: ����2�ĳ�ʼ��
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
 void Comm4_Init(uint32_t baudrate)
{
	// ������ʼ��
	USART4_Init(baudrate);
	/*��ʼ��Э��㻺��������*/
	ProtocolBuf[UART4_COM].pTxBuf = UARTBuf[UART4_COM].TxBuf;
	ProtocolBuf[UART4_COM].pRxBuf = UARTBuf[UART4_COM].RxBuf;
	ProtocolBuf[UART4_COM].RxLen = 0;
	ProtocolBuf[UART4_COM].TxLen = 0;
	//rs485FuncSelect(RS485_CTRL_1,SEL_R);
}
#endif

#if (BD_USART_NUM >= 4)
/******************************************************************************
 * ������:	Comm2_Init 
 * ����: ����2�ĳ�ʼ��
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
 void Comm5_Init(uint32_t baudrate)
{
	// ������ʼ��
	USART5_Init(baudrate);
	/*��ʼ��Э��㻺��������*/
	ProtocolBuf[UART5_COM].pTxBuf = UARTBuf[UART5_COM].TxBuf;
	ProtocolBuf[UART5_COM].pRxBuf = UARTBuf[UART5_COM].RxBuf;
	ProtocolBuf[UART5_COM].RxLen = 0;
	ProtocolBuf[UART5_COM].TxLen = 0;
	//rs485FuncSelect(RS485_CTRL_2,SEL_R);
}
#endif


/******************************************************************************
 * ������:	message_pack 
 * ����: ��Ϣ�Ĵ��,�γ�Э���ʽ����
 *         		
 * �������: uart_no, Ŀ���������, buf�����ݴ�����
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void message_pack(USART_LIST destUtNo, uint8_t msg_type,PROTOCOL_BUF *buf)
{
	/*ȡ��Ŀ�괮�ڶ�Ӧ�ķ��ͻ���*/
	uint8_t *pbuf = ProtocolBuf[destUtNo].pTxBuf;	//buf->pTxBuf;
	//uint8_t *prx_buf = buf->pRxBuf;
	uint8_t len = 0;
	
	switch (msg_type)
	{
	/* ͸��,ֱ�ӿ���*/
	case TRANS_MSG:
		for (len = 0; len < buf->RxLen; len++)
		{
			pbuf[len] = buf->pRxBuf[len];
			len++;
		}
		break;

	/*ר����������*/
	case TEST_MSG:
		pbuf[len++] = 0x02;
		pbuf[len++] = 0x03;		// ����pbuf[1] 
		pbuf[len++] = 0x05;		// ����pbuf[2] 
		
		pbuf[len++] = 0x64;
		pbuf[len++] = ++testVar;
		break;
		
	case NOT_USED_MSG:
		break;
	default:
		break;
	}

	/*���·��ͳ���*/
	//pbuf[len++] = '\0';
	ProtocolBuf[destUtNo].TxLen = len;
}
 
/******************************************************************************
 * ������:	message_send_printf 
 * ����: ��Ϣ�ķ���,�ѻ����������ݷ���������
 * 		���������͵ķ���,����ʱ���ܸ���������.
 *
 * �������:  ��scUtNo��Ӧ�ķ��ͻ������ݷ��͵�destUtNo����
 *				ֻ��͸�����õ�scUtNo, pScbuf.������������
 *	pack_en: �Ƿ���Ҫ���? 
 *		TRUE: ��, 
 *		FALSE: ����Ҫ�����ֱ�ӷ���,��ʱmsg_typeû������
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
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
		//rs485FuncSelect(RS485_CTRL_1,SEL_S);		//485��ʼ����
	}
#endif
#if (BD_USART_NUM >= 4)
	else if (destUtNo == UART5_COM)
	{
		PUSART = UART5;
		//rs485FuncSelect(RS485_CTRL_2,SEL_S);		//485��ʼ����
	}
#endif
	else
	{
		/* �������쳣���,ֱ�ӷ���*/
		return;
	}

	// �Ƿ�������������?
	if (pack_en)
	{
		message_pack(destUtNo, msg_type, pScbuf);
	}

	//LED_Set(LED_COM, ON); 	// ��ʼͨ��ָʾ
	/*��printfһ��,�������͵ķ���*/
	for (i = 0; i < pDestbuf->TxLen; i++)
	{
		ch = pDestbuf->pTxBuf[i];
		USART_SendData(PUSART, ch);
		/* �ȴ�������� */
		while (USART_GetFlagStatus(PUSART, USART_FLAG_TXE) == RESET);	
		
	}
	
	pDestbuf->TxLen = 0;
	LED_Set(LED_COM, OFF); 	// ͨ�����
	#if 0
	if ((destUtNo == UART4_COM) )
	{
		Delay_Ms(1);	// 9600������1ms ���ȴ�1���ֽ�
		//rs485FuncSelect(RS485_CTRL_1,SEL_R);	//�������ҪתΪ����
	}
	else if (destUtNo == UART5_COM)
	{
		Delay_Ms(1);	// 9600������1ms ���ȴ�1���ֽ�
		//rs485FuncSelect(RS485_CTRL_2,SEL_R);	//�������ҪתΪ����
	}
	#endif
}

/******************************************************************************
 * ������:	params_modify_deal 
 * ����: 
 *            -MODBUS client�������ݺ�Ĵ���,Ҫ����ѭ���������
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:20190613
 * 
 ******************************************************************************/
void params_modify_deal(USART_LIST uart_no)
{
#if (BD_USART_NUM >= 1)
	if((UARTBuf[uart_no].TxLen == 0)&& (system_flag&COMM1_MODIFIED) )	/*�޸���ͨ�Ų���*/
	{
		Comm1_Init(Baud[DevParams.BaudRate_1]);
		system_flag&=~COMM1_MODIFIED;
	}
#endif

#if (BD_USART_NUM >= 2)
	if((UARTBuf[uart_no].TxLen == 0)&& (system_flag&COMM2_MODIFIED) )	/*�޸���ͨ�Ų���*/
	{
		Comm2_Init(Baud[DevParams.BaudRate_2]);
		system_flag&=~COMM2_MODIFIED;
	}
#endif
	
#if (BD_USART_NUM >= 3)
	if((UARTBuf[uart_no].TxLen == 0)&& (system_flag&COMM3_MODIFIED) )	/*�޸���ͨ�Ų���*/
	{
		Comm4_Init(Baud[DevParams.BaudRate_3]);
		system_flag&=~COMM3_MODIFIED;
	}
#endif

#if (BD_USART_NUM >= 4)
	if((UARTBuf[uart_no].TxLen == 0)&& (system_flag&COMM4_MODIFIED) )	/*�޸���ͨ�Ų���*/
	{
		Comm5_Init(Baud[DevParams.BaudRate_4]);
		system_flag&=~COMM4_MODIFIED;
	}
#endif
	
	if( System_Reset &&(UARTBuf[uart_no].TxLen == 0))/*װ�ø�λ�������,�ȴ����в�����ɺ��������*/
	{
		System_Reset = 0;
		INT_DISABLE();		// ��ֹ��λ������
		NVIC_SystemReset();	// ����ط���Ҫ����һ��
	}
}

/******************************************************************************
 * ������:	comm_rec_proc 
 * ����: -ͨѶ�������ݴ�����,�������е���,����RS232������,
 *				   ���Ǳ�װ�õ����������������
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void comm_rec_proc(void)
{
	USART_LIST i = BD1_UART;
	USART_LIST validUt = UART1_COM;

	/*4�����κ�һ����������,��ִ��MODBUSЭ��*/
	for ( i = UART1_COM; i < UART_NUM; i++)
	{
		if (UARTBuf[i].RecFlag)		                      //client��RS485��������
		{
			if (test485Flag &&(i==testRxUt))
			{
				UARTBuf[i].RecFlag = 0;		//���������Ѵ��������ر�־
				ProtocolBuf[i].pTxBuf = UARTBuf[i].TxBuf;         //��ַ�û�
				ProtocolBuf[i].pRxBuf = UARTBuf[i].RxBuf;
				ProtocolBuf[i].RxLen = UARTBuf[i].RxLen;
				ProtocolBuf[i].TxLen = 0;
				// ������պͷ������,485�������
				if (!memcmp(ProtocolBuf[testRxUt].pRxBuf, ProtocolBuf[testTxUt].pTxBuf,ProtocolBuf[i].RxLen))
				{
					test485Flag = FALSE;
				}
			}
			else if (test232Flag &&(i==UART1_COM))
			{
				UARTBuf[i].RecFlag = 0;		//���������Ѵ��������ر�־
				ProtocolBuf[i].pTxBuf = UARTBuf[i].TxBuf;         //��ַ�û�
				ProtocolBuf[i].pRxBuf = UARTBuf[i].RxBuf;
				ProtocolBuf[i].RxLen = UARTBuf[i].RxLen;
				ProtocolBuf[i].TxLen = 0;
				// ������պͷ������,232�������
				if (!memcmp(ProtocolBuf[i].pRxBuf, ProtocolBuf[testTxUt].pTxBuf,ProtocolBuf[i].RxLen))
				{
					test232Flag = FALSE;
				}
			}
			else
			{
				UARTBuf[i].RecFlag = 0;		//���������Ѵ��������ر�־
				ProtocolBuf[i].pTxBuf = UARTBuf[i].TxBuf;         //��ַ�û�
				ProtocolBuf[i].pRxBuf = UARTBuf[i].RxBuf;
				ProtocolBuf[i].RxLen = UARTBuf[i].RxLen;
				ProtocolBuf[i].TxLen = 0;

				modbus_rtu_process(&ProtocolBuf[i], DevParams.Address);	/*MODBUSͨ��Э�鴦��*/

				UARTBuf[i].TxLen = ProtocolBuf[i].TxLen;  /*�û��������������������*/
				if(UARTBuf[i].TxLen >0)
				{
					message_send_printf(i, i, FALSE, NOT_USED_MSG);
					UARTBuf[i].TxLen = 0;
				}
				Delay_clk(50);
				UARTBuf[i].RxLen = 0;	        /*���������Ѵ��������ر�־*/
			}
			validUt = i;
		}
		params_modify_deal(validUt);	//���������ݸı䴦��,ע�����ĵ����ڴ�ѭ������
	}
}
/*********************************************END OF FILE**********************/
