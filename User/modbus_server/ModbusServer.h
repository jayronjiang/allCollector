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


#define 	RSU_STATION_ADDRESS  	01		/*Ĭ��RSU��վ��ַ*/
#define 	AIR_STATION_ADDRESS  	02		/*Ĭ�Ͽյ���վ��վ��ַ*/
#define 	TEMP_STATION_ADDRESS  	03		/*Ĭ����ʪ�ȴ�վ��վ��ַ*/

#define Wait_max_time  		200 		/* ���ͺ�ȴ�����֡���ʱ��Ϊ200ms */

/*********************************************************************************
*                                      ͨ�����ò�����־
**********************************************************************************/
#define REAL_DATA_SEND_FLAG    	BIT0		/*ʵʱ���ݲ�ѯ*/
#define UPS_PARAM_SEND_FLAG		BIT1
#define SPD_STATUS_SEND_FLAG	BIT2
#define SPD_TIMES_SEND_FLAG		BIT3

#define ENVI_TEMP_SEND_FLAG         	BIT4		// ��ʪ��
#define ENVI_AIRCOND_ONOFF_FLAG         	BIT5		// �յ�������ȡ
#define ENVI_AIRCOND_TEMP_FLAG         		BIT6		// �յ�������ȡ
#define ENVI_AIRCOND_ALARM_FLAG         	BIT7		// �յ�������ȡ

#define DEV_PARAM_SEND_FLAG_1         BIT8		/*������ѯ*/
#define DEV_PARAM_SEND_FLAG_2         BIT9		/*������ѯ*/

#define DEV_PARAM_SET_FLAG_1          	BIT10	 // ��������	--�յ����ػ�
#define DEV_PARAM_SET_FLAG_2       	BIT11	 // ��������	--�յ��¶�����

#define DOOR_OPEN_SET_FLAG          	BIT12	 // ��������
#define DOOR_CLOSE_SET_FLAG          	BIT13	 // ��������


/*********************************************************************************
*                                     ��ȡ��ز�����ʼ��ַ
**********************************************************************************/
#define RSU_REG        		0x40			// �ӵ�һ·��ѹ��ʼ��
#define UPS_REG   			0x01
#define SPD_STATUS_REG   	0x01
#define SPD_TIMES_REG   	0x0F
#define AIR_ONOFF_REG             		0x0801	// �յ����ػ�
#define AIR_TEMP_REG             		0x0700	// �յ����µ��µ�

#define ENVI_TEMP_REG             					0x02		// ��ʪ��
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
#define REAL_DATA_ANALYSE		7	/*ʵʱ���ݽ���,��RSU����*/
#define ENVI_TEMP_ANALYSE	9	/*��ʪ�Ȳ�������*/
//#define ENVI_AIR_ANALYSE		10	/*�յ���������*/
#define ENVI_AIR_ONOFF_ANALYSE		10	/*�յ���������*/
#define ENVI_AIR_TEMP_ANALYSE		11	/*�յ���������*/
#define ENVI_AIR_ALARM_ANALYSE		12	/*�յ���������*/

#define UPS_DATA_ANALYSE	13	/*UPS��������*/
#define SPD_STATUS_ANALYSE	14	/*���ײ�������*/
#define SPD_TIMES_ANALYSE	15	/*���ײ�������*/

#define DEVICE_DATA_1_ANALYSE	16	/*װ�ò�����ȡ����,�յ����ػ�*/
#define DEVICE_DATA_2_ANALYSE	17	/*װ�ò�����ȡ����,�յ��¶�*/



/*********************************************************************************
*                                     ��ȡ��ز�������
**********************************************************************************/	
#define REAL_DATA_NUM		42  	/*��ʵʱ�������ݳ��ȣ�0x69-0x40*/
//#define REAL_TIME_SOE_NUM			37  		/*��ȡSOE��ͳ����Ϣ�����ݳ���*/
#define UPS_DATA_NUM 			4
#define SPD_STATUS_NUM 			1
#define SPD_TIMES_NUM 			1
#define AIR_ONOFF_SET_NUM 	01 		// �յ�ң��,ֻ��1����ַ
#define AIR_TEMP_SET_NUM 	04 		// �յ�ң��,ֻ��4����ַ

#define ENVI_TEMP_NUM 			2
#define ENVI_AIRCOND_ONOFF_NUM 			1
#define ENVI_AIRCOND_TEMP_NUM 			3
#define ENVI_AIRCOND_ALARM_NUM 			2


#define FRAME_HEAD_NUM 			3		/*������ʱ����֡��Ч����ǰ���ݸ���*/
#define SET_FRAME_HEAD_NUM 		7		/*д����ʱ����֡��Ч����ǰ���ݸ���*/

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
void start_comm(void);	
INT8U CRC_check(void);
void data_received_handle(USART_LIST uartNo);
void data_send_directly(USART_LIST destUtNo);
void comm_polling_process(void);
#endif
/*********************************************************************************************************
**                            �ļ�����
********************************************************************************************************/



