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
#define	WRITEFILE_COMMAND				0x15
#define 	READFILE_COMMAND				0x14
#define	WRITESECRATEREG_COMMAND		0x47 
#define	READSECRATEREG_COMMAND		0x46
#define	WRITESINGLECOIL_COMMAND		0x05 
#define 	REF_TYPE_CODE   					0x06


#ifdef RENDA
#define   LOCK_NUM			3			// 3����

#define BRK_NUM				2			//��·����Ŀ
#define ARD_NUM				11			//�غ�բ
#define SPD_NUM				2			//����������

#define 	SMOKE_STATION_ADDRESS  		50		/*�̸е�ַ*/
#define 	WATERIN_STATION_ADDRESS  		51		/*Ĭ��ˮ����ַ*/
#define 	TEMP_STATION_ADDRESS  			52		/*Ĭ����ʪ�ȴ�վ��վ��ַ*/
#define 	AIR_STATION_ADDRESS  			60		/*Ĭ�Ͽյ���վ��վ��ַ*/

#define 	SPD_STATION_1_ADDRESS			35
#define 	SPD_STATION_2_ADDRESS			36

#define 	BREAKER_STATION_1_ADDRESS  	1	/*��·��1��ַ*/
#define 	BREAKER_STATION_2_ADDRESS  	2	/*��·��2��ַ*/

#define 	ARD_STATION_1_ADDRESS  	3	/*�Զ��غ�բ1��ַ*/
#define 	ARD_STATION_2_ADDRESS  	4	/*�Զ��غ�բ2��ַ*/
#define 	ARD_STATION_3_ADDRESS  	5	/*�Զ��غ�բ3��ַ*/
#define 	ARD_STATION_4_ADDRESS  	6	/*�Զ��غ�բ4��ַ*/
#define 	ARD_STATION_5_ADDRESS  	7	/*�Զ��غ�բ5��ַ*/
#define 	ARD_STATION_6_ADDRESS  	8	/*�Զ��غ�բ6��ַ*/
#define 	ARD_STATION_7_ADDRESS  	9	/*�Զ��غ�բ7��ַ*/
#define 	ARD_STATION_8_ADDRESS  	10	/*�Զ��غ�բ8��ַ*/
#define 	ARD_STATION_9_ADDRESS  	11	/*�Զ��غ�բ9��ַ*/
// �غ�բ10�ճ�
#define 	ARD_STATION_10_ADDRESS  	13	/*�Զ��غ�բ11��ַ*/
#define 	ARD_STATION_11_ADDRESS  	14	/*�Զ��غ�բ12��ַ*/

#define 	UPS_STATION_ADDRESS  	01		/*Ĭ��UPS��վ��ַ*/

/*********************************************************************************
*                                    ��·��Э��궨��
**********************************************************************************/
#define BRK_REMOTE_ADDR 		0x6802		// Զ�̷ֺ�բ��ַ
#define BRK_SINGLE_WRITE 		0x06			// �����Ĵ���д����

#define BRK_OPEN_LOCK				0x03
#define BRK_OPEN_WITHOUT_LOCK		0x13
#define BRK_OPEN_UNLOCK				0x23		// ��բ����
#define BRK_CLOSE					0x33

#define BREAKER_REG             					0x5000	// ��·��״̬renda
/*********************************************************************************
*                                   �Զ��غ�բЭ��궨��
**********************************************************************************/
#define ARD_REMOTE_ADDR 	0x11			// Զ�̷ֺ�բ��ַ
#define ARD_SINGLE_WRITE 	0x06			// �����Ĵ���д����

#define ARD_OPEN				0x01
#define ARD_CLOSE			0x02

#define ARD_REG             						0x10		// �غ�բ״̬renda
/*********************************************************************************
*                                   SPDЭ��궨��
**********************************************************************************/
#define SPD_TIMES_REG   	2001				// �׻�����

/*********************************************************************************
*                                  �յ�Э��궨��
**********************************************************************************/
#define AIR_ONOFF_REG             		0x0801	// �յ����ػ�
#define AIR_TEMP_REG             		0x0700	// �յ����µ��µ�

