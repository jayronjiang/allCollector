/********************************************************************************
*  ��Ȩ����: �������е�����������޹�˾				
*
*  �ļ���:       ModbusClient.c
*
*
*  ��ǰ�汾:                       0.1
*  
*  ����:                                  ��ΰ
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
#include "Includes.h"

/*
*********************************************************************************************************
*                                   ��̬����
*********************************************************************************************************
*/ 
static void FReadHoldingReg(PDUResponse_Struct *Response) ;
static void FReadSecretReg(PDUResponse_Struct *Response) ;
static void FWriteCommonReg(PDUResponse_Struct *Response) ;
static void FWriteSecretReg(PDUResponse_Struct *Response);
static unsigned char ReadHoldingReg(unsigned char *DataPtr, unsigned int StAddr, unsigned int Quantity,E_MODBUSREGMODULE    moduleFlag) ;
static unsigned char MB_GetRegValue(unsigned int *ValuePtr,unsigned int Address,E_MODBUSREGMODULE    moduleFlag) ;
static unsigned char WriteHoldingReg(unsigned char *KeepDataPtr,unsigned int StAddr,unsigned int Quantity,E_MODBUSREGMODULE moduleFlag) ;
static unsigned char MB_SetRegValue(unsigned int Address, unsigned int value,E_MODBUSREGMODULE    moduleFlag) ;
static unsigned char  CheckFrameBorder(unsigned int StAddr, unsigned int Quantity,E_MODBUSREGMODULE moduleFlag);
static void FwriteHook(void) ;
//static void UARTInit(unsigned char comNum);
static void ModbusAppDisp(unsigned char comNum);
static void ModbusLinkDisp(unsigned char comNUm);
static void FunctionHandle(unsigned char comNum);
static void SendResponse(unsigned char comNum);
static void PackADU2PDU(unsigned char comNum);
static void PackPDU2ADU(unsigned char comNum);
unsigned char  MB_CheckRegValue(unsigned int Address,unsigned int value,E_MODBUSREGMODULE    moduleFlag); 
#if PMC_23M
unsigned char CheckCTmultiPTlimit();
#endif
void ComInitHook(void);
//#define StartCounterT35(x)	(g_T35_num[x] = 0)
//#define StartCounterT15(x)	(g_T15_num[x] = 0)
/********************************************************************************************************
                                  ��̬ȫ�ֱ���
*********************************************************************************************************/
DeviceCommInformationStruct  g_DevicComInfor;
ADU_Struct   g_ADUData[COMM_NUM];  
PDU_Struct   g_PDUData[COMM_NUM]; 
unsigned char g_CommRxFlag[COMM_NUM];   /*���������־��ע����Ҫ��ʹ��ΪTRUE�������ϵ�ʱ����ʹ��*/
									/*�����ж���ֻΪfalse����������������������������Ƿ������*/
unsigned int  g_TxDataCtr[COMM_NUM];              /*�������ݷ��͸�������*/
unsigned char delayed_baud = FALSE;		  /*�ӳ����ò�����*/

//uint8     g_CommRxFlag;           /*ע����Ҫ��ʹ��ΪTRUE�������ϵ�ʱ����ʹ��*/

/**************************************************************************************************/
/************************�������빦����ص�ȫ�־�̬����***************************/
/**************************************************************************************************/
/*�����뼰�������Ӧ�Ĵ�����*/
const  FunctionArray_Struct g_FCodeHandleArray[] =          
{
	{READREG_COMMAND,FReadHoldingReg},
	{WRITEREG_COMMAND,FWriteCommonReg},
	{READSECRATEREG_COMMAND,FReadSecretReg},
	{WRITESECRATEREG_COMMAND,FWriteSecretReg},
};
/*�����������Ĺ���������*/
#define MAX_FUNCTION_NUM	sizeof(g_FCodeHandleArray) / sizeof(FunctionArray_Struct)      

//static unsigned char 	g_ENDT15Flag[COMM_NUM];    /*ע����Ҫ����ֵΪFALSE*/
//static unsigned char 	g_ENDT35Flag[COMM_NUM];    /*ע����Ҫ����ֵΪFALSE*/
//static signed char	g_T35_num[COMM_NUM];       /*ע����Ҫ����ֵΪ-1*/
//static signed char	g_T15_num[COMM_NUM];       /*ע����Ҫ����ֵΪ-1*/
static unsigned char	g_ComBuf[COMM_NUM][COMMBUFLEN];  /*Ϊͨ�ŷ��仺��*/
//const unsigned char	T15DelayTime[NUM_OF_BAUDRATES] = {24,12,6,3,2};
//const unsigned char	T35DelayTime[NUM_OF_BAUDRATES] = {46,23,12,6,3};

//unsigned int g_PT_Checkvalue = 0;   	/*ͨ�Ž�Ҫ�޸ĵ�PT���ֵ��Ϊ0��ʾͨ��δ�ģ�Ϊ������Ϊͨ���޸ĺ��PT���ֵ*/
//unsigned int g_CT_Checkvalue = 0;         /*ͨ�Ž�Ҫ�޸ĵ�CT���ֵ��Ϊ0��ʾͨ��δ�ģ�Ϊ������Ϊͨ���޸ĺ��CT���ֵ*/

/**************************************************************************************************/
/************************��������Ӧ����ص�ȫ�־�̬����***************************/
/**************************************************************************************************/

/* --------------------------------------------  ʵʱ���ݼĴ���---------------------------------------- */
const unsigned int blank = 0;
/* MODBUS���ݼĴ������*/
int  *const ModbusDataRegTable[] =
{
	#if !PMC_23I
	(int *)&vars.phase[0].vln + 1,		/*0000,Van+1*/			
	(int *)&vars.phase[0].vln,		/*0001,Van*/		
	(int *)&vars.phase[1].vln + 1,		/*0002,Vbn+1*/		
	(int *)&vars.phase[1].vln ,		/*0003,Vbn*/				
	(int *)&vars.phase[2].vln + 1,		/*0004,Vcn+1*/				
	(int *)&vars.phase[2].vln ,		/*0005,Vcn*/
	(int *)&vars.total.vln + 1,		/*0006*/				
	(int *)&vars.total.vln ,
	(int *)&vars.phase[0].vll + 1,		/*0008*/	
	(int *)&vars.phase[0].vll,					
	(int *)&vars.phase[1].vll + 1,		/*0010*/	
	(int *)&vars.phase[1].vll,					
	(int *)&vars.phase[2].vll + 1,		/*0012*/		
	(int *)&vars.phase[2].vll,
	(int *)&vars.total.vll + 1,				
	(int *)&vars.total.vll,			/*0015*/
	
	#else
	(int *)&blank,				/*0000*/
	(int *)&blank,				/*0001*/
	(int *)&blank,				/*0002*/
	(int *)&blank,				/*0003*/
	(int *)&blank,				/*0004*/
	(int *)&blank,				/*0005*/
	(int *)&blank,				/*0006*/
	(int *)&blank,				/*0007*/
	(int *)&blank,				/*0008*/
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,				/*0015*/
	#endif
	
	#if !PMC_23V
	(int *)&vars.phase[0].amp + 1,		/*0016,Ia*/
	(int *)&vars.phase[0].amp,					
	(int *)&vars.phase[1].amp + 1,		/*0018,Ib*/
	(int *)&vars.phase[1].amp,
	(int *)&vars.phase[2].amp + 1,		/*0020,Ic*/
	(int *)&vars.phase[2].amp,
	(int *)&vars.total.amp + 1,			/*0022,Iav*/
	(int *)&vars.total.amp,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif
	

	#if PMC_23M
	(int *)&vars.phase[0].kw + 1,			/*0024,KWA*/	
	(int *)&vars.phase[0].kw,				
	(int *)&vars.phase[1].kw + 1,			/*0026,KWB*/
	(int *)&vars.phase[1].kw,
	(int *)&vars.phase[2].kw + 1,			/*0028,KWC*/
	(int *)&vars.phase[2].kw,
	(int *)&vars.total.kw + 1,			/*0030,KW*/
	(int *)&vars.total.kw,
	(int *)&vars.phase[0].kvar + 1,				
	(int *)&vars.phase[0].kvar,				
	(int *)&vars.phase[1].kvar + 1,
	(int *)&vars.phase[1].kvar,
	(int *)&vars.phase[2].kvar + 1,
	(int *)&vars.phase[2].kvar,
	(int *)&vars.total.kvar + 1,			/*0038,��KVAR*/
	(int *)&vars.total.kvar,
	(int *)&vars.phase[0].kva + 1,				
	(int *)&vars.phase[0].kva,				
	(int *)&vars.phase[1].kva + 1,
	(int *)&vars.phase[1].kva,
	(int *)&vars.phase[2].kva + 1,
	(int *)&vars.phase[2].kva,
	(int *)&vars.total.kva + 1,		/*0046,��KVA*/
	(int *)&vars.total.kva,				
	(int *)&vars.phase[0].pf,				
	(int *)&vars.phase[1].pf,
	(int *)&vars.phase[2].pf,
	(int *)&vars.total.pf,			/*0051,��PF*/
        #else
	(int *)&blank,		/*0024*/
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,		/*0040*/
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,		/*0051*/
	#endif
	#if !PMC_23I
	(int *)&vars.freq, 	/*0052*/
	#else 
	(int *)&blank,
	#endif
	  #if  PMC_23M    
    	(int *)&vars.total.I4 + 1,		/*0053,0054:  I0*/
   	(int *)&vars.total.I4,
    	#else
	(int *)&blank,
	(int *)&blank,	
        #endif
	
};
#define MBUS_DATA_REG_NUM		sizeof(ModbusDataRegTable) / sizeof(int *)

#define START_DATA_REG	        0000	                                     /*���ݼĴ�����ʼ��ַ*/
#define END_DATA_REG	        (START_DATA_REG + MBUS_DATA_REG_NUM - 1)     /*���ݼĴ���������ַ*/

int* const ModbusEnergyRegTable[] =				             /* MODBUS���ܼĴ������*/
{
	#if PMC_23M
    	(int *)&energy.kwhr.khour + 1,	/*0200,�����й�*/
	(int *)&energy.kwhr.khour,
	(int *)&energy.n_kwhr.khour + 1,	/*0202�������й�*/
	(int *)&energy.n_kwhr.khour,	
	(int *)&energy.kvarhr.khour + 1,	/*0204�������޹�*/
	(int *)&energy.kvarhr.khour,
	(int *)&energy.n_kvarhr.khour + 1,		/*0206�������޹�*/
	(int *)&energy.n_kvarhr.khour,
	(int *)&energy.kvahr.khour + 1,		/*0208�����ڹ���*/
	(int *)&energy.kvahr.khour,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,	
	#endif

};
#define    MBUS_ENERGY_REG_NUM     sizeof(ModbusEnergyRegTable) / sizeof(int *)

#define     START_ENERGY_REG       200                  /*���ܼĴ�����ʼ��ַ������д��0200�������8������*/
#define     END_ENERGY_REG         (START_ENERGY_REG + MBUS_ENERGY_REG_NUM - 1)             /*���ܼĴ���������ַ*/

	#if 0
unsigned int* const ModbusThdRegTable[] =				        /* Thd�Ĵ������*///2010.07.30
{
	#if PMC_23M
	( unsigned int *)&thd.thd_u[0],	
	( unsigned int *)&thd.thd_u[1],
	( unsigned int *)&thd.thd_u[2],
	( unsigned int *)&thd.thd_i[0],	
	( unsigned int *)&thd.thd_i[1],
	( unsigned int *)&thd.thd_i[2],
	#else
	( unsigned int *)&blank,
	( unsigned int *)&blank,
	( unsigned int *)&blank,
	( unsigned int *)&blank,
	( unsigned int *)&blank,	
	( unsigned int *)&blank,	
	#endif

};
#define     MBUS_THD_REG_NUM     sizeof(ModbusThdRegTable) / sizeof(unsigned int *)

#define     START_THD_REG        40718                              /*г���Ĵ�����ʼ��ַ*/
#define     END_THD_REG          (START_THD_REG + MBUS_THD_REG_NUM - 1) /*г���Ĵ���������ַ*/
	#endif

/* --------------------------------------------  ʵʱ���ݼĴ���---------------------------------------- */

/* --------------------------------------------  �������üĴ���---------------------------------------- */
/******************װ�ò����Ĵ������**********************************/
unsigned char  clr_energy_Flag=FALSE;
//unsigned char  clr_soe_Flag=FALSE;
/* MODBUS�����Ĵ����б�*/
int *const ModbusParamsRegTable[] =	/*�����Ĵ�����Ӧ����ָ��*/
{	
	#if !PMC_23I
	(int *)&params.volt_rate,		/*6000��PT���*/
	#else
	(int *)&blank,
	#endif
	#if !PMC_23V
	(int *)&params.amp_rate,   		/*6001��CT���*/
	#else
	(int *)&blank,		
	#endif
	#if !PMC_23I
	(int *)&params.inmode,   		/*6002�����߷�ʽ*/
	#else
	(int *)&blank,
	#endif
			
	(int *)&params.siteid,		/*6003���豸ID*/
	(int *)&params.baudrate,	/*6004��������*/
	(int *)&params.parity,		/*6005����żУ��*/

	#if PMC_23M
	(int *)&params.pfmode,		/*�������ط���*/	/*6006*/
	(int *)&params.kvamode,	/*���ڹ��ʷ���*/	/*6007*/
	#else
	(int *)&blank,
	(int *)&blank,	
        #endif
	
	#if !PMC_23V
	(int *)&params.CTReverseFlag[0],   /*A���������*/	/*6008*/
	(int *)&params.CTReverseFlag[1],   /*B*/
	(int *)&params.CTReverseFlag[2],   /*C*/
	#else
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,	
	#endif
		
	(int *)&BackLightTime,		/*6011����ʱ��*/
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,	
	
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,				/*6020*/

	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6030*/
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6040*/	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6050*/	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6060*/	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6070*/	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6080*/	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6090*/	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6100*/
	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,		
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,		
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,		
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6200*/

	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,		
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,		
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,		
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6300*/

	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,		
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,		
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,		
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*6400*/
};

