/********************************************************************************
*  ��Ȩ����: �������е�����������޹�˾				
*
*  �ļ���:       ModbusClient.h
*
*
*  ��ǰ�汾:                       0.1
*  
*  ����:                                  ��ΰ
*
*  �������:  
*
*
*  ȡ���汾:    ��
*                 
*  ����:    ��
*
*  �������:     ��
*
*
*  ����: 
*
*  ����: ����51���жϺ�������Ҫ���䵽COMMON����˽�MODBUSģ�����������
*                �����ŵ�COMM.C���γ�һ��������Դ�ļ���������Ӧ���п��Խ�COMM.C
*                �еĴ��뵼�뱾ģ��
*
*  ��ʷ: 
*                1. 
*                2. 
********************************************************************************/

#ifndef _ModbusServer_H
#define _ModbusServer_H
/*********************************************************************************
*                                     ModbusClientģ�����ó���
***********************************************************************************/
#define COMMBUFLEN		  256     /*ͨ�Ż������ֽ�*/
#define BAUDNUM		          5      /*��ѡ�Ĳ����ʸ���*/

#define COMM_NUM		   BD_USART_NUM        /*ͨ�ſڵ���Ŀ����ͨ�ſ�*/
/*********************************************************************************
*                                     ͨ�Ŵ�����
**********************************************************************************/
#define	SUCCESSFULLCOMMAND		00                            /*�ɹ���MODBUS����*/
#define	ILLEGALfUNCTIONCODE		01                           /*����ȷ�Ĺ�����*/ 
#define	ILLEGALREGQADDRESS		02                           /*����ȷ�ļĴ�����ַ*/
#define	ILLEGALFILEADDRESS		02                           /*����ȷ���ļ���ַ*/
#define	ILLEGALREGQATITY			03                           /*����ȷ�ļĴ�������*/ 
#define	FAILREGOPERATOR			04                            /*ʧ�ܵļĴ�������*/
#define	ILLEGALFRAMELENGTH		03                            /*�Ƿ�������֡����*/    
#define	ILLEGALREGVALUE			03                             /*�Ƿ�����ֵ*/    

/*********************************************************************************
*                                   	 ͨ�Ź�����
**********************************************************************************/
#define	WRITEREG_COMMAND				0x10
#define	READREG_COMMAND				0x03
#define	WRITEFILE_COMMAND			0x15
#define 	READFILE_COMMAND				0x14
#define	WRITESECRATEREG_COMMAND	0x47 
#define	READSECRATEREG_COMMAND		0x46
#define	WRITESINGLECOIL_COMMAND		0x05 
#define 	REF_TYPE_CODE   	0x06


#define	SPD_DIREAD_COMMAND			0x02		//�����Լ��Ķ�������Ϊ02
#define	SPD_TIMESREAD_COMMAND		0x04		//�����Լ����׻���������Ϊ04


#define 	RSU_STATION_ADDRESS  	01		/*Ĭ��RSU��վ��ַ,δʹ��*/
#define 	AIR_STATION_ADDRESS  	02		/*Ĭ�Ͽյ���վ��վ��ַ*/
#define 	TEMP_STATION_ADDRESS  	03		/*Ĭ����ʪ�ȴ�վ��վ��ַ*/
#define 	UPS_STATION_ADDRESS  	01		/*Ĭ��UPS��վ��ַ*/
#define 	SPD_STATION_ADDRESS	05

#define Wait_max_time  		200 		/* ���ͺ�ȴ�����֡���ʱ��Ϊ200ms */

/*********************************************************************************
*                                      ͨ�����ò�����־
**********************************************************************************/
#define REAL_DATA0_SEND_FLAG    	BIT0		/*ʵʱ���ݲ�ѯ,������0*/
#define REAL_DATA1_SEND_FLAG    	BIT1		/*ʵʱ���ݲ�ѯ,������1*/
#define REAL_DATA2_SEND_FLAG    	BIT2		/*ʵʱ���ݲ�ѯ,������2*/
#define REAL_DATA3_SEND_FLAG    	BIT3		/*ʵʱ���ݲ�ѯ,������3*/

#define UPS_PARAM_SEND_FLAG		BIT4		/*USP���������*/
#define UPS_IN_SEND_FLAG			BIT5		/*USP����������*/
#define UPS_OUT_SEND_FLAG		BIT6		/*USP����������*/
#define UPS_BAT_SEND_FLAG		BIT7		/*USP�ĵ�ز���*/
#define UPS_TIME_SEND_FLAG		BIT8		/*USP������ʱ�����*/
#define UPS_STATUS_SEND_FLAG	BIT9		/*USP������״̬����*/