#define ENVI_AIRCOND_ONOFF_REG             		0x0100	// �յ�״̬
#define ENVI_AIRCOND_TEMP_REG             			0x0501	// �յ��¶�
#define ENVI_AIRCOND_ALARM_REG             		0x0600	// �յ��¶�

/*********************************************************************************
*                                 ��ʪ��/ˮ��/����Э��궨��
**********************************************************************************/
#define ENVI_TEMP_REG             					0x00		// ��ʪ��
#define WATER_IN_REG             					0x0010	// ˮ��renda
#define SMOKE_REG             						0x0		// �̸�renda


/*********************************************************************************
*                                     ��ȡ��ز�������
**********************************************************************************/	
#define REAL_DATA_NUM			24  		/*�̶�Ϊ24���ֽ�*/
#define SPD_TIMES_NUM 			1		// ��1��,�ֱ�Ϊ��ǰ�׻�����
#define AIR_ONOFF_SET_NUM 		1 		// �յ�ң��,ֻ��1����ַ
#define AIR_TEMP_SET_NUM 		4 		// �յ�ң��,ֻ��4����ַ

#define ENVI_TEMP_NUM 			2
#define ENVI_AIRCOND_ONOFF_NUM 			5	//�δ�ֻ��5��
#define ENVI_AIRCOND_TEMP_NUM 			7	// �����˵�ѹ����
#define ENVI_AIRCOND_ALARM_NUM 			17	// һ��17��������

#define WATER_IN_NUM 			1
#define SMOKE_EVENT_NUM 			1

/*��·�����غ�բ*/
#define BREAKER_STATUS_NUM 		1
#define ARD_REG_NUM             		0x01		// �غ�բrenda

#else	// ��ͨ�Լ��Ĺ���

#define LOCK_NUM			1	// 1��������

#define BRK_NUM			0	//��·����Ŀ
#define ARD_NUM			0	//�غ�բ
#define SPD_NUM			1	//����������

#define 	WATERIN_STATION_ADDRESS  		0x04		/*Ĭ��ˮ����ַ*/
#define 	TEMP_STATION_ADDRESS  			0x03		/*Ĭ����ʪ�ȴ�վ��վ��ַ*/
#define 	AIR_STATION_ADDRESS  			0x02		/*Ĭ�Ͽյ���վ��վ��ַ*/

#define 	SPD_STATION_1_ADDRESS			0x05
#define 	UPS_STATION_ADDRESS  			0x01		/*Ĭ��UPS��վ��ַ*/


#define	SPD_DIREAD_COMMAND			0x02		//�����Լ��Ķ�������Ϊ02
#define	SPD_TIMESREAD_COMMAND		0x03		//�����Լ����׻���������Ϊ03

/*********************************************************************************
*                                   �����Լ���Э���ַ����
**********************************************************************************/
#define RSU_REG        		0x40				// �ӵ�һ·��ѹ��ʼ��
//#define UPS_REG   			0x01
#define SPD_STATUS_REG   	0x00				// ���׵�����״̬�ӵ�ַ0��ʼ
#define SPD_TIMES_REG   	0x0E				// 04���0E��ʼ��
#define AIR_ONOFF_REG             		0x0801	// �յ����ػ�
#define AIR_TEMP_REG             		0x0700	// �յ����µ��µ�

#define ENVI_AIRCOND_ONOFF_REG             		0x0100	// �յ�״̬
#define ENVI_AIRCOND_TEMP_REG             			0x0501	// �յ��¶�
#define ENVI_AIRCOND_ALARM_REG             		0x0600	// �յ��¶�

#define ENVI_TEMP_REG             					0x00		// ��ʪ��
#define WATER_IN_REG             					0x0010	// ˮ��renda

