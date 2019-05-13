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

const char VerInfo[]="LC-301-V4.1-190316";

/*����ͨ�Ż�����*/
PROTOCOL_BUF ProtocolBuf[UART_NUM];

#if 0
static uint8_t SpkBuf[COM_CTRL_LENTH];	// �����洢���淢������Ϣ����Ҫ��������������
/******************************************************************************
 * ������:	message_unpack 
 * ����: ���յ�����Ϣ���չ涨��Э����н��,��ȡ��
 *
 * �������: uart_no, Ŀ�괮��, buf,Դ����
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
static uint8_t message_unpack(USART_LIST uart_no,PROTOCOL_BUF *buf)
{
	uint8_t *rx_buf;
	uint8_t  i = 0;
	uint8_t err  = ERR_OK;

	rx_buf = buf->pRxBuf;


	/* A����Ϣ: ��ʼ��+ͨ����+�豸��ַ+�����ֽ�1+���(4�ֽ�)*/
	/* +���(4�ֽ�)+���վ(6�ֽ�) +����+����+�����ֽ�2+ͨѶ���*/
	/* +������+У���� (��24�� �ֽ�)*/
	
	/* ��ȡ�����Ϣ*/
	for (i = 0; i<COM_CTRL_LENTH; i++)
	{
		SpkBuf[i]= rx_buf[i];
	}

	/*�ظ�֮ǰ�ȸ�������״̬*/
	detect_ALG_TTL_working();
	/* ��Ϣ����,�ȶ���λ��PC���лظ�*/
	//message_pack(uart_no, B_RES_MSG,buf);

	/* ��ȡ���õĿ�����Ϣ*/
	/* �����ֽ�1:	*/
	// 0λ: ������--1Ϊ̧�ˣ�0Ϊ����
	// 1λ: ������--1Ϊ���ˣ�0Ϊ����
	// 2λ: �����--1Ϊ�̵ƣ�0Ϊ���
	// 3λ: ������--1Ϊ��������0Ϊ������ȡ��
	// 4λ: �ⱨ��--1Ϊ�ⱨ����0Ϊ�ⱨ��ȡ��
	// 5λ: ͨ�е�--1Ϊ�̵ƣ�0Ϊ���
	// 6λ: δ����--����
	// 7λ: δ����--����
	device_control_used.control_word[USED]=SpkBuf[CTRL_T];  // �����ֽ�
	
	/* �����ֽ�2:	*/
	// 0λ: ���˿���λ  --1:���������Զ�����ģʽ��0:���������ֶ�����ģʽ
	// 1λ: �Ѷ���ʾλ  --1:�Ѷ���ʾ��            0:�Ѷ��ʾ
	// 2λ: ��������λ  --1:���ۣ�                0:������
	// 3λ: �Ѷ���ʾ����--1:���֣�            0:ˢ�£���Ϊ1ʱ������bit1
	// 4λ: ����ѡ��λ  --1:�㶫�汾���ޱ��ֹ��ܣ�0:ɽ���汾�б��ֹ��ܣ���������Ҫ����֧��
	// 5λ: ����������  --1:���ڳ��ڣ�            0:�������
	device_control_used.control_word[BACKUP]=SpkBuf[CTRL_B];

	return err;
}


/******************************************************************************
 * ������:	message_check 
 * ����: ���յ�����Ϣ����У��,�Ƿ���Ϲ涨��Э��
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
			j=rx_buf[COM_T];	// J�����������ֵ
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
		/*����Ƿ�����Ϣ*/
		else if ((rx_buf[BSOF] == FX_SOF) && (rx_buf[rx_len-2] ==FX_EOF))
		{
			 if (rx_buf[COM_T] == FX_LED)
			 {
			 	if (rx_buf[3] == 0)		// ����ǹرպ��̵�
			 	{
			 		err = TRANS_REQ;	// ֱ��ת��
			 	}
				else
				{
					j=rx_buf[COM_T];	// J�����������ֵ
					for(i=2; i <= (rx_len-3); i++)
					{
						j = rx_buf[i]^j;
					}

					if(j == rx_buf[rx_len-1])
					{
						err = DUMMY_ERROR;	// ֱ�������ﴦ��
						/*��Ϊ������������ĺ��̵Ʒ�����bug,��������Ϊ0*/
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
			 	/*ֻ�в����������⣬��������������������ֱ��ת��*/
			 	err = TRANS_REQ;	// ֱ��ת��
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
 * ������:	message_process 
 * ����: ��Ϣ������, ���յ�����Ϣ��ͬ������в�ͬ����.
 *
 * �������: seq, �����ǵڼ�������, buf,���ڶ�Ӧ��buf
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
static uint8_t message_process(USART_LIST uart_no)
{
	uint8_t err = 0;
	PROTOCOL_BUF *sbuf = &ProtocolBuf[uart_no]; // source buf

	err = message_check(sbuf);

	if ( err == ERR_OK)
	{
		/*������ǿ�������Ͳ���������ִ�к���*/
		err = message_unpack(uart_no, sbuf);
	}
	else if ((err == SITEID_ERROR) ||(err == DUMMY_ERROR)||(err == TRANS_REQ))
	{
		;  // ���Ƿ���������,����������һ�㴦��,ʲô������
	}
	else 
	{
		// ����Ǵ������Ϣ����
		message_pack(uart_no, ERR_RES_MSG,sbuf);
	}

	return err;
}

#endif
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
}
#endif


/******************************************************************************
 * ������:	Bits_Swap 
 * ����: �����ֽ��е�2��λ
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2019.03.10
 * 
 *------------------------
 * �޸���:
 * �޸�����:
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
			/*ȡʣ��λ*/
			tmp_re |= ((tmp_v1 >> i)&0x01)<<i;
		}
	}
	return tmp_re;
}


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
	//uint8_t xor_t = 0;
	uint8_t i = 0;
	//uint8_t temp_v = 0;

	switch (msg_type)
	{
	/* ͸��,ֱ�ӿ���*/
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
		/* �������쳣���,ֱ�ӷ���*/
		return;
	}

	// �Ƿ�������������?
	if (pack_en)
	{
		message_pack(destUtNo, msg_type, pScbuf);
	}

	LED_Set(LED_COM, ON); 	// ��ʼͨ��ָʾ
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
}

