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
		rs485FuncSelect(SEND_S);		//485��ʼ����
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


/******************************************************************************
 * ������:	comm_UPSParam_ANALYSE 
 * ����: 		UPS�ſ����ݽ���
 *            -
 * �������: 
 * �������: 
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
 #define UPS_PARAM_RES_LEN		72			 // 0x42�������ݳ���72
 #define VOLT_AIN_POS				15			// 15�ֽڿ�ʼ��A�������ѹ��ֵ
 #define VOLT_BIN_POS				(VOLT_AIN_POS+4)			// 19�ֽڿ�ʼ��B���ѹ��ֵ
 #define VOLT_CIN_POS				(VOLT_BIN_POS+4)			// 23�ֽڿ�ʼ��C���ѹ��ֵ

 #define VOLT_AOUT_POS			(VOLT_CIN_POS+4)			// 27�ֽڿ�ʼ��A���ѹ��ֵ
 #define VOLT_BOUT_POS			(VOLT_AOUT_POS+4)		// 31�ֽڿ�ʼ��B���ѹ��ֵ
 #define VOLT_COUT_POS			(VOLT_BOUT_POS+4)		// 35�ֽڿ�ʼ��C���ѹ��ֵ

 #define AMP_AOUT_POS			(VOLT_COUT_POS+4)			// 27�ֽڿ�ʼ��A���ѹ��ֵ
 #define AMP_BOUT_POS			(AMP_AOUT_POS+4)		// 31�ֽڿ�ʼ��B���ѹ��ֵ
 #define AMP_COUT_POS			(AMP_BOUT_POS+4)		// 35�ֽڿ�ʼ��C���ѹ��ֵ

 #define FREQ_OUT_POS			(AMP_COUT_POS+8)		// 35�ֽڿ�ʼ��C���ѹ��ֵ
 
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

		// ���Ƶ��
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FREQ_OUT_POS);
	}
}


/******************************************************************************
 * ������:	comm_UPSIn_ANALYSE 
 * ����: 		UPS�������ݽ���
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
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-16
 ******************************************************************************/
 #define UPS_IN_RES_LEN			96			 // 0x42�������ݳ���72
 #define UPS_PHASE_NUM_IN_POS	19			// ��������
 #define AMP_AIN_POS				35			// 35�ֽڿ�ʼ��A�����������ֵ
 #define AMP_BIN_POS				(AMP_AIN_POS+4)			// 19�ֽڿ�ʼ��B���ѹ��ֵ
 #define AMP_CIN_POS				(AMP_BIN_POS+4)			// 23�ֽڿ�ʼ��C���ѹ��ֵ
 #define FREQ_IN_POS				(AMP_CIN_POS+4)

 #define FACT_AIN_POS				(FREQ_IN_POS+4)
 #define FACT_BIN_POS				(FACT_AIN_POS+4)
 #define FACT_CIN_POS				(FACT_BIN_POS+4)

 #define BYPASS_AIN_POS			(FACT_CIN_POS+4)
 #define BYPASS_BIN_POS			(BYPASS_AIN_POS+4)
 #define BYPASS_CIN_POS			(BYPASS_BIN_POS+4)
 #define BYPASS_FREQ_POS			(BYPASS_CIN_POS+16)	//�������ߵ�ѹ����
 
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

		// ��·Ƶ��
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_FREQ_POS);
	}
}


/******************************************************************************
 * ������:	comm_UPSOut_ANALYSE 
 * ����: 		UPS������ݽ���0xE1
 *            -
 * �������: 
 * �������: 
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
 #define UPS_OUT_RES_LEN			88			 // 0xE1�������ݳ���72
 #define UPS_PHASE_NUM_OUT_POS	19			// ��������
 #define FACT_AOUT_POS			(UPS_PHASE_NUM_OUT_POS+4)			// 35�ֽڿ�ʼ��A�����������ֵ
 #define FACT_BOUT_POS			(FACT_AOUT_POS+4)			// 19�ֽڿ�ʼ��B���ѹ��ֵ
 #define FACT_COUT_POS			(FACT_BOUT_POS+4)			// 23�ֽڿ�ʼ��C���ѹ��ֵ

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
 * ������:	comm_UPSBat_ANALYSE 
 * ����: 		UPS������ݽ���0xE3
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
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-16
 ******************************************************************************/