/*********************************************************************************
*                                     ��ȡ��ز�������
**********************************************************************************/
#define REAL_DATA_NUM			24  	/*�̶�Ϊ24���ֽ�*/
#define SPD_STATUS_NUM 			0x11		// ���������ȡ��17�����ȣ���ʵ��17λ,��3���ֽ�
#define SPD_TIMES_NUM 			3		// ��3��,�ֱ�Ϊ��ǰ�׻�����,���׻�����,���������׻�����

#define AIR_ONOFF_SET_NUM 		1 		// �յ�ң��,ֻ��1����ַ
#define AIR_TEMP_SET_NUM 		4 		// �յ�ң��,ֻ��4����ַ

#define ENVI_AIRCOND_ONOFF_NUM 			6	//��չ��6��
#define ENVI_AIRCOND_TEMP_NUM 			7	// �����˵�ѹ����
#define ENVI_AIRCOND_ALARM_NUM 			17	// һ��17��������

#define ENVI_TEMP_NUM 			2
#define WATER_IN_NUM 			1
#endif


#define Wait_max_time  		200 		/* ���ͺ�ȴ�����֡���ʱ��Ϊ200ms */

/*********************************************************************************
*                                      ͨ����ѯ������־ comm_flag
**********************************************************************************/
//485��ѯ����ö��,����չ
typedef enum 
{	
	DEV_PARAM_SEND_FLAG_2 = 0,
	DEV_PARAM_SEND_FLAG_1,

	REAL_DATA0_SEND_FLAG,		/*ʵʱ���ݲ�ѯ,������2*/
	REAL_DATA1_SEND_FLAG,
	REAL_DATA2_SEND_FLAG,
	REAL_DATA3_SEND_FLAG,

	UPS_PARAM_SEND_FLAG,		// 6	/*USP���������*/
	UPS_IN_SEND_FLAG,			/*USP����������*/
	UPS_OUT_SEND_FLAG,
	UPS_BAT_SEND_FLAG,
	UPS_STATUS_SEND_FLAG,
	UPS_ALARM_SEND_FLAG,		// 11 /*USP�ı�����Ϣ����*/

#if (SPD_NUM >= 1)
	SPD_TIMES_SEND_FLAG_1,		// 12
#endif
#if (SPD_NUM >= 2)
	SPD_TIMES_SEND_FLAG_2,
#endif

	ENVI_TEMP_SEND_FLAG,		// 14
	ENVI_AIRCOND_ONOFF_FLAG,
	ENVI_AIRCOND_TEMP_FLAG,
	ENVI_AIRCOND_ALARM_FLAG,

	WATER_IN_FLAG,			// 18,ˮ��
#ifdef RENDA
	SMOKE_FLAG,			// ����
#endif

#if (BRK_NUM >= 1)
	BREAKER_OPEN_CLOSE_STATUS_1,  // 20��·��1�Ϸ�բ״̬
#endif
#if (BRK_NUM >= 2)
	BREAKER_OPEN_CLOSE_STATUS_2,
#endif

#if (ARD_NUM >= 1)
	ARD_STS_FLAG_1,		// 22 �Զ��غ�բ1
#endif
#if (ARD_NUM >= 2)
	ARD_STS_FLAG_2,
#endif
#if (ARD_NUM >= 3)
	ARD_STS_FLAG_3,
#endif
#if (ARD_NUM >= 4)
	ARD_STS_FLAG_4,
#endif
#if (ARD_NUM >= 5)
	ARD_STS_FLAG_5,
#endif
#if (ARD_NUM >= 6)
	ARD_STS_FLAG_6,
#endif
#if (ARD_NUM >= 7)
	ARD_STS_FLAG_7,
#endif
#if (ARD_NUM >= 8)
	ARD_STS_FLAG_8,
#endif
#if (ARD_NUM >= 9)
	ARD_STS_FLAG_9,
#endif
#if (ARD_NUM >= 10)
	ARD_STS_FLAG_10,
#endif
#if (ARD_NUM >= 11)
	ARD_STS_FLAG_11,	// 32
#endif
	
	POLLING_NUM		// 33
}POLLING_LIST;

