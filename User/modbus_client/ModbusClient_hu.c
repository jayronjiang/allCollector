/********************************************************************************
*  版权所有: 深圳市中电电力技术有限公司				
*
*  文件名:       ModbusClient.c
*
*
*  当前版本:                       0.1
*  
*  作者:                                  曾伟
*
*  完成日期:  
*
*
*  取代版本:                       无
*                 
*  作者:    无
*
*  完成日期:                       无
*
*
*  描述: 
*                           
*                           一类(与应用相关):MB_GetRegValue,MB_SetRegValue,FReadHook，FwriteHook，SecrateRegPreDeal
                                                                       write_tou_quarter, write_tou_date，DoPowerCal,CheckFrameBorad
*                           
*                          二类(与物理层相关):CommCfgPort，UARTInit，SendResponse,UART0_ISR(Comm.c),UART1_ISR(Comm.c)
*
*                           三类:与功能相关函数。其他函数
*
*
*  其它: 
*
*  历史: 
*                1. 
*                2. 
********************************************************************************/

/*
*********************************************************************************************************
*                                         包含文件
*********************************************************************************************************
*/
#include "Includes.h"

/*
*********************************************************************************************************
*                                   静态函数
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
                                  静态全局变量
*********************************************************************************************************/
DeviceCommInformationStruct  g_DevicComInfor;
ADU_Struct   g_ADUData[COMM_NUM];  
PDU_Struct   g_PDUData[COMM_NUM]; 
unsigned char g_CommRxFlag[COMM_NUM];   /*接收允许标志，注意需要初使化为TRUE，方便上电时接收使能*/
									/*发送中断中只为false，因此它可以用来表明本次数据是否发送完毕*/
unsigned int  g_TxDataCtr[COMM_NUM];              /*发送数据发送个数计数*/
unsigned char delayed_baud = FALSE;		  /*延迟设置波特率*/

//uint8     g_CommRxFlag;           /*注意需要初使化为TRUE，方便上电时接收使能*/

/**************************************************************************************************/
/************************以下是与功能相关的全局静态函数***************************/
/**************************************************************************************************/
/*功能码及功能码对应的处理函数*/
const  FunctionArray_Struct g_FCodeHandleArray[] =          
{
	{READREG_COMMAND,FReadHoldingReg},
	{WRITEREG_COMMAND,FWriteCommonReg},
	{READSECRATEREG_COMMAND,FReadSecretReg},
	{WRITESECRATEREG_COMMAND,FWriteSecretReg},
};
/*程序中所含的功能码数量*/
#define MAX_FUNCTION_NUM	sizeof(g_FCodeHandleArray) / sizeof(FunctionArray_Struct)      

//static unsigned char 	g_ENDT15Flag[COMM_NUM];    /*注意需要赋初值为FALSE*/
//static unsigned char 	g_ENDT35Flag[COMM_NUM];    /*注意需要赋初值为FALSE*/
//static signed char	g_T35_num[COMM_NUM];       /*注意需要赋初值为-1*/
//static signed char	g_T15_num[COMM_NUM];       /*注意需要赋初值为-1*/
static unsigned char	g_ComBuf[COMM_NUM][COMMBUFLEN];  /*为通信分配缓存*/
//const unsigned char	T15DelayTime[NUM_OF_BAUDRATES] = {24,12,6,3,2};
//const unsigned char	T35DelayTime[NUM_OF_BAUDRATES] = {46,23,12,6,3};

//unsigned int g_PT_Checkvalue = 0;   	/*通信将要修改的PT变比值，为0表示通信未改，为其它则为通信修改后的PT变比值*/
//unsigned int g_CT_Checkvalue = 0;         /*通信将要修改的CT变比值，为0表示通信未改，为其它则为通信修改后的CT变比值*/

/**************************************************************************************************/
/************************以下是与应用相关的全局静态函数***************************/
/**************************************************************************************************/

/* --------------------------------------------  实时数据寄存器---------------------------------------- */
const unsigned int blank = 0;
/* MODBUS数据寄存器点表*/
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
	(int *)&vars.total.kvar + 1,			/*0038,ΣKVAR*/
	(int *)&vars.total.kvar,
	(int *)&vars.phase[0].kva + 1,				
	(int *)&vars.phase[0].kva,				
	(int *)&vars.phase[1].kva + 1,
	(int *)&vars.phase[1].kva,
	(int *)&vars.phase[2].kva + 1,
	(int *)&vars.phase[2].kva,
	(int *)&vars.total.kva + 1,		/*0046,ΣKVA*/
	(int *)&vars.total.kva,				
	(int *)&vars.phase[0].pf,				
	(int *)&vars.phase[1].pf,
	(int *)&vars.phase[2].pf,
	(int *)&vars.total.pf,			/*0051,ΣPF*/
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

#define START_DATA_REG	        0000	                                     /*数据寄存器开始地址*/
#define END_DATA_REG	        (START_DATA_REG + MBUS_DATA_REG_NUM - 1)     /*数据寄存器结束地址*/

