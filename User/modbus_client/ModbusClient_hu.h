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

#ifndef _ModbusClient_H
#define _ModbusClient_H
/*********************************************************************************
*                                     ModbusClientģ�����ó���
***********************************************************************************/

#define COMM_NUM		   BD_USART_NUM        /*ͨ�ſڵ���Ŀ����ͨ�ſ�*/
#define COMMBUFLEN		  257       /*ͨ�Ż������ֽ�*/


/*********************************************************************************
*                                      ModbusClientģ�鳣��
**********************************************************************************/

   /*KEIL �ı�����ƾ�����������뺯����Ҫ��������,ͬʱ�������������
      �����ı�������(�ַ�̫��׾��),�����뺯��,�ݹ麯��,����ָ����ָ���
      ����������õĺ�������Ҫ����Ϊ������,���������ܶ�����,��Ϊ�˱�֤
      �����ͨ����,���Խ�ͨ�ŵĺ���ָ���ΪSWITCH������*/

#define	   SUCCESSFULLCOMMAND		        00                              /*�ɹ���MODBUS����*/
#define    ILLEGALfUNCTIONCODE		        01                           	/*����ȷ�Ĺ�����*/ 
#define    ILLEGALREGQADDRESS		        02                           	/*����ȷ�ļĴ�����ַ*/
#define    ILLEGALREGQATITY			03                           	/*����ȷ�ļĴ�������*/ 
#define    FAILREGOPERATOR			04                              /*ʧ�ܵļĴ�������*/
#define    ILLEGALFRAMELENGTH			03                              /*�Ƿ�������֡����*/    
#define    ILLEGALREGVALUE			03                         	/*�Ƿ�����ֵ*/    

#define    WRITEREG_COMMAND			0x10
#define    READREG_COMMAND			0x03
#define    WRITESECRATEREG_COMMAND		0x47 
#define    READSECRATEREG_COMMAND		0x46
#define    WRITESINGLECOIL_COMMAND		0x05 

#define MODIFY_DEVICE_PARAMS       	        BIT1		                /*װ�ò����޸�*/
#define MODIFY_SECRET_PARAMS       	        BIT2		                /*���ܼĴ��������޸�*/
#define MODIFY_CAL_PARAMS                 	BIT3		                /*����ϵ���޸�*/
#define MODIFY_COM_PARAMS                	BIT4		                /*ͨѶ�����޸�*/
#define MODIFY_DISPLAY_COM		        BIT5                            /*����޸�ͨ�Ų���*/
#define MODIFY_ENERGY_PARAMS		        BIT6                            /*���ܵ�ֵ�޸�*/

#define MODIFY_SN		                BIT7           

#define CLOCK_MODIFY_END		        BIT8
#define CLOCK_MODIFY_BEGIN		        BIT9
//#define MODIFY_FEATURE_PARAMS		        BITA
//#define MODIFY_DISPLAY_FEATURE		        BITB	/*����޸�����ֵ*/

/*********************************************************************************
*                                       ModbusClientģ����������
**********************************************************************************/

typedef  struct  _CommREG_Struct      /*��������д����ʱ�߽��жϣ������ֹ32BIT��ֻд��16BIT*/
{
    unsigned int 	regNo;                           /*�Ĵ������*/
    unsigned char  	length;                          /*�Ĵ�����ռ����(16BIT)�ĸ���*/    
}BlockRegStruct;

typedef enum _REGATTRIBUTE
{
	RW,                                              /*�ɶ���д*/
	RO,                                              /*ֻ��*/
	WO                                               /*ֻд*/
}E_REGATTRIBUTE;


typedef enum _MODBUSREGMODULE
{
	NONE_MODULE,
	DATA_MODULE,
	ENERGY_MODULE,                                   /*����ģ��*/ 
        //THD_MODULE,                                      /*г��ģ�� 2010.07.30*/
	DEVICE_SET_MODULE,
	//SOE_MODULE,
	//ADJUST_TIME_MODUBLE, 
        //ADJUST_UNIX_TIME_MODUBLE, 
        //ADJUST_BOTH_TIME_MODUBLE,
	SECRET_MODULE,
	DEVICE_INFORMATION_MODULE,
	DEVICE_INFORMATION_BACKUP_MODULE
}E_MODBUSREGMODULE;       