#define START_SET_REG	        6000           /*�������üĴ�����ʼ��ַ*/
#define PARITY_SET_REG    		6005
#define CLEAR_ENERGY_REG    	6400		/*��������ΪWO�ͼĴ���*/	
#define END_SET_REG		(START_SET_REG + sizeof(ModbusParamsRegTable) / sizeof(int *) - 1)        	/*�������üĴ���������ַ*/

#define ENDRO_SET_REG    6011		/*6012��ʼ���ܶ�*/
//#define CLEAR_ENERGY_REG	        6400           /*�������üĴ�����ʼ��ַ*/
/* --------------------------------------------  �������üĴ���---------------------------------------- */


/* --------------------------------------------  װ����Ϣ�Ĵ���------------------------------------------ */

unsigned int * const ModbusDeviceInforTable[] =                           /* װ����Ϣ�Ĵ������*/
{
	(unsigned int *)&g_DevicComInfor.deviceType[0],		/*9800��װ���ͺ�*/
	(unsigned int *)&g_DevicComInfor.deviceType[1],
	(unsigned int *)&g_DevicComInfor.deviceType[2],
	(unsigned int *)&g_DevicComInfor.deviceType[3],
	(unsigned int *)&g_DevicComInfor.deviceType[4],
	(unsigned int *)&g_DevicComInfor.deviceType[5],
	(unsigned int *)&g_DevicComInfor.deviceType[6],
	(unsigned int *)&g_DevicComInfor.deviceType[7],
	(unsigned int *)&g_DevicComInfor.deviceType[8],
	(unsigned int *)&g_DevicComInfor.deviceType[9],
	(unsigned int *)&g_DevicComInfor.deviceType[10],
	(unsigned int *)&g_DevicComInfor.deviceType[11],
	(unsigned int *)&g_DevicComInfor.deviceType[12],
	(unsigned int *)&g_DevicComInfor.deviceType[13],
	(unsigned int *)&g_DevicComInfor.deviceType[14],
	(unsigned int *)&g_DevicComInfor.deviceType[15],
	(unsigned int *)&g_DevicComInfor.deviceType[16],
	(unsigned int *)&g_DevicComInfor.deviceType[17],
	(unsigned int *)&g_DevicComInfor.deviceType[18],
	(unsigned int *)&g_DevicComInfor.deviceType[19],
	(unsigned int *)&g_DevicComInfor.softVersion,		/*9820������汾��*/
	(unsigned int *)&g_DevicComInfor.protocolVersion,	/*9821����Լ�汾��*/
	(unsigned int *)&g_DevicComInfor.softYear,		/*9822����ǰ�汾���ڣ���*/
	(unsigned int *)&g_DevicComInfor.softMonth,
	(unsigned int *)&g_DevicComInfor.softDate,
    	
    	/* SN�޸�Ϊֻռ�����Ĵ���@ stone @    2010.12.02 */
	
    	(unsigned int *)&secret_params.sn + 1,	/*9825���кŸ�λ*/
	(unsigned int *)&secret_params.sn,	
	(unsigned int *)&blank,
	(unsigned int *)&blank,

	(unsigned int *)&secret_params.feature,  /* 9829�������� */

	#if !PMC_23V
    	(unsigned int *)&secret_params.amp_scale,	/*9830�������ֵ*/
    	#else
	(unsigned int *)&blank,		
   	 #endif
};

#define     START_DeviceInfor_REG	9800               /*װ����Ϣ�Ĵ�����ʼ��ַ*/
#define     END_DeviceInfor_REG		(START_DeviceInfor_REG + sizeof(ModbusDeviceInforTable) / sizeof(int *) - 1)      /*װ����Ϣ�Ĵ���������ַ*/
/* ------------------------------------------  װ����Ϣ�Ĵ���----------------------------------------- */

/* --------------------------------------------  װ����Ϣ�Ĵ�������------------------------------------------ */

unsigned int * const ModbusDeviceInforBackupTable[] =                           /* װ����Ϣ�Ĵ��������*/
{
	(unsigned int *)&g_DevicComInfor.deviceType[0],		/*60200��װ���ͺ�*/
	(unsigned int *)&g_DevicComInfor.deviceType[1],
	(unsigned int *)&g_DevicComInfor.deviceType[2],
	(unsigned int *)&g_DevicComInfor.deviceType[3],
	(unsigned int *)&g_DevicComInfor.deviceType[4],
	(unsigned int *)&g_DevicComInfor.deviceType[5],
	(unsigned int *)&g_DevicComInfor.deviceType[6],
	(unsigned int *)&g_DevicComInfor.deviceType[7],
	(unsigned int *)&g_DevicComInfor.deviceType[8],
	(unsigned int *)&g_DevicComInfor.deviceType[9],
	(unsigned int *)&g_DevicComInfor.deviceType[10],
	(unsigned int *)&g_DevicComInfor.deviceType[11],
	(unsigned int *)&g_DevicComInfor.deviceType[12],
	(unsigned int *)&g_DevicComInfor.deviceType[13],
	(unsigned int *)&g_DevicComInfor.deviceType[14],
	(unsigned int *)&g_DevicComInfor.deviceType[15],
	(unsigned int *)&g_DevicComInfor.deviceType[16],
	(unsigned int *)&g_DevicComInfor.deviceType[17],
	(unsigned int *)&g_DevicComInfor.deviceType[18],
	(unsigned int *)&g_DevicComInfor.deviceType[19],
	(unsigned int *)&g_DevicComInfor.softVersion,		/*60220������汾��*/
	(unsigned int *)&g_DevicComInfor.protocolVersion,	/*60221����Լ�汾��*/
	(unsigned int *)&g_DevicComInfor.softYear,		/*60222����ǰ�汾���ڣ���*/
	(unsigned int *)&g_DevicComInfor.softMonth,
	(unsigned int *)&g_DevicComInfor.softDate,
    	
    	/* SN�޸�Ϊֻռ�����Ĵ���@ stone @    2010.12.02 */
	
    	(unsigned int *)&secret_params.sn + 1,	/*60225���кŸ�λ*/
	(unsigned int *)&secret_params.sn,	
	(unsigned int *)&blank,
	(unsigned int *)&blank,

	(unsigned int *)&secret_params.feature,  /* 60229�������� */

	#if !PMC_23V
    	(unsigned int *)&secret_params.amp_scale,	/*60230�������ֵ*/
    	#else
	(unsigned int *)&blank,		
   	 #endif
};

#define     START_DeviceInfor_BACKUP_REG	60200               /*װ����Ϣ�Ĵ�����ʼ��ַ*/
#define     END_DeviceInfor_BACKUP_REG		(START_DeviceInfor_BACKUP_REG + sizeof(ModbusDeviceInforTable) / sizeof(int *) - 1)      /*װ����Ϣ�Ĵ���������ַ*/
/* ------------------------------------------  װ����Ϣ�Ĵ������ݽ���----------------------------------------- */

#if 0
/* --------------------------------------------  SOE ��¼�Ĵ���------------------------------------------ */
/*SOE��¼�Ĵ���δ�õ��*/

#define     START_SOE_REG               42000                                /*SOE�Ĵ�����ʼ��ַ*/
#define     END_SOE_REG                 (START_SOE_REG + 5 * SOE_BUFFER_NUM - 1)  /*SOE�Ĵ���������ַ*/


/* --------------------------------------------  SOE ��¼�Ĵ���------------------------------------------ */


/* -----------------------------------------  װ��ʱ������Ĵ���----------------------------------------- */

#define BROADCAST_ADDRESS		0                                   /*modbus�㲥��ַ*/
#define ADJUST_TIME_START_REG	        60000                               /*�㲥��ʱ������ʼ��ַ*/
#define ADJUST_TIME_REGNUM		4                                   /*�㲥��ʱ����Ĵ�����Ŀ*/   //2010.8.10 
#define ADJUST_TIME_END_REG		(ADJUST_TIME_START_REG + ADJUST_TIME_REGNUM - 1)/*�㲥��ʱ���������ַ*/

/*--------------------------------------2010.9.19����UNIXʱ��Ĵ���---------------------------------------*/
#define ADJUST_UNIXTIME_START_REG	ADJUST_TIME_START_REG + 4   
#define ADJUST_UNIXTIME_REGNUM		2
#define ADJUST_UNIXTIME_END_REG		(ADJUST_UNIXTIME_START_REG + ADJUST_UNIXTIME_REGNUM - 1)

/* ------------------------------------------  װ��ʱ������Ĵ���---------------------------------------- */

#endif
/* ------------------------------------------  ���ܼĴ���------------------------------------------------ */
unsigned int	adzero_adjust = 0;                                           /*�����־*/
unsigned int 	Cal_Coeff_Reset = 0;                                         /*����ϵ����λ*/
unsigned int 	Cal_Coeff_Reset_A = 0;                                         /*A�����ϵ����λ*/
unsigned int 	Cal_Coeff_Reset_B = 0;                                         /*B�����ϵ����λ*/
unsigned int 	Cal_Coeff_Reset_C = 0;                                         /*C�����ϵ����λ*/

//FCWong @2011.11.28 �޸��ָ���������ʱ��ֻ�ָ����ܼĴ�����
//unsigned int   Secret_Default = 0;                                          /*���ܼĴ������в����ָ�Ĭ�ϲ���λ*/
unsigned int   Common_Default = 0; 								//��������

unsigned int   System_Reset = 0;                                            /*ϵͳ��λ*/
unsigned int   System_Default = 0;                                          /*�����ָ���ʼ�ϵ�״̬*/

unsigned int    g_adjust_millisecond = 0;                                    /*�㲥��ʱ�ĺ��뱣��*/

unsigned long  high_word_reg = 0;		                             /*����˫�ּĴ�������ʱ�����λ��*/



