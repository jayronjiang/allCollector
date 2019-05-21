/********************************************************************************
*  ��Ȩ����: �������е�����������޹�˾				
*
*  �ļ���:       ModbusServer.c
*
*
*  ��ǰ�汾:                       0.1
*  
*  ����:                                 
*
*  �������:  
*
*
*  ȡ���汾:                       ��
*                 
*  ����:    ��
*
*  �������:                       ��
*
*
*  ����: 
*                           
*                           һ��(��Ӧ�����):MB_GetRegValue,MB_SetRegValue,FReadHook��FwriteHook��SecrateRegPreDeal
                                                                       write_tou_quarter, write_tou_date��DoPowerCal,CheckFrameBorad
*                           
*                          ����(����������):CommCfgPort��UARTInit��SendResponse,UART0_ISR(Comm.c),UART1_ISR(Comm.c)
*
*                           ����:�빦����غ�������������
*
*
*  ����: 
*
*  ��ʷ: 
*                1. 
*                2. 
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
#define StoptCounterT100			(g_T100_num = -1)

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

INT32U  comm_flag=0;

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

INT8U g_CommRxFlag;          	/*ע����Ҫ��ʹ��ΪTRUE�������ϵ�ʱ����ʹ��*/
INT16U g_TxDataCtr;              	/*�������ݷ��͸�������*/
//LONG32U CommAppFlag = 0;
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
 * ������:	comm_ProtectData_analyse 
 * ����: 		���������Ľ���
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 *------------------------
 * �޸���:Amy Wen	
 * �޸�����: ʹ��ָ��Խṹ�������ֵ
 * �޸�����:2012-2-13
 * �޸���:CZH
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-09
 ******************************************************************************/
void comm_RealData_analyse(void)			 
{
	INT8U i;
	INT16U * pointer = &RSUParams.phase[0].vln;/*��0��*/

	if(CRC_check() && (g_PDUData.PDULength == (REAL_DATA_NUM*2+5)))
	{		
		for (i=0;i<2;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i));
		}

		/*��1��*/
		pointer = &RSUParams.phase[1].vln;
		for(i = 7;i <= 8;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i-7));
		}

		/*��2��*/
		pointer = &RSUParams.phase[2].vln;
		for(i = 14;i <= 15;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i-14));
		}

		/*��3��*/
		pointer = &RSUParams.phase[3].vln;
		for(i = 21;i <= 22;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i-21));
		}

		/*��4��*/
		pointer = &RSUParams.phase[4].vln;
		for(i = 27;i <= 28;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i-27));
		}

		/*��5��*/
		pointer = &RSUParams.phase[5].vln;
		for(i = 35;i <= 36;i++)
		{
			char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, (pointer+i-35));
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

/******************************************************************************
 * ������:	comm_ProtectData_set 
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
 * �޸���:	CZH
 * �޸�����: ʹ��ָ��Խṹ�������ֵ
 * �޸�����:2012-10-10
 ******************************************************************************/
void comm_ProtectData_set(void)					/* ������������֡ */
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
	*(g_SENData.SENDBuffPtr + 3) = int_value.b[0];		/* �Ĵ�����ַ */
	*(g_SENData.SENDBuffPtr + 4) = 0X00;
	*(g_SENData.SENDBuffPtr + 5) = reg_num;
	*(g_SENData.SENDBuffPtr + 6) = reg_num*2;  
	
	for(i =0;i<3;i++)	/*������բ�澯���鷽ʽ,���ƹ���Ͷ����*/
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM+ i*2,*(pointer+i));
	}
	/*����������բ�澯Ͷ����*/
	long_to_char(g_SENData.SENDBuffPtr+ SET_FRAME_HEAD_NUM + 3*2, protect_param.trip_key_word);
	long_to_char(g_SENData.SENDBuffPtr+ SET_FRAME_HEAD_NUM + 5*2, protect_param.alarm_key_word);	
	/*�������ƹ�����ز���*/
	protect_param.black1 = 0;		/*����λ��0*/
	protect_param.black2 = 0;
	protect_param.black3 = 0;
	protect_param.black4 = 0;
	pointer = &protect_param.short_I_rate;
	for(i =1;i<=50;i++)
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM+ (6+i)*2,*(pointer+(i-1)));
	}
	/*R1���ڿ�����*/
	for(i=0;i<3;i++)
	{
		long_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM+ 57*2+4*i, protect_param.protect_link_switch[i]);
	}   
	/*R1����չ�������������Ʋ�����*/
	pointer = &protect_param.link_action_return_time1;
	for(i =0;i<30;i++)
	{
		int_to_char(g_SENData.SENDBuffPtr + SET_FRAME_HEAD_NUM+ (63+i)*2,*(pointer+ i));
	}
	/*CRCУ����*/
	int_value.i=get_crc16(g_SENData.SENDBuffPtr,SET_FRAME_HEAD_NUM+PROTECT_PARAM_NUM*2);
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+PROTECT_PARAM_NUM*2)= int_value.b[0];
	*(g_SENData.SENDBuffPtr+SET_FRAME_HEAD_NUM+PROTECT_PARAM_NUM*2+1)= int_value.b[1];
	g_SENData.SENDLength=SET_FRAME_HEAD_NUM+PROTECT_PARAM_NUM*2+2;
	SendResponse();
	#endif
}


