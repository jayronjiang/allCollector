/********************************************************************************
*  ��Ȩ����: 	�㶫��ͨ�Ƽ�		
*
*  �ļ���:       ModbusServer.c
*
*
*  ��ǰ�汾:                       0.1
*  
*  ����:     JERRY                     
*
*  �������:  20190613
*                 
*  ����:    ��
*
*  �������:                       ��
*
*
*  ����: 
********************************************************************************/

/*
*********************************************************************************************************
*                                         �����ļ�
*********************************************************************************************************
*/
#include "include.h"

/********************************************************************************************************
                                  ͨ�Ŷ�ʱ��������������
*********************************************************************************************************/
//#define StartCounterT35		(g_T35_num = 0)
//#define StartCounterT15		(g_T15_num = 0)
#define StartCounterT100 		(g_T100_num = 0)
/*���ܳ�ʼ��Ϊ-1, ����CommTimer��g_T100_num++����0*/
/*������g_T100_num>= 0����,��������ֹ����CommTimer�ж�*/
/*����Ϊʲô��һ�λ����,���п�����g_T100_num��ȫ��*/
/*������2���ж�ͬʱ��д����*/
#define StoptCounterT100			(g_T100_num = -2)

/********************************************************************************************************
                                  ͨ�Ŷ�ʱ����ʹ�õı���
*********************************************************************************************************/
//static INT8U g_ENDT15Flag;	/*ע����Ҫ����ֵΪFALSE*/
//static INT8U g_ENDT35Flag;	/*ע����Ҫ����ֵΪFALSE*/
//static INT8S g_T35_num;		/*ע����Ҫ����ֵΪ-1*/
//static INT8S g_T15_num;		/*ע����Ҫ����ֵΪ-1*/
static INT16S g_T100_num;	/*������ɺ�ȴ����ռ���������ֵΪ-1*/
//static INT8U g_ComBuf[COMMBUFLEN];  
//static INT8U T15DelayTime[BAUDNUM] = {24,12,8,5,3};
//static INT8U T35DelayTime[BAUDNUM] = {46,23,12,6,4};

INT64U  comm_flag=0;
INT64U  control_flag=0;

/********************************************************************************************************
                                  ȫ�ֱ���
*********************************************************************************************************/
//INT8U  g_T15SetNumVal;/*����ѡȡ��ͨ�Ŷ�ʱ��ʱ����Ƶ�ʺ�ͨ��Ƶ�ʸ�ֵ*/
//INT8U  g_T35SetNumVal;/*����ѡȡ��ͨ�Ŷ�ʱ��ʱ����Ƶ�ʺ�ͨ��Ƶ�ʸ�ֵ*/

INT8U Recive_Flag = 0;			/* ���ձ�־*/
INT8U WAIT_response_flag = 0;	/* ��ʾ��ͬͨ�Ŵ����־*/
INT8U Com_err_counter = 0;		/* ͨ�Ŵ��������*/
INT8U Com_err_flag = 0;			/* ͨ�Ŵ����־*/
//INT8U g_Comm_init = FALSE;		/* ͨ�ų�ʼ����־*/

INT8U g_CommRxFlag = TRUE;          	/*ע����Ҫ��ʹ��ΪTRUE�������ϵ�ʱ����ʹ��*/
INT16U g_TxDataCtr;              	/*�������ݷ��͸�������*/
SEND_Struct g_SENData;		/* �������ݵĽṹ�� */
PDU_Struct  g_PDUData;  		/* �������ݵĽṹ�� */

/**************************************************************************************************/
/************************��������Ӧ����ص�ȫ�־�̬����********************************************/
/**************************************************************************************************/

/************************************************************************************
������:	       ReceOneChar

�������:	����ش���Ӳ��ͨ�ŵ�·�������ı���������

�������:	g_ADUData���洢��·���Ӧ�ò�����ݼ���ر�־

����ֵ:		��

����˵��:	�ɽ����жϵ��ã�ÿ����һ�ξ͵���һ��

**************************************************************************************/
void ReceOneChar(INT8U ReceCharacter) 
{
	if (g_CommRxFlag == TRUE)//�����������
	{	  
		StoptCounterT100;		/*��ʼ�������ݣ���ֹ���ճ�ʱ����*/

		if (g_PDUData.PDULength < UART_RXBUF_SIZE)/*��ָֹ������߽����*/
		{
			*(g_PDUData.PDUBuffPtr+g_PDUData.PDULength) = ReceCharacter;
			g_PDUData.PDULength++;
		}
	}
}

/************************************************************************************
������:	       data_send_directly

�������:	ֱ�ӷ�������

�������:	g_ADUData���洢��·���Ӧ�ò�����ݼ���ر�־

����ֵ:		��

����˵��:	�ɽ����жϵ��ã�ÿ����һ�ξ͵���һ��

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
		rs485FuncSelect(SEND_S);		//485��ʼ����
		Delay_Ms(1);	// �ȴ�1ms
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

	g_TxDataCtr = 0;
	LED_Set(LED_COM, ON); 	// ��ʼͨ��ָʾ
	while (g_TxDataCtr  <  g_SENData.SENDLength)
	{
		ch= *(g_SENData.SENDBuffPtr+g_TxDataCtr);
		USART_SendData(PUSART, ch);
		/* �ȴ�������� */
		while (USART_GetFlagStatus(PUSART, USART_FLAG_TXE) == RESET);
		g_TxDataCtr++;
	}
	LED_Set(LED_COM, OFF); 	// ͨ�����

	StartCounterT100;					/*��ʼ�ȴ�����*/
	g_SENData.SENDLength = 0;
	g_TxDataCtr  = 0;

	g_CommRxFlag = TRUE;            	/* ����Ϊ����״̬*/
	if (destUtNo == UART4_COM)
	{
		Delay_Ms(1);	// �ȴ�2ms�����һ�����ݷ������,����rs485FuncSelect(RECEIVE_S);�������ݳ���
		rs485FuncSelect(RECEIVE_S);		//485Ĭ��Ϊ
	}
	g_PDUData.PDUBuffPtr = UARTBuf[destUtNo].RxBuf;
	g_PDUData.PDULength = 0;	// ׼������
}


/******************************************************************************
 * ������:	CRC_check 
 * ����: 
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
INT8U CRC_check(void)
{
	INTEGER_UNION CRC_data;

	/*�˴������ڳ���С��2ʱ��ָ��Խ������װ����������*/	
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
 * ������:	realSum_check 
 * ����: 
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
INT8U realSum_check(void)
{
	INT16U re_value = 0;
	INT16U i = 0;

	/*�˴������ڳ���С��2ʱ��ָ��Խ������װ����������*/	
	if(g_PDUData.PDULength != REAL_DATA_NUM)
	{
		return 0;
	}

	/*�Դӵ�ѹ�����Ĵ�����ʼ���*/
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
 * ������:	comm_DeviceParam_set 
 * ����: ֻ���ÿյ��������յ���MODBUSЭ��
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 *------------------------
 * �޸���:	CZH
 * �޸�����:2012-10-10
 ******************************************************************************/
void comm_DeviceParam_set_1(void)
{
	INT8U i=0;
	INT16U* pointer = &DevParams.AirCondSet;
	INTEGER_UNION int_value;
	INT8U reg_num = AIR_ONOFF_SET_NUM;	/*װ�ò����Ĵ����ĸ���*/
	int_value.i=AIR_ONOFF_REG;					/*װ�ò����Ĵ������׵�ַ*/
	g_SENData.SENDBuffPtr = UARTBuf[AIR_COND_UART].RxBuf;;
	*g_SENData.SENDBuffPtr = AIR_STATION_ADDRESS;
	*(g_SENData.SENDBuffPtr+1) = WRITEREG_COMMAND;
	*(g_SENData.SENDBuffPtr+2)=int_value.b[1];
	*(g_SENData.SENDBuffPtr+3)=int_value.b[0];/*�Ĵ�����ַ*/		
	*(g_SENData.SENDBuffPtr+4) = 0X00;
	*(g_SENData.SENDBuffPtr+5) = reg_num;
	*(g_SENData.SENDBuffPtr+6) = reg_num*2; 
	
	for(i=0;i<1;i++)
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM +i*2 ,	*(pointer+i));
	}
	
	/*CRCУ��*/
	int_value.i=get_crc16(g_SENData.SENDBuffPtr,SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2);
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2)= int_value.b[0];
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2+1)= int_value.b[1];
	g_SENData.SENDLength=SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2+2;
	data_send_directly(AIR_COND_UART);
}