#define UPS_BAT_RES_LEN			72			 // 0x42�������ݳ���72
#define BAT_RUNNING_DAY_POS		19			// 35�ֽڿ�ʼ��A�����������ֵ
#define BAT_VOLT_POS				(BAT_RUNNING_DAY_POS+4)			// 19�ֽڿ�ʼ�ǵ����������
#define BAT_CHARGE_AMP_POS		(BAT_VOLT_POS+4)
#define BAT_DISCHG_AMP_POS		(BAT_CHARGE_AMP_POS+4)
#define BAT_BACKUP_TIME_POS		(BAT_DISCHG_AMP_POS+16)
#define BAT_TMP_POS				(BAT_BACKUP_TIME_POS+4)	//����¶�,��֧��
#define BAT_ENVI_TMP_POS			(BAT_TMP_POS+4)
#define BAT_VOLUMN_POS			(BAT_ENVI_TMP_POS+4)
#define BAT_DISCHG_TIMES_POS	(BAT_VOLUMN_POS+4)		//�ŵ����
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
		*(pointer+5) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_ENVI_TMP_POS);	// �����¶�
		*(pointer+6) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_VOLUMN_POS);		// �������
		*(pointer+7) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_DISCHG_TIMES_POS);		// �ŵ����
	}
}

#if 0
/******************************************************************************
 * ������:	comm_UPSTime_ANALYSE 
 * ����: 		UPS�������ʱ�����0xE4
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
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-16
 ******************************************************************************/
#define UPS_BATTIME_RES_LEN			56			 // 0xE4�������ݳ���72



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
		*(pointer+5) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_ENVI_TMP_POS);	// �����¶�
		*(pointer+6) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_VOLUMN_POS);		// �������
		*(pointer+7) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_DISCHG_TIMES_POS);		// �ŵ����
	}
}
#endif


/******************************************************************************
 * ������:	comm_UPSStatus_ANALYSE 
 * ����: 		UPS������ݽ���0x43
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
 * �޸�����: ���¹�Լ���н���
 * �޸�����:2012-10-16
 ******************************************************************************/