typedef struct _ComModuelStruct
{
      unsigned int   ModuleStartAddress;                 /*���ʼ��ַ*/
      unsigned int   ModuleEndAdress;	                 /*��������ַ*/
      E_REGATTRIBUTE      ModuleAttribute;           	 /*ģ������*/	  
      E_MODBUSREGMODULE   ModuleFlag;                	 /*ģ���־*/
}ComModuelStruct;


/*******************************/
/*��·���Ӧ�ò�����*/
/***********************  ******/
typedef struct __ADU_Struct
{
        unsigned char 	  address;
        unsigned char	  *ADUBuffPtr;                     /*ָ��ĵ�ַ��Ҫ����ϵͳ����ͻ�Ŀռ���������*/
        unsigned int   	  ADULength;
        unsigned char     FrameOK;
}ADU_Struct;


/******************/
/*Ӧ�ò�����*/
/******************/
typedef struct __PDU_Struct
{
   unsigned char     	FunctionCode;
   unsigned char   	*PDUBuffPtr;                     /*ָ��ĵ�ַ��Ҫ����ϵͳ����ͻ�Ŀռ���������*/
   unsigned char	PDULength;
   unsigned char     	FrameOK;
   unsigned char     	ExceptionCode;
}PDU_Struct;

typedef struct _PDUResponse_Struct
{
	unsigned char       	*PDUDataPtr;             /*ָ��ĵ�ַ��Ҫ����ϵͳ����ͻ�Ŀռ���������*/                             
	unsigned char       	PDUByteLength;
	unsigned char         	ExceptionCode;
}PDUResponse_Struct;

typedef struct _FunctionArray_Struct
{
   unsigned char    functionCode;
   void   (*Function)(PDUResponse_Struct *Response);
}FunctionArray_Struct;

typedef  struct  _DeviceCommInformation_Struct           /*��������д����ʱ�߽��жϣ������ֹ32BIT��ֻд��16BIT*/
{
   unsigned int deviceType[20];
   unsigned int softVersion;
   unsigned int protocolVersion;
   unsigned int softYear;
   unsigned int softMonth;
   unsigned int softDate;
   unsigned int deviceYear;
   unsigned int deviceMonth;
   unsigned int deviceDate;
   unsigned int deviceMadeSeiral;   
}DeviceCommInformationStruct;

/*********************************************************************************
*                                     ModbusClientģ��ȫ�ֱ���
**********************************************************************************/
extern  ADU_Struct    g_ADUData[COMM_NUM];  
extern  PDU_Struct    g_PDUData[COMM_NUM]; 

extern  unsigned char g_CommRxFlag[COMM_NUM];             /*ע����Ҫ��ʹ��ΪTRUE�������ϵ�ʱ����ʹ��*/
extern  unsigned int  g_TxDataCtr[COMM_NUM];              /*�������ݷ��͸�������*/
extern  unsigned char delayed_baud;                       /*��ʾ����������ȵ�ͨѶ��Ӧ��Ÿ���*/

extern unsigned int  adzero_adjust;                    /*�����־*/
extern unsigned char  flag_yk;

extern unsigned int   g_adjust_millisecond;
extern unsigned int   *const ModbusDeviceInforTable[];
extern unsigned int   CommAppFlag;
/********************************************************************************************************
*                                          ModbusClientģ��ӿں���
********************************************************************************************************/
void SnUpdateDeviceInfor(void);
void CommProc(void);
void CommTimer(void);
void UARTInit();
void CommInit(void);
void ReceOneChar(unsigned char  comNum, unsigned char  ReceCharacter);
void CommCfgPort(unsigned char  comNum, unsigned char  baud); 

#endif
/*********************************************************************************************************
**                            �ļ�����
********************************************************************************************************/