void params_modify_deal(void)
{
	if((UARTBuf[PC_UART].TxLen == 0)&& (system_flag&COMM1_MODIFIED) )	/*�޸���ͨ�Ų���*/
	{
		Comm1_Init(Baud[DevParams.BaudRate_1]);
		system_flag&=~COMM1_MODIFIED;
	}
	
	if((UARTBuf[PC_UART].TxLen == 0)&& (system_flag&COMM2_MODIFIED) )	/*�޸���ͨ�Ų���*/
	{
		Comm2_Init(Baud[DevParams.BaudRate_2]);
		system_flag&=~COMM2_MODIFIED;
	}

	if((UARTBuf[PC_UART].TxLen == 0)&& (system_flag&COMM3_MODIFIED) )	/*�޸���ͨ�Ų���*/
	{
		Comm4_Init(Baud[DevParams.BaudRate_3]);
		system_flag&=~COMM3_MODIFIED;
	}

	if((UARTBuf[PC_UART].TxLen == 0)&& (system_flag&COMM4_MODIFIED) )	/*�޸���ͨ�Ų���*/
	{
		Comm5_Init(Baud[DevParams.BaudRate_4]);
		system_flag&=~COMM4_MODIFIED;
	}
	
	if( System_Reset &&(UARTBuf[PC_UART].TxLen == 0))/*װ�ø�λ�������,�ȴ����в�����ɺ��������*/
	{
		System_Reset = 0;
		INT_DISABLE();		// ��ֹ��λ������
		NVIC_SystemReset();	// ����ط���Ҫ����һ��
	}
}

/******************************************************************************
 * ������:	Comm_Proc 
 * ����: -ͨѶ������,�������е���,��Ҫ����RS 485������,
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
void Comm_Proc(void)
{
	//uint8_t err = ERR_OK;
	USART_LIST i = BD1_UART;

	/*ע�ⲻ�ܸĳ�i < pc_com[PC_USART_NUM], �������*/
	for (i = bd_com[0]; i <= bd_com[BD_USART_NUM-1]; i++)
	{
		if (UARTBuf[i].RecFlag)		                      //RS485��������
		{
			// ��λ������������վ��ѯ��Ϣ
			if (i == PC_UART)
			{
				UARTBuf[i].RecFlag = 0;		//���������Ѵ��������ر�־
				ProtocolBuf[i].pTxBuf = UARTBuf[i].TxBuf;         //��ַ�û�
				ProtocolBuf[i].pRxBuf = UARTBuf[i].RxBuf;
				ProtocolBuf[i].RxLen = UARTBuf[i].RxLen;
				ProtocolBuf[i].TxLen = 0;
				//UARTBuf[i].RxLen = 0;		//�Ѿ�����ȡ��ProtocolBuf0.RxLen, ������0

				modbus_rtu_process(&ProtocolBuf[i], DevParams.Address);	/*MODBUSͨ��Э�鴦��*/

				UARTBuf[i].TxLen = ProtocolBuf[i].TxLen;  /*�û��������������������*/
				if(UARTBuf[i].TxLen >0)
				{
					message_send_printf(i, i, FALSE, NOT_USED_MSG);
					UARTBuf[i].TxLen = 0;
					//Rs485StartSend();
				}
				Delay_clk(50);
				UARTBuf[i].RxLen = 0;	        /*���������Ѵ��������ر�־*/
			}

	#if 0
			err = message_process(i);		//ͨ��Э�鴦��
			if (err == TRANS_REQ)							// ��Ҫ͸����
			{
				message_send_printf(TRANS_UART, i, TRUE, TRANS_MSG);
			}

			UARTBuf[i].TxLen = ProtocolBuf[i].TxLen;  //�û��������������������
			if(UARTBuf[i].TxLen >0)
			{
				/*�ظ�B/C��Ϣ����λ��*/
				message_send_printf(i, i,FALSE, 0xFF);
			}
			Delay_Ms(5);				// ��΢�е���ʱ,���Բ�Ҫ

			/*����������,ֻ���յ��µ���Ϣ�Ų���*/
			if (err == ERR_OK)
			{
				params_modify_deal();		//���������ݸı䴦��
			}
		#endif
			else
			{
				// ��ѯ��վ�Ļظ�
			}
		}
		params_modify_deal();	//���������ݸı䴦��,ע�����ĵ����ڴ�ѭ������
	}
}
/*********************************************END OF FILE**********************/