/******************************************************************************
 * ������:	comm_DeviceParam_analyse 
 * ����: 		�ɱ���߼���������
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 *------------------------
 * �޸���:CZH
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-16
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
 * ������:	comm_DeviceParam_analyse 
 * ����: 		�ɱ���߼���������
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2009.1.6
 * 
 *------------------------
 * �޸���:CZH
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-16
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
		if (ENVIParms.air_cond_temp_out > 1000)
		{
			ENVIParms.air_cond_temp_out = 0xFFFF;
		}

		/*������*/
		pointer = &ENVIParms.air_cond_temp_in;
		i = 2;	// �����¶�����1���Ĵ���
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + i*2, pointer);
		if (ENVIParms.air_cond_temp_in > 1000)
		{
			ENVIParms.air_cond_temp_in = 0xFFFF;
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

	if(reg_type!=READREG_COMMAND )
	{
		return;
	}
	else 
	{		
		*(g_SENData.SENDBuffPtr + 1) = reg_type;
	}	
	
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
������:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

�������:		start_reg:	��ȡ�ļĴ�����ʼ��ַ
              reg_num:		��ȡ�ļĴ�������

�������:		��.

����ֵ:		��.

����˵��:              ��ȡ�Ĵ�������
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
	
	*(g_SENData.SENDBuffPtr + 2) = reg.b[1];				/*�Ĵ�����ַ*/
	*(g_SENData.SENDBuffPtr + 3) = reg.b[0];
	
	*(g_SENData.SENDBuffPtr + 4) = 0x00;					/*�Ĵ�������*/
	*(g_SENData.SENDBuffPtr + 5) = reg_num;
			
	 reg.i = get_crc16(g_SENData.SENDBuffPtr,6);			/*����CRCУ��*/
	*(g_SENData.SENDBuffPtr + 6) = reg.b[0];
	*(g_SENData.SENDBuffPtr + 7) = reg.b[1];
	
	g_SENData.SENDLength= 8;
	g_TxDataCtr = 0;
	//SendResponse();
} 