#define UPS_STATUS_RES_LEN		40			 // 0x42�������ݳ���40
#define BAT_SUPPLY_OUT_POS		15			// 15�ֽڿ�ʼ��UPS������緽ʽ
#define BAT_SUPPLY_IN_POS		(BAT_SUPPLY_OUT_POS+4)		// 19�ֽڿ�ʼ��UPS���빩�緽ʽ
#define BAT_STATUS_POS			(BAT_SUPPLY_IN_POS+2)		// 19�ֽڿ�ʼ��UPS���빩�緽ʽ

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
void comm_SPDTimes_analyse(void)	
{
	INT16U* pointer = &SPDParams.struck_times;

	if(CRC_check() && (g_PDUData.PDULength == (SPD_TIMES_NUM*2+5)))
	{
		// ֻ��Ҫ���׻�����
		char_to_int(g_PDUData.PDUBuffPtr + FRAME_HEAD_NUM + 1*2, pointer);
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
} 


/*����lenth�������Э���LCHKSUM*/
 UINT16 lchkSumCalc(UINT16 len_value)
{
	INT16U re_value = 0;

	/*�ȶ�D0D1D2D3+D4D5D6D7+D8D9D10D11���*/
	re_value = ((len_value&0x0F) + ((len_value>>4)&0x0F) + ((len_value>>8)&0x0F));
	re_value = (~(re_value&0x0F))+1;	// ��ģ16����ȡ����1
	re_value = re_value&0x0F;

	return re_value;
}


/*����lenth�������Э���CHECKSUM*/
 UINT16 checkSumCalc(UINT8 *buffer, UINT8 len)
{
	INT16U re_value = 0;
	INT8U i = 0;

	/*�ȶ����г�SIO֮������*/
	for (i = 0; i<len; i++)
	{
		re_value = re_value+buffer[i];
	}
	// ģ65535������+1
	re_value = (~(re_value&0xFFFF))+1;	// ��ģ65536����ȡ����1

	return re_value;
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
	//static INT8U last_rec_flag = 0;
	//bool try_again = FALSE;
	
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
		comm_flag &= ~(REAL_DATA0_SEND_FLAG +REAL_DATA1_SEND_FLAG+REAL_DATA2_SEND_FLAG +REAL_DATA3_SEND_FLAG \
					     +UPS_PARAM_SEND_FLAG +SPD_STATUS_SEND_FLAG +SPD_TIMES_SEND_FLAG \
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
			case REAL_DATA0_ANALYSE:
				comm_RealData_analyse(CHANNEL_0);		/*���յ�ʵʱ����*/
				#if 0		// �ڴ��ڽ��յ�ʱ���Ѿ�У���˳���
				if (!comm_RealData_analyse(CHANNEL_0))		// ���ݲ��ԣ��ٽ���һ��
				{
					if (last_rec_flag != Recive_Flag)		// ����ǵ�һ�ν���
					{
						try_again = TRUE;				//�ٽ���һ��
					}
				}
				last_rec_flag = Recive_Flag;
				#endif
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
		
		// RSU����
		else if (comm_flag & REAL_DATA0_SEND_FLAG)
		{
			comm_flag &= ~REAL_DATA0_SEND_FLAG;
			/*��ȡʵʱ��������,�������ݵ�*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_0);
			WAIT_response_flag = REAL_DATA0_ANALYSE;
		}
		// RSU����
		else if (comm_flag & REAL_DATA1_SEND_FLAG)
		{
			comm_flag &= ~REAL_DATA1_SEND_FLAG;
			/*��ȡʵʱ��������,�������ݵ�*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_1);
			WAIT_response_flag = REAL_DATA1_ANALYSE;
		}
		// RSU����
		else if (comm_flag & REAL_DATA2_SEND_FLAG)
		{
			comm_flag &= ~REAL_DATA2_SEND_FLAG;
			/*��ȡʵʱ��������,�������ݵ�*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_2);
			WAIT_response_flag = REAL_DATA2_ANALYSE;
		}
		// RSU����
		else if (comm_flag & REAL_DATA3_SEND_FLAG)
		{
			comm_flag &= ~REAL_DATA3_SEND_FLAG;
			/*��ȡʵʱ��������,�������ݵ�*/  
			//comm_ask(RSU_STATION_ADDRESS, MEAS_UART,RSU_REG, REAL_DATA_NUM, READREG_COMMAND);
			comm_wait(REAL_DATA_UART,CHANNEL_3);
			WAIT_response_flag = REAL_DATA3_ANALYSE;
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
			comm_flag &= ~UPS_PARAM_SEND_FLAG;
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_ALL, 0, NULL);
			WAIT_response_flag = UPS_PARAM_ANALYSE;
		}
		else if (comm_flag & UPS_IN_SEND_FLAG)
		{
			comm_flag &= ~UPS_IN_SEND_FLAG;
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_IN, 0, NULL);
			WAIT_response_flag = UPS_IN_ANALYSE;
		}
		else if (comm_flag & UPS_OUT_SEND_FLAG)
		{
			comm_flag &= ~UPS_OUT_SEND_FLAG;
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_OUT, 0, NULL);
			WAIT_response_flag = UPS_OUT_ANALYSE;
		}
		else if (comm_flag & UPS_BAT_SEND_FLAG)
		{
			comm_flag &= ~UPS_BAT_SEND_FLAG;
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_BAT, 0, NULL);
			WAIT_response_flag = UPS_BAT_ANALYSE;
		}
		#if 0
		// Э�鲻֧��
		else if (comm_flag & UPS_TIME_SEND_FLAG)
		{
			comm_flag &= ~UPS_TIME_SEND_FLAG;
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_TIME, 0, NULL);
			WAIT_response_flag = UPS_TIME_ANALYSE;
		}
		#endif
		else if (comm_flag & UPS_STATUS_SEND_FLAG)
		{
			comm_flag &= ~UPS_STATUS_SEND_FLAG;
			//ups�ߵ��ǵ���Э��
			/*��ȡUPS��������*/  
			comm_ask_ups(UPS_STATION_ADDRESS, UPS_UART,UPS_CID2_STATUS, 0, NULL);
			WAIT_response_flag = UPS_STATUS_ANALYSE;
		}


		else if (comm_flag & SPD_STATUS_SEND_FLAG)
		{
			comm_flag &= ~SPD_STATUS_SEND_FLAG;
			comm_ask_spd(SPD_STATION_ADDRESS, SPD_UART,SPD_STATUS_REG, SPD_STATUS_NUM, SPD_DIREAD_COMMAND);	/*��ȡSPD��������*/  
			WAIT_response_flag = SPD_STATUS_ANALYSE;
		}

		else if (comm_flag & SPD_TIMES_SEND_FLAG)
		{
			comm_flag &= ~SPD_TIMES_SEND_FLAG;
			comm_ask_spd(SPD_STATION_ADDRESS, SPD_UART,SPD_TIMES_REG, SPD_TIMES_NUM, SPD_TIMESREAD_COMMAND);		/*��ȡSPD��������*/  
			WAIT_response_flag = SPD_TIMES_ANALYSE;
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
		;
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
		comm_flag |= SPD_STATUS_SEND_FLAG;

	}
	// ��ѯSPD��������
	else if (polling_counter ==18)
	{
		comm_flag |= SPD_TIMES_SEND_FLAG;
		polling_counter = 0;
	}
}