#if 0
#define REAL_DATA0_SEND_FLAG    	BIT0		/*ʵʱ���ݲ�ѯ,������0*/
#define REAL_DATA1_SEND_FLAG    	BIT1		/*ʵʱ���ݲ�ѯ,������1*/
#define REAL_DATA2_SEND_FLAG    	BIT2		/*ʵʱ���ݲ�ѯ,������2*/
#define REAL_DATA3_SEND_FLAG    	BIT3		/*ʵʱ���ݲ�ѯ,������3*/

#define UPS_PARAM_SEND_FLAG		BIT4		/*USP���������*/
#define UPS_IN_SEND_FLAG			BIT5		/*USP����������*/
#define UPS_OUT_SEND_FLAG		BIT6		/*USP����������*/
#define UPS_BAT_SEND_FLAG		BIT7		/*USP�ĵ�ز���*/
#define UPS_STATUS_SEND_FLAG	BIT8		/*USP������״̬����*/
#define UPS_ALARM_SEND_FLAG		BIT9		/*USP�ı�����Ϣ����*/


//#define SPD_STATUS_SEND_FLAG		BIT10
#define SPD_TIMES_SEND_FLAG_1		BIT10
#define SPD_TIMES_SEND_FLAG_2		BIT11
//#define SPD_TIMES_SEND_FLAG_3		BIT24

#define ENVI_TEMP_SEND_FLAG         		BIT12		// ��ʪ��
#define ENVI_AIRCOND_ONOFF_FLAG         BIT13		// �յ�������ȡ
#define ENVI_AIRCOND_TEMP_FLAG         	BIT14		// �յ�������ȡ
#define ENVI_AIRCOND_ALARM_FLAG         BIT15		// �յ�������ȡ

#define DEV_PARAM_SEND_FLAG_1         	BIT16		/*������ѯ*/
#define DEV_PARAM_SEND_FLAG_2         	BIT17		/*������ѯ*/


#define WATER_IN_FLAG          	BIT22	 // ˮ��Ҳ��485����renda
#define SMOKE_FLAG          		BIT23	 // �̸�Ҳ��485����renda

#define BREAKER_OPEN_CLOSE_STATUS_1          	BIT24	 // ��·��1�Ϸ�բ״̬
#define BREAKER_OPEN_CLOSE_STATUS_2          	BIT25	 // ��·��2�Ϸ�բ״̬

#define ARD_STS_FLAG_1          					BIT26	 	// �Զ��غ�բ1
#define ARD_STS_FLAG_2          					BIT27	 	// �Զ��غ�բ2
#define ARD_STS_FLAG_3          					BIT28	 	// �Զ��غ�բ3
#define ARD_STS_FLAG_4          					BIT29	 	// �Զ��غ�բ4
#define ARD_STS_FLAG_5          					BIT30	 	// �Զ��غ�բ5
#define ARD_STS_FLAG_6          					BIT31	 	// �Զ��غ�բ6
#define ARD_STS_FLAG_7          					LBIT(32)	 	// �Զ��غ�բ7
#define ARD_STS_FLAG_8          					LBIT(33)	 	// �Զ��غ�բ8
#define ARD_STS_FLAG_9          					LBIT(34)	 	// �Զ��غ�բ9
#define ARD_STS_FLAG_10          					LBIT(35)	 	// �Զ��غ�բ10
#define ARD_STS_FLAG_11          					LBIT(36)	 	// �Զ��غ�բ11
#endif