int *const ModbusSecretRegTable[] =                                          /*���ܼĴ�����Ӧ������ָ��*/ 
{	

	/*******************************************************************************
	FCWong @2011.05.11
	�����ڲ��Ĵ����µ�˳����ţ��������д����顣
	********************************************************************************/

	(int *)&secret_params.feature,	/*65000��������*/
	(int *)&secret_params.sn + 1,	/*65001�����к�*/
	(int *)&secret_params.sn,

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.amp_scale,		/*65003*/
	#else
	(int *)&blank,
	#endif	
	
	(int *)&blank,						/*65004��Ԥ��*/

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.amp_level,			/*65005�������ż�ֵ*/
	#else
	(int *)&blank,
	#endif

	#if PART_PMC_23V_SIGN
	(int *)&secret_params.volt_level,			/*65006����ѹ�ż�ֵ*/
	#else
	(int *)&blank,
	#endif
	
	(int *)&blank,      
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,		/*65011��Ԥ��*/

	#if PART_PMC_23V_SIGN
	(int *)&secret_params.volt_adzero[0],          	/*65012��A���ѹͨ�����ֵ*/          
	(int *)&secret_params.volt_adzero[1],   
	(int *)&secret_params.volt_adzero[2],   		/*65014��A���ѹͨ�����ֵ*/   
	#else	
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.amp_adzero[0],			/*65015��A���ѹͨ�����ֵ*/
	(int *)&secret_params.amp_adzero[1],
	(int *)&secret_params.amp_adzero[2],			/*65017��A���ѹͨ�����ֵ*/
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif
	

	(int *)&blank,      /*65018*/
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,      
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,	/*65026*/

	
	(int *)&adzero_adjust,    	/*65027�����У׼*/
	(int *)&blank,
	(int *)&System_Reset, 		/*65029��ϵͳ��λ*/
	(int *)&System_Default,	/*65030���ָ�����Ĭ��*/
	(int *)&Cal_Coeff_Reset, 	/*65031��У׼ϵ���ָ�Ĭ��*/
	(int *)&Common_Default,	/*65032����ͨ�����ָ�Ĭ��*/
	(int *)&Cal_Coeff_Reset_A, 	/*65033��A��У׼ϵ���ָ�Ĭ��*/
	(int *)&Cal_Coeff_Reset_B, 	/*65034��B��У׼ϵ���ָ�Ĭ��*/
	(int *)&Cal_Coeff_Reset_C, 	/*65035��C��У׼ϵ���ָ�Ĭ��*/
    
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,      
	(int *)&blank,
	(int *)&blank,				/*65040*/

	(int *)&blank,				/*65041*/
	(int *)&blank,
	(int *)&blank,      
	(int *)&blank,
	(int *)&blank,				
	(int *)&blank,				
	(int *)&blank,
	(int *)&blank,      
	(int *)&blank,
	(int *)&blank,				/*65050*/

	(int *)&blank,				/*65051*/
	(int *)&blank,
	(int *)&blank,      
	(int *)&blank,
	(int *)&blank,				
	(int *)&blank,				
	(int *)&blank,
	(int *)&blank,      
	(int *)&blank,
	(int *)&blank,				/*65060*/


	#if PART_PMC_23I_SIGN
	(int *)&secret_params.I_section_flag,		/*65061��������ʽ*/
	(int *)&secret_params.I_subsection[0],		/*65062���ֶε�0����*/
	(int *)&secret_params.I_subsection[1],
	(int *)&secret_params.I_subsection[2],
	(int *)&secret_params.I_subsection[3],
	(int *)&secret_params.I_subsection[4],		/*65066���ֶε�4����*/
	#else
	(int *)&blank,      
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif
      
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,


	#if PART_PMC_23V_SIGN
	(int *)&secret_params.voltcal[0],       /*65070���ֶε�0�ĵ�ѹϵ��*/             
	(int *)&secret_params.voltcal[1],   
	(int *)&secret_params.voltcal[2],     
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	(int *)&blank,      
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.Ampcal[0].phs[0].ampcal,		/*65079���ֶε�0��A�����ϵ��*/	
	(int *)&secret_params.Ampcal[0].phs[1].ampcal,
	(int *)&secret_params.Ampcal[0].phs[2].ampcal,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif
	
	#if PART_PMC_23M_SIGN
	(int *)&secret_params.Ampcal[0].phs[0].pf_radian_compensate+1,	/*65082����������ֵ*/
	(int *)&secret_params.Ampcal[0].phs[0].pf_radian_compensate,
	(int *)&secret_params.Ampcal[0].phs[1].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[0].phs[1].pf_radian_compensate,
	(int *)&secret_params.Ampcal[0].phs[2].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[0].phs[2].pf_radian_compensate,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.Ampcal[1].phs[0].ampcal,		/*65088���ֶε�1��A��Ȳ�ϵ��*/
	(int *)&secret_params.Ampcal[1].phs[1].ampcal,
	(int *)&secret_params.Ampcal[1].phs[2].ampcal,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23M_SIGN
	(int *)&secret_params.Ampcal[1].phs[0].pf_radian_compensate+1,	/*65091���ֶε�1��A�๦�ʲ�������*/
	(int *)&secret_params.Ampcal[1].phs[0].pf_radian_compensate,
	(int *)&secret_params.Ampcal[1].phs[1].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[1].phs[1].pf_radian_compensate,
	(int *)&secret_params.Ampcal[1].phs[2].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[1].phs[2].pf_radian_compensate,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.Ampcal[2].phs[0].ampcal,	/*65097���ֶε�2��ʼ*/	
	(int *)&secret_params.Ampcal[2].phs[1].ampcal,
	(int *)&secret_params.Ampcal[2].phs[2].ampcal,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23M_SIGN
	(int *)&secret_params.Ampcal[2].phs[0].pf_radian_compensate+1,/*65100*/
	(int *)&secret_params.Ampcal[2].phs[0].pf_radian_compensate,
	(int *)&secret_params.Ampcal[2].phs[1].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[2].phs[1].pf_radian_compensate,
	(int *)&secret_params.Ampcal[2].phs[2].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[2].phs[2].pf_radian_compensate,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.Ampcal[3].phs[0].ampcal,			/*65106���ֶε�3��ʼ*/
	(int *)&secret_params.Ampcal[3].phs[1].ampcal,
	(int *)&secret_params.Ampcal[3].phs[2].ampcal,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23M_SIGN
	(int *)&secret_params.Ampcal[3].phs[0].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[3].phs[0].pf_radian_compensate,
	(int *)&secret_params.Ampcal[3].phs[1].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[3].phs[1].pf_radian_compensate,
	(int *)&secret_params.Ampcal[3].phs[2].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[3].phs[2].pf_radian_compensate,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.Ampcal[4].phs[0].ampcal,			/*65115���ֶε�4��ʼ*/
	(int *)&secret_params.Ampcal[4].phs[1].ampcal,
	(int *)&secret_params.Ampcal[4].phs[2].ampcal,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23M_SIGN
	(int *)&secret_params.Ampcal[4].phs[0].pf_radian_compensate+1,	/*65118*/
	(int *)&secret_params.Ampcal[4].phs[0].pf_radian_compensate,
	(int *)&secret_params.Ampcal[4].phs[1].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[4].phs[1].pf_radian_compensate,
	(int *)&secret_params.Ampcal[4].phs[2].pf_radian_compensate+1,
	(int *)&secret_params.Ampcal[4].phs[2].pf_radian_compensate,		/*65123*/
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif
	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*65124��ʼ*/
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*65224*/
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,
	(int *)&blank,

	(int *)&calibrateStartFlag,		/*65300������У׼*/
	(int *)&operatingSection,		/*65301����ǰУ׼�Ķκ�*/
	(int *)&standardVars.vln + 1,		/*65302����׼Դ��ѹ*/
    	(int *)&standardVars.vln,
	(int *)&standardVars.amps + 1,
    	(int *)&standardVars.amps,
	(int *)&standardVars.angle + 1,	/*65306��׼Դ��λ�н�*/
    	(int *)&standardVars.angle,
	(int *)&precisionRequest.volt,	/*65308��ѹ��������Ҫ��*/
	(int *)&precisionRequest.power,
	//FCWong @2011.06.10 ����У׼���ݲɼ���������,ʹ�ô˱��������ĵ�ַ
	//(int *)&precisionRequest.powerFactor,   //�˱���δʹ�ã�ֻ��Ԥ���˱���
	(int *)&calibrateAccTimes,	/*65310��У׼��������*/
	
	
	(int *)&calibrateSuccessfulFlag + 1,	/*65311��У׼���*/
	(int *)&calibrateSuccessfulFlag,
	(int *)&checkError.voltError[0],		/*65313��A���ѹ���*/
	(int *)&checkError.voltError[1],
	(int *)&checkError.voltError[2],
	(int *)&checkError.ampError[0],		/*65316��A��������*/
	(int *)&checkError.ampError[1],
	(int *)&checkError.ampError[2],
	(int *)&checkError.PError[0],			/*65319��A���й����*/
	(int *)&checkError.PError[1],
	(int *)&checkError.PError[2],
	(int *)&checkError.QError[0],			/*65322��A���޹����*/
	(int *)&checkError.QError[1],
	(int *)&checkError.QError[2],
	(int *)&checkError.SError[0],			/*65325��A�����ڹ������*/
	(int *)&checkError.SError[1],
	(int *)&checkError.SError[2],
	(int *)&checkError.PFError[0],			/*65328��A�๦���������*/
	(int *)&checkError.PFError[1],
	(int *)&checkError.PFError[2],
	
};

#define START_SECRET_REG	65000      		/*�������ݼĴ�����ʼ��ַ*/

#define ADZERO_ADJUST_REG	65027	/*AD����ĵ�ַ*/	
#define SYSTEM_DEFAULT_REG      65032		/*ϵͳ�����ָ�Ĭ��*/
#define SYSTEM_COEFF_A_REG      65033	/*A��У׼�����ָ�Ĭ��*/
#define SYSTEM_COEFF_B_REG      65034
#define SYSTEM_COEFF_C_REG      65035	/*C������ָ�Ĭ��*/

#define END_SECRET_REG		(START_SECRET_REG + sizeof(ModbusSecretRegTable) / sizeof(int *) - 1)  

#define COEFF_MIN_SECTION0	8000		/*��ѹ�����ֶε�0����Сϵ��*/
#define COEFF_MAX_SECTION0	15000		/*��ѹ�����ֶε�0����Сϵ��*/

#define COEFF_MIN_SECTION1	5000		/*�����ֶε�1����Сϵ��*/
#define COEFF_MAX_SECTION1	15000		/*�����ֶε�1�����ϵ��*/

#define COEFF_MIN_SECTION2	5000		/*�����ֶε�2����Сϵ��*/
#define COEFF_MAX_SECTION2	15000		/*�����ֶε�2�����ϵ��*/

#define COEFF_MIN_SECTION3	5000		/*�����ֶε�3����Сϵ��*/
#define COEFF_MAX_SECTION3	15000		/*�����ֶε�3�����ϵ��*/

#define COEFF_MIN_SECTION4	5000		/*�����ֶε�4����Сϵ��*/
#define COEFF_MAX_SECTION4	15000		/*�����ֶε�4�����ϵ��*/

/* --------------------------------------------  ���ܼĴ���------------------------------------------ */


/*******************�����Ĵ����������*******************************/

/*�ɶ�д(03,10)������MODBUS���ֿ�*/
const ComModuelStruct  ModuleTable[] =                         
{
	{START_DATA_REG, END_DATA_REG, RO,DATA_MODULE},  
	{START_ENERGY_REG,END_ENERGY_REG,RW,ENERGY_MODULE},
	//{START_THD_REG,END_THD_REG,RO,THD_MODULE},   //2010.07.30 ����Ϊֻ��
	{START_SET_REG,END_SET_REG,RW,DEVICE_SET_MODULE},
	//{START_SOE_REG,END_SOE_REG,RO,SOE_MODULE},
	//{ADJUST_TIME_START_REG,ADJUST_TIME_END_REG,RW,ADJUST_TIME_MODUBLE},
	//{ADJUST_UNIXTIME_START_REG,ADJUST_UNIXTIME_END_REG,RW,ADJUST_UNIX_TIME_MODUBLE},  //2010.9.19
	//{ADJUST_TIME_START_REG,ADJUST_UNIXTIME_END_REG,RW,ADJUST_BOTH_TIME_MODUBLE},
	{START_DeviceInfor_REG,END_DeviceInfor_REG,RO,DEVICE_INFORMATION_MODULE},
	{START_DeviceInfor_BACKUP_REG,END_DeviceInfor_BACKUP_REG,RO,DEVICE_INFORMATION_BACKUP_MODULE},	/*��Ϣ�Ĵ�������*/
};
#define MODULENUM  sizeof(ModuleTable) / sizeof(ComModuelStruct)

/*������Ҫ����д��ļĴ�����ż�����*/
static  BlockRegStruct  EnergyBlockWrRegTable[] =                       
{	
    {START_ENERGY_REG,2},
    {START_ENERGY_REG + 2,2},
    {START_ENERGY_REG + 6,2},
    {START_ENERGY_REG + 8,2},
    {START_ENERGY_REG + 12,2},
};
#define EnergyBlockNum  sizeof(EnergyBlockWrRegTable) / sizeof(BlockRegStruct)


static BlockRegStruct SecretBlockWrRegTable[] =
{
	{START_SECRET_REG + 1 , 2},		/*65001�����к�*/
	{START_SECRET_REG + 82 , 2},	/*65082���ֶε�0��A�๦�ʲ���*/
	{START_SECRET_REG + 84 , 2},
	{START_SECRET_REG + 86 , 2},
	{START_SECRET_REG + 91 , 2},	/*65091���ֶε�1*/
	{START_SECRET_REG + 93 , 2},
	{START_SECRET_REG + 95 , 2},
	{START_SECRET_REG + 100 , 2},	/*�ֶε�2*/
	{START_SECRET_REG + 102 , 2},
	{START_SECRET_REG + 104 , 2},
	{START_SECRET_REG + 109 , 2},	/*�ֶε�3*/
	{START_SECRET_REG + 111 , 2},	
	{START_SECRET_REG + 113 , 2},
	{START_SECRET_REG + 118 , 2},	/*�ֶε�4*/
	{START_SECRET_REG + 120 , 2},
	{START_SECRET_REG + 122 , 2},
	{START_SECRET_REG + 302 , 2},
	{START_SECRET_REG + 304 , 2},
	{START_SECRET_REG + 306 , 2},
	{START_SECRET_REG + 311 , 2},
	
};
#define SecretBlockNum  sizeof(SecretBlockWrRegTable) / sizeof(BlockRegStruct)

//unsigned long high_word_reg;/*����˫�ּĴ�������ʱ�����λ��*/

/* ͨ���޸Ĳ�����Խ����㼰ʱ�ӱ�־*/
unsigned int CommAppFlag = 0;
/* CRC ��λ�ֽ�ֵ�� */
const  unsigned char auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};
/* CRC��λ�ֽ�ֵ��*/
const unsigned char auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
0x43, 0x83, 0x41, 0x81, 0x80, 0x40
} ;
unsigned int  get_crc16(unsigned int len,unsigned char * pBuffer)
{
  unsigned char uchCRCHi = 0xFF ;               /* ��CRC�ֽڳ�ʼ�� */
  unsigned char uchCRCLo = 0xFF ;               /* ��CRC �ֽڳ�ʼ�� */
  unsigned char uIndex;                         /* CRCѭ���е����� */
  while (len--)                                 /* ������Ϣ������ */
  {
      uIndex = uchCRCHi ^ * (pBuffer++);        /* ����CRC */
      uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
      uchCRCLo = auchCRCLo[uIndex] ;
   }
      return (uchCRCLo << 8 | uchCRCHi) ; 
}


void SnUpdateDeviceInfor()
{
   	g_DevicComInfor.deviceYear =   secret_params.sn / 100000000;
   	g_DevicComInfor.deviceMonth = (secret_params.sn / 1000000) % 100;
   	g_DevicComInfor.deviceDate =  (secret_params.sn / 10000) % 100;
   	g_DevicComInfor.deviceMadeSeiral = secret_params.sn % 10000;
}