#define SPD_STATUS_SEND_FLAG		BIT10
#define SPD_TIMES_SEND_FLAG			BIT11

#define ENVI_TEMP_SEND_FLAG         		BIT12		// ��ʪ��
#define ENVI_AIRCOND_ONOFF_FLAG         BIT13		// �յ�������ȡ
#define ENVI_AIRCOND_TEMP_FLAG         	BIT14		// �յ�������ȡ
#define ENVI_AIRCOND_ALARM_FLAG         BIT15		// �յ�������ȡ

#define DEV_PARAM_SEND_FLAG_1         	BIT16		/*������ѯ*/
#define DEV_PARAM_SEND_FLAG_2         	BIT17		/*������ѯ*/

#define DEV_PARAM_SET_FLAG_1          	BIT18	 // ��������	--�յ����ػ�
#define DEV_PARAM_SET_FLAG_2       	BIT19	 // ��������	--�յ��¶�����

#define DOOR_OPEN_SET_FLAG          		BIT20	 // ��������
#define DOOR_CLOSE_SET_FLAG          	BIT21	 // ��������


/*********************************************************************************
*                                     ��ȡ��ز�����ʼ��ַ
**********************************************************************************/
#define RSU_REG        		0x40				// �ӵ�һ·��ѹ��ʼ��
#define UPS_REG   			0x01
#define SPD_STATUS_REG   	0x00				// ���׵�����״̬�ӵ�ַ0��ʼ
#define SPD_TIMES_REG   	0x0E				// 04���0E��ʼ��
#define AIR_ONOFF_REG             		0x0801	// �յ����ػ�
#define AIR_TEMP_REG             		0x0700	// �յ����µ��µ�

#define ENVI_TEMP_REG             					0x00		// ��ʪ��
#define ENVI_AIRCOND_ONOFF_REG             		0x0100	// �յ�״̬
#define ENVI_AIRCOND_TEMP_REG             			0x0501	// �յ��¶�
#define ENVI_AIRCOND_ALARM_REG             		0x0600	// �յ��¶�


/*********************************************************************************
*                                         �ȴ�֡�ظ�״̬
**********************************************************************************/
#define WAIT_NONE			0	/*�޵ȴ�*/
#define WAIT_PARAM_SET_1		1	/*�ȴ�װ�ò�������֡�ظ�*/
#define WAIT_PARAM_SET_2		2	/*�ȴ�װ�ò�������֡�ظ�*/
// ��Ҫ����������֡
#define REAL_DATA0_ANALYSE		3	/*ʵʱ���ݽ���,��RSU����*/
#define REAL_DATA1_ANALYSE		4	/*ʵʱ���ݽ���,��RSU����*/
#define REAL_DATA2_ANALYSE		5	/*ʵʱ���ݽ���,��RSU����*/
#define REAL_DATA3_ANALYSE		6	/*ʵʱ���ݽ���,��RSU����*/

#define ENVI_TEMP_ANALYSE	9	/*��ʪ�Ȳ�������*/
//#define ENVI_AIR_ANALYSE		10	/*�յ���������*/
#define ENVI_AIR_ONOFF_ANALYSE		10	/*�յ���������*/
#define ENVI_AIR_TEMP_ANALYSE		11	/*�յ���������*/
#define ENVI_AIR_ALARM_ANALYSE		12	/*�յ���������*/

//#define UPS_DATA_ANALYSE	13	/*UPS��������*/
#define SPD_STATUS_ANALYSE	14	/*���ײ�������*/
#define SPD_TIMES_ANALYSE	15	/*���ײ�������*/

#define DEVICE_DATA_1_ANALYSE	16	/*װ�ò�����ȡ����,�յ����ػ�*/
#define DEVICE_DATA_2_ANALYSE	17	/*װ�ò�����ȡ����,�յ��¶�*/

#define UPS_PARAM_ANALYSE		18
#define UPS_IN_ANALYSE			19
#define UPS_OUT_ANALYSE			20
#define UPS_BAT_ANALYSE			21
#define UPS_TIME_ANALYSE			22
#define UPS_STATUS_ANALYSE		23