/*********************************************************************************
*                                      ͨ�����ò�����־control_flag
**********************************************************************************/
//485����д����ö��,����չ
typedef enum 
{	
	DEV_PARAM_SET_FLAG_1 = 0,	// ��������	--�յ����ػ�
	DEV_PARAM_SET_FLAG_2,		// ��������	--�յ��¶�����

#if (LOCK_NUM >= 1)
	DOOR1_OPEN_SET_FLAG,		// 1 ,��������renda-����
	DOOR1_CLOSE_SET_FLAG,		// ��������
#endif
#if (LOCK_NUM >= 2)
	DOOR2_OPEN_SET_FLAG,		// 2 ,��������renda-����
	DOOR2_CLOSE_SET_FLAG,		// ��������
#endif
#if (LOCK_NUM >= 3)
	DOOR3_OPEN_SET_FLAG,		// 4,��������renda-����
	DOOR3_CLOSE_SET_FLAG,		// ��������
#endif

#if (BRK_NUM >= 1)
	BRK1_CLOSE_SET_FLAG,		// 6,��·��1��
	BRK1_OPEN_SET_FLAG,		// ��·��1��������
	BRK1_OPEN_LOCK_SET_FLAG,	// ��·��1������
	BRK1_OPEN_UNLOCK_SET_FLAG,	// ��·��1������
#endif
#if (BRK_NUM >= 2)
	BRK2_CLOSE_SET_FLAG,			// 10, ��·��2��
	BRK2_OPEN_SET_FLAG,			// 11��·��2��������
	BRK2_OPEN_LOCK_SET_FLAG,		// 12��·��2������
	BRK2_OPEN_UNLOCK_SET_FLAG,	// 13��·��2������
#endif

#if (ARD_NUM >= 1)
	ARD1_CLOSE_SET_FLAG,			// 14,�Զ��غ�բ1��
	ARD1_OPEN_SET_FLAG,			// 15�Զ��غ�բ1��
#endif
#if (ARD_NUM >= 2)
	ARD2_CLOSE_SET_FLAG,			// �Զ��غ�բ2��
	ARD2_OPEN_SET_FLAG,			// �Զ��غ�բ2��
#endif
#if (ARD_NUM >= 3)
	ARD3_CLOSE_SET_FLAG,			// �Զ��غ�բ3��
	ARD3_OPEN_SET_FLAG,			// �Զ��غ�բ3��
#endif
#if (ARD_NUM >= 4)
	ARD4_CLOSE_SET_FLAG,			// 20�Զ��غ�բ4��
	ARD4_OPEN_SET_FLAG,			// 21�Զ��غ�բ4��
#endif
#if (ARD_NUM >= 5)
	ARD5_CLOSE_SET_FLAG,			// 22�Զ��غ�բ5��
	ARD5_OPEN_SET_FLAG,			// 23�Զ��غ�բ5��
#endif
#if (ARD_NUM >= 6)
	ARD6_CLOSE_SET_FLAG,			// 24�Զ��غ�բ6��
	ARD6_OPEN_SET_FLAG,			// 25�Զ��غ�բ6��
#endif
#if (ARD_NUM >= 7)
	ARD7_CLOSE_SET_FLAG,			// 26�Զ��غ�բ7��
	ARD7_OPEN_SET_FLAG,			// 27�Զ��غ�բ7��
#endif
#if (ARD_NUM >= 8)
	ARD8_CLOSE_SET_FLAG,			// �Զ��غ�բ8��
	ARD8_OPEN_SET_FLAG,			// �Զ��غ�բ8��
#endif
#if (ARD_NUM >= 9)
	ARD9_CLOSE_SET_FLAG,			// �Զ��غ�բ9��
	ARD9_OPEN_SET_FLAG,			// �Զ��غ�բ9��
#endif
#if (ARD_NUM >= 10)
	ARD10_CLOSE_SET_FLAG,			// 32�Զ��غ�բ10��
	ARD10_OPEN_SET_FLAG,			// 33�Զ��غ�բ10��
#endif
#if (ARD_NUM >= 11)
	ARD11_CLOSE_SET_FLAG,			// 34�Զ��غ�բ11��
	ARD11_OPEN_SET_FLAG,			// 35�Զ��غ�բ11��
#endif	
	CONTROL_NUM		// 36
}CONTROL_LIST;

#if 0
#define DEV_PARAM_SET_FLAG_1          	BIT0	 	// ��������	--�յ����ػ�
#define DEV_PARAM_SET_FLAG_2       	BIT1	 	// ��������	--�յ��¶�����

#define DOOR2_OPEN_SET_FLAG          	BIT2	 	// ��������renda-����
#define DOOR2_CLOSE_SET_FLAG          	BIT3	 	// ��������

#define DOOR3_OPEN_SET_FLAG          	BIT4	 	// ��������renda-����
#define DOOR3_CLOSE_SET_FLAG          	BIT5	 	// ��������

#define BRK1_CLOSE_SET_FLAG          	BIT6	 	// ��·��1��
#define BRK1_OPEN_SET_FLAG          		BIT7 		// ��·��1��������
#define BRK1_OPEN_LOCK_SET_FLAG          	BIT8 		// ��·��1������
#define BRK1_OPEN_UNLOCK_SET_FLAG          	BIT9 		// ��·��1������


#define BRK2_CLOSE_SET_FLAG          	BIT10	 	// ��·��1��
#define BRK2_OPEN_SET_FLAG          		BIT11 		// ��·��1��������
#define BRK2_OPEN_LOCK_SET_FLAG          	BIT12 		// ��·��1������
#define BRK2_OPEN_UNLOCK_SET_FLAG          	BIT13 		// ��·��1������

#define ARD1_CLOSE_SET_FLAG          	BIT14	 	// �Զ��غ�բ1��
#define ARD1_OPEN_SET_FLAG          		BIT15	 	// �Զ��غ�բ1��

#define ARD2_CLOSE_SET_FLAG          	BIT16	 	// �Զ��غ�բ2��
#define ARD2_OPEN_SET_FLAG          		BIT17	 	// �Զ��غ�բ2��

#define ARD3_CLOSE_SET_FLAG          	BIT18	 	// �Զ��غ�բ3��
#define ARD3_OPEN_SET_FLAG          		BIT19	 	// �Զ��غ�բ3��

#define ARD4_CLOSE_SET_FLAG          	BIT20	 	// �Զ��غ�բ4��
#define ARD4_OPEN_SET_FLAG          		BIT21	 	// �Զ��غ�բ4��

#define ARD5_CLOSE_SET_FLAG          	BIT22	 	// �Զ��غ�բ5��
#define ARD5_OPEN_SET_FLAG          		BIT23	 	// �Զ��غ�բ5��

#define ARD6_CLOSE_SET_FLAG          	BIT24	 	// �Զ��غ�բ6��
#define ARD6_OPEN_SET_FLAG          		BIT25	 	// �Զ��غ�բ6��

#define ARD7_CLOSE_SET_FLAG          	BIT26	 	// �Զ��غ�բ7��
#define ARD7_OPEN_SET_FLAG          		BIT27	 	// �Զ��غ�բ7��

#define ARD8_CLOSE_SET_FLAG          	BIT28	 	// �Զ��غ�բ8��
#define ARD8_OPEN_SET_FLAG          		BIT29	 	// �Զ��غ�բ8��

#define ARD9_CLOSE_SET_FLAG          	BIT30	 	// �Զ��غ�բ9��
#define ARD9_OPEN_SET_FLAG          		BIT31	 	// �Զ��غ�բ9��

#define ARD10_CLOSE_SET_FLAG          	LBIT(32)	 	// �Զ��غ�բ10��
#define ARD10_OPEN_SET_FLAG          	LBIT(33)	 	// �Զ��غ�բ10��

#define ARD11_CLOSE_SET_FLAG          	LBIT(34)	 	// �Զ��غ�բ11��
#define ARD11_OPEN_SET_FLAG          	LBIT(35)	 	// �Զ��غ�բ11��
#endif


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
#define SPD_TIMES_ANALYSE_1	14	/*���ײ�������*/
#define SPD_TIMES_ANALYSE_2	15	/*���ײ�������*/

#define DEVICE_DATA_1_ANALYSE	16	/*װ�ò�����ȡ����,�յ����ػ�*/
#define DEVICE_DATA_2_ANALYSE	17	/*װ�ò�����ȡ����,�յ��¶�*/