/******************************************************************************
 * ������:	ComInitHook 
 * ����: 
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:huhc
 * ��������:2008.7.14
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void ComInitHook(void)
{
	unsigned char   i = 0;
	unsigned char   j;
	unsigned char *pDeviceType = DEVICE_TYPE;

	while((*pDeviceType != '\0') && (i < 20))
	{
		g_DevicComInfor.deviceType[i] = *pDeviceType;
		pDeviceType++;
		i++;   
	}

	if(i < 20)
	{
		for(j = i; j < 20; j++)
		{
			g_DevicComInfor.deviceType[j] = 0x20; 
		}
	}
	
   	g_DevicComInfor.softVersion = SOFTWARE_REVISION;
   	g_DevicComInfor.protocolVersion = PROTOCAL_REVISION;
   	g_DevicComInfor.softYear = REV_YEAR;
   	g_DevicComInfor.softMonth = REV_MONTH;
   	g_DevicComInfor.softDate = REV_DATE;

  	SnUpdateDeviceInfor();
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
	uint8_t err = ERR_OK;
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
			ModbusLinkDisp(i);                /*modbus ��·�㴦��*/
			ModbusAppDisp(i);                 /*ModbusӦ�ò㴦��*/
			params_modify_deal();
				}

			#if 0
			
			ProtocolBuf[i].pTxBuf = UARTBuf[i].TxBuf;         //��ַ�û�
			ProtocolBuf[i].pRxBuf = UARTBuf[i].RxBuf;
			ProtocolBuf[i].RxLen = UARTBuf[i].RxLen;
			ProtocolBuf[i].TxLen = 0;
			//UARTBuf[i].RxLen = 0;		//�Ѿ�����ȡ��ProtocolBuf0.RxLen, ������0

			modbus_rtu_process(&ProtocolBuf[i], DevParams.Address);	/*MODBUSͨ��Э�鴦��*/

			UARTBuf[i].TxLen = ProtocolBuf[i].TxLen;  /*�û��������������������*/
			if(UARTBuf[i].TxLen >0)
			{
				message_send_printf(TRANS_UART, i, TRUE, TRANS_MSG);
				//Rs485StartSend();
			}
			Delay_clk(50);
			UARTBuf[i].RxLen = 0;	        /*���������Ѵ��������ر�־*/
			}

			
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
	}
}
/***********************************************************************
������:	void ModbusLinkDisp(void)

�������:��.

�������:��.

����ֵ:	��.

����˵��:     ��·�㴦����   ,��Ҫ��CRCУ��͵�ַ����У��
***********************************************************************/
void ModbusLinkDisp(unsigned char comNUm)
{
	unsigned int RedundancyCheck;           /*��ʱ���ͨѶ֡crcУ���*/
	unsigned char  FramCheck = FALSE;       /*ͨѶ֡��Ч��־*/
	
	//g_ENDT15Flag[comNUm] = FALSE;
	//g_ENDT35Flag[comNUm] = FALSE;

	//g_T35_num[comNUm]  = -1;
	//g_T15_num[comNUm]  = -1;

	/*��·�� ���ݴ���*/
	g_ADUData[comNUm].FrameOK = FALSE; 
	/************************************************************************* 
	*FCWong @2011.05.30
	*�������ʱ��������λ��ͨ�ŵ�װ�û��յ���������ݣ��������ж�ʱ��
	*���������ݵĳ���ΪС��3����id����ȷ�������crc�ĵ�ַƫ��ʱ��ᷢ�������
	*Modbus����С�ĵ�����֡����Ϊ8����˴˴���Ϊ����7���жϡ�
	**************************************************************************/
	//if (( g_ADUData[comNUm] .ADULength > 0) && ( g_ADUData[comNUm] .ADULength < COMMBUFLEN))
	if (( g_ADUData[comNUm] .ADULength > 7) && ( g_ADUData[comNUm] .ADULength < COMMBUFLEN))
	{
		if (*g_ADUData[comNUm].ADUBuffPtr == g_ADUData[comNUm].address)
		{
			FramCheck = TRUE;
		}

		if(FramCheck == TRUE  )/*֡CRC�����Ч*/
		{ 
			RedundancyCheck = *(g_ADUData[comNUm] .ADUBuffPtr + g_ADUData[comNUm] .ADULength - 2) |
				                  (*(g_ADUData[comNUm] .ADUBuffPtr + g_ADUData[comNUm] .ADULength - 1) << 8);
           	 	if (RedundancyCheck == get_crc16(g_ADUData[comNUm] .ADULength - 2,g_ADUData[comNUm] .ADUBuffPtr))
			{
				g_ADUData[comNUm] .FrameOK = TRUE;
				return;
			}
		}
	}		/*��·�� ���ݴ������*/
}
/***********************************************************************
������:			void ModbusAppDisp(void)

�������:		��.

�������:		��.

����ֵ:			��.

����˵��:           Ӧ�ò㴦����   
***********************************************************************/
void ModbusAppDisp(unsigned char comNum)
{ 
	if (g_ADUData[comNum].FrameOK)             /*CRCУ����ȷ����֡Ϊ�Ϸ���*/
	{
		PackADU2PDU(comNum);
	}
	if (g_PDUData[comNum].FrameOK)
	{
		g_PDUData[comNum].FrameOK = FALSE;
		FunctionHandle(comNum);
		if (g_PDUData[comNum].ExceptionCode)
		{
			*g_PDUData[comNum].PDUBuffPtr = g_PDUData[comNum].FunctionCode | 0x80;
			*(g_PDUData[comNum].PDUBuffPtr + 1) = g_PDUData[comNum].ExceptionCode;
			g_PDUData[comNum].PDULength = 2;//�����쳣��Ӧ֡
		}
		g_PDUData[comNum].FrameOK = TRUE;
	}

	if (g_PDUData[comNum].FrameOK)
	{
		PackPDU2ADU(comNum);
		
		if (*(g_ADUData[comNum].ADUBuffPtr) != 0)//������ǹ㲥����
		{
			SendResponse(comNum);//��������൱���������ͺ��������͵�һ������
		}
		else
		{
			g_CommRxFlag[comNum] = TRUE;
			g_ADUData[comNum].FrameOK = FALSE;
			g_ADUData[comNum].ADULength = 0;			
		}	
	}
	else
	{
		g_CommRxFlag[comNum] = TRUE;
		g_ADUData[comNum].ADULength = 0;	
	}
}    
/**************************************************************************
������:	              PackADU2PDU

�������:		��

�������:	      	g_ADUData��g_PDUData

����ֵ:			��

����˵��:	       ��ADU���ݰ�������·����صĲ��ְ���
***************************************************************************/
void PackADU2PDU(unsigned char comNum)//����Ĳ��ְ�����ַ��CRCУ����
{
	g_ADUData[comNum].FrameOK =FALSE;
	g_PDUData[comNum].PDUBuffPtr = g_ADUData[comNum].ADUBuffPtr + 1;		/*ȥ����ַ��*/
	g_PDUData[comNum].PDULength = g_ADUData[comNum].ADULength - 3;	/*ȥ��У����*/
	g_PDUData[comNum].FunctionCode = *g_PDUData[comNum].PDUBuffPtr;	/*����֡�ӹ����뿪ʼ*/
	g_PDUData[comNum].FrameOK = TRUE;						
	g_ADUData[comNum].ADULength = 0;
}

/************************************************************************************
������:	       FunctionHandle

�������:	��

�������:	g_PDUData

����ֵ:		��

����˵��:	���ݹ���������빦���������׵Ĺ����봦����

**************************************************************************************/
void FunctionHandle(unsigned char comNum)
{
	unsigned char i;
	PDUResponse_Struct  Response;
	
	for (i = 0; i < MAX_FUNCTION_NUM; i++)
	{
		if( g_PDUData[comNum].FunctionCode == g_FCodeHandleArray[i].functionCode)
		{
			Response.PDUDataPtr = g_PDUData[comNum].PDUBuffPtr;
			Response.PDUByteLength = g_PDUData[comNum].PDULength;
			switch( g_FCodeHandleArray[i].functionCode)
			{
			case  READREG_COMMAND:
				FReadHoldingReg(&Response);    
				break;
			case  WRITEREG_COMMAND:
				FWriteCommonReg(&Response);    
				break;
			case  READSECRATEREG_COMMAND:
				FReadSecretReg(&Response);    
				break;
			case  WRITESECRATEREG_COMMAND:
				FWriteSecretReg(&Response);    
				break;
			default:
				break;
			}
			g_PDUData[comNum].PDULength = Response.PDUByteLength;
			g_PDUData[comNum].ExceptionCode = Response.ExceptionCode;
			return;
		}  
	}
	g_PDUData[comNum].ExceptionCode = ILLEGALfUNCTIONCODE;		/*û����ȷ���أ�����ȷ�Ĺ�����*/
}

/**************************************************************************
������:	              PackPDU2ADU

�������:		��

�������:	      	g_ADUData��g_PDUData

����ֵ:			ADU���������ݳ���

����˵��:	       ��PDU���ݰ��������·����ز������ݣ�
                                   �γ������ķ������ݰ�
***************************************************************************/
void  PackPDU2ADU(unsigned char comNum)
{
	unsigned int CRC_Value;
	
	//*(g_ADUData[comNum].ADUBuffPtr) = g_ADUData[comNum].address;/*zengwei delete 2008.1.29*/
	CRC_Value = get_crc16(g_PDUData[comNum].PDULength + 1,g_ADUData[comNum].ADUBuffPtr);
	*(g_ADUData[comNum].ADUBuffPtr + g_PDUData[comNum].PDULength + 2) = (unsigned char)(CRC_Value / 0x100);
	*(g_ADUData[comNum].ADUBuffPtr + g_PDUData[comNum].PDULength + 1) = (unsigned char)CRC_Value;
	g_ADUData[comNum].ADULength = g_PDUData[comNum].PDULength + 3;
	g_ADUData[comNum].FrameOK = TRUE;
	g_PDUData[comNum].FrameOK = FALSE;
}
/************************************************************************************
������:	       FReadHoldingReg

�������:	Response��׼�����ص�PDU���ݽṹ��ָ��

�������:	Responseָ��ָ���g_PDU_Response����

����ֵ:		��

����˵��:	0X03�����ּĴ�������Ӧ�ĺ���

**************************************************************************************/
void FReadHoldingReg(PDUResponse_Struct *Response) 
{
	unsigned char *PDUPtr;
	unsigned int StAddr;//03����֡��ʼ��ַ
	unsigned int Quantity;//03����֡��ȡ�Ĵ�������
	unsigned char i;

	E_MODBUSREGMODULE moduleFlag;//�Ĵ���������(ö�ٱ���)

	if (Response->PDUByteLength != 5)/*���Ĵ�����֡���ȴ���ֱ�ӷ���*/
	{
		Response->ExceptionCode = ILLEGALFRAMELENGTH;
		return;
	}

	PDUPtr = Response->PDUDataPtr;
	StAddr  = PDUPtr[1] << 8 | PDUPtr[2];
	Quantity = PDUPtr[3] << 8 | PDUPtr[4];
	Response->PDUByteLength = 2 + Quantity * 2;//������Ȳ�������ַ��CRCУ����
	
	moduleFlag = NONE_MODULE;
	for (i = 0; i < MODULENUM; i++)/*�����Ҫ�����ļĴ���ģ��*/
	{
		if((StAddr >= ModuleTable[i].ModuleStartAddress) && ((StAddr+Quantity - 1) <= ModuleTable[i].ModuleEndAdress))
		{
			moduleFlag = ModuleTable[i].ModuleFlag;
			break;
		}
	}
	
	if ((Quantity >= 0x001) && (Quantity <= 0x007d))	//�Ĵ�������С��125��
	{
		if(moduleFlag != NONE_MODULE)
		{
			Response->ExceptionCode = ReadHoldingReg(PDUPtr + 2,StAddr,Quantity,moduleFlag);
			
			if (Response->ExceptionCode == SUCCESSFULLCOMMAND)
			{
				*(PDUPtr + 1) = Quantity * 2;//�����ֽڵĸ���	                         
			}
		}
		else
		{
			Response->ExceptionCode = ILLEGALREGQADDRESS;
		}
	}
	else
	{
		Response->ExceptionCode = ILLEGALREGQATITY;
	}
}
/************************************************************************************
������:	       FWriteMultipleReg

�������:	Response:

�������:	Responseָ������PDU�ṹ�����

����ֵ:		��

����˵��:	0x10��д����Ĵ���

**************************************************************************************/
void FWriteCommonReg(PDUResponse_Struct *Response) 
{
	unsigned int StAddr;
	unsigned int Quantity;
	unsigned char ByteCount;
	unsigned char *PDUPtr;
	unsigned char i;
	E_MODBUSREGMODULE    moduleFlag;
	
	PDUPtr = Response->PDUDataPtr;
	StAddr = PDUPtr[1] << 8 | PDUPtr[2];
	Quantity = PDUPtr[3] << 8 | PDUPtr[4];
	ByteCount = PDUPtr[5];   
	
	moduleFlag= NONE_MODULE;/*�����Ҫ�����ļĴ���ģ��*/
	for (i = 0; i < MODULENUM; i++)
	{
		if((StAddr >= ModuleTable[i].ModuleStartAddress) && (ModuleTable[i].ModuleAttribute != RO)	  /*RO����������ų���*/
			&& ((StAddr + Quantity - 1) <= ModuleTable[i].ModuleEndAdress))
		{
			moduleFlag = ModuleTable[i].ModuleFlag;
			break;
		}
	}
	
	if ((CheckFrameBorder(StAddr,Quantity,moduleFlag) == FALSE) || ((Response->PDUByteLength - 6) != ByteCount))/*���յ����ݳ���������֡�б�Ƶ����ݳ��Ȳ���*/
	{
		Response->ExceptionCode = ILLEGALFRAMELENGTH;
		return;
	}
	
	Response->PDUByteLength = 5;
	
	if ((Quantity >= 0x001) && (Quantity <= 0x007d) && (ByteCount == (Quantity * 2)))
	{
		if(moduleFlag != NONE_MODULE)
		{
		//	if((StAddr <= PARITY_SET_REG) && (StAddr + Quantity - 1) >= PARITY_SET_REG)//У�鷽ʽ��Ϊ��д
		//	{
		//		Response->ExceptionCode = ILLEGALREGQADDRESS;
		//	}
		//	else
			{
				Response->ExceptionCode =  WriteHoldingReg(PDUPtr + 6,StAddr,Quantity, moduleFlag);
			}
		}
		else
		{
			Response->ExceptionCode = ILLEGALREGQADDRESS;
		}
	}
	else
	{
		Response->ExceptionCode = ILLEGALREGQATITY;
	}
	/*д�������Ĺ��Ӻ��������ڲ���д���Ӧ������Ҫ��һЩ��ش���*/
	//FwriteHook();                    
}
/************************************************************************************
������:	       FReadSecretReg

�������:	Response��׼�����ص�PDU���ݽṹ��ָ��

�������:	Responseָ��ָ���g_PDU_Response����

����ֵ:		��

����˵��:	0X46�����ܼĴ�������Ӧ�ĺ���

**************************************************************************************/
void FReadSecretReg(PDUResponse_Struct *Response)
{
	unsigned char *PDUPtr;
	unsigned int StAddr;
	unsigned int Quantity;
	
	if (Response->PDUByteLength != 5)
	{
		Response->ExceptionCode = ILLEGALFRAMELENGTH;
		return;
	}
	
	PDUPtr = Response->PDUDataPtr;
	StAddr = PDUPtr[1] << 8 | PDUPtr[2];
	Quantity = PDUPtr[3] << 8 | PDUPtr[4];
	Response->PDUByteLength = 2 + Quantity * 2;
	
	if ((Quantity>=0x001) && (Quantity<=0x007d))
	{
		/*�����ȡ�������ܼĴ�������DIDO���üĴ���*/
		if ((StAddr >= START_SECRET_REG) && ((StAddr + Quantity - 1) <= END_SECRET_REG ))
		{
			Response->ExceptionCode = ReadHoldingReg(PDUPtr + 2,StAddr,Quantity,SECRET_MODULE);
			
			if (Response->ExceptionCode == SUCCESSFULLCOMMAND)
			{
				*(PDUPtr + 1) = Quantity * 2;                            
			}
		}
		else
		{
			Response->ExceptionCode = ILLEGALREGQADDRESS;
		}
	}
	else
	{
		Response->ExceptionCode = ILLEGALREGQATITY;
	}
}
/************************************************************************************
������:	       FWriteSecretReg

�������:	Response:

�������:	Responseָ������PDU�ṹ�����

����ֵ:		��

����˵��:	0x47��д���ܼĴ���

**************************************************************************************/
void FWriteSecretReg(PDUResponse_Struct *Response) 
{
	unsigned char *PDUPtr;
	unsigned char ByteCount;
	unsigned int StAddr;
	unsigned int Quantity;
	
	PDUPtr = Response->PDUDataPtr;
	StAddr = PDUPtr[1] << 8 | PDUPtr[2];
	Quantity = PDUPtr[3] << 8 | PDUPtr[4];
	ByteCount = PDUPtr[5];
	
	//���ܼĴ���Ҳ��1��߽���
	if ((FALSE == CheckFrameBorder(StAddr,Quantity,SECRET_MODULE)) || ((Response->PDUByteLength - 6) != ByteCount))
	{
		Response->ExceptionCode = ILLEGALFRAMELENGTH;
		return;
	}
	
	Response->PDUByteLength = 5;
	if ((Quantity >= 0x001) && (Quantity <= 0x007d) && (ByteCount == (Quantity * 2)))
	{
		//FCWong @2011.05.12 ʹ���µ����ܼĴ����������ж�ֻ����
		//���ܼĴ�����һ������ֻ�������޳�//65000~65021
		//if ((StAddr > START_SECRET_REG + 5) && ((StAddr + Quantity - 1) <= END_SECRET_REG) | (StAddr == 0xFFDC))
		if((( ((StAddr < START_SECRET_REG + 12)&&(StAddr >=START_SECRET_REG)) || 
			((StAddr > START_SECRET_REG + 17) && (StAddr < START_SECRET_REG + 311))) && 
			((StAddr + Quantity - 1) <= (START_SECRET_REG + 311)))||(StAddr == 0xFFDC))		/*����StAddr >=START_SECRET_REG����ֹ��С��ַҲ�ɹ�*/
		{
			Response->ExceptionCode = WriteHoldingReg(PDUPtr + 6,StAddr,Quantity,SECRET_MODULE);
		}
		else
		{
			Response->ExceptionCode = ILLEGALREGQADDRESS;
		}
	}
	else
	{
		Response->ExceptionCode = ILLEGALREGQATITY;
	}
	//FwriteHook();
}