/*********************************************************************************
*                                     ��ȡ��ز�������
**********************************************************************************/	
#define REAL_DATA_NUM		24  	/*�̶�Ϊ24���ֽ�*/
//#define REAL_TIME_SOE_NUM			37  		/*��ȡSOE��ͳ����Ϣ�����ݳ���*/
//#define UPS_DATA_NUM 			4
#define SPD_STATUS_NUM 			0x11		// ���������ȡ��17�����ȣ���ʵ��17λ,��3���ֽ�
#define SPD_TIMES_NUM 			3		// ��3��,�ֱ�Ϊ��ǰ�׻�����,���׻�����,���������׻�����
#define AIR_ONOFF_SET_NUM 		1 		// �յ�ң��,ֻ��1����ַ
#define AIR_TEMP_SET_NUM 		4 		// �յ�ң��,ֻ��4����ַ

#define ENVI_TEMP_NUM 			2
#define ENVI_AIRCOND_ONOFF_NUM 			1
#define ENVI_AIRCOND_TEMP_NUM 			3
#define ENVI_AIRCOND_ALARM_NUM 			2


#define FRAME_HEAD_NUM 			3		/*������ʱ����֡��Ч����ǰ���ݸ���*/
#define SET_FRAME_HEAD_NUM 		7		/*д����ʱ����֡��Ч����ǰ���ݸ���*/

/*********************************************************************************
*                                    UPS Э��궨��
**********************************************************************************/
/*״̬��ЧΪ1, ��ЧΪ0*/
typedef union{
	struct
	{
		uint16_t lenid:12;
		uint16_t lchksum:4;
	}lenth_bits;
	/* ������������ʽ����,������������2�����������ͬһ����ַ*/
	uint16_t lenth_word;
}USP_LENGTH_BITS;


#define   UPS_SOI		0x7E
// ver-0x21
#define   UPS_VER		0x21
// ver-0x2A
#define   UPS_CID1		0x2A

#define   UPS_CID2_ALL	0x42
#define   UPS_CID2_IN		0xE0
#define   UPS_CID2_OUT	0xE1
#define   UPS_CID2_BAT	0xE3
#define   UPS_CID2_TIME	0xE4
#define   UPS_CID2_STATUS	0x43

#define   UPS_EOI		0x0D


/*******************************/
/*��·���Ӧ�ò�����*/
/*****************************/
typedef struct __PDU_Struct
{
        INT8U 	address;
        INT8U	*PDUBuffPtr;                          /*ָ��ĵ�ַ��Ҫ����ϵͳ����ͻ�Ŀռ���������*/
        INT16U   	PDULength;
        INT8U     FrameOK;
}PDU_Struct;
typedef struct __SEND_Struct
{
	INT8U FunctionCode;
	INT8U *SENDBuffPtr;                        /*ָ��ĵ�ַ��Ҫ����ϵͳ����ͻ�Ŀռ���������*/
	INT8U SENDLength;
}SEND_Struct;
/*********************************************************************************
*                                     ModbusClientģ��ȫ�ֱ���
**********************************************************************************/
extern INT8U g_CommRxFlag; 		/*ע����Ҫ��ʹ��ΪTRUE�������ϵ�ʱ����ʹ��*/
extern INT16U g_TxDataCtr;     		/*�������ݷ��͸�������*/

extern INT8U Recive_Flag;				/* ���ձ�־*/
extern INT8U WAIT_response_flag;		/* 0:��ʾ�޵ȴ���1:�ȴ����ûظ�֡��2:�ȴ�������²�ѯ֡��3:�ȴ���ʱ��ѯ�ظ� */
extern INT8U Com_err_counter;		/* ͨ�Ŵ��������*/
extern INT8U Com_err_flag;			/* ͨ�Ŵ����־*/
//extern INT8U g_Comm_init;			/* ͨ�ų�ʼ����־*/

extern INT32U CommAppFlag;
extern PDU_Struct  g_PDUData;  

extern INT32U  comm_flag;

/********************************************************************************************************
*                                          ModbusClientģ��ӿں���
********************************************************************************************************/
void UARTInit(void);
void comm_process(void);
void CommTimer(void);
void ModbusServer_init(void);
void ReceOneChar(INT8U ReceCharacter);
void comm_ask(INT16U station,USART_LIST buf_no,INT16U start_reg,INT8U reg_num,INT8U reg_type);
void comm_ask_ups(INT16U station,USART_LIST buf_no,INT16U cid2,INT8U info_len,INT8U command);
void start_comm(void);	
INT8U CRC_check(void);
void data_received_handle(USART_LIST uartNo);
void data_send_directly(USART_LIST destUtNo);
void comm_polling_process(void);

UINT16 checkSumCalc(UINT8 *buffer, UINT8 len);
void comm_wait(USART_LIST destUtNo, UINT16 seq);
INT8U realSum_check(void);
#endif
/*********************************************************************************************************
**                            �ļ�����
********************************************************************************************************/