int* const ModbusEnergyRegTable[] =				             /* MODBUS电能寄存器点表*/
{
	#if PMC_23M
    	(int *)&energy.kwhr.khour + 1,	/*0200,正向有功*/
	(int *)&energy.kwhr.khour,
	(int *)&energy.n_kwhr.khour + 1,	/*0202，反向有功*/
	(int *)&energy.n_kwhr.khour,	
	(int *)&energy.kvarhr.khour + 1,	/*0204，正向无功*/
	(int *)&energy.kvarhr.khour,
	(int *)&energy.n_kvarhr.khour + 1,		/*0206，反向无功*/
	(int *)&energy.n_kvarhr.khour,
	(int *)&energy.kvahr.khour + 1,		/*0208，视在功率*/
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

#define     START_ENERGY_REG       200                  /*电能寄存器开始地址，不能写出0200，这个是8进制了*/
#define     END_ENERGY_REG         (START_ENERGY_REG + MBUS_ENERGY_REG_NUM - 1)             /*电能寄存器结束地址*/

	#if 0
unsigned int* const ModbusThdRegTable[] =				        /* Thd寄存器点表*///2010.07.30
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

#define     START_THD_REG        40718                              /*谐波寄存器开始地址*/
#define     END_THD_REG          (START_THD_REG + MBUS_THD_REG_NUM - 1) /*谐波寄存器结束地址*/
	#endif

/* --------------------------------------------  实时数据寄存器---------------------------------------- */

/* --------------------------------------------  参数设置寄存器---------------------------------------- */
/******************装置参数寄存器点表**********************************/
unsigned char  clr_energy_Flag=FALSE;
//unsigned char  clr_soe_Flag=FALSE;
/* MODBUS参数寄存器列表*/
int *const ModbusParamsRegTable[] =	/*参数寄存器对应变量指针*/
{	
	#if !PMC_23I
	(int *)&params.volt_rate,		/*6000，PT变比*/
	#else
	(int *)&blank,
	#endif
	#if !PMC_23V
	(int *)&params.amp_rate,   		/*6001，CT变比*/
	#else
	(int *)&blank,		
	#endif
	#if !PMC_23I
	(int *)&params.inmode,   		/*6002，接线方式*/
	#else
	(int *)&blank,
	#endif
			
	(int *)&params.siteid,		/*6003，设备ID*/
	(int *)&params.baudrate,	/*6004，波特率*/
	(int *)&params.parity,		/*6005，奇偶校验*/

	#if PMC_23M
	(int *)&params.pfmode,		/*功率因素符号*/	/*6006*/
	(int *)&params.kvamode,	/*视在功率方法*/	/*6007*/
	#else
	(int *)&blank,
	(int *)&blank,	
        #endif
	
	#if !PMC_23V
	(int *)&params.CTReverseFlag[0],   /*A相电流方向*/	/*6008*/
	(int *)&params.CTReverseFlag[1],   /*B*/
	(int *)&params.CTReverseFlag[2],   /*C*/
	#else
	(int *)&blank,	
	(int *)&blank,	
	(int *)&blank,	
	#endif
		
	(int *)&BackLightTime,		/*6011背光时间*/
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

#define START_SET_REG	        6000           /*参数设置寄存器开始地址*/
#define PARITY_SET_REG    		6005
#define CLEAR_ENERGY_REG    	6400		/*从清除电度为WO型寄存器*/	
#define END_SET_REG		(START_SET_REG + sizeof(ModbusParamsRegTable) / sizeof(int *) - 1)        	/*参数设置寄存器结束地址*/

#define ENDRO_SET_REG    6011		/*6012开始不能读*/
//#define CLEAR_ENERGY_REG	        6400           /*参数设置寄存器开始地址*/
/* --------------------------------------------  参数设置寄存器---------------------------------------- */


/* --------------------------------------------  装置信息寄存器------------------------------------------ */

unsigned int * const ModbusDeviceInforTable[] =                           /* 装置信息寄存器点表*/
{
	(unsigned int *)&g_DevicComInfor.deviceType[0],		/*9800，装置型号*/
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
	(unsigned int *)&g_DevicComInfor.softVersion,		/*9820，程序版本号*/
	(unsigned int *)&g_DevicComInfor.protocolVersion,	/*9821，规约版本号*/
	(unsigned int *)&g_DevicComInfor.softYear,		/*9822，当前版本日期，年*/
	(unsigned int *)&g_DevicComInfor.softMonth,
	(unsigned int *)&g_DevicComInfor.softDate,
    	
    	/* SN修改为只占两个寄存器@ stone @    2010.12.02 */
	
    	(unsigned int *)&secret_params.sn + 1,	/*9825序列号高位*/
	(unsigned int *)&secret_params.sn,	
	(unsigned int *)&blank,
	(unsigned int *)&blank,

	(unsigned int *)&secret_params.feature,  /* 9829，特征码 */

	#if !PMC_23V
    	(unsigned int *)&secret_params.amp_scale,	/*9830，电流额定值*/
    	#else
	(unsigned int *)&blank,		
   	 #endif
};

#define     START_DeviceInfor_REG	9800               /*装置信息寄存器开始地址*/
#define     END_DeviceInfor_REG		(START_DeviceInfor_REG + sizeof(ModbusDeviceInforTable) / sizeof(int *) - 1)      /*装置信息寄存器结束地址*/
/* ------------------------------------------  装置信息寄存器----------------------------------------- */

/* --------------------------------------------  装置信息寄存器备份------------------------------------------ */

unsigned int * const ModbusDeviceInforBackupTable[] =                           /* 装置信息寄存器点表备份*/
{
	(unsigned int *)&g_DevicComInfor.deviceType[0],		/*60200，装置型号*/
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
	(unsigned int *)&g_DevicComInfor.softVersion,		/*60220，程序版本号*/
	(unsigned int *)&g_DevicComInfor.protocolVersion,	/*60221，规约版本号*/
	(unsigned int *)&g_DevicComInfor.softYear,		/*60222，当前版本日期，年*/
	(unsigned int *)&g_DevicComInfor.softMonth,
	(unsigned int *)&g_DevicComInfor.softDate,
    	
    	/* SN修改为只占两个寄存器@ stone @    2010.12.02 */
	
    	(unsigned int *)&secret_params.sn + 1,	/*60225序列号高位*/
	(unsigned int *)&secret_params.sn,	
	(unsigned int *)&blank,
	(unsigned int *)&blank,

	(unsigned int *)&secret_params.feature,  /* 60229，特征码 */

	#if !PMC_23V
    	(unsigned int *)&secret_params.amp_scale,	/*60230，电流额定值*/
    	#else
	(unsigned int *)&blank,		
   	 #endif
};

#define     START_DeviceInfor_BACKUP_REG	60200               /*装置信息寄存器开始地址*/
#define     END_DeviceInfor_BACKUP_REG		(START_DeviceInfor_BACKUP_REG + sizeof(ModbusDeviceInforTable) / sizeof(int *) - 1)      /*装置信息寄存器结束地址*/
/* ------------------------------------------  装置信息寄存器备份结束----------------------------------------- */

#if 0
/* --------------------------------------------  SOE 记录寄存器------------------------------------------ */
/*SOE记录寄存器未用点表*/

#define     START_SOE_REG               42000                                /*SOE寄存器开始地址*/
#define     END_SOE_REG                 (START_SOE_REG + 5 * SOE_BUFFER_NUM - 1)  /*SOE寄存器结束地址*/


/* --------------------------------------------  SOE 记录寄存器------------------------------------------ */


/* -----------------------------------------  装置时间命令寄存器----------------------------------------- */

#define BROADCAST_ADDRESS		0                                   /*modbus广播地址*/
#define ADJUST_TIME_START_REG	        60000                               /*广播对时命令起始地址*/
#define ADJUST_TIME_REGNUM		4                                   /*广播对时命令寄存器数目*/   //2010.8.10 
#define ADJUST_TIME_END_REG		(ADJUST_TIME_START_REG + ADJUST_TIME_REGNUM - 1)/*广播对时命令结束地址*/

/*--------------------------------------2010.9.19增加UNIX时间寄存器---------------------------------------*/
#define ADJUST_UNIXTIME_START_REG	ADJUST_TIME_START_REG + 4   
#define ADJUST_UNIXTIME_REGNUM		2
#define ADJUST_UNIXTIME_END_REG		(ADJUST_UNIXTIME_START_REG + ADJUST_UNIXTIME_REGNUM - 1)

/* ------------------------------------------  装置时间命令寄存器---------------------------------------- */

#endif
/* ------------------------------------------  秘密寄存器------------------------------------------------ */
unsigned int	adzero_adjust = 0;                                           /*调零标志*/
unsigned int 	Cal_Coeff_Reset = 0;                                         /*计算系数复位*/
unsigned int 	Cal_Coeff_Reset_A = 0;                                         /*A相计算系数复位*/
unsigned int 	Cal_Coeff_Reset_B = 0;                                         /*B相计算系数复位*/
unsigned int 	Cal_Coeff_Reset_C = 0;                                         /*C相计算系数复位*/

//FCWong @2011.11.28 修复恢复出厂设置时，只恢复秘密寄存器。
//unsigned int   Secret_Default = 0;                                          /*秘密寄存器所有参数恢复默认并复位*/
unsigned int   Common_Default = 0; 								//出厂设置

unsigned int   System_Reset = 0;                                            /*系统复位*/
unsigned int   System_Default = 0;                                          /*参数恢复初始上电状态*/

unsigned int    g_adjust_millisecond = 0;                                    /*广播对时的毫秒保存*/

unsigned long  high_word_reg = 0;		                             /*用于双字寄存器设置时保存高位字*/



int *const ModbusSecretRegTable[] =                                          /*秘密寄存器对应变量的指针*/ 
{	

	/*******************************************************************************
	FCWong @2011.05.11
	根据内部寄存器新的顺序编排，重新排列此数组。
	********************************************************************************/

	(int *)&secret_params.feature,	/*65000，特征码*/
	(int *)&secret_params.sn + 1,	/*65001，序列号*/
	(int *)&secret_params.sn,

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.amp_scale,		/*65003*/
	#else
	(int *)&blank,
	#endif	
	
	(int *)&blank,						/*65004，预留*/

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.amp_level,			/*65005，电流门槛值*/
	#else
	(int *)&blank,
	#endif

	#if PART_PMC_23V_SIGN
	(int *)&secret_params.volt_level,			/*65006，电压门槛值*/
	#else
	(int *)&blank,
	#endif
	
	(int *)&blank,      
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,		/*65011，预留*/

	#if PART_PMC_23V_SIGN
	(int *)&secret_params.volt_adzero[0],          	/*65012，A相电压通道零点值*/          
	(int *)&secret_params.volt_adzero[1],   
	(int *)&secret_params.volt_adzero[2],   		/*65014，A相电压通道零点值*/   
	#else	
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23I_SIGN
	(int *)&secret_params.amp_adzero[0],			/*65015，A相电压通道零点值*/
	(int *)&secret_params.amp_adzero[1],
	(int *)&secret_params.amp_adzero[2],			/*65017，A相电压通道零点值*/
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

	
	(int *)&adzero_adjust,    	/*65027，零点校准*/
	(int *)&blank,
	(int *)&System_Reset, 		/*65029，系统复位*/
	(int *)&System_Default,	/*65030，恢复出厂默认*/
	(int *)&Cal_Coeff_Reset, 	/*65031，校准系数恢复默认*/
	(int *)&Common_Default,	/*65032，普通参数恢复默认*/
	(int *)&Cal_Coeff_Reset_A, 	/*65033，A相校准系数恢复默认*/
	(int *)&Cal_Coeff_Reset_B, 	/*65034，B相校准系数恢复默认*/
	(int *)&Cal_Coeff_Reset_C, 	/*65035，C相校准系数恢复默认*/
    
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
	(int *)&secret_params.I_section_flag,		/*65061，补偿方式*/
	(int *)&secret_params.I_subsection[0],		/*65062，分段点0电流*/
	(int *)&secret_params.I_subsection[1],
	(int *)&secret_params.I_subsection[2],
	(int *)&secret_params.I_subsection[3],
	(int *)&secret_params.I_subsection[4],		/*65066，分段点4电流*/
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
	(int *)&secret_params.voltcal[0],       /*65070，分段点0的电压系数*/             
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
	(int *)&secret_params.Ampcal[0].phs[0].ampcal,		/*65079，分段点0的A相电流系数*/	
	(int *)&secret_params.Ampcal[0].phs[1].ampcal,
	(int *)&secret_params.Ampcal[0].phs[2].ampcal,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif
	
	#if PART_PMC_23M_SIGN
	(int *)&secret_params.Ampcal[0].phs[0].pf_radian_compensate+1,	/*65082，补偿弧度值*/
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
	(int *)&secret_params.Ampcal[1].phs[0].ampcal,		/*65088，分段点1的A相比差系数*/
	(int *)&secret_params.Ampcal[1].phs[1].ampcal,
	(int *)&secret_params.Ampcal[1].phs[2].ampcal,
	#else
	(int *)&blank,
	(int *)&blank,
	(int *)&blank,
	#endif

	#if PART_PMC_23M_SIGN
	(int *)&secret_params.Ampcal[1].phs[0].pf_radian_compensate+1,	/*65091，分段点1的A相功率补偿弧度*/
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
	(int *)&secret_params.Ampcal[2].phs[0].ampcal,	/*65097，分段点2开始*/	
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
	(int *)&secret_params.Ampcal[3].phs[0].ampcal,			/*65106，分段点3开始*/
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
	(int *)&secret_params.Ampcal[4].phs[0].ampcal,			/*65115，分段点4开始*/
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
	
	(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,(int *)&blank,	/*65124开始*/
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

	(int *)&calibrateStartFlag,		/*65300，启动校准*/
	(int *)&operatingSection,		/*65301，当前校准的段号*/
	(int *)&standardVars.vln + 1,		/*65302，标准源电压*/
    	(int *)&standardVars.vln,
	(int *)&standardVars.amps + 1,
    	(int *)&standardVars.amps,
	(int *)&standardVars.angle + 1,	/*65306标准源相位夹角*/
    	(int *)&standardVars.angle,
	(int *)&precisionRequest.volt,	/*65308电压电流精度要求*/
	(int *)&precisionRequest.power,
	//FCWong @2011.06.10 增加校准数据采集次数变量,使用此保留变量的地址
	//(int *)&precisionRequest.powerFactor,   //此变量未使用，只是预留此变量
	(int *)&calibrateAccTimes,	/*65310，校准采样次数*/
	
	
	(int *)&calibrateSuccessfulFlag + 1,	/*65311，校准结果*/
	(int *)&calibrateSuccessfulFlag,
	(int *)&checkError.voltError[0],		/*65313，A相电压误差*/
	(int *)&checkError.voltError[1],
	(int *)&checkError.voltError[2],
	(int *)&checkError.ampError[0],		/*65316，A相电流误差*/
	(int *)&checkError.ampError[1],
	(int *)&checkError.ampError[2],
	(int *)&checkError.PError[0],			/*65319，A相有功误差*/
	(int *)&checkError.PError[1],
	(int *)&checkError.PError[2],
	(int *)&checkError.QError[0],			/*65322，A相无功误差*/
	(int *)&checkError.QError[1],
	(int *)&checkError.QError[2],
	(int *)&checkError.SError[0],			/*65325，A相视在功率误差*/
	(int *)&checkError.SError[1],
	(int *)&checkError.SError[2],
	(int *)&checkError.PFError[0],			/*65328，A相功率因素误差*/
	(int *)&checkError.PFError[1],
	(int *)&checkError.PFError[2],
	
};

#define START_SECRET_REG	65000      		/*秘密数据寄存器开始地址*/

#define ADZERO_ADJUST_REG	65027	/*AD调零的地址*/	
#define SYSTEM_DEFAULT_REG      65032		/*系统参数恢复默认*/
#define SYSTEM_COEFF_A_REG      65033	/*A相校准参数恢复默认*/
#define SYSTEM_COEFF_B_REG      65034
#define SYSTEM_COEFF_C_REG      65035	/*C相参数恢复默认*/

#define END_SECRET_REG		(START_SECRET_REG + sizeof(ModbusSecretRegTable) / sizeof(int *) - 1)  

#define COEFF_MIN_SECTION0	8000		/*电压电流分段点0的最小系数*/
#define COEFF_MAX_SECTION0	15000		/*电压电流分段点0的最小系数*/

#define COEFF_MIN_SECTION1	5000		/*电流分段点1的最小系数*/
#define COEFF_MAX_SECTION1	15000		/*电流分段点1的最大系数*/

#define COEFF_MIN_SECTION2	5000		/*电流分段点2的最小系数*/
#define COEFF_MAX_SECTION2	15000		/*电流分段点2的最大系数*/

#define COEFF_MIN_SECTION3	5000		/*电流分段点3的最小系数*/
#define COEFF_MAX_SECTION3	15000		/*电流分段点3的最大系数*/

#define COEFF_MIN_SECTION4	5000		/*电流分段点4的最小系数*/
#define COEFF_MAX_SECTION4	15000		/*电流分段点4的最大系数*/

/* --------------------------------------------  秘密寄存器------------------------------------------ */


/*******************点表与寄存器分配结束*******************************/

/*可读写(03,10)功能码MODBUS点表分块*/
const ComModuelStruct  ModuleTable[] =                         
{
	{START_DATA_REG, END_DATA_REG, RO,DATA_MODULE},  
	{START_ENERGY_REG,END_ENERGY_REG,RW,ENERGY_MODULE},
	//{START_THD_REG,END_THD_REG,RO,THD_MODULE},   //2010.07.30 属性为只读
	{START_SET_REG,END_SET_REG,RW,DEVICE_SET_MODULE},
	//{START_SOE_REG,END_SOE_REG,RO,SOE_MODULE},
	//{ADJUST_TIME_START_REG,ADJUST_TIME_END_REG,RW,ADJUST_TIME_MODUBLE},
	//{ADJUST_UNIXTIME_START_REG,ADJUST_UNIXTIME_END_REG,RW,ADJUST_UNIX_TIME_MODUBLE},  //2010.9.19
	//{ADJUST_TIME_START_REG,ADJUST_UNIXTIME_END_REG,RW,ADJUST_BOTH_TIME_MODUBLE},
	{START_DeviceInfor_REG,END_DeviceInfor_REG,RO,DEVICE_INFORMATION_MODULE},
	{START_DeviceInfor_BACKUP_REG,END_DeviceInfor_BACKUP_REG,RO,DEVICE_INFORMATION_BACKUP_MODULE},	/*信息寄存器备份*/
};
#define MODULENUM  sizeof(ModuleTable) / sizeof(ComModuelStruct)

/*定义需要整块写入的寄存器点号及数量*/
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
	{START_SECRET_REG + 1 , 2},		/*65001，序列号*/
	{START_SECRET_REG + 82 , 2},	/*65082，分段点0，A相功率补偿*/
	{START_SECRET_REG + 84 , 2},
	{START_SECRET_REG + 86 , 2},
	{START_SECRET_REG + 91 , 2},	/*65091，分段点1*/
	{START_SECRET_REG + 93 , 2},
	{START_SECRET_REG + 95 , 2},
	{START_SECRET_REG + 100 , 2},	/*分段点2*/
	{START_SECRET_REG + 102 , 2},
	{START_SECRET_REG + 104 , 2},
	{START_SECRET_REG + 109 , 2},	/*分段点3*/
	{START_SECRET_REG + 111 , 2},	
	{START_SECRET_REG + 113 , 2},
	{START_SECRET_REG + 118 , 2},	/*分段点4*/
	{START_SECRET_REG + 120 , 2},
	{START_SECRET_REG + 122 , 2},
	{START_SECRET_REG + 302 , 2},
	{START_SECRET_REG + 304 , 2},
	{START_SECRET_REG + 306 , 2},
	{START_SECRET_REG + 311 , 2},
	
};
#define SecretBlockNum  sizeof(SecretBlockWrRegTable) / sizeof(BlockRegStruct)

//unsigned long high_word_reg;/*用于双字寄存器设置时保存高位字*/

/* 通信修改参数，越限设点及时钟标志*/
unsigned int CommAppFlag = 0;
/* CRC 高位字节值表 */
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
/* CRC低位字节值表*/
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
  unsigned char uchCRCHi = 0xFF ;               /* 高CRC字节初始化 */
  unsigned char uchCRCLo = 0xFF ;               /* 低CRC 字节初始化 */
  unsigned char uIndex;                         /* CRC循环中的索引 */
  while (len--)                                 /* 传输消息缓冲区 */
  {
      uIndex = uchCRCHi ^ * (pBuffer++);        /* 计算CRC */
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
 * 函数名:	ComInitHook 
 * 描述: 
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:huhc
 * 创建日期:2008.7.14
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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
 * 函数名:	Comm_Proc 
 * 描述: -通讯处理函数,主程序中调用,主要处理RS 485口数据,
 *				   若是本装置的数据则进入解包处理
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Comm_Proc(void)
{
	uint8_t err = ERR_OK;
	USART_LIST i = BD1_UART;

	/*注意不能改成i < pc_com[PC_USART_NUM], 数组溢出*/
	for (i = bd_com[0]; i <= bd_com[BD_USART_NUM-1]; i++)
	{
		if (UARTBuf[i].RecFlag)		                      //RS485口有数据
		{
			// 上位机过来的是主站轮询信息
			if (i == PC_UART)
			{
			UARTBuf[i].RecFlag = 0;		//接收数据已处理，清除相关标志
			ModbusLinkDisp(i);                /*modbus 链路层处理*/
			ModbusAppDisp(i);                 /*Modbus应用层处理*/
			params_modify_deal();
				}

			#if 0
			
			ProtocolBuf[i].pTxBuf = UARTBuf[i].TxBuf;         //地址置换
			ProtocolBuf[i].pRxBuf = UARTBuf[i].RxBuf;
			ProtocolBuf[i].RxLen = UARTBuf[i].RxLen;
			ProtocolBuf[i].TxLen = 0;
			//UARTBuf[i].RxLen = 0;		//已经被读取到ProtocolBuf0.RxLen, 尽快清0

			modbus_rtu_process(&ProtocolBuf[i], DevParams.Address);	/*MODBUS通信协议处理*/

			UARTBuf[i].TxLen = ProtocolBuf[i].TxLen;  /*置换回来，处理物理层数据*/
			if(UARTBuf[i].TxLen >0)
			{
				message_send_printf(TRANS_UART, i, TRUE, TRANS_MSG);
				//Rs485StartSend();
			}
			Delay_clk(50);
			UARTBuf[i].RxLen = 0;	        /*接收数据已处理，清除相关标志*/
			}

			
			err = message_process(i);		//通信协议处理
			if (err == TRANS_REQ)							// 需要透传的
			{
				message_send_printf(TRANS_UART, i, TRUE, TRANS_MSG);
			}

			UARTBuf[i].TxLen = ProtocolBuf[i].TxLen;  //置换回来，处理物理层数据
			if(UARTBuf[i].TxLen >0)
			{
				/*回复B/C信息给上位机*/
				message_send_printf(i, i,FALSE, 0xFF);
			}
			Delay_Ms(5);				// 稍微有点延时,可以不要

			/*放在括号内,只有收到新的信息才操作*/
			if (err == ERR_OK)
			{
				params_modify_deal();		//后续的数据改变处理
			}
		#endif
			else
			{
				// 轮询从站的回复
			}
	
		}
	}
}
/***********************************************************************
函数名:	void ModbusLinkDisp(void)

输入参数:无.

输出参数:无.

返回值:	无.

功能说明:     链路层处理函数   ,主要对CRC校验和地址进行校验
***********************************************************************/
void ModbusLinkDisp(unsigned char comNUm)
{
	unsigned int RedundancyCheck;           /*临时存放通讯帧crc校验和*/
	unsigned char  FramCheck = FALSE;       /*通讯帧有效标志*/
	
	//g_ENDT15Flag[comNUm] = FALSE;
	//g_ENDT35Flag[comNUm] = FALSE;

	//g_T35_num[comNUm]  = -1;
	//g_T15_num[comNUm]  = -1;

	/*链路层 数据处理*/
	g_ADUData[comNUm].FrameOK = FALSE; 
	/************************************************************************* 
	*FCWong @2011.05.30
	*多机联机时，不与上位机通信的装置会收到随机的数据，以零作判断时，
	*当接收数据的长度为小于3，而id又正确，则计算crc的地址偏移时便会发生溢出。
	*Modbus中最小的的数据帧长度为8，因此此处改为大于7作判断。
	**************************************************************************/
	//if (( g_ADUData[comNUm] .ADULength > 0) && ( g_ADUData[comNUm] .ADULength < COMMBUFLEN))
	if (( g_ADUData[comNUm] .ADULength > 7) && ( g_ADUData[comNUm] .ADULength < COMMBUFLEN))
	{
		if (*g_ADUData[comNUm].ADUBuffPtr == g_ADUData[comNUm].address)
		{
			FramCheck = TRUE;
		}

		if(FramCheck == TRUE  )/*帧CRC检测有效*/
		{ 
			RedundancyCheck = *(g_ADUData[comNUm] .ADUBuffPtr + g_ADUData[comNUm] .ADULength - 2) |
				                  (*(g_ADUData[comNUm] .ADUBuffPtr + g_ADUData[comNUm] .ADULength - 1) << 8);
           	 	if (RedundancyCheck == get_crc16(g_ADUData[comNUm] .ADULength - 2,g_ADUData[comNUm] .ADUBuffPtr))
			{
				g_ADUData[comNUm] .FrameOK = TRUE;
				return;
			}
		}
	}		/*链路层 数据处理结束*/
}
/***********************************************************************
函数名:			void ModbusAppDisp(void)

输入参数:		无.

输出参数:		无.

返回值:			无.

功能说明:           应用层处理函数   
***********************************************************************/
void ModbusAppDisp(unsigned char comNum)
{ 
	if (g_ADUData[comNum].FrameOK)             /*CRC校验正确切且帧为合法桢*/
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
			g_PDUData[comNum].PDULength = 2;//构造异常响应帧
		}
		g_PDUData[comNum].FrameOK = TRUE;
	}

	if (g_PDUData[comNum].FrameOK)
	{
		PackPDU2ADU(comNum);
		
		if (*(g_ADUData[comNum].ADUBuffPtr) != 0)//如果不是广播命令
		{
			SendResponse(comNum);//这个函数相当于启动发送函数，发送第一个数据
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
函数名:	              PackADU2PDU

输入参数:		无

输出参数:	      	g_ADUData、g_PDUData

返回值:			无

函数说明:	       将ADU数据包中与链路层相关的部分剥离
***************************************************************************/
void PackADU2PDU(unsigned char comNum)//剥离的部分包括地址与CRC校验码
{
	g_ADUData[comNum].FrameOK =FALSE;
	g_PDUData[comNum].PDUBuffPtr = g_ADUData[comNum].ADUBuffPtr + 1;		/*去掉地址码*/
	g_PDUData[comNum].PDULength = g_ADUData[comNum].ADULength - 3;	/*去掉校验码*/
	g_PDUData[comNum].FunctionCode = *g_PDUData[comNum].PDUBuffPtr;	/*缓存帧从功能码开始*/
	g_PDUData[comNum].FrameOK = TRUE;						
	g_ADUData[comNum].ADULength = 0;
}

/************************************************************************************
函数名:	       FunctionHandle

输入参数:	无

输出参数:	g_PDUData

返回值:		无

函数说明:	根据功能码调用与功能码相配套的功能码处理函数

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
	g_PDUData[comNum].ExceptionCode = ILLEGALfUNCTIONCODE;		/*没有正确返回，不正确的功能码*/
}

/**************************************************************************
函数名:	              PackPDU2ADU

输入参数:		无

输出参数:	      	g_ADUData、g_PDUData

返回值:			ADU缓冲区数据长度

函数说明:	       将PDU数据包打包上链路层相关部分数据，
                                   形成完整的发送数据包
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
函数名:	       FReadHoldingReg

输入参数:	Response，准备返回的PDU数据结构体指针

输出参数:	Response指针指向的g_PDU_Response变量

返回值:		无

函数说明:	0X03读保持寄存器所对应的函数

**************************************************************************************/
void FReadHoldingReg(PDUResponse_Struct *Response) 
{
	unsigned char *PDUPtr;
	unsigned int StAddr;//03命令帧起始地址
	unsigned int Quantity;//03命令帧读取寄存器个数
	unsigned char i;

	E_MODBUSREGMODULE moduleFlag;//寄存器的类型(枚举变量)

	if (Response->PDUByteLength != 5)/*读寄存器的帧长度错误，直接返回*/
	{
		Response->ExceptionCode = ILLEGALFRAMELENGTH;
		return;
	}

	PDUPtr = Response->PDUDataPtr;
	StAddr  = PDUPtr[1] << 8 | PDUPtr[2];
	Quantity = PDUPtr[3] << 8 | PDUPtr[4];
	Response->PDUByteLength = 2 + Quantity * 2;//这个长度不包括地址和CRC校验码
	
	moduleFlag = NONE_MODULE;
	for (i = 0; i < MODULENUM; i++)/*求出需要操作的寄存器模块*/
	{
		if((StAddr >= ModuleTable[i].ModuleStartAddress) && ((StAddr+Quantity - 1) <= ModuleTable[i].ModuleEndAdress))
		{
			moduleFlag = ModuleTable[i].ModuleFlag;
			break;
		}
	}
	
	if ((Quantity >= 0x001) && (Quantity <= 0x007d))	//寄存器个数小于125个
	{
		if(moduleFlag != NONE_MODULE)
		{
			Response->ExceptionCode = ReadHoldingReg(PDUPtr + 2,StAddr,Quantity,moduleFlag);
			
			if (Response->ExceptionCode == SUCCESSFULLCOMMAND)
			{
				*(PDUPtr + 1) = Quantity * 2;//返回字节的个数	                         
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
函数名:	       FWriteMultipleReg

输入参数:	Response:

输出参数:	Response指向的相关PDU结构体参数

返回值:		无

函数说明:	0x10，写多个寄存器

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
	
	moduleFlag= NONE_MODULE;/*求出需要操作的寄存器模块*/
	for (i = 0; i < MODULENUM; i++)
	{
		if((StAddr >= ModuleTable[i].ModuleStartAddress) && (ModuleTable[i].ModuleAttribute != RO)	  /*RO的在这里就排除了*/
			&& ((StAddr + Quantity - 1) <= ModuleTable[i].ModuleEndAdress))
		{
			moduleFlag = ModuleTable[i].ModuleFlag;
			break;
		}
	}
	
	if ((CheckFrameBorder(StAddr,Quantity,moduleFlag) == FALSE) || ((Response->PDUByteLength - 6) != ByteCount))/*接收的数据长度与数据帧中标称的数据长度不等*/
	{
		Response->ExceptionCode = ILLEGALFRAMELENGTH;
		return;
	}
	
	Response->PDUByteLength = 5;
	
	if ((Quantity >= 0x001) && (Quantity <= 0x007d) && (ByteCount == (Quantity * 2)))
	{
		if(moduleFlag != NONE_MODULE)
		{
		//	if((StAddr <= PARITY_SET_REG) && (StAddr + Quantity - 1) >= PARITY_SET_REG)//校验方式改为可写
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
	/*写完参数后的钩子函数，用于参数写完后应用所需要的一些相关处理*/
	//FwriteHook();                    
}
/************************************************************************************
函数名:	       FReadSecretReg

输入参数:	Response，准备返回的PDU数据结构体指针

输出参数:	Response指针指向的g_PDU_Response变量

返回值:		无

函数说明:	0X46读秘密寄存器所对应的函数

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
		/*如果读取的是秘密寄存器或者DIDO配置寄存器*/
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
函数名:	       FWriteSecretReg

输入参数:	Response:

输出参数:	Response指向的相关PDU结构体参数

返回值:		无

函数说明:	0x47，写秘密寄存器

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
	
	//秘密寄存器也有1块边界检查
	if ((FALSE == CheckFrameBorder(StAddr,Quantity,SECRET_MODULE)) || ((Response->PDUByteLength - 6) != ByteCount))
	{
		Response->ExceptionCode = ILLEGALFRAMELENGTH;
		return;
	}
	
	Response->PDUByteLength = 5;
	if ((Quantity >= 0x001) && (Quantity <= 0x007d) && (ByteCount == (Quantity * 2)))
	{
		//FCWong @2011.05.12 使用新的秘密寄存器表，重新判断只读块
		//秘密寄存器有一块属于只读，需剔出//65000~65021
		//if ((StAddr > START_SECRET_REG + 5) && ((StAddr + Quantity - 1) <= END_SECRET_REG) | (StAddr == 0xFFDC))
		if((( ((StAddr < START_SECRET_REG + 12)&&(StAddr >=START_SECRET_REG)) || 
			((StAddr > START_SECRET_REG + 17) && (StAddr < START_SECRET_REG + 311))) && 
			((StAddr + Quantity - 1) <= (START_SECRET_REG + 311)))||(StAddr == 0xFFDC))		/*加上StAddr >=START_SECRET_REG，防止读小地址也成功*/
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
 * 函数名:	FWriteSingleCoil 
 * 描述: 
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2008.4.2
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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
函数名:	       ReadHoldingReg

输入参数:	DataPtr:读出数据保存地址
                            StAddr:命令读取的开始地址
                            Quantity:读取的寄存器数量
                            moduleFlag:命令读取的寄存器模块名称 
输出参数:	无
返回值:		TRUE   读寄存器成功
                             FALSE 读寄存器失败

函数说明:	读保持寄存器所对应的函数

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
函数名:	       WriteHoldingReg

输入参数:	KeepDataPtr:准备写入数据的指针
                            StAddr:准备写入数据的地址
                            Quantity:准备写入的寄存器数量
输出参数:	无

返回值:		写入成功返回TRUE，错误为FALSE

函数说明:	写多个寄存器

**************************************************************************************/
unsigned char WriteHoldingReg(unsigned char *KeepDataPtr,unsigned int StAddr,unsigned int Quantity, E_MODBUSREGMODULE moduleFlag) 
{
	unsigned char    i;
	unsigned char    wrFlag;
	unsigned int     Value;
	unsigned char    returnValue = SUCCESSFULLCOMMAND;
	
	for (i = 0; i < Quantity; i++)//首先检查写入数据是否在允许范围内
	{
		Value = (*(KeepDataPtr + i * 2) << 8)|(*(KeepDataPtr + i * 2 + 1));
		if (MB_CheckRegValue(StAddr + i,Value,moduleFlag) == FALSE)
		{
			return ILLEGALREGVALUE;
		}
	}
	
	#if PMC_23M
	if (FALSE == CheckCTmultiPTlimit())		/*比较特殊，检查PT变比*CT 变比是否在限制范围内*/
	{
		return ILLEGALREGVALUE;
	}  
	#endif
	
	/*写入有效数据*/
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
函数名:	       MB_GetRegValue

输入参数:	ValuePtr:读取寄存值保存的地址
                            Address: 寄存器地址                          

输出参数:	将相关保持寄存器的值复制到DataPtr指向的区域

返回值:		读成功返回TRUE，失败则为FALSE

函数说明:	读取保持寄存器的值，并传递给PDU

备注:跟应用相关,且保留数据区数据即使不做操作也要在CASE语句中出现
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
		
	case DEVICE_SET_MODULE://清除soe后面的寄存器为WO     			
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
				
		case SECRET_MODULE://因为参数点表后面5个寄存器为WO  
					
					//FCWong @2011.05.12 应用新的内部寄存器表
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
					//FCWong @2011.05.12 应用新的内部寄存器表
					//FCWong @2011.05.12 判断哪些寄存器是只写的
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
函数名:	       MB_SetRegValue

输入参数:	Address:准备写入数据的地址
                            value:准备写入数据的值
                            
输出参数:	无

返回值:		无

函数说明:	与具体应用相?且保留数据区数据即使不做操作也要在CASE语句中出现?
**************************************************************************************/
unsigned char  MB_SetRegValue(unsigned int Address,unsigned int value,E_MODBUSREGMODULE moduleFlag) 
{
	unsigned char returnValue = SUCCESSFULLCOMMAND;
	//unsigned char temp8;
	unsigned int tmp ;
	switch(moduleFlag)
	{
	case DEVICE_SET_MODULE:
		tmp = Address - START_SET_REG;			/*范围已经检查，直接写入*/
		*ModbusParamsRegTable[tmp] = value;
		switch (tmp) 
		{
		case 0:								/*修改PT变比*/
	#if !PMC_23I
			CommAppFlag |= MODIFY_DEVICE_PARAMS;
	#if PMC_23M
			calculate_scales();
	#endif
	#endif
		break;
	
		case 1:				/*修改CT变比*/
	#if !PMC_23V
			CommAppFlag |= MODIFY_DEVICE_PARAMS;
	#if PMC_23M
			calculate_scales();
	#endif
	#endif
			break;
	
		case 2:			/*接线方式*/
	#if !PMC_23I
			CommAppFlag |= MODIFY_DEVICE_PARAMS;
	#endif
			break;

		case 3:
		case 4:
		case 5:
			CommAppFlag|= MODIFY_COM_PARAMS;
			//CommAppFlag |= MODIFY_DEVICE_PARAMS;	/*一起初始化*/
			//g_ADUData[0].address = params.siteid;
			break;
			#if 0
			#if _MSP430F449_H
			while((U0TCTL & BIT0) != BIT0);			/*发送缓冲区UTXBUF有数据则等待*/
			#elif _MSP430F4616_H
			while((UCA0STAT & UCBUSY) == UCBUSY)	/*等待发送完毕*/
			{
				;
			}
			#else 
			while((UCA1STAT & UCBUSY) == UCBUSY)	/*等待发送完毕*///2010.8.4
			{
				;
			}
			#endif
			UARTInit(0);
			#endif
		case 6:			/*功率因素*/
	#if PMC_23M
			CommAppFlag |= MODIFY_DEVICE_PARAMS;	
	#endif
			break;
		case 7:			/*视在功率*/
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
		
		case 11:			/*背光*/
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
		if (Address <= START_ENERGY_REG + 9) //电能寄存器第8个寄存器后面为RO
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
			((Address >= ADZERO_ADJUST_REG) && (Address < START_SECRET_REG + 311))))/*注意65000没有在这里赋值*/
		{
			if( Address != START_SECRET_REG )			/*65000先不赋值*/
			{
				*ModbusSecretRegTable[tmp] = value;
			}
		}	
		switch(tmp)
		{
		#if !PMC_23V
		case 3:				/*电流配置5A/1A*/
			if( secret_params.amp_scale == 0)		/*5A*/
			{
				secret_params.feature &= ~BIT0;		/*更新特征码*/
			}
			else
			{
				secret_params.feature |= BIT0;		/*1A*/
			}
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		case 5:				/*电流门槛值*/
		case 61:				/*补偿方式*/
		case 62:				/*分段点1电流值*/
		case 63:
		case 64:
		case 65:
		case 66:
		case 88:				/*分段点1比差系数*/
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
		case 79:				/*分段点0的比差系数*/
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
		case 6:			/*电压门槛值*/
			CommAppFlag |= MODIFY_SECRET_PARAMS;
			break;
		case 70:			/*分段点0电压系数*/
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
		
		case 82:			/*分段点0的功率补偿弧度*/
		case 84:
		case 86:
		case 91:			/*分段点1*/
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
			if( value !=secret_params.feature )			/*写入值和以前不一样才进行赋值*/
			{
				#if !PMC_23V
				if( (value&BIT0) == 0 )/*5A电流选型，防止从1A选型修改而来，范围超出了*/
				{
					secret_params.amp_scale = 0;	/*5A,修改电流选型*/
					if(  params.amp_rate > 6000 )
					{
						params.amp_rate = 6000;		/*超出，设置为最大值*/
					}
					else if(  params.amp_rate < 1 )
					{
						params.amp_rate = 1;		/*超出，设置为最小值*/
					}
				}
				else
				{
					secret_params.amp_scale = 1;	/*1A,修改电流选型*/
				}
				#endif	
				if( ( HAS_COM  && (( value & BIT2) ==0) )  || (( !HAS_COM ) && ( value & BIT2 )) )
				{
					comChange = 1;		/*若通信位改变，则置一标志，在hook中进行处理*/
				}
				*ModbusSecretRegTable[tmp] = value;			/*先判断再赋值*/
				CommAppFlag |= MODIFY_FEATURE_PARAMS;	/*表示修改了特征码*/
			}
			break;
			
		case 1:
		case 2:
			CommAppFlag |= MODIFY_SECRET_PARAMS;	/*序列号*/
			//SnUpdateDeviceInfor();
			break;
			
		case 27:
			if (value == 0xFF00)
			{
				adzero_adjust = 1;			/*零点校准*/
			}
			break;
			
		case 29:							/*系统复位*/
			if (value == 0xFF00)
			{
				System_Reset = TRUE;
			}
			break;
		case 31:							/*校准系数恢复默认*/
			if (value == 0xff00)
			{
				Cal_Coeff_Reset = TRUE;
			}
			break;
		case 32:							/*普通参数恢复默认*/
			if (value == 0xff00)
			{
				Common_Default = TRUE;
			}
			break;
		case 33:							/*A相校准系数恢复默认*/
			if (value == 0xff00)
			{
				Cal_Coeff_Reset_A= TRUE;
			}
			break;
		case 34:							/*B相校准系数恢复默认*/
			if (value == 0xff00)
			{
				Cal_Coeff_Reset_B= TRUE;
			}
			break;
		case 35:							/*C相校准系数恢复默认*/
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
			//FCWong @2011.05.12 应用新的内部寄存器表
		case 300:
			if (value == 0xFF00)
			{
				calibrateStartFlag = 0x55;
				///////////////////////////////////////////////////////////////
				//FCWong @2011.06.13 
				//首次进入，将FirstTimesFlag清零可防止前一次校准失败后，重新运行
				//软件后，前一次校准还在进行中，软件、标准源与装置运行
				//不同步导致校准混乱
				
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
函数名:	       MB_CheckRegValue

输入参数:	Address:准备写入数据的地址
                            value:准备写入数据的值
                            
输出参数:	无

返回值:		无

函数说明:	检测写入的值是否在范围之内,与具体应用相关
**************************************************************************************/
unsigned char  MB_CheckRegValue(unsigned int Address,unsigned int value,E_MODBUSREGMODULE moduleFlag) //预留寄存器写入任何值都是正确的
{
	unsigned char         flag = TRUE;
	//unsigned char         temp8;
	//static unsigned char  tempYear;
	//static unsigned char  tempMonth;
	//static unsigned long  temp_high = 0;//用于保存32位数据2个reg参数数据判断范围
	switch(moduleFlag)
	{
	case DEVICE_SET_MODULE:
		if(( Address > 6011 ) &&( Address < 6400 ))		/*中间一部分不能写*/
		{
			flag = FALSE;
		}
		else
		{
		switch(Address - START_SET_REG)
		{
	#if !PMC_23I
		case  0:  
			if ((value) && (value <= 2200))	/*从5000改为2200*/
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
			if (value > 3)   //M型电压接线方式4种模式
			{
				flag = FALSE; 
			}
	#elif PMC_23V
			if (value > 1)   //V型电压接线方式2种模式
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
			if (value > 5) 		/*奇偶校验*/
			{
				flag= FALSE; 
			}
			break;

	#if PMC_23M
		case 06:			/*功率因素符号*/
			if (value > 2)
			{
				flag = FALSE;
			}
			break;
		case 07:			/*视在功率方法*/
			if (value > 1)
			{
				flag = FALSE;
			}
			break;
	#endif
			
		case 8:
		case 9:
		case 10:
			if (value > 1) /*CT反向标志只能是0或者1*/
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
		case 400:		/*清电度6400*/
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
		break;			/*这个是对参数设置模块的break*/

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
                        			  
	case SECRET_MODULE:/*校准系数以及角度补偿装置序列号不做范围要求*/
		switch(Address - START_SECRET_REG)
		{
		//////////////////////////////////////begin///////////////////////////////////
		//FCWong @2011.08.25  序列号不再作年月限制，只做不大于0xFFFFFFFF的限制
		
		#if 0	
			/////////////////////////////////////////////////////////////	
			//FCWong @2011.08.09 增加对SN的格式判断
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
		
		
		case 0: 				/*M和I选型000~111都可以设*/
			#if  !PMC_23V
			if (value > 7 ) 			
			{
				flag = FALSE;
			}
			#else
			if (value > 5 ) 			/*I选型不能设第2位为1*/	
			{
				flag = FALSE;
			}
			#endif
			break;
		#endif
		case 0: 				/*M和V/I选型000~111都可以设*/
			if (value > 7 ) 			
			{
				flag = FALSE;
			}
			break;
			
		case 3:					/*电流选型配置1A/5A*/
			if ( value > 1 )
			{
				flag = FALSE;
			}	
			break;
			
		case 5:
			if (value > 200)		/*65005电流门槛值*/
			{
				flag = FALSE;
			}	
			break;	
			
		case 6:				/*65006，电压门槛值*/
			if (value > 200)
			{
				flag = FALSE;
			}	
			break;
			
	
		case 27:					/*零点校准*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
			
		case 29:					/*系统复位*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;

		case 30:					/*恢复出厂设置*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
				
		case 31:					/*校准系数全部恢复默认*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
		case 32:					/*普通参数恢复默认*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;

		case 33:					/*A相校准系数恢复默认*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
		case 34:					/*B相校准系数恢复默认*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
		case 35:					/*C相校准系数恢复默认*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
		case 61:					/*补偿方式，00~11*/
			if (value > 3)
			{
				flag = FALSE;
			}
			break;
			
	#if !PMC_23I
		case 70:					/*分段点0的电压系数*/
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
	
		case 500:				/*在线升级标志*/
			if (value == 0x55)
			{
				remote_update_flag = 1;
			}
			else
			{
				flag = FALSE;
			}
			break;
		case 300:				/*起动校准标志*/
			if (value != 0xFF00)
			{
				flag = FALSE;
			}
			break;
		case 301:				/*校准段号*/

			if (value > 4)
			{
				flag = FALSE;
			}
			break; 
		case 310:						/*采样次数*/
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
函数名:			void CheckFrameBorder(unsigned int StAddr, unsigned int Quantity,E_MODBUSREGMODULE moduleFlag)

输入参数:		无.

输出参数:		无.

返回值:			无.

功能说明: 0x10,0x47  写命令边界扫描，防止需要整块写入的数据被单独 写入。

备注:与应用相关
***********************************************************************/
static unsigned char  CheckFrameBorder(unsigned int StAddr, unsigned int Quantity,E_MODBUSREGMODULE moduleFlag)
{
	BlockRegStruct  *pBlockRegTable = NULL;
	unsigned char   tableRegNum = 0;//一块通讯点表中整块写入的个数 
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

	for (starti = 0; starti < tableRegNum; starti++)		/*左边界检查*/
	{
		if (StAddr <= ((*(pBlockRegTable + starti)).regNo))
		{
			break;
		}	
	}

	if (starti > 0)				/*地址在块读写范围内部*/
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
函数名:			void FwriteHook(void)

输入参数:		无.

输出参数:		无.

返回值:			无.

功能说明:          用于当在进行写操作之后的与应用相关的后续处理

备注:与应用相关
***********************************************************************/
static void FwriteHook(void) 
{
	if((HAS_COM)&&(((g_CommRxFlag[0]==TRUE)&&( CommAppFlag & MODIFY_COM_PARAMS ))||( CommAppFlag &MODIFY_DISPLAY_COM )))
	{
		g_ADUData[0].address = params.siteid;		/*通信参数初始化*/
		UARTInit();
		CommAppFlag &= ~ MODIFY_COM_PARAMS;
              	CommAppFlag &= ~ MODIFY_DISPLAY_COM;
		CommAppFlag |= MODIFY_DEVICE_PARAMS;	/*参数需要保存*/
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
		Read_FM24CL16_Memory(FM_SN_ADD, (unsigned char * )&snModifyBackup,4);	/*长度为4*/
		if( snModifyBackup != SN_Init )		/*sn经过修改*/
		{
			secret_params.sn = snModifyBackup;
		}
		SnUpdateDeviceInfor();
		update_secret_params(); 			/*写入铁电*/
	}
	
	if ((System_Default)&&(g_CommRxFlag[0]==TRUE))		/*恢复出厂设置，恢复所有参数*/
	{
		snModifyBackup = secret_params.sn;		/*sn备份*/
		//CommAppFlag |= MODIFY_SN;		/*修改了SN*/
		powerflag = 0;
		System_Default = 0;
		snBackupflag = SN_MODIFIED;
		#if FM
			#if !_MSP430F5418_H
		    	spi_write(POWER_FLAG_ADD,(unsigned char *)&powerflag, 2);
            		#else
			Write_FM24CL16_Memory(SN_FLAG_ADD,(unsigned char *)&snBackupflag, 2);//sn修改标志
			Write_FM24CL16_Memory(FM_SN_ADD,(unsigned char *)&snModifyBackup, 4);//备份的SN写入铁电
			//Delay_XuS(100);
            		Write_FM24CL16_Memory(POWER_FLAG_ADD,(unsigned char *)&powerflag, 2);//2010.8.6
           		 #endif
		#endif
		#if FLASH
		update_params();
		#endif
		reset();
	}

	if( adzero_adjust==1 )	/*零漂校准*/
	{
		adjust_adzero();
		adzero_adjust=0;
	}
	
	if((Common_Default)&&(g_CommRxFlag[0]==TRUE))		/*普通参数恢复默认*/
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
		reset();				/*复位，因此不用对通信等参数进行初始化*/
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
	
	if ((System_Reset)&&(g_CommRxFlag[0]==TRUE))	/*系统复位*/
	{
		System_Reset =0;
		reset();
	}

	if(( g_CommRxFlag[0]==TRUE )&&( remote_update_flag == 1 ))
	{
		//if((UCA1STAT & UCBUSY) != UCBUSY)    //升级跳转，2010.05.11添加
		 {
		     	UpgradeProcess();
			remote_update_flag = 0;
		 }
	}		
}
/************************************************************************************
函数名:	       ReceOneChar

输入参数:	由相关串行硬件通信电路传递来的被接收数据

输出参数:	g_ADUData，存储链路层加应用层的数据及相关标志

返回值:		无

函数说明:	由接收中断调用，每接收一次就调用一次

**************************************************************************************/
void ReceOneChar(unsigned char comNum, unsigned char ReceCharacter) 
{
	if (g_CommRxFlag[comNum] == TRUE)//如果接收允许
	{
		if (g_ENDT15Flag[comNum] == TRUE)//如果字节间距超时，则放弃所有数据
		{
			StartCounterT35(comNum);
			g_ADUData[comNum].ADULength = 0;
			return;
		}

		g_ADUData[comNum].FrameOK = FALSE;
		StartCounterT15(comNum);	  
		StartCounterT35(comNum);	  

		if (g_ADUData[comNum].ADULength < COMMBUFLEN)     /*防止指针操作边界溢出*/
		{
			*(g_ADUData[comNum].ADUBuffPtr + g_ADUData[comNum].ADULength) = ReceCharacter;
			g_ADUData[comNum].ADULength++;
		}
	}
}
/**************************************************************************
函数名:	            CommTimer

输入参数:		无

输出参数:		无

返回值:			无

函数说明:	      通信定时器计时函数，由该函数来
                      维持MODBUS RTU规约中的T15和T35标志
***************************************************************************/
void CommTimer(void)
{
	unsigned char comNum = 0;
	
	if (g_T15_num[comNum] >= 0)
	{
		if (g_T15_num[comNum] > T15DelayTime[params.baudrate])//如果字符间隔时间到
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
	*利用接收超时来提供CommProc()数据帧接受完成的判断，
	*超时分两种情况，字节间传播超时，第二种是接收完成超时。因为接收完成后，
	*不会再调用再调用ReceOneChar()来对g_T15_num,g_T35_num清零，必然会出现超时
	*******************************************************************************************************/
	if ((g_ENDT15Flag[comNum] == TRUE) && (g_ENDT35Flag[comNum] == TRUE))
	{
		/*一次有效的接收后，置该位为FALSE，防止在数据处理之前
		又来数据冲掉已保存在数据接收缓冲区中的数据*/
		g_CommRxFlag[comNum] = FALSE; 
		if (*(g_ADUData[comNum].ADUBuffPtr) == 0x00 && (*(g_ADUData[comNum].ADUBuffPtr + 1) == 0x10))
		{
			t_adjusttime = 0;
			CommAppFlag |= CLOCK_MODIFY_BEGIN;
		}
	}
}

/************************************************************************************
函数名:	       CheckCTmultiPTlimit

输入参数:	
              g_PT_Checkvalue:通信将要修改的PT变比值，为0表示通信未改PT变比值，为其它则为通信修改后的PT变比值
              g_CT_Checkvalu:通信将要修改的CT变比值，为0表示通信未改CT变比值，为其它则为通信修改后的CT变比值
                            
输出参数:	无

返回值:		TRUE:PTCT变比设置正确
                            FALSE:PTCT变比设置错误

函数说明:	判断PTCT变比乘积是否超限

修改日期:   
**************************************************************************************/
#if PMC_23M
unsigned char CheckCTmultiPTlimit()
{
	unsigned char returnFlag=TRUE;
	
	if((g_PT_Checkvalue != 0) && (g_CT_Checkvalue != 0))  	/*通信同时设置了电压变比与电流变比*/
	{
		if ((unsigned long)g_PT_Checkvalue *(unsigned long)g_CT_Checkvalue > ((SCALE_MAX/ampscale[secret_params.amp_scale])/100))
		{
			returnFlag = FALSE;						
		}			
	}
	else if((g_PT_Checkvalue != 0) && (g_CT_Checkvalue == 0))         /*通信只设置了PT变比*/
	{
		if ((unsigned long)g_PT_Checkvalue *(unsigned long)params.amp_rate > ((SCALE_MAX/ampscale[secret_params.amp_scale])/100))
		{
			returnFlag = FALSE;		
		}			
	}
	else if((g_PT_Checkvalue == 0) && (g_CT_Checkvalue != 0))         /*通信只设置了CT变比*/
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
/* 输入参数:		无.																*/
/*																									*/
/* 输出参数:		无.															*/
/*																									*/
/* 返回值:		无.																*/
/*																									*/
/* 这个函数计算功率计算因子.						*/
/*																			*/
/***************************************************************************/
//void DoPowerCal(unsigned char p)
//{
//	cal_base.cal_coeff[p].powercal = lRound( (long)cal_base.cal_coeff[p].voltcal *
//	(long)cal_base.cal_coeff[p].ampcal,10000L);
//} 
/***********************************************************************
函数名:			SendResponse

输入参数:		

输出参数:		无.

返回值:			无.

功能说明:              
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
	while((U0TCTL & BIT0) != BIT0);		        /*发送缓冲区UTXBUF有数据则等待*/
#elif _MSP430F5418_H
	//UCA1IFG = 0x00;
	while (!(UCA1IFG & UCTXIFG));
#else
	IFG2 = 0x00;                                    /*通讯中断标志位清零，不清的话会出现通讯延时过长的问题。出现问题的原因有待考证。*/
	while((UCA0STAT & UCBUSY) == UCBUSY);		/*发送缓冲区UTXBUF有数据则等待*/
#endif        
	
#if   _MSP430F449_H
	IE1 |= UTXIE0;
#elif _MSP430F5418_H
	UCA1IE |= UCTXIE;
#else
	IE2 |= UCA0TXIE;                                /*允许UCA0发送中断*/
#endif

}

#if 0
/******************************************************************************
  函数名: set_baud()

  描述: 波特率设置函数.

  输入: 波特率的宏定义编号

  输出: 设置波特率控制字

  返回值: 

  其它: 
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
	ubr = temp;	/*取整*/
	if (ubr < 3)	/*UBR<3则接收和发送会发生不可预测的情况*/
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
        
	/*2006.6.24修改，添加波特率修正*/
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
        P5SEL &=~( BIT(6)|BIT(7) );		/*IO口*/
	P5DIR|= ( BIT(6)|BIT(7) ); 		/*输出*/
	P5OUT &= ~( BIT(6)|BIT(7) );  	/*低电平*/
	
	P7SEL &= ~BIT(2);
	P7DIR |= BIT2;		
	P7OUT &= ~BIT2;
	
	UCA1IE &= ~UCRXIE;             		/*关中断*/              
	UCA1IFG = 0x00;  
 }

/******************************************************************************
 * 函数名:	UARTInit 
 * 描述:            对通信口进习初始化设置并初始化波特率
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:付志武 
 * 创建日期:2008.4.1
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
#if _MSP430F449_H
void UARTInit()
{
    	UCTL0 |= SWRST;                             /*控制寄存器位*/
	UCTL0 |= CHAR;
	UTCTL0 = SSEL1;                             /*选择发送时钟源*/
	set_baud(params.baudrate);	            /*设置波特率*/
	switch (params.parity)                      /*选择奇偶校验*/
	{
	case 3:				
		UCTL0 &= ~PENA;	            /*无校验，1个停止位*/
		UCTL0 &= ~SPB;
		break;
		
	case 1:
		UCTL0 |= PENA;
		UCTL0 &= ~PEV;		    /*奇校验，1个停止位*/
		UCTL0 &= ~SPB;
		break;
		
	case 2:
		UCTL0 |= PENA;
		UCTL0 |= PEV;		    /*偶校验，1个停止位*/
		UCTL0 &= ~SPB;
		break;
	case 0:				
		UCTL0 &= ~PENA;	            /*无校验，2个停止位*/
		UCTL0 |= SPB;
		break;
		
	case 4:
		UCTL0 |= PENA;
		UCTL0 &= ~PEV;		    /*奇校验，2个停止位*/
		UCTL0 |= SPB;
		break;
		
	case 5:
		UCTL0 |= PENA;
		UCTL0 |= PEV;		    /*偶校验，2个停止位*/
		UCTL0 |= SPB;
		break;
		
	default:
		break;
	}
	UCTL0 &= ~SWRST;                            /*选择发送时钟源置位*/
	ME1 |= URXE0 + UTXE0;                       /*使能TXD  RXD*/

	IE1 |= URXIE0;
	IFG1 = 0x00;

	P2SEL |= 0x30;                              /*模块选用*/
	P2DIR |= 0x50;


	ENABLE_RX();				    /*把UART模块初始化为接收状态*/

}
#elif _MSP430F4616_H
void UARTInit()
{

	UCA0CTL1 |= UCSWRST;                        /*控制寄存器位USCI-reset*/
	
	UCA0CTL1 |= UCSSEL1 + UCSSEL0;              /*选择发送时钟源SMCLK*/
	
	set_baud(params.baudrate);		    /*设置波特率*/
	switch (params.parity)                      /*选择奇偶校验*/
	{
	case 0:				
		UCA0CTL0 &= ~UCPEN;	    /*无校验*/
		UCA0CTL0 |= UCSPB;
		break;
		
	case 1:
		UCA0CTL0 |= UCPEN;
		UCA0CTL0 &= ~UCPAR;	    /*奇校验*/
		UCA0CTL0 &= ~UCSPB;
		break;
		
	case 2:
		UCA0CTL0 |= UCPEN;
		UCA0CTL0 |= UCPAR;	    /*偶校验*/
		UCA0CTL0 &= ~UCSPB;
		break;
		
	default:
		break;
	}

	UCA0CTL1 &= ~UCSWRST;
	UCA0IE |= UCRXIE;                           
	UCA0IFG = 0x00;                            
	
	P2SEL |= 0x30;                              /*模块选用*/
	P2DIR |= 0x10;

}
#else                      
void UARTInit()
{
	UCA1CTL1 |= UCSWRST;                        /*控制寄存器位USCI-reset*/
	
	UCA1CTL1 |= UCSSEL1 + UCSSEL0;              /*选择发送时钟源SMCLK*/
	
	set_baud(params.baudrate);		    /*设置波特率*/
        
	switch (params.parity)                      /*选择奇偶校验*/
	{ 
	case 3:				
		UCA1CTL0 &= ~UCPEN;	    /*无校验，1个停止位*/
		UCA1CTL0 &= ~UCSPB;
		break;
		
	case 1:
		UCA1CTL0 |= UCPEN;
		UCA1CTL0 &= ~UCPAR;	    /*奇校验，1个停止位*/
		UCA1CTL0 &= ~UCSPB;
		break;
		
	case 2:
		UCA1CTL0 |= UCPEN;
		UCA1CTL0 |= UCPAR;	    /*偶校验，1个停止位*/
		UCA1CTL0 &= ~UCSPB;
		break;
	case 0:				
		UCA1CTL0 &= ~UCPEN;	    /*无校验，2个停止位*/
		UCA1CTL0 |= UCSPB;
		break;
		
	case 4:
		UCA1CTL0 |= UCPEN;
		UCA1CTL0 &= ~UCPAR;	    /*奇校验，2个停止位*/
		UCA1CTL0 |= UCSPB;
		break;
		
	case 5:
		UCA1CTL0 |= UCPEN;
		UCA1CTL0 |= UCPAR;	    /*偶校验，2个停止位*/
		UCA1CTL0 |= UCSPB;
		break;
		
	default:
		break;
	}
        
    	P5SEL |= (BIT6 + BIT7);                        /*模块选用*/
	P5DIR |= BIT6;				/*TxD的方向为输出*/
	P7SEL &= ~BIT2;
	P7DIR |=BIT2;					/*使能接收和发送的引脚*/
	UCA1CTL1 &= ~UCSWRST;
	UCA1IE |= UCRXIE;                           
	UCA1IFG = 0x00;                            
	ENABLE_RX();					 /*发送使能控制位P7.2*/
}
#endif
void CommInit(void)
{
	if( HAS_COM==0 )  //无通信选型
	{	
		UART_IO_Init();		/*移到这里，方便阅读*/
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
		ComInitHook();		/*一些装置信息*/
	/*初始化UART通信数据*/
	}
}

/***********************************************************************************************
**                            文件结束
***********************************************************************************************/