#if 0
/******************************************************************************
 * ������:	FWriteSingleCoil 
 * ����: 
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2008.4.2
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
unsigned char flag_yk =0;
void FWriteSingleCoil(PDUResponse_Struct *Response)
{
	unsigned char *PDUPtr;
	unsigned int OutputAddress;
	unsigned int OutputValue;
	
	if (Response->PDUByteLength != 5)
	{
		Response->ExceptionCode = ILLEGALFRAMELENGTH;
		return;
	}
	
	PDUPtr = Response->PDUDataPtr;
	OutputAddress = PDUPtr[1] << 8 | PDUPtr[2];
	OutputValue = PDUPtr[3] << 8 | PDUPtr[4];
	Response->PDUByteLength = 5;
	
	if ((OutputValue == 0x0000) || (OutputValue == 0xff00))
	{
		if ((OutputAddress >= START_COIL_REG ) && (OutputAddress <= END_COIL_REG ))
		{
			Response->ExceptionCode = MB_SetRegValue(OutputAddress,OutputValue,REMOTE_MODULE);
		}
		else
		{
			Response->ExceptionCode = ILLEGALREGQADDRESS;
		}
	}
	else
	{
		Response->ExceptionCode = ILLEGALREGQATITY;
	}
}
#endif

/************************************************************************************
������:	       ReadHoldingReg

�������:	DataPtr:�������ݱ����ַ
                            StAddr:�����ȡ�Ŀ�ʼ��ַ
                            Quantity:��ȡ�ļĴ�������
                            moduleFlag:�����ȡ�ļĴ���ģ������ 
�������:	��
����ֵ:		TRUE   ���Ĵ����ɹ�
                             FALSE ���Ĵ���ʧ��

����˵��:	�����ּĴ�������Ӧ�ĺ���

**************************************************************************************/
unsigned char  ReadHoldingReg(unsigned char *DataPtr, unsigned int StAddr, unsigned int Quantity,E_MODBUSREGMODULE moduleFlag) 
{
	unsigned char i;
	unsigned char tempExceptionCode;
	unsigned int value;
	
	for (i = 0; i < Quantity; i++)
	{
		tempExceptionCode = MB_GetRegValue(&value, StAddr + i,moduleFlag);  
		if (tempExceptionCode == SUCCESSFULLCOMMAND)
		{
			*(DataPtr + i * 2) = (unsigned char)(value >> 8);
			*(DataPtr + i * 2 + 1) = (unsigned char)value;
		}
		else
		{
			return tempExceptionCode;
		}
	}
	return tempExceptionCode;
}
/************************************************************************************
������:	       WriteHoldingReg

�������:	KeepDataPtr:׼��д�����ݵ�ָ��
                            StAddr:׼��д�����ݵĵ�ַ
                            Quantity:׼��д��ļĴ�������
�������:	��

����ֵ:		д��ɹ�����TRUE������ΪFALSE

����˵��:	д����Ĵ���

**************************************************************************************/
unsigned char WriteHoldingReg(unsigned char *KeepDataPtr,unsigned int StAddr,unsigned int Quantity, E_MODBUSREGMODULE moduleFlag) 
{
	unsigned char    i;
	unsigned char    wrFlag;
	unsigned int     Value;
	unsigned char    returnValue = SUCCESSFULLCOMMAND;
	
	for (i = 0; i < Quantity; i++)//���ȼ��д�������Ƿ�������Χ��
	{
		Value = (*(KeepDataPtr + i * 2) << 8)|(*(KeepDataPtr + i * 2 + 1));
		if (MB_CheckRegValue(StAddr + i,Value,moduleFlag) == FALSE)
		{
			return ILLEGALREGVALUE;
		}
	}
	
	#if PMC_23M
	if (FALSE == CheckCTmultiPTlimit())		/*�Ƚ����⣬���PT���*CT ����Ƿ������Ʒ�Χ��*/
	{
		return ILLEGALREGVALUE;
	}  
	#endif
	
	/*д����Ч����*/
	for (i = 0; i < Quantity; i++)
	{
		Value = (*(KeepDataPtr + i * 2) << 8)|(*(KeepDataPtr + i * 2 + 1));
		wrFlag = MB_SetRegValue(StAddr + i, Value,moduleFlag); 
		if (wrFlag != SUCCESSFULLCOMMAND)
		{
			return wrFlag;
		}
	}
	return returnValue;
}
/************************************************************************************
������:	       MB_GetRegValue

�������:	ValuePtr:��ȡ�Ĵ�ֵ����ĵ�ַ
                            Address: �Ĵ�����ַ                          

�������:	����ر��ּĴ�����ֵ���Ƶ�DataPtrָ�������

����ֵ:		���ɹ�����TRUE��ʧ����ΪFALSE

����˵��:	��ȡ���ּĴ�����ֵ�������ݸ�PDU

��ע:��Ӧ�����,�ұ������������ݼ�ʹ��������ҲҪ��CASE����г���
**************************************************************************************/
unsigned char  MB_GetRegValue(unsigned int *ValuePtr,unsigned int Address,E_MODBUSREGMODULE moduleFlag) 
{
	//unsigned int  location;  
	//unsigned int  temp1, temp2; 
	unsigned int tmp;
	unsigned char returnValue = SUCCESSFULLCOMMAND;
	
	*ValuePtr = 0;
	
	switch(moduleFlag)
	{
	case  DATA_MODULE:
		if(Address < ( START_DATA_REG + MBUS_DATA_REG_NUM ))
		{
			tmp = Address - START_DATA_REG;
			*ValuePtr= *(ModbusDataRegTable[tmp]);
		}
		else
		{
			returnValue = ILLEGALREGQADDRESS;
		}
		break;
		
	case ENERGY_MODULE:
		if(Address < ( START_ENERGY_REG + MBUS_ENERGY_REG_NUM ))
		{
			tmp = Address - START_ENERGY_REG;
			*ValuePtr= *(ModbusEnergyRegTable[tmp]);
		}
		else
		{
			returnValue = ILLEGALREGQADDRESS;
		}      
		break;    
		
	case DEVICE_SET_MODULE://���soe����ļĴ���ΪWO     			
		if((Address >= START_SET_REG ) && (Address <= ENDRO_SET_REG)
			&& ( Address != CLEAR_ENERGY_REG) )
		{
			tmp = Address - START_SET_REG;
			*ValuePtr = *(ModbusParamsRegTable[tmp]);
		}
		else
		{
			returnValue = ILLEGALREGQADDRESS;
		}
		break;
				
		case SECRET_MODULE://��Ϊ����������5���Ĵ���ΪWO  
					
					//FCWong @2011.05.12 Ӧ���µ��ڲ��Ĵ�����
					/*	if (((Address >= START_SECRET_REG) && (Address < ADZERO_ADJUST_REG))
					|| ((Address > SYSTEM_DEFAULT_REG) && (Address <= END_SECRET_REG)))
					{
					tmp = Address - START_SECRET_REG;
					*ValuePtr = *(ModbusSecretRegTable[tmp]);
					}
					else
					{
					returnValue = ILLEGALREGQADDRESS;
					}
					*/
					//FCWong @2011.05.12 Ӧ���µ��ڲ��Ĵ�����
					//FCWong @2011.05.12 �ж���Щ�Ĵ�����ֻд��
			if (Address == 65500 || ((Address >=ADZERO_ADJUST_REG) && (Address <= SYSTEM_COEFF_C_REG)) )
			{
				returnValue = ILLEGALREGQADDRESS;
			}
			else
			{
				tmp = Address - START_SECRET_REG;
				*ValuePtr = *(ModbusSecretRegTable[tmp]);
			}
			break;
					
		case   DEVICE_INFORMATION_MODULE:
			if ((Address >= START_DeviceInfor_REG) && (Address <= END_DeviceInfor_REG))
			{
				tmp=Address - START_DeviceInfor_REG;
				*ValuePtr = *(ModbusDeviceInforTable[tmp]);
			}
			else
			{
				returnValue = ILLEGALREGQADDRESS;
			}
			break;

		case   DEVICE_INFORMATION_BACKUP_MODULE:
			if ((Address >= START_DeviceInfor_BACKUP_REG) && (Address <= END_DeviceInfor_BACKUP_REG))
			{
				tmp=Address - START_DeviceInfor_BACKUP_REG;
				*ValuePtr = *(ModbusDeviceInforBackupTable[tmp]);
			}
			else
			{
				returnValue = ILLEGALREGQADDRESS;
			}
			break;
					
		default :
			returnValue = ILLEGALREGQADDRESS;
			break;
					
	}
	return returnValue;
}
/************************************************************************************
������:	       MB_SetRegValue

�������:	Address:׼��д�����ݵĵ�ַ
                            value:׼��д�����ݵ�ֵ
                            
�������:	��

����ֵ:		��

����˵��:	�����Ӧ����?�ұ������������ݼ�ʹ��������ҲҪ��CASE����г���?
**************************************************************************************/
unsigned char  MB_SetRegValue(unsigned int Address,unsigned int value,E_MODBUSREGMODULE moduleFlag) 
{
	unsigned char returnValue = SUCCESSFULLCOMMAND;
	//unsigned char temp8;
	unsigned int tmp ;
	switch(moduleFlag)
	{
	case DEVICE_SET_MODULE:
		tmp = Address - START_SET_REG;			/*��Χ�Ѿ���飬ֱ��д��*/
		*ModbusParamsRegTable[tmp] = value;
		switch (tmp) 
		{
		case 0:								/*�޸�PT���*/
	#if !PMC_23I
			CommAppFlag |= MODIFY_DEVICE_PARAMS;
	#if PMC_23M
			calculate_scales();
	#endif
	#endif
		break;
	
		case 1:				/*�޸�CT���*/
	#if !PMC_23V
			CommAppFlag |= MODIFY_DEVICE_PARAMS;
	#if PMC_23M
			calculate_scales();
	#endif
	#endif
			break;
	
		case 2:			/*���߷�ʽ*/
	#if !PMC_23I
			CommAppFlag |= MODIFY_DEVICE_PARAMS;
	#endif
			break;

		case 3:
		case 4:
		case 5:
			CommAppFlag|= MODIFY_COM_PARAMS;
			//CommAppFlag |= MODIFY_DEVICE_PARAMS;	/*һ���ʼ��*/
			//g_ADUData[0].address = params.siteid;
			break;
			#if 0
			#if _MSP430F449_H
			while((U0TCTL & BIT0) != BIT0);			/*���ͻ�����UTXBUF��������ȴ�*/
			#elif _MSP430F4616_H
			while((UCA0STAT & UCBUSY) == UCBUSY)	/*�ȴ��������*/
			{
				;
			}
			#else 
			while((UCA1STAT & UCBUSY) == UCBUSY)	/*�ȴ��������*///2010.8.4
			{
				;
			}
			#endif
			UARTInit(0);
			#endif
		case 6:			/*��������*/
	#if PMC_23M
			CommAppFlag |= MODIFY_DEVICE_PARAMS;	
	#endif
			break;
		case 7:			/*���ڹ���*/
	#if PMC_23M
			CommAppFlag |= MODIFY_DEVICE_PARAMS;	
	#endif
			break;
			
	
		case 8:
		case 9:
		case 10:
	#if !PMC_23V
			CommAppFlag |= MODIFY_DEVICE_PARAMS;
	#endif
			break;
		
		case 11:			/*����*/
			if (backlight_sign)
			{
				backlight_time_count = 0;
			}
			write_BLightTimeOut();
			break;
		////////////////////////////////////////////////////////////////	

		case 400:
	#if PMC_23M
			if (value == 0xFF00)
			{
				clear_energy();
				CommAppFlag |= MODIFY_DEVICE_PARAMS;	
			}
	#endif
			break;
	
		default:
			break;
		}
		break;

	case ENERGY_MODULE:
		if (Address <= START_ENERGY_REG + 9) //���ܼĴ�����8���Ĵ�������ΪRO
		{  
			tmp = Address - START_ENERGY_REG;
			*ModbusEnergyRegTable[tmp] = value;
				
			switch(Address - START_ENERGY_REG)
			{
				
			case 1:
			case 3:
			case 5:
			case 7:
			case 9:
				CommAppFlag |= MODIFY_ENERGY_PARAMS;
				break;
			default:
				break;
			}
		}
		break;
                        
	case SECRET_MODULE:	
		tmp = Address - START_SECRET_REG;
		if ((((Address < START_SECRET_REG + 12) && (Address >= START_SECRET_REG ) )|| 
			((Address >= ADZERO_ADJUST_REG) && (Address < START_SECRET_REG + 311))))/*ע��65000û�������︳ֵ*/
		{
			if( Address != START_SECRET_REG )			/*65000�Ȳ���ֵ*/
			{
				*ModbusSecretRegTable[tmp] = value;
			}
		}	
		switch(tmp)
		{
		#if !PMC_23V
		case 3:				/*��������5A/1A*/
			if( secret_params.amp_scale == 0)		/*5A*/
			{
				secret_params.feature &= ~BIT0;		/*����������*/
			}
			else
			{
				secret_params.feature |= BIT0;		/*1A*/
			}
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		case 5:				/*�����ż�ֵ*/
		case 61:				/*������ʽ*/
		case 62:				/*�ֶε�1����ֵ*/
		case 63:
		case 64:
		case 65:
		case 66:
		case 88:				/*�ֶε�1�Ȳ�ϵ��*/
		case 89:
		case 90:
		case 97:
		case 98:
		case 99:
		case 106:
		case 107:
		case 108:
		case 115: 	
		case 116:	
		case 117:
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		case 79:				/*�ֶε�0�ıȲ�ϵ��*/
			#if PMC_23M
			do_powercal(0);
			#endif
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;	
		case 80:
			#if PMC_23M
			do_powercal(1);
			#endif
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		case 81:
			#if PMC_23M
			do_powercal(2);
			#endif
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		#endif
			
		#if !PMC_23I
		case 6:			/*��ѹ�ż�ֵ*/
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		case 70:			/*�ֶε�0��ѹϵ��*/
			#if PMC_23M
			do_powercal(0);
			#endif
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		case 71:
			#if PMC_23M
			do_powercal(1);
			#endif
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		case 72:
			#if PMC_23M
			do_powercal(2);
			#endif
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		#endif
			
		#if PMC_23M
		
		case 82:			/*�ֶε�0�Ĺ��ʲ�������*/
		case 84:
		case 86:
		case 91:			/*�ֶε�1*/
		case 93:
		case 95:
		case 100:
		case 102:
		case 104:
		case 109:
		case 111:
		case 113:
		case 118:	
		case 120:	
		case 122:	
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		#endif
			
		case 0:   	
			if( value !=secret_params.feature )			/*д��ֵ����ǰ��һ���Ž��и�ֵ*/
			{
				#if !PMC_23V
				if( (value&BIT0) == 0 )/*5A����ѡ�ͣ���ֹ��1Aѡ���޸Ķ�������Χ������*/
				{
					secret_params.amp_scale = 0;	/*5A,�޸ĵ���ѡ��*/
					if(  params.amp_rate > 6000 )
					{
						params.amp_rate = 6000;		/*����������Ϊ���ֵ*/
					}
					else if(  params.amp_rate < 1 )
					{
						params.amp_rate = 1;		/*����������Ϊ��Сֵ*/
					}
				}
				else
				{
					secret_params.amp_scale = 1;	/*1A,�޸ĵ���ѡ��*/
				}
				#endif	
				if( ( HAS_COM  && (( value & BIT2) ==0) )  || (( !HAS_COM ) && ( value & BIT2 )) )
				{
					comChange = 1;		/*��ͨ��λ�ı䣬����һ��־����hook�н��д���*/
				}
				*ModbusSecretRegTable[tmp] = value;			/*���ж��ٸ�ֵ*/
				CommAppFlag |= MODIFY_FEATURE_PARAMS;	/*��ʾ�޸���������*/
			}
			break;
			
		case 1:
		case 2:
			CommAppFlag |= MODIFY_SECRET_PARAMS;	/*���к�*/
			//SnUpdateDeviceInfor();
			break;
			
		case 27:
			if (value == 0xFF00)
			{
				adzero_adjust = 1;			/*���У׼*/
			}
			break;
			
		case 29:							/*ϵͳ��λ*/
			if (value == 0xFF00)
			{
				System_Reset = TRUE;
			}
			break;
		case 31:							/*У׼ϵ���ָ�Ĭ��*/
			if (value == 0xff00)
			{
				Cal_Coeff_Reset = TRUE;
			}
			break;
		case 32:							/*��ͨ�����ָ�Ĭ��*/
			if (value == 0xff00)
			{
				Common_Default = TRUE;
			}
			break;
		case 33:							/*A��У׼ϵ���ָ�Ĭ��*/
			if (value == 0xff00)
			{
				Cal_Coeff_Reset_A= TRUE;
			}
			break;
		case 34:							/*B��У׼ϵ���ָ�Ĭ��*/
			if (value == 0xff00)
			{
				Cal_Coeff_Reset_B= TRUE;
			}
			break;
		case 35:							/*C��У׼ϵ���ָ�Ĭ��*/
			if (value == 0xff00)
			{
				Cal_Coeff_Reset_C= TRUE;
			}
			break;
		case 30:
			if (value == 0xff00)
			{
				System_Default = TRUE;
			}
			break;
			//FCWong @2011.05.12 Ӧ���µ��ڲ��Ĵ�����
		case 300:
			if (value == 0xFF00)
			{
				calibrateStartFlag = 0x55;
				///////////////////////////////////////////////////////////////
				//FCWong @2011.06.13 
				//�״ν��룬��FirstTimesFlag����ɷ�ֹǰһ��У׼ʧ�ܺ���������
				//�����ǰһ��У׼���ڽ����У��������׼Դ��װ������
				//��ͬ������У׼����
				
				FirstTimesFlag = 1;
				//////////////////////////////////////////////////////////////////
			}
			break;           
		default:
			
			break;
		}
		break;
			
	default:
		break;
	}
	return returnValue;
}
/************************************************************************************
������:	       MB_CheckRegValue

�������:	Address:׼��д�����ݵĵ�ַ
                            value:׼��д�����ݵ�ֵ
                            
�������:	��

����ֵ:		��

����˵��:	���д���ֵ�Ƿ��ڷ�Χ֮��,�����Ӧ�����
**************************************************************************************/
unsigned char  MB_CheckRegValue(unsigned int Address,unsigned int value,E_MODBUSREGMODULE moduleFlag) //Ԥ���Ĵ���д���κ�ֵ������ȷ��
{
	unsigned char         flag = TRUE;
	//unsigned char         temp8;
	//static unsigned char  tempYear;
	//static unsigned char  tempMonth;
	//static unsigned long  temp_high = 0;//���ڱ���32λ����2��reg���������жϷ�Χ
	switch(moduleFlag)
	{
	case DEVICE_SET_MODULE:
		if(( Address > 6011 ) &&( Address < 6400 ))		/*�м�һ���ֲ���д*/
		{
			flag = FALSE;
		}
		else
		{
		switch(Address - START_SET_REG)
		{
	#if !PMC_23I
		case  0:  
			if ((value) && (value <= 2200))	/*��5000��Ϊ2200*/
			{
				g_PT_Checkvalue = value;	   
			}
			else 
			{
				flag = FALSE;
			}  
		break;
	#endif
	#if !PMC_23V
		case 1:           
			if ((value) && (value <= (CT_MAX/ampscale[secret_params.amp_scale])))
			{
				g_CT_Checkvalue = value;	   
			}
			else 
			{
				flag = FALSE;
			}  
		break;
	#endif
		case  2:   
	#if PMC_23M
			if (value > 3)   //M�͵�ѹ���߷�ʽ4��ģʽ
			{
				flag = FALSE; 
			}
	#elif PMC_23V
			if (value > 1)   //V�͵�ѹ���߷�ʽ2��ģʽ
			{
				flag = FALSE; 
			}
	#endif
			break;

		case 3:           
			if ((value == 0) || (value > 247))
			{
				flag = FALSE; 
			}
			break;
		case  4:           
			if (value >= NUM_OF_BAUDRATES) 
			{
				flag = FALSE; 
			}
			break;
		case  5:           
			if (value > 5) 		/*��żУ��*/
			{
				flag= FALSE; 
			}
			break;

	#if PMC_23M
		case 06:			/*�������ط���*/
			if (value > 2)
			{
				flag = FALSE;
			}
			break;
		case 07:			/*���ڹ��ʷ���*/
			if (value > 1)
			{
				flag = FALSE;
			}
			break;
	#endif
			
		case 8:
		case 9:
		case 10:
			if (value > 1) /*CT�����־ֻ����0����1*/
			{
				flag = FALSE; 
			}
			break;
	
	
		case 11:
			if (value > 60)
			{
				flag = FALSE;
			}
			break;
				
	#if PMC_23M
		case 400:		/*����6400*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;	
	#endif
		default:
			break;
			}
		}
		break;			/*����ǶԲ�������ģ���break*/

	case ENERGY_MODULE:
		switch(Address)
		{
		case START_ENERGY_REG:
		case START_ENERGY_REG + 2:
		case START_ENERGY_REG + 4:	
		case START_ENERGY_REG + 6:	
		case START_ENERGY_REG + 8:	
			high_word_reg = value;					  
			break;
		case START_ENERGY_REG + 1:
		case START_ENERGY_REG + 3:
		case START_ENERGY_REG + 5:	
		case START_ENERGY_REG + 7:
		case START_ENERGY_REG + 9:
			high_word_reg = ( high_word_reg << 16 ) + value;
			if(high_word_reg > MAXKHOURS)
			{
				flag = FALSE;
			} 
			break;
		default:
			break;
		}
		break;
                        			  
	case SECRET_MODULE:/*У׼ϵ���Լ��ǶȲ���װ�����кŲ�����ΧҪ��*/
		switch(Address - START_SECRET_REG)
		{
		//////////////////////////////////////begin///////////////////////////////////
		//FCWong @2011.08.25  ���кŲ������������ƣ�ֻ��������0xFFFFFFFF������
		
		#if 0	
			/////////////////////////////////////////////////////////////	
			//FCWong @2011.08.09 ���Ӷ�SN�ĸ�ʽ�ж�
		case 1:
			temp_high = ((long)value ) << 16;
			break;
		case 2:
			temp_high = (temp_high & 0xFFFF0000) + value;
			if(((temp_high / 1000000) % 100) >12 || 
				((temp_high / 1000000) % 100) == 0)
			{
				flag = FALSE;
			}
			break;
		
			////////////////////////////////////////////////////////////////
		
		
		case 0: 				/*M��Iѡ��000~111��������*/
			#if  !PMC_23V
			if (value > 7 ) 			
			{
				flag = FALSE;
			}
			#else
			if (value > 5 ) 			/*Iѡ�Ͳ������2λΪ1*/	
			{
				flag = FALSE;
			}
			#endif
			break;
		#endif
		case 0: 				/*M��V/Iѡ��000~111��������*/
			if (value > 7 ) 			
			{
				flag = FALSE;
			}
			break;
			
		case 3:					/*����ѡ������1A/5A*/
			if ( value > 1 )
			{
				flag = FALSE;
			}	
			break;
			
		case 5:
			if (value > 200)		/*65005�����ż�ֵ*/
			{
				flag = FALSE;
			}	
			break;	
			
		case 6:				/*65006����ѹ�ż�ֵ*/
			if (value > 200)
			{
				flag = FALSE;
			}	
			break;
			
	
		case 27:					/*���У׼*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
			
		case 29:					/*ϵͳ��λ*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;

		case 30:					/*�ָ���������*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
				
		case 31:					/*У׼ϵ��ȫ���ָ�Ĭ��*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
		case 32:					/*��ͨ�����ָ�Ĭ��*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;

		case 33:					/*A��У׼ϵ���ָ�Ĭ��*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
		case 34:					/*B��У׼ϵ���ָ�Ĭ��*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
		case 35:					/*C��У׼ϵ���ָ�Ĭ��*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
		case 61:					/*������ʽ��00~11*/
			if (value > 3)
			{
				flag = FALSE;
			}
			break;
			
	#if !PMC_23I
		case 70:					/*�ֶε�0�ĵ�ѹϵ��*/
		case 71:
		case 72:
	#endif	
	#if !PMC_23V
		case 79:
		case 80:
		case 81:
	#endif
		if ( ( value > COEFF_MAX_SECTION0 ) || ( value < COEFF_MIN_SECTION0 ))
			{
				flag = FALSE;
			}
			break;
			
	#if  !PMC_23V
		case 88:
		case 89:
		case 90:
			if ( ( value > COEFF_MAX_SECTION1 ) || ( value < COEFF_MIN_SECTION1 ))
			{
				flag = FALSE;
			}
			break;

		case 97:
		case 98:
		case 99:
			if ( ( value > COEFF_MAX_SECTION2 ) || ( value < COEFF_MIN_SECTION2 ))
			{
				flag = FALSE;
			}
			break;

		case 106:
		case 107:
		case 108:
			if ( ( value > COEFF_MAX_SECTION3 ) || ( value < COEFF_MIN_SECTION3 ))
			{
				flag = FALSE;
			}
			break;

		case 115:
		case 116:
		case 117:
			if ( ( value > COEFF_MAX_SECTION4 ) || ( value < COEFF_MIN_SECTION4 ))
			{
				flag = FALSE;
			}
			break;
	#endif
	
		case 500:				/*����������־*/
			if (value == 0x55)
			{
				remote_update_flag = 1;
			}
			else
			{
				flag = FALSE;
			}
			break;
		case 300:				/*��У׼��־*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
		case 301:				/*У׼�κ�*/

			if (value > 4)
			{
				flag = FALSE;
			}
			break; 
		case 310:						/*��������*/
			if (value < 5 || value > 100)
			{
				flag = FALSE;
			}
			break;
			
		default:
			break;	
		}
		break;
		
	default:
		break;
	}
	return flag;
}
/***********************************************************************
������:			void CheckFrameBorder(unsigned int StAddr, unsigned int Quantity,E_MODBUSREGMODULE moduleFlag)

�������:		��.

�������:		��.

����ֵ:			��.

����˵��: 0x10,0x47  д����߽�ɨ�裬��ֹ��Ҫ����д������ݱ����� д�롣

��ע:��Ӧ�����
***********************************************************************/
static unsigned char  CheckFrameBorder(unsigned int StAddr, unsigned int Quantity,E_MODBUSREGMODULE moduleFlag)
{
	BlockRegStruct  *pBlockRegTable = NULL;
	unsigned char   tableRegNum = 0;//һ��ͨѶ���������д��ĸ��� 
	unsigned int    starti;
	unsigned int    endi;
	unsigned int    endAddr = StAddr + Quantity - 1;

	switch(moduleFlag)
	{
	case SECRET_MODULE:
		pBlockRegTable = SecretBlockWrRegTable;
		tableRegNum = SecretBlockNum;
		break;
	case ENERGY_MODULE:
		pBlockRegTable = EnergyBlockWrRegTable;
		tableRegNum = EnergyBlockNum; 
		break;
	default:
		return TRUE;
	}

	for (starti = 0; starti < tableRegNum; starti++)		/*��߽���*/
	{
		if (StAddr <= ((*(pBlockRegTable + starti)).regNo))
		{
			break;
		}	
	}

	if (starti > 0)				/*��ַ�ڿ��д��Χ�ڲ�*/
	{
		if (StAddr <= ((*(pBlockRegTable + starti - 1)).regNo + (*(pBlockRegTable + starti - 1)).length - 1))
		{
		    	return FALSE;
		}
	}

	for (endi = starti; endi < tableRegNum; endi++)
	{
		if(endAddr < (*(pBlockRegTable + endi)).regNo)
		{
			break;            
		}
	}

	if (endi > 0)
	{
		if (endAddr < ((*(pBlockRegTable + endi - 1)).regNo + (*(pBlockRegTable + endi - 1)).length - 1))
		{
			return FALSE;
		}
	}
	return TRUE;
}


/***********************************************************************
������:			void FwriteHook(void)

�������:		��.

�������:		��.

����ֵ:			��.

����˵��:          ���ڵ��ڽ���д����֮�����Ӧ����صĺ�������

��ע:��Ӧ�����
***********************************************************************/
static void FwriteHook(void) 
{
	if((HAS_COM)&&(((g_CommRxFlag[0]==TRUE)&&( CommAppFlag & MODIFY_COM_PARAMS ))||( CommAppFlag &MODIFY_DISPLAY_COM )))
	{
		g_ADUData[0].address = params.siteid;		/*ͨ�Ų�����ʼ��*/
		UARTInit();
		CommAppFlag &= ~ MODIFY_COM_PARAMS;
              	CommAppFlag &= ~ MODIFY_DISPLAY_COM;
		CommAppFlag |= MODIFY_DEVICE_PARAMS;	/*������Ҫ����*/
	}

	if (CommAppFlag & MODIFY_DEVICE_PARAMS)
	{
		CommAppFlag &= ~ MODIFY_DEVICE_PARAMS;
		update_params();
     	}
	if (((CommAppFlag & MODIFY_FEATURE_PARAMS) &&( g_CommRxFlag[0]==TRUE))||( CommAppFlag & MODIFY_DISPLAY_FEATURE ))
	{
		CommAppFlag &= ~ MODIFY_FEATURE_PARAMS;
		CommAppFlag &= ~ MODIFY_DISPLAY_FEATURE;
		CommAppFlag |= MODIFY_SECRET_PARAMS;
		
		if( comChange )
		{
			init_feature();
			comChange = 0;
		}
     	}
	#if PMC_23M
	if(CommAppFlag & MODIFY_ENERGY_PARAMS)
	{
		CommAppFlag &= ~ MODIFY_ENERGY_PARAMS;	
		write_energy();
	}
	#endif
	
	if (CommAppFlag & MODIFY_SECRET_PARAMS)
	{
		CommAppFlag &= ~ MODIFY_SECRET_PARAMS; 	 
		update_secret_params(); 
	}
	
	if (CommAppFlag & MODIFY_SN)
	{
		CommAppFlag &= ~ MODIFY_SN; 	 
		Read_FM24CL16_Memory(FM_SN_ADD, (unsigned char * )&snModifyBackup,4);	/*����Ϊ4*/
		if( snModifyBackup != SN_Init )		/*sn�����޸�*/
		{
			secret_params.sn = snModifyBackup;
		}
		SnUpdateDeviceInfor();
		update_secret_params(); 			/*д������*/
	}
	
	if ((System_Default)&&(g_CommRxFlag[0]==TRUE))		/*�ָ��������ã��ָ����в���*/
	{
		snModifyBackup = secret_params.sn;		/*sn����*/
		//CommAppFlag |= MODIFY_SN;		/*�޸���SN*/
		powerflag = 0;
		System_Default = 0;
		snBackupflag = SN_MODIFIED;
		#if FM
			#if !_MSP430F5418_H
		    	spi_write(POWER_FLAG_ADD,(unsigned char *)&powerflag, 2);
            		#else
			Write_FM24CL16_Memory(SN_FLAG_ADD,(unsigned char *)&snBackupflag, 2);//sn�޸ı�־
			Write_FM24CL16_Memory(FM_SN_ADD,(unsigned char *)&snModifyBackup, 4);//���ݵ�SNд������
			//Delay_XuS(100);
            		Write_FM24CL16_Memory(POWER_FLAG_ADD,(unsigned char *)&powerflag, 2);//2010.8.6
           		 #endif
		#endif
		#if FLASH
		update_params();
		#endif
		reset();
	}

	if( adzero_adjust==1 )	/*��ƯУ׼*/
	{
		adjust_adzero();
		adzero_adjust=0;
	}
	
	if((Common_Default)&&(g_CommRxFlag[0]==TRUE))		/*��ͨ�����ָ�Ĭ��*/
	{
		unsigned int i;
	
		unsigned char* addr1;
		unsigned char* addr2;
		
		addr1 = (unsigned char*)(&init_params);
		addr2 = (unsigned char*)&params;
		
		for(i = 0; i < PARAMS_CNT; i++)
		{
			*addr2 = *addr1;			
			addr1++;
			addr2++;
		}
		update_params();
		reset();				/*��λ����˲��ö�ͨ�ŵȲ������г�ʼ��*/
	}
	
	if ((Cal_Coeff_Reset)&&(g_CommRxFlag[0]==TRUE))
	{
		Cal_Coeff_Reset=0;
		init_coeff();
		update_secret_params();
		reset();
	}

	if ((Cal_Coeff_Reset_A)&&(g_CommRxFlag[0]==TRUE))
	{
		Cal_Coeff_Reset_A=0;
		phase_init_coeff( PHASE_A );
		update_secret_params();
		reset();
	}
	if ((Cal_Coeff_Reset_B)&&(g_CommRxFlag[0]==TRUE))
	{
		Cal_Coeff_Reset_B=0;
		phase_init_coeff( PHASE_B );
		update_secret_params();
		reset();
	}
	if ((Cal_Coeff_Reset_C)&&(g_CommRxFlag[0]==TRUE))
	{
		Cal_Coeff_Reset_C=0;
		phase_init_coeff( PHASE_C );
		update_secret_params();
		reset();
	}
	
	if ((System_Reset)&&(g_CommRxFlag[0]==TRUE))	/*ϵͳ��λ*/
	{
		System_Reset =0;
		reset();
	}

	if(( g_CommRxFlag[0]==TRUE )&&( remote_update_flag == 1 ))
	{
		//if((UCA1STAT & UCBUSY) != UCBUSY)    //������ת��2010.05.11���
		 {
		     	UpgradeProcess();
			remote_update_flag = 0;
		 }
	}		
}
/************************************************************************************
������:	       ReceOneChar

�������:	����ش���Ӳ��ͨ�ŵ�·�������ı���������

�������:	g_ADUData���洢��·���Ӧ�ò�����ݼ���ر�־

����ֵ:		��

����˵��:	�ɽ����жϵ��ã�ÿ����һ�ξ͵���һ��

**************************************************************************************/
void ReceOneChar(unsigned char comNum, unsigned char ReceCharacter) 
{
	if (g_CommRxFlag[comNum] == TRUE)//�����������
	{
		if (g_ENDT15Flag[comNum] == TRUE)//����ֽڼ�೬ʱ���������������
		{
			StartCounterT35(comNum);
			g_ADUData[comNum].ADULength = 0;
			return;
		}

		g_ADUData[comNum].FrameOK = FALSE;
		StartCounterT15(comNum);	  
		StartCounterT35(comNum);	  

		if (g_ADUData[comNum].ADULength < COMMBUFLEN)     /*��ָֹ������߽����*/
		{
			*(g_ADUData[comNum].ADUBuffPtr + g_ADUData[comNum].ADULength) = ReceCharacter;
			g_ADUData[comNum].ADULength++;
		}
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
	unsigned char comNum = 0;
	
	if (g_T15_num[comNum] >= 0)
	{
		if (g_T15_num[comNum] > T15DelayTime[params.baudrate])//����ַ����ʱ�䵽
		{
			g_T15_num[comNum] = -1;
			g_ENDT15Flag[comNum] = TRUE;
		}
		else
		{
			g_T15_num[comNum]++;
		}
	}

	if (g_T35_num[comNum] >= 0)
	{
		if (g_T35_num[comNum] > T35DelayTime[params.baudrate])
		{
			g_T35_num[comNum] = -1;
			g_ENDT35Flag[comNum] = TRUE;
		}
		else
		{
			g_T35_num[comNum]++;
		}
	}
	/******************************************************************************************************
	*���ý��ճ�ʱ���ṩCommProc()����֡������ɵ��жϣ�
	*��ʱ������������ֽڼ䴫����ʱ���ڶ����ǽ�����ɳ�ʱ����Ϊ������ɺ�
	*�����ٵ����ٵ���ReceOneChar()����g_T15_num,g_T35_num���㣬��Ȼ����ֳ�ʱ
	*******************************************************************************************************/
	if ((g_ENDT15Flag[comNum] == TRUE) && (g_ENDT35Flag[comNum] == TRUE))
	{
		/*һ����Ч�Ľ��պ��ø�λΪFALSE����ֹ�����ݴ���֮ǰ
		�������ݳ���ѱ��������ݽ��ջ������е�����*/
		g_CommRxFlag[comNum] = FALSE; 
		if (*(g_ADUData[comNum].ADUBuffPtr) == 0x00 && (*(g_ADUData[comNum].ADUBuffPtr + 1) == 0x10))
		{
			t_adjusttime = 0;
			CommAppFlag |= CLOCK_MODIFY_BEGIN;
		}
	}
}

/************************************************************************************
������:	       CheckCTmultiPTlimit

�������:	
              g_PT_Checkvalue:ͨ�Ž�Ҫ�޸ĵ�PT���ֵ��Ϊ0��ʾͨ��δ��PT���ֵ��Ϊ������Ϊͨ���޸ĺ��PT���ֵ
              g_CT_Checkvalu:ͨ�Ž�Ҫ�޸ĵ�CT���ֵ��Ϊ0��ʾͨ��δ��CT���ֵ��Ϊ������Ϊͨ���޸ĺ��CT���ֵ
                            
�������:	��

����ֵ:		TRUE:PTCT���������ȷ
                            FALSE:PTCT������ô���

����˵��:	�ж�PTCT��ȳ˻��Ƿ���

�޸�����:   
**************************************************************************************/
#if PMC_23M
unsigned char CheckCTmultiPTlimit()
{
	unsigned char returnFlag=TRUE;
	
	if((g_PT_Checkvalue != 0) && (g_CT_Checkvalue != 0))  	/*ͨ��ͬʱ�����˵�ѹ�����������*/
	{
		if ((unsigned long)g_PT_Checkvalue *(unsigned long)g_CT_Checkvalue > ((SCALE_MAX/ampscale[secret_params.amp_scale])/100))
		{
			returnFlag = FALSE;						
		}			
	}
	else if((g_PT_Checkvalue != 0) && (g_CT_Checkvalue == 0))         /*ͨ��ֻ������PT���*/
	{
		if ((unsigned long)g_PT_Checkvalue *(unsigned long)params.amp_rate > ((SCALE_MAX/ampscale[secret_params.amp_scale])/100))
		{
			returnFlag = FALSE;		
		}			
	}
	else if((g_PT_Checkvalue == 0) && (g_CT_Checkvalue != 0))         /*ͨ��ֻ������CT���*/
	{
		if ((unsigned long)g_CT_Checkvalue *(unsigned long)params.volt_rate > ((SCALE_MAX/ampscale[secret_params.amp_scale])/100))
		{
			returnFlag = FALSE;		
		}
	}	
	
	g_PT_Checkvalue = 0;
	g_CT_Checkvalue = 0;
	
	return returnFlag;
}

#endif
/***************************************************************************/
/* void DoPowerCalc()																		*/
/*																									*/
/* �������:		��.																*/
/*																									*/
/* �������:		��.															*/
/*																									*/
/* ����ֵ:		��.																*/
/*																									*/
/* ����������㹦�ʼ�������.						*/
/*																			*/
/***************************************************************************/
//void DoPowerCal(unsigned char p)
//{
//	cal_base.cal_coeff[p].powercal = lRound( (long)cal_base.cal_coeff[p].voltcal *
//	(long)cal_base.cal_coeff[p].ampcal,10000L);
//} 
/***********************************************************************
������:			SendResponse

�������:		

�������:		��.

����ֵ:			��.

����˵��:              
***********************************************************************/
void SendResponse(unsigned char comNum)
{
	
	ENABLE_TX();
	
#if _MSP430F449_H
	TXBUF0 = *(g_ADUData[0].ADUBuffPtr);
#elif  _MSP430F4616_H
	UCA0TXBUF = *(g_ADUData[0].ADUBuffPtr);
#else 
	UCA1TXBUF = *(g_ADUData[0].ADUBuffPtr);
#endif
	g_TxDataCtr[0] = 1;
	
#if   _MSP430F449_H
	while((U0TCTL & BIT0) != BIT0);		        /*���ͻ�����UTXBUF��������ȴ�*/
#elif _MSP430F5418_H
	//UCA1IFG = 0x00;
	while (!(UCA1IFG & UCTXIFG));
#else
	IFG2 = 0x00;                                    /*ͨѶ�жϱ�־λ���㣬����Ļ������ͨѶ��ʱ���������⡣���������ԭ���д���֤��*/
	while((UCA0STAT & UCBUSY) == UCBUSY);		/*���ͻ�����UTXBUF��������ȴ�*/
#endif        
	
#if   _MSP430F449_H
	IE1 |= UTXIE0;
#elif _MSP430F5418_H
	UCA1IE |= UCTXIE;
#else
	IE2 |= UCA0TXIE;                                /*����UCA0�����ж�*/
#endif

}

#if 0
/******************************************************************************
  ������: set_baud()

  ����: ���������ú���.

  ����: �����ʵĺ궨����

  ���: ���ò����ʿ�����

  ����ֵ: 

  ����: 
 ******************************************************************************/
void set_baud(unsigned char baud)
{
  	unsigned int ubr;
	double temp;
	unsigned char modul_val = 0;
	
  	if (baud >= NUM_OF_BAUDRATES)
  	{
		return;
  	}
	temp = (double)BRCLK / baudrate[baud];
	ubr = temp;	/*ȡ��*/
	if (ubr < 3)	/*UBR<3����պͷ��ͻᷢ������Ԥ������*/
		return;
#if AO
        #if  _MSP430F5418_H    //2010.8.4
        modul_val = (temp - ubr) * 8;
	UCA1BR0 = ubr & 0xFF;                         
	UCA1BR1 = (ubr >> 8);
	UCA1MCTL = modul_val << 1;
        #else
	modul_val = (temp - ubr) * 8;
	UCA0BR0 = ubr & 0xFF;                         
	UCA0BR1 = (ubr >> 8);
	UCA0MCTL = modul_val << 1;
        #endif
#else
	temp = temp - ubr;
        
	/*2006.6.24�޸ģ���Ӳ���������*/
	if (temp < 0.0625)
	{
		modul_val = 0x00;
	}
	else if (temp < 0.1875)
	{
		modul_val = 0x08;
	}
	else if (temp < 0.3125)
	{
		modul_val = 0x24;
	}
	else if (temp < 0.4375)
	{
		modul_val = 0x4A;
	}
	else if (temp < 0.5625)
	{
		modul_val = 0x55;
	}
	else if (temp < 0.6875)
	{
		modul_val = 0xD5;
	}
	else if (temp < 0.8125)
	{
		modul_val = 0xDB;
	}
	else if (temp < 0.9375)
	{
		modul_val = 0xDF;
	}
	else if (temp < 1)
	{
		modul_val = 0xFF;
	}


	U0BR0 = ubr & 0xFF;                         
	U0BR1 = (ubr >> 8);
	UMCTL0 = modul_val;	
#endif
}
#endif

 void UART_IO_Init()
 {
        P5SEL &=~( BIT(6)|BIT(7) );		/*IO��*/
	P5DIR|= ( BIT(6)|BIT(7) ); 		/*���*/
	P5OUT &= ~( BIT(6)|BIT(7) );  	/*�͵�ƽ*/
	
	P7SEL &= ~BIT(2);
	P7DIR |= BIT2;		
	P7OUT &= ~BIT2;
	
	UCA1IE &= ~UCRXIE;             		/*���ж�*/              
	UCA1IFG = 0x00;  
 }

/******************************************************************************
 * ������:	UARTInit 
 * ����:            ��ͨ�ſڽ�ϰ��ʼ�����ò���ʼ��������
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��־�� 
 * ��������:2008.4.1
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
#if _MSP430F449_H
void UARTInit()
{
    	UCTL0 |= SWRST;                             /*���ƼĴ���λ*/
	UCTL0 |= CHAR;
	UTCTL0 = SSEL1;                             /*ѡ����ʱ��Դ*/
	set_baud(params.baudrate);	            /*���ò�����*/
	switch (params.parity)                      /*ѡ����żУ��*/
	{
	case 3:				
		UCTL0 &= ~PENA;	            /*��У�飬1��ֹͣλ*/
		UCTL0 &= ~SPB;
		break;
		
	case 1:
		UCTL0 |= PENA;
		UCTL0 &= ~PEV;		    /*��У�飬1��ֹͣλ*/
		UCTL0 &= ~SPB;
		break;
		
	case 2:
		UCTL0 |= PENA;
		UCTL0 |= PEV;		    /*żУ�飬1��ֹͣλ*/
		UCTL0 &= ~SPB;
		break;
	case 0:				
		UCTL0 &= ~PENA;	            /*��У�飬2��ֹͣλ*/
		UCTL0 |= SPB;
		break;
		
	case 4:
		UCTL0 |= PENA;
		UCTL0 &= ~PEV;		    /*��У�飬2��ֹͣλ*/
		UCTL0 |= SPB;
		break;
		
	case 5:
		UCTL0 |= PENA;
		UCTL0 |= PEV;		    /*żУ�飬2��ֹͣλ*/
		UCTL0 |= SPB;
		break;
		
	default:
		break;
	}
	UCTL0 &= ~SWRST;                            /*ѡ����ʱ��Դ��λ*/
	ME1 |= URXE0 + UTXE0;                       /*ʹ��TXD  RXD*/

	IE1 |= URXIE0;
	IFG1 = 0x00;

	P2SEL |= 0x30;                              /*ģ��ѡ��*/
	P2DIR |= 0x50;


	ENABLE_RX();				    /*��UARTģ���ʼ��Ϊ����״̬*/

}
#elif _MSP430F4616_H
void UARTInit()
{

	UCA0CTL1 |= UCSWRST;                        /*���ƼĴ���λUSCI-reset*/
	
	UCA0CTL1 |= UCSSEL1 + UCSSEL0;              /*ѡ����ʱ��ԴSMCLK*/
	
	set_baud(params.baudrate);		    /*���ò�����*/
	switch (params.parity)                      /*ѡ����żУ��*/
	{
	case 0:				
		UCA0CTL0 &= ~UCPEN;	    /*��У��*/
		UCA0CTL0 |= UCSPB;
		break;
		
	case 1:
		UCA0CTL0 |= UCPEN;
		UCA0CTL0 &= ~UCPAR;	    /*��У��*/
		UCA0CTL0 &= ~UCSPB;
		break;
		
	case 2:
		UCA0CTL0 |= UCPEN;
		UCA0CTL0 |= UCPAR;	    /*żУ��*/
		UCA0CTL0 &= ~UCSPB;
		break;
		
	default:
		break;
	}

	UCA0CTL1 &= ~UCSWRST;
	UCA0IE |= UCRXIE;                           
	UCA0IFG = 0x00;                            
	
	P2SEL |= 0x30;                              /*ģ��ѡ��*/
	P2DIR |= 0x10;

}
#else                      
void UARTInit()
{
	UCA1CTL1 |= UCSWRST;                        /*���ƼĴ���λUSCI-reset*/
	
	UCA1CTL1 |= UCSSEL1 + UCSSEL0;              /*ѡ����ʱ��ԴSMCLK*/
	
	set_baud(params.baudrate);		    /*���ò�����*/
        
	switch (params.parity)                      /*ѡ����żУ��*/
	{ 
	case 3:				
		UCA1CTL0 &= ~UCPEN;	    /*��У�飬1��ֹͣλ*/
		UCA1CTL0 &= ~UCSPB;
		break;
		
	case 1:
		UCA1CTL0 |= UCPEN;
		UCA1CTL0 &= ~UCPAR;	    /*��У�飬1��ֹͣλ*/
		UCA1CTL0 &= ~UCSPB;
		break;
		
	case 2:
		UCA1CTL0 |= UCPEN;
		UCA1CTL0 |= UCPAR;	    /*żУ�飬1��ֹͣλ*/
		UCA1CTL0 &= ~UCSPB;
		break;
	case 0:				
		UCA1CTL0 &= ~UCPEN;	    /*��У�飬2��ֹͣλ*/
		UCA1CTL0 |= UCSPB;
		break;
		
	case 4:
		UCA1CTL0 |= UCPEN;
		UCA1CTL0 &= ~UCPAR;	    /*��У�飬2��ֹͣλ*/
		UCA1CTL0 |= UCSPB;
		break;
		
	case 5:
		UCA1CTL0 |= UCPEN;
		UCA1CTL0 |= UCPAR;	    /*żУ�飬2��ֹͣλ*/
		UCA1CTL0 |= UCSPB;
		break;
		
	default:
		break;
	}
        
    	P5SEL |= (BIT6 + BIT7);                        /*ģ��ѡ��*/
	P5DIR |= BIT6;				/*TxD�ķ���Ϊ���*/
	P7SEL &= ~BIT2;
	P7DIR |=BIT2;					/*ʹ�ܽ��պͷ��͵�����*/
	UCA1CTL1 &= ~UCSWRST;
	UCA1IE |= UCRXIE;                           
	UCA1IFG = 0x00;                            
	ENABLE_RX();					 /*����ʹ�ܿ���λP7.2*/
}
#endif
void CommInit(void)
{
	if( HAS_COM==0 )  //��ͨ��ѡ��
	{	
		UART_IO_Init();		/*�Ƶ���������Ķ�*/
	}
	else
	{
		unsigned char   comNum = 0; 
   
		g_ENDT15Flag[comNum] = FALSE;
		g_ENDT35Flag[comNum] = FALSE;
		g_T35_num[comNum]  = -1;
		g_T15_num[comNum] = -1;
		g_CommRxFlag[comNum] = TRUE;
		g_TxDataCtr[comNum] = 0;
		g_ADUData[0].address = params.siteid;
		UARTInit();
		g_ADUData[comNum].ADUBuffPtr = &g_ComBuf[comNum][0];
		g_ADUData[comNum].ADULength = 0;
		g_ADUData[comNum].FrameOK = FALSE;  
		g_PDUData[comNum].FrameOK = FALSE;
		ComInitHook();		/*һЩװ����Ϣ*/
	/*��ʼ��UARTͨ������*/
	}
}

/***********************************************************************************************
**                            �ļ�����
***********************************************************************************************/