/***********************************************************************
������:		comm_ask(INT16U start_reg,INT8U reg_num,INT8U reg_type)

�������:		start_reg:	��ȡ�ļĴ�����ʼ��ַ
              reg_num:		��ȡ�ļĴ�������

�������:		��.

����ֵ:		��.

����˵��:              ��ȡ�Ĵ�������
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
	
	*(g_SENData.SENDBuffPtr + 2) = reg.b[1];				/*�Ĵ�����ַ*/
	*(g_SENData.SENDBuffPtr + 3) = reg.b[0];
	
	*(g_SENData.SENDBuffPtr + 4) = 0x00;					/*�Ĵ�������*/
	*(g_SENData.SENDBuffPtr + 5) = reg_num;
			
	 reg.i = get_crc16(g_SENData.SENDBuffPtr,6);			/*����CRCУ��*/
	*(g_SENData.SENDBuffPtr + 6) = reg.b[0];
	*(g_SENData.SENDBuffPtr + 7) = reg.b[1];
	
	g_SENData.SENDLength= 8;
	g_TxDataCtr = 0;
	//SendResponse();
} 
#endif
/***********************************************************************
������:			void comm_process(void)

�������:		��.

�������:		��.

����ֵ:			��.

����˵��:           ͨ�Ŵ�����,��ģ���ʹ���߸���ͨ�Ÿ��ɺ�Ƶ�ʵĴ�С����
***********************************************************************/
void comm_polling_process(void)
{
	static INT8U Init_sign = FALSE;
	static INT8U Init_send = FALSE;
	
	if( Init_sign == FALSE)	/*��ʼ��װ�ò���*/
	{
		Init_sign = TRUE;
		comm_flag |= DEV_PARAM_SEND_FLAG_2;		/*�ȶ�ȡװ�ò���*/
		//comm_flag |= PROTECT_INIT_FLAG;			/*��ʼ����������*/
		//comm_flag &= ~RANDOM_SEND_FLAG;		/*�ڳ�ʼ��װ�ò���ǰ����ˢ��ʵʱ����*/
	}

	/********************************************************************************************************************
	***�˴���ͨ�Ų�ѯ������ָ���ͻ������д���SOE��ѯ���ȼ���ߣ����Ϊ���� ***
	***���ã����Ϊ������ѯ�����ڽ��յ���ȷ�Ĳ�������ָ��󣬻�δ��װ�ò�����  ***
	***������ʱ�ֽ��յ����ò���ָ����������ȡ����	����������ֱ�ӽ�������	    ***
	***����������ָ����SOE��ѯָ���ͻʱ��SOE��ѯָ������,��Recive_Flag�����ж�����     ***
	***��־�����Դ��ַ����ڲ�������ʱ��������ȫ���β�������������Ҫ�ڲ�������  ***
	***ʱ��һ�����ˣ���װ�ò�����������"if(!(comm_flag & PARAM_SET_FLAG))"����Ҫʮ��ע�� !!!!    ***
	********************************************************************************************************************/
	 /*������ֲ�������֡���򽫽��ձ�־�Լ����ͱ�־������*/
	if (comm_flag & (DEV_PARAM_SET_FLAG_1 +DEV_PARAM_SET_FLAG_2 + DOOR_OPEN_SET_FLAG + DOOR_CLOSE_SET_FLAG) )		
	{
		// ��ѯҲȡ����
		comm_flag &= ~(REAL_DATA_SEND_FLAG +UPS_PARAM_SEND_FLAG +SPD_STATUS_SEND_FLAG +SPD_TIMES_SEND_FLAG \
					     +ENVI_TEMP_SEND_FLAG +ENVI_AIRCOND_ONOFF_FLAG+ENVI_AIRCOND_TEMP_FLAG \
					     +ENVI_AIRCOND_ALARM_FLAG + DEV_PARAM_SEND_FLAG_1+ DEV_PARAM_SEND_FLAG_2);
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
			case REAL_DATA_ANALYSE:					/*���յ�ʵʱ����*/
				comm_RealData_analyse();
			break;
			
			case DEVICE_DATA_1_ANALYSE:				/*���յ�װ�ò���*/
				if(!(comm_flag & DEV_PARAM_SET_FLAG_1))
				{
					// װ�ò���
					comm_DeviceParam_1_analyse();
				}
			break;

			case DEVICE_DATA_2_ANALYSE:				/*���յ�װ�ò���*/
				if(!(comm_flag & DEV_PARAM_SET_FLAG_2))
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
		WAIT_response_flag = 0;						/*���ȴ���־�Լ����ձ�־������*/
		g_PDUData.PDULength = 0;
	}
	/**************************************���͹��̴���*********************************************/
	// 2��ͬʱ������bug
	if((!WAIT_response_flag) && (g_CommRxFlag == FALSE))	
	{
		/********************************���Ͳ�������֡����*************************************/
		/*�յ���������ΪMODBUSͨ��*/
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
		
		// RSU���ݣ������MODBUSЭ��
		else if (comm_flag & REAL_DATA_SEND_FLAG)
		{
			comm_flag &= ~REAL_DATA_SEND_FLAG;
			/*��ȡʵʱ��������,�������ݵ�*/  
			comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			WAIT_response_flag = REAL_DATA_ANALYSE;
		}

		else if (comm_flag & DEV_PARAM_SEND_FLAG_1)
		{
			comm_flag &= ~DEV_PARAM_SEND_FLAG_1;
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,AIR_ONOFF_REG, AIR_ONOFF_SET_NUM, READREG_COMMAND);	/*��ȡװ�ò�������,�������ݵ�*/  
			WAIT_response_flag = DEVICE_DATA_1_ANALYSE;
		}

		else if (comm_flag & DEV_PARAM_SEND_FLAG_2)
		{
			comm_flag &= ~DEV_PARAM_SEND_FLAG_2;
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,AIR_TEMP_REG, AIR_TEMP_SET_NUM, READREG_COMMAND);	/*��ȡװ�ò�������,�������ݵ�*/  
			if( Init_send == FALSE)	/*��ʼ��װ�ò���*/
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
			//ups�ߵ��ǵ���Э��
			comm_ask_ups(UPS_REG, UPS_DATA_NUM, READREG_COMMAND);	/*��ȡUPS��������*/  
			WAIT_response_flag = UPS_DATA_ANALYSE;
			#endif
		}

		else if (comm_flag & SPD_STATUS_SEND_FLAG)
		{
			#if 0
			comm_flag &= ~SPD_STATUS_SEND_FLAG;
			comm_ask_spd(SPD_STATUS_REG, SPD_STATUS_NUM, 0x02);	/*��ȡSPD��������*/  
			WAIT_response_flag = SPD_STATUS_ANALYSE;
			#endif
		}

		else if (comm_flag & SPD_TIMES_SEND_FLAG)
		{
			#if 0
			comm_flag &= ~SPD_TIMES_SEND_FLAG;
			comm_ask_spd(SPD_TIMES_REG, SPD_STATUS_NUM, 0x04);		/*��ȡSPD��������*/  
			WAIT_response_flag = SPD_TIMES_ANALYSE;
			#endif
		}

		else if (comm_flag & ENVI_TEMP_SEND_FLAG)
		{
			comm_flag &= ~ENVI_TEMP_SEND_FLAG;
			comm_ask(TEMP_STATION_ADDRESS,TEMP_UART,ENVI_TEMP_REG, ENVI_TEMP_NUM, READREG_COMMAND);	/*��ȡ��ʪ������*/  
			WAIT_response_flag = ENVI_TEMP_ANALYSE;
		}

		else if (comm_flag & ENVI_AIRCOND_ONOFF_FLAG)
		{
			comm_flag &= ~ENVI_AIRCOND_ONOFF_FLAG;
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,ENVI_AIRCOND_ONOFF_REG, ENVI_AIRCOND_ONOFF_NUM, READREG_COMMAND);	/*��ȡ�յ�״̬����*/  
			WAIT_response_flag = ENVI_AIR_ONOFF_ANALYSE;
		}

		else if (comm_flag & ENVI_AIRCOND_TEMP_FLAG)
		{
			comm_flag &= ~ENVI_AIRCOND_TEMP_FLAG;
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,ENVI_AIRCOND_TEMP_REG, ENVI_AIRCOND_TEMP_NUM, READREG_COMMAND);	/*��ȡ�յ��¶�����*/  
			WAIT_response_flag = ENVI_AIR_TEMP_ANALYSE;
		}

		else if (comm_flag & ENVI_AIRCOND_ALARM_FLAG)
		{
			comm_flag &= ~ENVI_AIRCOND_ALARM_FLAG;
			comm_ask(AIR_STATION_ADDRESS, AIR_COND_UART,ENVI_AIRCOND_ALARM_REG, ENVI_AIRCOND_ALARM_NUM, READREG_COMMAND);	/*��ȡ�յ���������*/  
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
	g_CommRxFlag = FALSE; 	/*��վʹ�ܷ���*/
	g_TxDataCtr = 0;
		
	g_PDUData.PDUBuffPtr = UARTBuf[MEAS_UART].RxBuf;
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
 * ����:		CZH
 * ��������:	2012.10.10
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
 void comm_overtime_deal(void)
{
	switch(WAIT_response_flag)
	{
		case WAIT_PARAM_SET_1:	/*װ�ò������ûظ���ʱ����*/
			comm_flag |= DEV_PARAM_SET_FLAG_1;
		break;

		case WAIT_PARAM_SET_2:	/*װ�ò������ûظ���ʱ����*/
			comm_flag |= DEV_PARAM_SET_FLAG_2;
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
	if ((MEAS_UART == uartNo) ||(AIR_COND_UART == uartNo)||(UPS_UART == uartNo) ||(TEMP_UART== uartNo))
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
		if(g_T100_num > Wait_max_time)				//������Ӧ��ʱ
		{
			StoptCounterT100;
			Com_err_counter++;						//ͨѶ�������

			comm_overtime_deal();
			g_CommRxFlag = FALSE;					/*����Ϊ����״̬*/  
																							
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
 * ������:	start_comm 
 * ����: 		����1sˢ�£�ǰ500msˢ�����ݣ���500msˢ��SOE��¼
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
void start_comm(void)				
{
	static INT16U polling_counter = 0;	// ÿ5s����һ�����Ͳ�������ѯ

	polling_counter++;

	// ��ѯRSU����
	if (polling_counter == 1)
	{
		comm_flag |= REAL_DATA_SEND_FLAG;
	}
	// ��ѯ��������1
	else if (polling_counter == 2)
	{
		comm_flag |= DEV_PARAM_SEND_FLAG_1;
	}
	// ��ѯ��������2
	else if (polling_counter == 3)
	{
		comm_flag |= DEV_PARAM_SEND_FLAG_2;
	}
	// ��ѯ��ʪ������
	else if (polling_counter ==4)
	{
		comm_flag |= ENVI_TEMP_SEND_FLAG;
	}
	// ��ѯ�յ�״̬����
	else if (polling_counter ==5)
	{
		comm_flag |= ENVI_AIRCOND_ONOFF_FLAG;
	}
	// ��ѯ�յ��¶�����
	else if (polling_counter ==6)
	{
		comm_flag |= ENVI_AIRCOND_TEMP_FLAG;
	}
	// ��ѯ�յ���������
	else if (polling_counter ==7)
	{
		comm_flag |= ENVI_AIRCOND_ALARM_FLAG;
		polling_counter = 0;
	}
}


