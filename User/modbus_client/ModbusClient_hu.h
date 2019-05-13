/********************************************************************************
*  版权所有: 深圳市中电电力技术有限公司				
*
*  文件名:       ModbusClient.h
*
*
*  当前版本:                       0.1
*  
*  作者:                                  曾伟
*
*  完成日期:  
*
*
*  取代版本:    无
*                 
*  作者:    无
*
*  完成日期:     无
*
*
*  描述: 
*
*  其它: 由于51的中断函数都需要分配到COMMON，因此将MODBUS模块的物理层接收
*                函数放到COMM.C中形成一个单独的源文件，在其他应用中可以将COMM.C
*                中的代码导入本模块
*
*  历史: 
*                1. 
*                2. 
********************************************************************************/

#ifndef _ModbusClient_H
#define _ModbusClient_H
/*********************************************************************************
*                                     ModbusClient模块配置常量
***********************************************************************************/

#define COMM_NUM		   BD_USART_NUM        /*通信口的数目，单通信口*/
#define COMMBUFLEN		  257       /*通信缓冲区字节*/


/*********************************************************************************
*                                      ModbusClient模块常量
**********************************************************************************/

   /*KEIL 的编译机制决定了其可重入函数都要单独申明,同时根据其编译器对
      函数的编译特性(手法太笨拙了),可重入函数,递归函数,函数指针所指向的
      函数及其调用的函数都需要申明为可重入,否则会带来很多问题,单为了保证
      程序的通用性,所以将通信的函数指针改为SWITCH语句调用*/

#define	   SUCCESSFULLCOMMAND		        00                              /*成功的MODBUS命令*/
#define    ILLEGALfUNCTIONCODE		        01                           	/*不正确的功能码*/ 
#define    ILLEGALREGQADDRESS		        02                           	/*不正确的寄存器地址*/
#define    ILLEGALREGQATITY			03                           	/*不正确的寄存器数量*/ 
#define    FAILREGOPERATOR			04                              /*失败的寄存器操作*/
#define    ILLEGALFRAMELENGTH			03                              /*非法的数据帧长度*/    
#define    ILLEGALREGVALUE			03                         	/*非法的桢值*/    

#define    WRITEREG_COMMAND			0x10
#define    READREG_COMMAND			0x03
#define    WRITESECRATEREG_COMMAND		0x47 
#define    READSECRATEREG_COMMAND		0x46
#define    WRITESINGLECOIL_COMMAND		0x05 

#define MODIFY_DEVICE_PARAMS       	        BIT1		                /*装置参数修改*/
#define MODIFY_SECRET_PARAMS       	        BIT2		                /*秘密寄存器参数修改*/
#define MODIFY_CAL_PARAMS                 	BIT3		                /*计算系数修改*/
#define MODIFY_COM_PARAMS                	BIT4		                /*通讯参数修改*/
#define MODIFY_DISPLAY_COM		        BIT5                            /*面板修改通信参数*/
#define MODIFY_ENERGY_PARAMS		        BIT6                            /*电能底值修改*/

#define MODIFY_SN		                BIT7           

#define CLOCK_MODIFY_END		        BIT8
#define CLOCK_MODIFY_BEGIN		        BIT9
//#define MODIFY_FEATURE_PARAMS		        BITA
//#define MODIFY_DISPLAY_FEATURE		        BITB	/*面板修改特征值*/

/*********************************************************************************
*                                       ModbusClient模块数据类型
**********************************************************************************/

typedef  struct  _CommREG_Struct      /*用于整块写命令时边界判断，例如防止32BIT数只写了16BIT*/
{
    unsigned int 	regNo;                           /*寄存器点号*/
    unsigned char  	length;                          /*寄存器所占的字(16BIT)的个数*/    
}BlockRegStruct;

typedef enum _REGATTRIBUTE
{
	RW,                                              /*可读可写*/
	RO,                                              /*只读*/
	WO                                               /*只写*/
}E_REGATTRIBUTE;


typedef enum _MODBUSREGMODULE
{
	NONE_MODULE,
	DATA_MODULE,
	ENERGY_MODULE,                                   /*电能模块*/ 
        //THD_MODULE,                                      /*谐波模块 2010.07.30*/
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
      unsigned int   ModuleStartAddress;                 /*点表开始地址*/
      unsigned int   ModuleEndAdress;	                 /*点表结束地址*/
      E_REGATTRIBUTE      ModuleAttribute;           	 /*模块属性*/	  
      E_MODBUSREGMODULE   ModuleFlag;                	 /*模块标志*/
}ComModuelStruct;


/*******************************/
/*链路层加应用层数据*/
/***********************  ******/
typedef struct __ADU_Struct
{
        unsigned char 	  address;
        unsigned char	  *ADUBuffPtr;                     /*指向的地址需要有与系统不冲突的空间容纳数据*/
        unsigned int   	  ADULength;
        unsigned char     FrameOK;
}ADU_Struct;


/******************/
/*应用层数据*/
/******************/
typedef struct __PDU_Struct
{
   unsigned char     	FunctionCode;
   unsigned char   	*PDUBuffPtr;                     /*指向的地址需要有与系统不冲突的空间容纳数据*/
   unsigned char	PDULength;
   unsigned char     	FrameOK;
   unsigned char     	ExceptionCode;
}PDU_Struct;

typedef struct _PDUResponse_Struct
{
	unsigned char       	*PDUDataPtr;             /*指向的地址需要有与系统不冲突的空间容纳数据*/                             
	unsigned char       	PDUByteLength;
	unsigned char         	ExceptionCode;
}PDUResponse_Struct;

typedef struct _FunctionArray_Struct
{
   unsigned char    functionCode;
   void   (*Function)(PDUResponse_Struct *Response);
}FunctionArray_Struct;

typedef  struct  _DeviceCommInformation_Struct           /*用于整块写命令时边界判断，例如防止32BIT数只写了16BIT*/
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
*                                     ModbusClient模块全局变量
**********************************************************************************/
extern  ADU_Struct    g_ADUData[COMM_NUM];  
extern  PDU_Struct    g_PDUData[COMM_NUM]; 

extern  unsigned char g_CommRxFlag[COMM_NUM];             /*注意需要初使化为TRUE，方便上电时接收使能*/
extern  unsigned int  g_TxDataCtr[COMM_NUM];              /*发送数据发送个数计数*/
extern  unsigned char delayed_baud;                       /*表示该类数据须等到通讯响应后才更新*/

extern unsigned int  adzero_adjust;                    /*调零标志*/
extern unsigned char  flag_yk;

extern unsigned int   g_adjust_millisecond;
extern unsigned int   *const ModbusDeviceInforTable[];
extern unsigned int   CommAppFlag;
/********************************************************************************************************
*                                          ModbusClient模块接口函数
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
**                            文件结束
********************************************************************************************************/