#define UPS_PARAM_ANALYSE		18
#define UPS_IN_ANALYSE			19
#define UPS_OUT_ANALYSE			20
#define UPS_BAT_ANALYSE			21
#define UPS_ALARM_ANALYSE		22
#define UPS_STATUS_ANALYSE		23

#define WATER_IN_ANALYSE		24	/*ˮ����������*/
#define SMOKE_ANALYSE		25	/*�̸в�������*/

#define WAIT_DOOR_OPEN		26	/*�ȴ���������֡�ظ�*/
#define WAIT_DOOR_CLOSE		27	/*�ȴ���������֡�ظ�*/

#define BREAKER_OPEN_CLOSE_ST_ANALYSE_1	28
#define BREAKER_OPEN_CLOSE_ST_ANALYSE_2	29

#define ARD_STS_ANALYSE_1	30
#define ARD_STS_ANALYSE_2	31
#define ARD_STS_ANALYSE_3	32
#define ARD_STS_ANALYSE_4	33
#define ARD_STS_ANALYSE_5	34
#define ARD_STS_ANALYSE_6	35
#define ARD_STS_ANALYSE_7	36
#define ARD_STS_ANALYSE_8	37
#define ARD_STS_ANALYSE_9	38
#define ARD_STS_ANALYSE_10	39
#define ARD_STS_ANALYSE_11	40


#define FRAME_HEAD_NUM 			3		/*������ʱ����֡��Ч����ǰ���ݸ���*/
#define SET_FRAME_HEAD_NUM 		7		/*д����ʱ����֡��Ч����ǰ���ݸ���*/


/*********************************************************************************
*                                    ������ Э��궨��
**********************************************************************************/

#define   LOCK_ADDR_1		0x01
#define   LOCK_SOI		0x7E

#define   LOCK_DES_THREAD	0x00
#define   LOCK_SRC_THREAD	0x00

#define   LOCK_EOI1		0x7E
#define   LOCK_EOI2		0x7E

/* Э���ֽ�˳��Ŷ���*/
#define   LOCK_SQU_SOI		0
#define   LOCK_SQU_DES_STA	1	// Ŀ�ĵ�ַ
#define   LOCK_SQU_SRC_STA	2	// Դ��ַ
#define   LOCK_SQU_DES_THRD	3	// Ŀ�Ľ���
#define   LOCK_SQU_SRC_THRD	4	// Դ���̺�
#define   LOCK_SQU_LENTH_HIGH	5	// ���ȸ��ֽ�
#define   LOCK_SQU_LENTH_LOW		6	// ���ȸ��ֽ�
#define   LOCK_SQU_SIGNAL		7	// ���ȸ��ֽ�


#define   LOCK_OPEN		0x0005
#define   LOCK_CLOSE		0x0006

#if (LOCK_NUM >= 2)
#define   LOCK_ADDR_2		0x02
#endif

#if (LOCK_NUM >= 3)
#define   LOCK_ADDR_3		0x03
#endif

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

extern INT64U  comm_flag;
extern INT64U  control_flag;

/********************************************************************************************************
*                                          ModbusClientģ��ӿں���
********************************************************************************************************/
void UARTInit(void);
void CommTimer(void);
void ModbusServer_init(void);
void ReceOneChar(INT8U ReceCharacter);
void comm_ask(INT16U station,USART_LIST buf_no,INT16U start_reg,INT8U reg_num,INT8U reg_type);
void start_comm(void);	
INT8U CRC_check(void);
void data_received_handle(USART_LIST uartNo);
void data_send_directly(USART_LIST destUtNo);
void comm_polling_process(void);
UINT16 checkSumCalc(UINT8 *buffer, UINT8 len);
void comm_wait(USART_LIST destUtNo, UINT16 seq);
INT8U realSum_check(void);
void comm_ask_locker(INT16U station,USART_LIST buf_no,INT16U signal,INT16U info_len,INT16U msg);
void CalulateCRCbySoft(INT8U *pucData,INT8U wLength,INT8U *pOutData);
#endif
/*********************************************************************************************************
**                            �ļ�����
********************************************************************************************************/