/******************************************************************************
 * ������:	comm_DeviceParam_set 
 * ����: ֻ���ÿյ��������յ���MODBUSЭ��
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 *------------------------
 * �޸���:	CZH
 * �޸�����:2012-10-10
 ******************************************************************************/
void comm_DeviceParam_set_2(void)
{
	INT8U i=0;
	INT16U* pointer = &DevParams.AirColdStartPoint;
	INTEGER_UNION int_value;
	INT8U reg_num = AIR_TEMP_SET_NUM;	/*װ�ò����Ĵ����ĸ���*/
	int_value.i=AIR_TEMP_REG;					/*װ�ò����Ĵ������׵�ַ*/
	g_SENData.SENDBuffPtr = UARTBuf[AIR_COND_UART].RxBuf;;
	*g_SENData.SENDBuffPtr = AIR_STATION_ADDRESS;
	*(g_SENData.SENDBuffPtr+1) = WRITEREG_COMMAND;
	*(g_SENData.SENDBuffPtr+2)=int_value.b[1];
	*(g_SENData.SENDBuffPtr+3)=int_value.b[0];/*�Ĵ�����ַ*/		
	*(g_SENData.SENDBuffPtr+4) = 0X00;
	*(g_SENData.SENDBuffPtr+5) = reg_num;
	*(g_SENData.SENDBuffPtr+6) = reg_num*2; 
	
	for(i=0;i<4;i++)
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM +i*2 ,	*(pointer+i));
	}
	
	/*CRCУ��*/
	int_value.i=get_crc16(g_SENData.SENDBuffPtr,SET_FRAME_HEAD_NUM+AIR_TEMP_SET_NUM*2);
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+AIR_TEMP_SET_NUM*2)= int_value.b[0];
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+AIR_TEMP_SET_NUM*2+1)= int_value.b[1];
	g_SENData.SENDLength=SET_FRAME_HEAD_NUM+AIR_TEMP_SET_NUM*2+2;
	data_send_directly(AIR_COND_UART);
}


/******************************************************************************
 * ������:	comm_RealData0_analyse 
 * ����: 		��ѹ����������0ͨ�����ݽ���
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:
 * �޸�����: ʹ��ָ��Խṹ�������ֵ
 * �޸�����:
 ******************************************************************************/
bool comm_RealData_analyse(INT8U ch)			 
{
	INT32U * pointer = &RSUParams.phase[ch].param_v;	/*��0�����*/
	INT32U temp = 0;

	//USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);	//��ֹ�ж�,��ֹƵ���յ���Ч����
	USART_Cmd(UART5, DISABLE);
	if(realSum_check())
	{		
		char3_to_int(g_PDUData.PDUBuffPtr + 2, pointer);
		char3_to_int(g_PDUData.PDUBuffPtr + 5, &temp);
		// �������㣬����100��,��Ϊkv������100��
		RSUParams.phase[ch].vln = ((float)RSUParams.phase[ch].param_v/temp)*RSUParams.phase[ch].k_v;

		pointer = &RSUParams.phase[ch].param_a;
		char3_to_int(g_PDUData.PDUBuffPtr + 8, pointer);
		char3_to_int(g_PDUData.PDUBuffPtr + 11, &temp);
		// �������㣬����100��
		RSUParams.phase[ch].amp = ((float)RSUParams.phase[ch].param_a/temp)*RSUParams.phase[ch].k_a;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


/******************************************************************************
 * ������:	comm_DeviceParam_1_analyse 
 * ����: 		�豸�����Ľ���,�յ����ػ�״̬
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
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
 * ������:	comm_DeviceParam_1_analyse 
 * ����: 		�豸�����Ľ���,�յ����ػ�״̬
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
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

#if 0
/******************************************************************************
 * ������:	comm_SPDStatus_analyse 
 * ����: 		���׵�����״̬����
 *            -
 * �������:  05 02 00 00 00 11 B9 82
 * �������:  05 02 03 8C 00 00 B8 21		// ����03�ǳ���,8C 00 00����DI״̬
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:CZH
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-16
 ******************************************************************************/
void comm_SPDStatus_analyse(void)	
{
	INT32U temp = 0;
	INT16U* pointer = &SPDParams.status;

	if(CRC_check() && (g_PDUData.PDULength == (3+5)))	// ����ֻ��3���ֽ�
	{
		// ��Դ״̬
		*pointer = (*(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM))&0x01;	//��һ����ֵ�ĵ�0λ

		/*ȡ����1λ~16λ,�ֱ����յ����ݵĵ�1���ֽڵ�1~7λ*/
		/*��2���ֽں͵�3���ֽڵ�1λ*/
		temp = (*(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM+2));
		temp = (temp << 8) +(*(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM+1));
		temp = (temp << 8) +(*(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM));

		pointer = &SPDParams.DI_status;	// DI״̬1~16
		*pointer = (temp>>1)&0xFF;
		
	}
}
#endif

/******************************************************************************
 * ������:	comm_SPDData_analyse 
 * ����: 		���׵�����״̬����
 *            -
 * �������:  05 04 00 0E 00 03 D0 4C			// ����14��ʼ��3��
 * �������:  05 04 06 00 00 00 00 00 00 52 53   // ����6������
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:CZH
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-16
 ******************************************************************************/
void comm_SPDTimes_analyse(INT8U SPD_seq)	
{
	INT16U* pointer = &SPDParams[SPD_seq].struck_times;

	if(CRC_check() && (g_PDUData.PDULength == (SPD_TIMES_NUM*2+5)))
	{
		// ֻ��Ҫ���׻�����
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + 0*2, pointer);
	}
}

/******************************************************************************
 * ������:	comm_config_analyse 
 * ����: 		��ȡ��ʪ��
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
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
 * ������:	comm_config_analyse 
 * ����: 		��ȡ��ʪ��
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
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

#ifdef RENDA
/*****************************************************************************
 * ������:	comm_config_analyse 
 * ����: 		��ȡ��ʪ��
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 ******************************************************************************/
void comm_smoke_analyse(void)				 
{
	INT8U i=0;
	INT16U* pointer = &ENVIParms.smoke_event_flag;

	/*�ظ�����ֻ��1���ֽ�*/
	if(CRC_check() && (g_PDUData.PDULength == (SMOKE_EVENT_NUM*2+5)))
	{
		for (i=0;i<SMOKE_EVENT_NUM;i++)
		{
			*(pointer+i) = *(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i);
			if (*(pointer+i) != 0)
			{
				*(pointer+i) = 1;
			}
		}
	}
}

/******************************************************************************
 * ������:	comm_SPDData_analyse 
 * ����: 		���׵�����״̬����
 *            -
 * �������:  05 04 00 0E 00 03 D0 4C			// ����14��ʼ��3��
 * �������:  05 04 06 00 00 00 00 00 00 52 53   // ����6������
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:CZH
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-16
 ******************************************************************************/
void comm_BRK_status_analyse(INT8U BRK_seq)	
{
	INT8U i=0;
	INT16U* pointer = &BRKParams[BRK_seq].status;

	if(CRC_check() && (g_PDUData.PDULength == (BREAKER_STATUS_NUM*2+5)))
	{
		for (i=0;i<BREAKER_STATUS_NUM;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
	}
}


/******************************************************************************
 * ������:	comm_SPDData_analyse 
 * ����: 		���׵�����״̬����
 *            -
 * �������:  05 04 00 0E 00 03 D0 4C			// ����14��ʼ��3��
 * �������:  05 04 06 00 00 00 00 00 00 52 53   // ����6������
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:CZH
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-16
 ******************************************************************************/
void comm_ARD_sts_analyse(INT8U ARD_seq)	
{
	INT8U i=0;
	INT16U* pointer = &ARDParams[ARD_seq].status;

	if(CRC_check() && (g_PDUData.PDULength == (ARD_REG_NUM*2+5)))
	{
		// ��ַ����i = 0;
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
	}
}
#endif


/******************************************************************************
 * ������:	comm_config_analyse 
 * ����: 		��ȡ��ʪ��
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 ******************************************************************************/
void comm_EnviAirOnOff_analyse(void)				 
{
	INT8U i=0;
	INT16U* pointer = &ENVIParms.air_cond_fan_in_status;	// ���ڷ����ʼ

	if(CRC_check() && (g_PDUData.PDULength == (ENVI_AIRCOND_ONOFF_NUM*2+5)))
	{
		for (i=0;i<ENVI_AIRCOND_ONOFF_NUM;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
	}
}


/******************************************************************************
 * ������:	comm_config_analyse 
 * ����: 		��ȡ��ʪ��
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 ******************************************************************************/
void comm_EnviAirTemp_analyse(void)				 
{
	INT8U i=0;
	INT16U* pointer = &ENVIParms.air_cond_temp_out;// ������

	if(CRC_check() && (g_PDUData.PDULength == (ENVI_AIRCOND_TEMP_NUM*2+5)))
	{
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, pointer);
		/*���Է����������ǲⲻ����,����Ϊ2000,��ʱ�ж�Ϊ��Ч*/
		if (ENVIParms.air_cond_temp_out >= 2000)
		{
			ENVIParms.air_cond_temp_out = 0xFFFF;
		}

		/*������*/
		pointer = &ENVIParms.air_cond_temp_in;
		i = 2;	// �����¶�����1���Ĵ���
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, pointer);
		if (ENVIParms.air_cond_temp_in >= 2000)
		{
			ENVIParms.air_cond_temp_in = 0xFFFF;
		}

		/*������ֵ*/
		pointer = &ENVIParms.air_cond_amp;
		i = 5;	// ����ֵ����5���Ĵ���
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, pointer);

		/*����ѹֵ*/
		pointer = &ENVIParms.air_cond_volt;
		i = 6;	// ��ѹֵ����6���Ĵ���
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, pointer);
	}
}

/******************************************************************************
 * ������:	comm_config_analyse 
 * ����: 		��ȡ��ʪ��
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 ******************************************************************************/
void comm_EnviAirAlarm_analyse(void)				 
{
	INT8U i=0;
	INT16U* pointer = &ENVIParms.air_cond_hightemp_alarm;

	if(CRC_check() && (g_PDUData.PDULength == (ENVI_AIRCOND_ALARM_NUM*2+5)))
	{
		for (i=0;i<4;i++)		// ���¡����¡���ʪ����ʪ����
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}
		/*����ѹֵ*/
		pointer = &ENVIParms.air_cond_infan_alarm;
		for (i=0;i<5;i++)		// ������,ѹ����,�����,Ӧ�����
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + (i+12)*2, (pointer+i));
		}
	}

}

/***********************************************************************
������:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

�������:		start_reg:	��ȡ�ļĴ�����ʼ��ַ
              reg_num:		��ȡ�ļĴ�������

�������:		��.

����ֵ:		��.

����˵��:              ��ȡ�Ĵ�������
***********************************************************************/
void comm_ask(INT16U station,USART_LIST buf_no,INT16U start_reg,INT8U reg_num,INT8U reg_type)
{
	INTEGER_UNION reg;
	
	reg.i = start_reg;	
	g_SENData.SENDBuffPtr = UARTBuf[buf_no].RxBuf;
	*g_SENData.SENDBuffPtr = station;
	*(g_SENData.SENDBuffPtr + 1) = reg_type;		// ������Ƿ�Ϊ0x03
	
	*(g_SENData.SENDBuffPtr + 2) = reg.b[1];				/*�Ĵ�����ַ*/
	*(g_SENData.SENDBuffPtr + 3) = reg.b[0];
	
	*(g_SENData.SENDBuffPtr + 4) = 0x00;					/*�Ĵ�������*/
	*(g_SENData.SENDBuffPtr + 5) = reg_num;
			
	 reg.i = get_crc16(g_SENData.SENDBuffPtr,6);			/*����CRCУ��*/
	*(g_SENData.SENDBuffPtr + 6) = reg.b[0];
	*(g_SENData.SENDBuffPtr + 7) = reg.b[1];
	
	g_SENData.SENDLength= 8;
	g_TxDataCtr = 0;
	data_send_directly(buf_no);
} 

#if 0
/***********************************************************************
������:		comm_set(INT16U start_reg,INT8U reg_num,INT8U reg_type)

�������:		start_reg:	��ȡ�ļĴ�����ʼ��ַ
              buffer:		Ҫ���õ����ݻ�����ʼ��ַ

�������:		��.

����ֵ:		��.

����˵��:              ������Ӧ�ļĴ���
***********************************************************************/
void comm_set(INT16U station,USART_LIST buf_no,INT16U start_reg,INT8U reg_num, INT16U *buffer)
{
	INT8U i=0;
	INT16U* pointer = buffer;
	INTEGER_UNION int_value;
	INT8U reg_num = reg_num;	/*װ�ò����Ĵ����ĸ���*/
	int_value.i=start_reg;					/*װ�ò����Ĵ������׵�ַ*/
	g_SENData.SENDBuffPtr = UARTBuf[buf_no].RxBuf;;
	*g_SENData.SENDBuffPtr = station;
	*(g_SENData.SENDBuffPtr+1) = 0x10;
	*(g_SENData.SENDBuffPtr+2)=int_value.b[1];
	*(g_SENData.SENDBuffPtr+3)=int_value.b[0];/*�Ĵ�����ַ*/		
	*(g_SENData.SENDBuffPtr+4) = 0X00;
	*(g_SENData.SENDBuffPtr+5) = reg_num;	/*4,5Ϊ�ֽ���*/
	*(g_SENData.SENDBuffPtr+6) = reg_num*2; 
	
	for(i=0;i<1;i++)
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM +i*2 ,	*(pointer+i));
	}
	
	/*CRCУ��*/
	int_value.i=get_crc16(g_SENData.SENDBuffPtr,SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2);
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2)= int_value.b[0];
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2+1)= int_value.b[1];
	g_SENData.SENDLength=SET_FRAME_HEAD_NUM+AIR_ONOFF_SET_NUM*2+2;
	data_send_directly(buf_no);
}
#endif


/***********************************************************************
������:		control_set(INT16U station,USART_LIST buf_no,INT16U start_reg,INT16U value)

�������:		start_reg:	��ȡ�ļĴ�����ʼ��ַ

�������:		��.

����ֵ:		��.

����˵��:              ң����Ӧ�ļĴ���
***********************************************************************/
void control_set(INT16U station,INT16U cmd, USART_LIST buf_no,INT16U start_reg,INT16U value)
{
	INTEGER_UNION reg;
	
	reg.i = start_reg;
	g_SENData.SENDBuffPtr = UARTBuf[buf_no].RxBuf;
	
	*g_SENData.SENDBuffPtr = station;
	*(g_SENData.SENDBuffPtr + 1) = cmd;		// Ҫ����06�Ĺ�����
	*(g_SENData.SENDBuffPtr + 2) = reg.b[1];
	*(g_SENData.SENDBuffPtr + 3) = reg.b[0];			/*�Ĵ�����ַ*/
	*(g_SENData.SENDBuffPtr + 4) = (value>>8)&0xFF;
	*(g_SENData.SENDBuffPtr + 5) = value&0xFF;
		
	reg.i = get_crc16(g_SENData.SENDBuffPtr,6);		/*����CRCУ��*/
	*(g_SENData.SENDBuffPtr + 6) = reg.b[0];
	*(g_SENData.SENDBuffPtr + 7) = reg.b[1];
		
	g_SENData.SENDLength = 8;
	g_TxDataCtr = 0;
	data_send_directly(buf_no);
}
/***********************************************************************
������:		comm_wait(INT16U start_reg,INT8U reg_num,INT8U reg_type)

�������:		start_reg:	��ȡ�ļĴ�����ʼ��ַ
              reg_num:		��ȡ�ļĴ�������

�������:		��.

����ֵ:		��.

����˵��:              ��ȡ�Ĵ�������
***********************************************************************/
void comm_wait(USART_LIST destUtNo, UINT16 seq)
{	
	// ��ʼ����
	realDataChannelSelect(seq);
	USART_Cmd(UART5, ENABLE);
	// ������,ֱ��׼������
	StartCounterT100;					/*��ʼ�ȴ�����*/
	//g_SENData.SENDLength = 0;
	g_CommRxFlag = TRUE;            	/* ����Ϊ����״̬*/
	if (destUtNo == UART4_COM)
	{
		rs485FuncSelect(RECEIVE_S);		//485Ĭ��Ϊ
	}
	g_PDUData.PDUBuffPtr = UARTBuf[destUtNo].RxBuf;
	g_PDUData.PDULength = 0;			// ׼������
	UARTBuf[destUtNo].RecFlag = FALSE;	// ���ϴεĽ��ձ�־���
	//USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);		// ֱ��ʹ��UART5��
} 


/***********************************************************************
������:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

�������:		start_reg:	��ȡ�ļĴ�����ʼ��ַ
              reg_num:		��ȡ�ļĴ�������

�������:		��.

����ֵ:		��.

����˵��:              ��ȡ�Ĵ�������
***********************************************************************/
void comm_ask_ups(INT16U station,USART_LIST buf_no,INT16U cid2,INT8U info_len,INT8U command)
{
	INT8U c_value[4] = {0,0,};
	USP_LENGTH_BITS uspLenthData;
	INT16U temp = 0;
	INT16U send_len = 0;
		
	g_SENData.SENDBuffPtr = UARTBuf[buf_no].RxBuf;
	
	*g_SENData.SENDBuffPtr = UPS_SOI;	// ����Э��0x7E��ʼ
	send_len++;
	
	hex2_to_ascii(UPS_VER, c_value);
	*(g_SENData.SENDBuffPtr + 1) = c_value[1]; // ͨѶ�汾��0x21
	*(g_SENData.SENDBuffPtr + 2) = c_value[0]; 
	send_len += 2;
	
	hex2_to_ascii(station, c_value);			// �豸��ַ
	*(g_SENData.SENDBuffPtr + 3) = c_value[1]; 
	*(g_SENData.SENDBuffPtr + 4) = c_value[0]; 
	send_len += 2;

	hex2_to_ascii(UPS_CID1, c_value);			// �豸��ַ
	*(g_SENData.SENDBuffPtr + 5) = c_value[1]; 
	*(g_SENData.SENDBuffPtr + 6) = c_value[0];
	send_len += 2;

	hex2_to_ascii(cid2, c_value);			// �豸��ַ
	*(g_SENData.SENDBuffPtr + 7) = c_value[1]; 
	*(g_SENData.SENDBuffPtr + 8) = c_value[0];
	send_len += 2;

	/*LENTH�ļ���*/
	uspLenthData.lenth_bits.lenid = info_len;
	uspLenthData.lenth_bits.lchksum = lchkSumCalc(uspLenthData.lenth_bits.lenid) &0x0F;
	hex4_to_ascii(uspLenthData.lenth_word, c_value);
	*(g_SENData.SENDBuffPtr + 9) = c_value[3]; 
	*(g_SENData.SENDBuffPtr + 10) = c_value[2];
	*(g_SENData.SENDBuffPtr + 11) = c_value[1]; 
	*(g_SENData.SENDBuffPtr + 12) = c_value[0];
	send_len += 4;

	/*g_SENData.SENDBuffPtr���Ѿ�ת����ASCII��*/
	temp = checkSumCalc(g_SENData.SENDBuffPtr+1, 12);	//SOI��������
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
������:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

�������:		start_reg:	��ȡ�ļĴ�����ʼ��ַ
              reg_num:		��ȡ�ļĴ�������

�������:		��.

����ֵ:		��.

����˵��:              ��ȡ�Ĵ�������
***********************************************************************/
void comm_ask_spd(INT16U station,USART_LIST buf_no,INT16U start_reg,INT8U reg_num,INT8U reg_type)
{
	INTEGER_UNION reg;
	
	reg.i = start_reg;	
	g_SENData.SENDBuffPtr = UARTBuf[buf_no].RxBuf;
	*g_SENData.SENDBuffPtr = station;
		
	*(g_SENData.SENDBuffPtr + 1) = reg_type;
	
	*(g_SENData.SENDBuffPtr + 2) = reg.b[1];				/*�Ĵ�����ַ*/
	*(g_SENData.SENDBuffPtr + 3) = reg.b[0];
	
	*(g_SENData.SENDBuffPtr + 4) = 0x00;					/*�Ĵ�������*/
	*(g_SENData.SENDBuffPtr + 5) = reg_num;
			
	 reg.i = get_crc16(g_SENData.SENDBuffPtr,6);			/*����CRCУ��*/
	*(g_SENData.SENDBuffPtr + 6) = reg.b[0];
	*(g_SENData.SENDBuffPtr + 7) = reg.b[1];
	
	g_SENData.SENDLength= 8;
	g_TxDataCtr = 0;
	data_send_directly(buf_no);
} 


//���������CRC4����
void CalulateCRCbySoft(INT8U *pucData,INT8U wLength,INT8U *pOutData)
{
	INT8U ucTemp;
	INT16U wValue;
	INT16U crc_tbl[16]={0x0000,0x1021,0x2042,0x3063,0x4084,0x50a5,0x60c6,0x70e7,
	0x8108,0x9129,0xa14a,0xb16b,0xc18c,0xd1ad,0xe1ce,0xf1ef};  //��λ��ʽ��

	wValue=0;

	//���ֽڵ�CRC��ʽ������һ�ֽڵ�CRC��ʽ�ĵ�12λ����4λ��
	//�ټ�����һ�ֽ�CRC��ʽ����4λ��Ҳ��ȡ��4λ���ͱ��ֽ�֮�ͺ�����õ�CRC��
	while(wLength--!=0)
	{
		//������λCRC��ʽ���ȼ������λCRC��ʽ
		ucTemp=((wValue>>8))>>4;
		wValue<<=4;
		wValue^=crc_tbl[ucTemp^((*pucData)>>4)];
		//�ټ������λ��ʽ
		ucTemp=((wValue>>8))>>4;
		wValue<<=4;
		wValue^=crc_tbl[ucTemp^((*pucData)&0x0f)];
		pucData++;
	}
	pOutData[0]=wValue;	
	pOutData[1]=(wValue>>8);
    
}

/***********************************************************************
������:		comm_ask_locker(INT16U start_reg,INT8U reg_num,INT8U reg_type)

�������:		info_len:	���ɳ���
              msg:		��Ϣ����

�������:		��.

����ֵ:		��.

����˵��:       �����������
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
������:			void comm_polling_process(void)

�������:		��.

�������:		��.

����ֵ:			��.

����˵��:           ͨ�Ŵ�����,��ģ���ʹ���߸���ͨ�Ÿ��ɺ�Ƶ�ʵĴ�С����
***********************************************************************/
void comm_polling_process(void)
{
	//static INT8U Init_sign = FALSE;
	//static INT8U Init_send = FALSE;
	INT8U i = 0;
	INT8U reg_num = 0;		/*�ڼ����豸*/
	INT8U reg_action = 0;	/*��ǰ�豸�Ĳ���*/

#if 0	
	if( Init_sign == FALSE)	/*��ʼ��װ�ò���*/
	{
		Init_sign = TRUE;
		comm_flag |= LBIT(DEV_PARAM_SEND_FLAG_2);		/*�ȶ�ȡװ�ò���*/
	}
#endif

	/********************************************************************************************************************
	***�˴���ͨ�Ų�ѯ������ָ���ͻ������д���SOE��ѯ���ȼ���ߣ����Ϊ���� ***
	***���ã����Ϊ������ѯ�����ڽ��յ���ȷ�Ĳ�������ָ��󣬻�δ��װ�ò�����  ***
	***��־�����Դ��ַ����ڲ�������ʱ��������ȫ���β�������������Ҫ�ڲ�������  ***
	***ʱ��һ�����ˣ���װ�ò�����������"if(!(comm_flag & PARAM_SET_FLAG))"����Ҫʮ��ע�� !!!!    ***
	********************************************************************************************************************/
	 /*������ֲ�������֡���򽫽��ձ�־�Լ����ͱ�־������*/
	if (control_flag)		
	{
		// ��ѯȫ��ȡ����
		comm_flag = 0;
		if(Recive_Flag>0)
		{
			WAIT_response_flag = 0;
			Recive_Flag = 0;  
			g_PDUData.PDULength = 0;				/*������֡���������ȽϺ�*/
		}
		Com_err_counter = 0;
	}

	/**********************************���յ���ȷ���ݺ��������******************************************/
	if(Recive_Flag)									/*������յ�����*/
	{
		switch(Recive_Flag)
		{
			case REAL_DATA0_ANALYSE:
				comm_RealData_analyse(CHANNEL_0);		/*���յ�ʵʱ����*/
			break;

			case REAL_DATA1_ANALYSE:					/*���յ�ʵʱ����*/
				comm_RealData_analyse(CHANNEL_1);
			break;

			case REAL_DATA2_ANALYSE:					/*���յ�ʵʱ����*/
				comm_RealData_analyse(CHANNEL_2);
			break;

			case REAL_DATA3_ANALYSE:					/*���յ�ʵʱ����*/
				comm_RealData_analyse(CHANNEL_3);
			break;
			
			case DEVICE_DATA_1_ANALYSE:				/*���յ�װ�ò���*/
				if(!(control_flag & LBIT(DEV_PARAM_SET_FLAG_1)))
				{
					// װ�ò���
					comm_DeviceParam_1_analyse();
				}
			break;

			case DEVICE_DATA_2_ANALYSE:				/*���յ�װ�ò���*/
				if(!(control_flag & LBIT(DEV_PARAM_SET_FLAG_2)))
				{
					// װ�ò���
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

			case SPD_TIMES_ANALYSE_1:
				comm_SPDTimes_analyse(0);
			break;
			
			case SPD_TIMES_ANALYSE_2:
				comm_SPDTimes_analyse(1);
			break;

			case WATER_IN_ANALYSE:
				comm_water_in_analyse();
			break;
			
		#ifdef RENDA
			case SMOKE_ANALYSE:
				comm_smoke_analyse();
			break;
		#endif
			
		#if (BRK_NUM >= 1)
			case BREAKER_OPEN_CLOSE_ST_ANALYSE_1:
				comm_BRK_status_analyse(0);
			break;
		#endif

		#if (BRK_NUM >= 2)
			case BREAKER_OPEN_CLOSE_ST_ANALYSE_2:
				comm_BRK_status_analyse(1);
			break;
		#endif

		#if (ARD_NUM >= 1)
			case ARD_STS_ANALYSE_1:
				comm_ARD_sts_analyse(0);
			break;
		#endif
		
		#if (ARD_NUM >= 2)
			case ARD_STS_ANALYSE_2:
				comm_ARD_sts_analyse(1);
			break;
		#endif
		
		#if (ARD_NUM >= 3)
			case ARD_STS_ANALYSE_3:
				comm_ARD_sts_analyse(2);
			break;
		#endif
		
		#if (ARD_NUM >= 4)
			case ARD_STS_ANALYSE_4:
				comm_ARD_sts_analyse(3);
			break;
		#endif
		
		#if (ARD_NUM >= 5)
			case ARD_STS_ANALYSE_5:
				comm_ARD_sts_analyse(4);
			break;
		#endif
		
		#if (ARD_NUM >= 6)
			case ARD_STS_ANALYSE_6:
				comm_ARD_sts_analyse(5);
			break;
		#endif
		
		#if (ARD_NUM >= 7)
			case ARD_STS_ANALYSE_7:
				comm_ARD_sts_analyse(6);
			break;
		#endif
		
		#if (ARD_NUM >= 8)
			case ARD_STS_ANALYSE_8:
				comm_ARD_sts_analyse(7);
			break;
		#endif
		
		#if (ARD_NUM >= 9)
			case ARD_STS_ANALYSE_9:
				comm_ARD_sts_analyse(8);
			break;
		#endif
		
		#if (ARD_NUM >= 10)
			case ARD_STS_ANALYSE_10:
				comm_ARD_sts_analyse(9);
			break;
		#endif
		
		#if (ARD_NUM >= 11)
			case ARD_STS_ANALYSE_11:
				comm_ARD_sts_analyse(10);
			break;
		#endif
			
			default:
			break;
		}

		Recive_Flag = 0;
		WAIT_response_flag = 0;						/*���ȴ���־�Լ����ձ�־������*/
		g_PDUData.PDULength = 0;
	}
	/**************************************���͹��̴���*********************************************/
	// 2��ͬʱ������bug
	if ((!WAIT_response_flag) && (g_CommRxFlag == FALSE))	
	{
		/********************************���Ͳ�������֡����*************************************/
		/*�յ���������ΪMODBUSͨ��*/
		if(control_flag & LBIT(DEV_PARAM_SET_FLAG_1))			
		{
			control_flag &= ~(LBIT(DEV_PARAM_SET_FLAG_1));
			comm_DeviceParam_set_1();
			//WAIT_response_flag = WAIT_PARAM_SET_1; 
		}

		else if(control_flag & LBIT(DEV_PARAM_SET_FLAG_2))			
		{
			control_flag &= ~(LBIT(DEV_PARAM_SET_FLAG_2));
			comm_DeviceParam_set_2();
			// ���ú��Ӧ�����Ȳ���
			//WAIT_response_flag = WAIT_PARAM_SET_2; 
		}

	#if (LOCK_NUM >= 1)
		else if(control_flag & LBIT(DOOR1_OPEN_SET_FLAG))			
		{
			control_flag &= ~(LBIT(DOOR1_OPEN_SET_FLAG));
			// ����
			comm_ask_locker(LOCK_ADDR_1, LOCKER_UART, LOCK_OPEN, 0x02, NULL);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & LBIT(DOOR1_CLOSE_SET_FLAG))			
		{
			control_flag &= ~(LBIT(DOOR1_CLOSE_SET_FLAG));
			// ����
			comm_ask_locker(LOCK_ADDR_1, LOCKER_UART, LOCK_CLOSE, 0x02, NULL);
			//WAIT_response_flag = WAIT_DOOR_CLOSE; 
		}
	#endif

	#if (LOCK_NUM >= 2)
		else if(control_flag & LBIT(DOOR2_OPEN_SET_FLAG))			
		{
			control_flag &= ~(LBIT(DOOR2_OPEN_SET_FLAG));
			// ����
			comm_ask_locker(LOCK_ADDR_2, LOCKER_UART, LOCK_OPEN, 0x02, NULL);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & LBIT(DOOR2_CLOSE_SET_FLAG))			
		{
			control_flag &= ~(LBIT(DOOR2_CLOSE_SET_FLAG));
			// ����
			comm_ask_locker(LOCK_ADDR_2, LOCKER_UART, LOCK_CLOSE, 0x02, NULL);
			//WAIT_response_flag = WAIT_DOOR_CLOSE; 
		}
	#endif

	#if (LOCK_NUM >= 3)
		else if(control_flag & LBIT(DOOR3_OPEN_SET_FLAG))			
		{
			control_flag &= ~(LBIT(DOOR3_OPEN_SET_FLAG));
			// ����
			comm_ask_locker(LOCK_ADDR_3, LOCKER_UART, LOCK_OPEN, 0x02, NULL);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & LBIT(DOOR3_CLOSE_SET_FLAG))			
		{
			control_flag &= ~(LBIT(DOOR3_CLOSE_SET_FLAG));
			// ����
			comm_ask_locker(LOCK_ADDR_3, LOCKER_UART, LOCK_CLOSE, 0x02, NULL);
			//WAIT_response_flag = WAIT_DOOR_CLOSE; 
		}
	#endif

#if 0
		/*���ظ��Ķ�·�����غ�բͳһ����--���ô���*/
		else if(control_flag)
		{
			for (i = BRK1_CLOSE_SET_FLAG; i < (BRK1_CLOSE_SET_FLAG+4*BRK_NUM); i++)
			{
				if (control_flag & LBIT(i))
				{
					control_flag &= ~(LBIT(i));
					reg_num = (i - BRK1_CLOSE_SET_FLAG)/4;	// �ڼ���BRK
					reg_action = (i - BRK1_CLOSE_SET_FLAG)%4;	//���ڵĶ���
					
					switch(reg_action)
					{
						case 0:	// ��·��1��
							control_set(BREAKER_STATION_1_ADDRESS+reg_num,BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_CLOSE);
							break;

						case 1:	// ��·��1��բ������
							control_set(BREAKER_STATION_1_ADDRESS+reg_num,BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_OPEN_WITHOUT_LOCK);
							break;

						case 2:	// ��·��1��բ����
							control_set(BREAKER_STATION_1_ADDRESS+reg_num,BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_OPEN_LOCK);
							break;

						case 3:	// ��·��1��բ����
							control_set(BREAKER_STATION_1_ADDRESS+reg_num,BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_OPEN_UNLOCK);
							break;
							
						default:
							break;
					}
				}
			}
		}
	#endif

	#if (BRK_NUM >= 1)
		else if(control_flag & LBIT(BRK1_CLOSE_SET_FLAG))			
		{
			control_flag &= ~(LBIT(BRK1_CLOSE_SET_FLAG));
			// ����
			control_set(BREAKER_STATION_1_ADDRESS,BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & LBIT(BRK1_OPEN_SET_FLAG))			
		{
			control_flag &= ~(LBIT(BRK1_OPEN_SET_FLAG));
			// ����
			control_set(BREAKER_STATION_1_ADDRESS, BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_OPEN_WITHOUT_LOCK);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & LBIT(BRK1_OPEN_LOCK_SET_FLAG))			
		{
			control_flag &= ~(LBIT(BRK1_OPEN_LOCK_SET_FLAG));
			// ����
			control_set(BREAKER_STATION_1_ADDRESS, BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_OPEN_LOCK);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & LBIT(BRK1_OPEN_UNLOCK_SET_FLAG)	)		
		{
			control_flag &= ~(LBIT(BRK1_OPEN_UNLOCK_SET_FLAG));
			// ����
			control_set(BREAKER_STATION_1_ADDRESS, BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_OPEN_UNLOCK);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
	#endif

	#if (BRK_NUM >= 2)
		else if(control_flag & LBIT(BRK2_CLOSE_SET_FLAG))			
		{
			control_flag &= ~(LBIT(BRK2_CLOSE_SET_FLAG));
			// ����
			control_set(BREAKER_STATION_2_ADDRESS,BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & LBIT(BRK2_OPEN_SET_FLAG))			
		{
			control_flag &= ~(LBIT(BRK2_OPEN_SET_FLAG));
			// ����
			control_set(BREAKER_STATION_2_ADDRESS, BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_OPEN_WITHOUT_LOCK);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & LBIT(BRK2_OPEN_LOCK_SET_FLAG))			
		{
			control_flag &= ~(LBIT(BRK2_OPEN_LOCK_SET_FLAG));
			// ����
			control_set(BREAKER_STATION_2_ADDRESS, BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_OPEN_LOCK);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & LBIT(BRK2_OPEN_UNLOCK_SET_FLAG))			
		{
			control_flag &= ~(LBIT(BRK2_OPEN_UNLOCK_SET_FLAG));
			// ����
			control_set(BREAKER_STATION_2_ADDRESS, BRK_SINGLE_WRITE, BREAKER_UART, BRK_REMOTE_ADDR, BRK_OPEN_UNLOCK);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
	#endif

	#if (ARD_NUM > 0)
		else if(control_flag)
		{
			for (i = ARD1_CLOSE_SET_FLAG; i < (ARD1_CLOSE_SET_FLAG+2*ARD_NUM); i++)
			{
				if (control_flag & LBIT(i))
				{
					reg_num = (i - ARD1_CLOSE_SET_FLAG)/2;	// �ڼ���ARD
					reg_action = (i - ARD1_CLOSE_SET_FLAG)%2;	//���ڵĶ���
					if(reg_num >= 9)		// ��ַ12������ȥ��
					{
						reg_num++;
					}
					
					switch(reg_action)
					{
						case 0:	// �غ�բ��
							control_set(ARD_STATION_1_ADDRESS+reg_num, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
							break;

						case 1:	// �غ�բ��
							control_set(ARD_STATION_1_ADDRESS+reg_num, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
							break;
							
						default:
							break;
					}
					
					control_flag &= ~(LBIT(i));
					break;		// ���break������forѭ��
				}
			}
		}
	#endif
	
	#if 0
		else if(control_flag & ARD1_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD1_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_1_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD1_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD1_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_1_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		
		else if(control_flag & ARD2_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD2_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_2_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD2_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD2_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_2_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & ARD3_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD3_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_3_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD3_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD3_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_3_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & ARD4_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD4_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_4_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD4_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD4_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_4_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & ARD5_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD5_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_5_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD5_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD5_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_5_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & ARD6_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD6_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_6_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD6_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD6_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_6_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & ARD7_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD7_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_7_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD7_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD7_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_7_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & ARD8_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD8_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_8_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD8_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD8_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_8_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & ARD9_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD9_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_9_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD9_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD9_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_9_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & ARD10_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD10_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_10_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD10_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD10_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_10_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}

		else if(control_flag & ARD11_CLOSE_SET_FLAG)			
		{
			control_flag &= ~ARD11_CLOSE_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_11_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_CLOSE);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
		else if(control_flag & ARD11_OPEN_SET_FLAG)			
		{
			control_flag &= ~ARD11_OPEN_SET_FLAG;
			// ��բ
			control_set(ARD_STATION_11_ADDRESS, ARD_SINGLE_WRITE,ARD_UART, ARD_REMOTE_ADDR, ARD_OPEN);
			//WAIT_response_flag = WAIT_DOOR_OPEN; 
		}
	#endif

		/*��ʼ��ѯ����*/
		/*RSU������ѯ*/
		else if (comm_flag & LBIT(REAL_DATA0_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(REAL_DATA0_SEND_FLAG));
			/*��ȡʵʱ��������,�������ݵ�*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_0);
			WAIT_response_flag = REAL_DATA0_ANALYSE;
		}
		// RSU����
		else if (comm_flag & LBIT(REAL_DATA1_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(REAL_DATA1_SEND_FLAG));
			/*��ȡʵʱ��������,�������ݵ�*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_1);
			WAIT_response_flag = REAL_DATA1_ANALYSE;
		}
		// RSU����
		else if (comm_flag & LBIT(REAL_DATA2_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(REAL_DATA2_SEND_FLAG));
			/*��ȡʵʱ��������,�������ݵ�*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_2);
			WAIT_response_flag = REAL_DATA2_ANALYSE;
		}
		// RSU����
		else if (comm_flag & LBIT(REAL_DATA3_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(REAL_DATA3_SEND_FLAG));
			/*��ȡʵʱ��������,�������ݵ�*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_3);
			WAIT_response_flag = REAL_DATA3_ANALYSE;
		}

		else if (comm_flag & LBIT(DEV_PARAM_SEND_FLAG_1))
		{
			comm_flag &= ~(LBIT(DEV_PARAM_SEND_FLAG_1));
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,AIR_ONOFF_REG, AIR_ONOFF_SET_NUM, READREG_COMMAND);	/*��ȡװ�ò�������,�������ݵ�*/  
			WAIT_response_flag = DEVICE_DATA_1_ANALYSE;
		}

		else if (comm_flag & LBIT(DEV_PARAM_SEND_FLAG_2))
		{
			comm_flag &= ~(LBIT(DEV_PARAM_SEND_FLAG_2));
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,AIR_TEMP_REG, AIR_TEMP_SET_NUM, READREG_COMMAND);	/*��ȡװ�ò�������,�������ݵ�*/  
			WAIT_response_flag = DEVICE_DATA_2_ANALYSE;
		}

		else if (comm_flag & LBIT(UPS_PARAM_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(UPS_PARAM_SEND_FLAG));
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_ALL, 0, NULL);
			WAIT_response_flag = UPS_PARAM_ANALYSE;
		}
		else if (comm_flag & LBIT(UPS_IN_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(UPS_IN_SEND_FLAG));
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_IN, 0, NULL);
			WAIT_response_flag = UPS_IN_ANALYSE;
		}
		else if (comm_flag & LBIT(UPS_OUT_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(UPS_OUT_SEND_FLAG));
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_OUT, 0, NULL);
			WAIT_response_flag = UPS_OUT_ANALYSE;
		}
		else if (comm_flag & LBIT(UPS_BAT_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(UPS_BAT_SEND_FLAG));
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_BAT, 0, NULL);
			WAIT_response_flag = UPS_BAT_ANALYSE;
		}
		// ������Ϣ��ѯ
		else if (comm_flag & LBIT(UPS_ALARM_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(UPS_ALARM_SEND_FLAG));
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_ALARM, 0, NULL);
			WAIT_response_flag = UPS_ALARM_ANALYSE;
		}
		else if (comm_flag & LBIT(UPS_STATUS_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(UPS_STATUS_SEND_FLAG));
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_STATUS, 0, NULL);
			WAIT_response_flag = UPS_STATUS_ANALYSE;
		}

	#if (SPD_NUM >= 1)
		else if (comm_flag & LBIT(SPD_TIMES_SEND_FLAG_1))
		{
			comm_flag &= ~(LBIT(SPD_TIMES_SEND_FLAG_1));
			/*�׻�������MODBUSЭ����*/
			comm_ask(SPD_STATION_1_ADDRESS,SPD_UART,SPD_TIMES_REG, SPD_TIMES_NUM, READREG_COMMAND);	/*��ȡ��ʪ������*/  
			//comm_ask_spd(SPD_STATION_ADDRESS, SPD_UART,SPD_TIMES_REG, SPD_TIMES_NUM, SPD_TIMESREAD_COMMAND);		/*��ȡSPD��������*/  
			WAIT_response_flag = SPD_TIMES_ANALYSE_1;
		}
	#endif
	#if (SPD_NUM >= 2)
		else if (comm_flag & LBIT(SPD_TIMES_SEND_FLAG_2))
		{
			comm_flag &= ~(LBIT(SPD_TIMES_SEND_FLAG_2));
			/*�׻�������MODBUSЭ����*/
			comm_ask(SPD_STATION_2_ADDRESS,SPD_UART,SPD_TIMES_REG, SPD_TIMES_NUM, READREG_COMMAND);	/*��ȡ��ʪ������*/  
			//comm_ask_spd(SPD_STATION_ADDRESS, SPD_UART,SPD_TIMES_REG, SPD_TIMES_NUM, SPD_TIMESREAD_COMMAND);		/*��ȡSPD��������*/  
			WAIT_response_flag = SPD_TIMES_ANALYSE_2;
		}
	#endif

		else if (comm_flag & LBIT(ENVI_TEMP_SEND_FLAG))
		{
			comm_flag &= ~(LBIT(ENVI_TEMP_SEND_FLAG));
			comm_ask(TEMP_STATION_ADDRESS,TEMP_UART,ENVI_TEMP_REG, ENVI_TEMP_NUM, READREG_COMMAND);	/*��ȡ��ʪ������*/  
			WAIT_response_flag = ENVI_TEMP_ANALYSE;
		}

		else if (comm_flag & LBIT(ENVI_AIRCOND_ONOFF_FLAG))
		{
			comm_flag &= ~(LBIT(ENVI_AIRCOND_ONOFF_FLAG));
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,ENVI_AIRCOND_ONOFF_REG, ENVI_AIRCOND_ONOFF_NUM, READREG_COMMAND);	/*��ȡ�յ�״̬����*/  
			WAIT_response_flag = ENVI_AIR_ONOFF_ANALYSE;
		}

		else if (comm_flag & LBIT(ENVI_AIRCOND_TEMP_FLAG))
		{
			comm_flag &= ~(LBIT(ENVI_AIRCOND_TEMP_FLAG));
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,ENVI_AIRCOND_TEMP_REG, ENVI_AIRCOND_TEMP_NUM, READREG_COMMAND);	/*��ȡ�յ��¶�����*/  
			WAIT_response_flag = ENVI_AIR_TEMP_ANALYSE;
		}

		else if (comm_flag & LBIT(ENVI_AIRCOND_ALARM_FLAG))
		{
			comm_flag &= ~(LBIT(ENVI_AIRCOND_ALARM_FLAG));
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,ENVI_AIRCOND_ALARM_REG, ENVI_AIRCOND_ALARM_NUM, READREG_COMMAND);	/*��ȡ�յ���������*/  
			WAIT_response_flag = ENVI_AIR_ALARM_ANALYSE;
		}

		else if (comm_flag & LBIT(WATER_IN_FLAG))
		{
			comm_flag &= ~(LBIT(WATER_IN_FLAG));
			comm_ask(WATERIN_STATION_ADDRESS, WATER_UART,WATER_IN_REG, WATER_IN_NUM, READREG_COMMAND);	/*��ȡˮ������*/  
			WAIT_response_flag = WATER_IN_ANALYSE;
		}

	#ifdef RENDA
		else if (comm_flag & LBIT(SMOKE_FLAG))
		{
			comm_flag &= ~(LBIT(SMOKE_FLAG));
			comm_ask(SMOKE_STATION_ADDRESS, SMOKE_UART,SMOKE_REG, SMOKE_EVENT_NUM, READREG_COMMAND);	/*��ȡ�̸�����*/  
			WAIT_response_flag = SMOKE_ANALYSE;
		}
	#endif
		
	#if (BRK_NUM >= 1)
		else if (comm_flag & LBIT(BREAKER_OPEN_CLOSE_STATUS_1))
		{
			comm_flag &= ~(LBIT(BREAKER_OPEN_CLOSE_STATUS_1));
			comm_ask(BREAKER_STATION_1_ADDRESS, BREAKER_UART,BREAKER_REG, BREAKER_STATUS_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = BREAKER_OPEN_CLOSE_ST_ANALYSE_1;
		}
	#endif

	#if (BRK_NUM >= 2)
		else if (comm_flag & LBIT(BREAKER_OPEN_CLOSE_STATUS_2))
		{
			comm_flag &= ~(LBIT(BREAKER_OPEN_CLOSE_STATUS_2));
			comm_ask(BREAKER_STATION_2_ADDRESS, BREAKER_UART,BREAKER_REG, BREAKER_STATUS_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = BREAKER_OPEN_CLOSE_ST_ANALYSE_2;
		}
	#endif

	#if (ARD_NUM > 0)
		else if(comm_flag)
		{
			for (i = ARD_STS_FLAG_1; i < (ARD_STS_FLAG_1+ARD_NUM); i++)
			{
				if (comm_flag & LBIT(i))
				{
					reg_num = (i - ARD_STS_FLAG_1);	// �ڼ���ARD
					WAIT_response_flag = ARD_STS_ANALYSE_1+reg_num;
					/*WAIT_response_flag��ֵ���ټ�*/
					if(reg_num >= 9)		// ��ַ12������ȥ��
					{
						reg_num++;
					}
					comm_ask(ARD_STATION_1_ADDRESS+reg_num, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/
					comm_flag &= ~(LBIT(i));
					break;		// ���break������forѭ��
				}
			}
		}
	#endif
	#if 0
		else if (comm_flag & LBIT(ARD_STS_FLAG_1))
		{
			comm_flag &= ~(LBIT(ARD_STS_FLAG_1));
			comm_ask(ARD_STATION_1_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_1;
		}
		else if (comm_flag & LBIT(ARD_STS_FLAG_2))
		{
			comm_flag &= ~(LBIT(ARD_STS_FLAG_2));
			comm_ask(ARD_STATION_2_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_2;
		}
		else if (comm_flag & ARD_STS_FLAG_3)
		{
			comm_flag &= ~ARD_STS_FLAG_3;
			comm_ask(ARD_STATION_3_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_3;
		}
		else if (comm_flag & ARD_STS_FLAG_4)
		{
			comm_flag &= ~ARD_STS_FLAG_4;
			comm_ask(ARD_STATION_4_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_4;
		}
		else if (comm_flag & ARD_STS_FLAG_5)
		{
			comm_flag &= ~ARD_STS_FLAG_5;
			comm_ask(ARD_STATION_5_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_5;
		}
		else if (comm_flag & ARD_STS_FLAG_6)
		{
			comm_flag &= ~ARD_STS_FLAG_6;
			comm_ask(ARD_STATION_6_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_6;
		}
		else if (comm_flag & ARD_STS_FLAG_7)
		{
			comm_flag &= ~ARD_STS_FLAG_7;
			comm_ask(ARD_STATION_7_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_7;
		}
		else if (comm_flag & ARD_STS_FLAG_8)
		{
			comm_flag &= ~ARD_STS_FLAG_8;
			comm_ask(ARD_STATION_8_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_8;
		}
		else if (comm_flag & ARD_STS_FLAG_9)
		{
			comm_flag &= ~ARD_STS_FLAG_9;
			comm_ask(ARD_STATION_9_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_9;
		}
		else if (comm_flag & ARD_STS_FLAG_10)
		{
			comm_flag &= ~ARD_STS_FLAG_10;
			comm_ask(ARD_STATION_10_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_10;
		}
		else if (comm_flag & ARD_STS_FLAG_11)
		{
			comm_flag &= ~ARD_STS_FLAG_11;
			comm_ask(ARD_STATION_11_ADDRESS, ARD_UART,ARD_REG, ARD_REG_NUM, READREG_COMMAND);	/*��ȡ��·��״̬*/  
			WAIT_response_flag = ARD_STS_ANALYSE_11;
		}
	#endif
	}    
	
}

void ModbusServer_init(void)
{
	StoptCounterT100;
	g_CommRxFlag = FALSE; 	/*��վʹ�ܷ���*/
	g_TxDataCtr = 0;
		
	g_PDUData.PDUBuffPtr = UARTBuf[REAL_DATA_UART].RxBuf;
	g_PDUData.PDULength =0;
}

/******************************************************************************
 * ������:	comm_soeinfo_analyse 
 * ����: 		���ò���ͨ���ط�����
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:		
 * ��������:
 * 
 ******************************************************************************/
 void comm_overtime_deal(void)
{
	switch(WAIT_response_flag)
	{
		case WAIT_PARAM_SET_1:	/*װ�ò������ûظ���ʱ����*/
			control_flag |= LBIT(DEV_PARAM_SET_FLAG_1);
		break;

		case WAIT_PARAM_SET_2:	/*װ�ò������ûظ���ʱ����*/
			control_flag |= LBIT(DEV_PARAM_SET_FLAG_2);
		break;
		
		default:
		break;
	}
	/*��ʱ�Ѵ������޵ȴ���־*/
	WAIT_response_flag = WAIT_NONE;
}

/**************************************************************************
������:	            data_received_handle

�������:		��

�������:		��

����ֵ:			��

����˵��:	      ͨ�Ŷ�ʱ����ʱ�������ɸú�����
                                  ά��MODBUS RTU��Լ�е�T15��T35��־
***************************************************************************/
void data_received_handle(USART_LIST uartNo)
{
	if ((REAL_DATA_UART == uartNo) ||(AIR_COND_UART == uartNo)||(UPS_UART == uartNo) ||(TEMP_UART== uartNo))
	{
		/*һ����Ч�Ľ��պ��ø�λΪFALSE����ֹ�����ݴ���֮ǰ
		�������ݳ���ѱ��������ݽ��ջ������е�����*/
		g_CommRxFlag = FALSE; 
		Com_err_counter = 0;
			
		if(Com_err_flag == TRUE)
		{
			Com_err_flag = FALSE;					/*���ͨ�ų����־*/
		}
		Recive_Flag = WAIT_response_flag;			/*��ʾ���յ�����*/
	}
}

/**************************************************************************
������:	            CommTimer

�������:		��

�������:		��

����ֵ:			��

����˵��:	      ͨ�Ŷ�ʱ����ʱ�������ɸú�����
                                  ά��MODBUS RTU��Լ�е�T15��T35��־
***************************************************************************/
void CommTimer(void)
{
	if (g_T100_num >= 0)
	{
		if(g_T100_num > Wait_max_time)			//������Ӧ��ʱ
		{
			StoptCounterT100;
			Com_err_counter++;				//ͨѶ�������

			comm_overtime_deal();
			g_CommRxFlag = FALSE;			/*����Ϊ����״̬*/  
																							
			if(Com_err_counter > 3)	// �ظ���ѯ3�κ���û��Ӧ������״̬		
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
 * ������:	start_comm 
 * ����: 	�豸��Ϊ��������ʼ�����Զ�ȡ��������
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 ******************************************************************************/
void start_comm(void)				
{
	static INT16U polling_counter = 0;	// ÿ5s����һ�����Ͳ�������ѯ

	comm_flag |=LBIT( polling_counter);

	polling_counter++;
	if (polling_counter >= POLLING_NUM)
	{
		polling_counter = 0;
	}
#if 0
	// ��ѯRSU����
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
	
	// ��ѯUPS����
	else if (polling_counter ==5)
	{
		comm_flag |= UPS_PARAM_SEND_FLAG;
	}
	// ��ѯUPS����
	else if (polling_counter ==6)
	{
		comm_flag |= UPS_IN_SEND_FLAG;
	}
	// ��ѯUPS����
	else if (polling_counter ==7)
	{
		comm_flag |= UPS_OUT_SEND_FLAG;
	}
	// ��ѯUPS����
	else if (polling_counter ==8)
	{
		comm_flag |= UPS_BAT_SEND_FLAG;
	}
	// ��ѯUPS����,��֧�ֲ�ѯ
	else if (polling_counter ==9)
	{
		comm_flag |= UPS_ALARM_SEND_FLAG;
	}
	// ��ѯUPS����
	else if (polling_counter ==10)
	{
		comm_flag |= UPS_STATUS_SEND_FLAG;
	}
	
	// ��ѯ��������1
	else if (polling_counter == 11)
	{
		comm_flag |= DEV_PARAM_SEND_FLAG_1;
	}
	// ��ѯ��������2
	else if (polling_counter == 12)
	{
		comm_flag |= DEV_PARAM_SEND_FLAG_2;
	}
	// ��ѯ��ʪ������
	else if (polling_counter ==13)
	{
		comm_flag |= ENVI_TEMP_SEND_FLAG;
	}
	// ��ѯ�յ�״̬����
	else if (polling_counter ==14)
	{
		comm_flag |= ENVI_AIRCOND_ONOFF_FLAG;
	}
	// ��ѯ�յ��¶�����
	else if (polling_counter ==15)
	{
		comm_flag |= ENVI_AIRCOND_TEMP_FLAG;
	}
	// ��ѯ�յ���������
	else if (polling_counter ==16)
	{
		comm_flag |= ENVI_AIRCOND_ALARM_FLAG;
	}
	// ��ѯSPD״̬����
	else if (polling_counter ==17)
	{
		comm_flag |= SPD_TIMES_SEND_FLAG_1;;//comm_flag |= SPD_STATUS_SEND_FLAG;
	}
	// ��ѯSPD��������
	else if (polling_counter ==18)
	{
		comm_flag |= SPD_TIMES_SEND_FLAG_2;
	}
	else if (polling_counter ==19)
	{
		comm_flag |= WATER_IN_FLAG;
	}
	else if (polling_counter ==20)
	{
		comm_flag |= SMOKE_FLAG;
	}
	else if (polling_counter ==21)
	{
		comm_flag |= BREAKER_OPEN_CLOSE_STATUS_1;
	}
	else if (polling_counter ==22)
	{
		comm_flag |= BREAKER_OPEN_CLOSE_STATUS_2;
	}
	
	else if (polling_counter ==23)
	{
		comm_flag |= ARD_STS_FLAG_1;
	}
	else if (polling_counter ==24)
	{
		comm_flag |= ARD_STS_FLAG_2;
	}
	else if (polling_counter ==25)
	{
		comm_flag |= ARD_STS_FLAG_3;
	}
	else if (polling_counter ==26)
	{
		comm_flag |= ARD_STS_FLAG_4;
	}
	else if (polling_counter ==27)
	{
		comm_flag |= ARD_STS_FLAG_5;
	}
	else if (polling_counter ==28)
	{
		comm_flag |= ARD_STS_FLAG_6;
	}
	else if (polling_counter ==29)
	{
		comm_flag |= ARD_STS_FLAG_7;
	}
	else if (polling_counter ==30)
	{
		comm_flag |= ARD_STS_FLAG_8;
	}
	else if (polling_counter ==31)
	{
		comm_flag |= ARD_STS_FLAG_9;
	}
	else if (polling_counter ==32)
	{
		comm_flag |= ARD_STS_FLAG_10;
	}
	else if (polling_counter ==33)
	{
		comm_flag |= ARD_STS_FLAG_11;
		polling_counter = 0;
	}
#endif
}


