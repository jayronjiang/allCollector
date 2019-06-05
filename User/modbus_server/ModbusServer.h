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

#ifndef _ModbusServer_H
#define _ModbusServer_H
/*********************************************************************************
*                                     ModbusClient模块配置常量
***********************************************************************************/
#define COMMBUFLEN		  256     /*通信缓冲区字节*/
#define BAUDNUM		          5      /*可选的波特率个数*/

#define COMM_NUM		   BD_USART_NUM        /*通信口的数目，单通信口*/
/*********************************************************************************
*                                     通信错误码
**********************************************************************************/
#define	SUCCESSFULLCOMMAND		00                            /*成功的MODBUS命令*/
#define	ILLEGALfUNCTIONCODE		01                           /*不正确的功能码*/ 
#define	ILLEGALREGQADDRESS		02                           /*不正确的寄存器地址*/
#define	ILLEGALFILEADDRESS		02                           /*不正确的文件地址*/
#define	ILLEGALREGQATITY			03                           /*不正确的寄存器数量*/ 
#define	FAILREGOPERATOR			04                            /*失败的寄存器操作*/
#define	ILLEGALFRAMELENGTH		03                            /*非法的数据帧长度*/    
#define	ILLEGALREGVALUE			03                             /*非法的桢值*/    

/*********************************************************************************
*                                   	 通信功能码
**********************************************************************************/
#define	WRITEREG_COMMAND				0x10
#define	READREG_COMMAND				0x03
#define	WRITEFILE_COMMAND			0x15
#define 	READFILE_COMMAND				0x14
#define	WRITESECRATEREG_COMMAND	0x47 
#define	READSECRATEREG_COMMAND		0x46
#define	WRITESINGLECOIL_COMMAND		0x05 
#define 	REF_TYPE_CODE   	0x06


#define	SPD_DIREAD_COMMAND			0x02		//防雷自己的读功能码为02
#define	SPD_TIMESREAD_COMMAND		0x04		//防雷自己的雷击读功能码为04


#define 	RSU_STATION_ADDRESS  	01		/*默认RSU从站地址,未使用*/
#define 	AIR_STATION_ADDRESS  	02		/*默认空调从站从站地址*/
#define 	TEMP_STATION_ADDRESS  	03		/*默认温湿度从站从站地址*/
#define 	UPS_STATION_ADDRESS  	01		/*默认UPS从站地址*/
#define 	SPD_STATION_ADDRESS	05

#define Wait_max_time  		200 		/* 发送后等待接收帧的最长时间为200ms */

/*********************************************************************************
*                                      通信设置参数标志
**********************************************************************************/
#define REAL_DATA0_SEND_FLAG    	BIT0		/*实时数据查询,传感器0*/
#define REAL_DATA1_SEND_FLAG    	BIT1		/*实时数据查询,传感器1*/
#define REAL_DATA2_SEND_FLAG    	BIT2		/*实时数据查询,传感器2*/
#define REAL_DATA3_SEND_FLAG    	BIT3		/*实时数据查询,传感器3*/

#define UPS_PARAM_SEND_FLAG		BIT4		/*USP的总体参数*/
#define UPS_IN_SEND_FLAG			BIT5		/*USP的输入侧参数*/
#define UPS_OUT_SEND_FLAG		BIT6		/*USP的输出侧参数*/
#define UPS_BAT_SEND_FLAG		BIT7		/*USP的电池参数*/
#define UPS_TIME_SEND_FLAG		BIT8		/*USP的运行时间参数*/
#define UPS_STATUS_SEND_FLAG	BIT9		/*USP的运行状态参数*/


#define SPD_STATUS_SEND_FLAG		BIT10
#define SPD_TIMES_SEND_FLAG			BIT11

#define ENVI_TEMP_SEND_FLAG         		BIT12		// 温湿度
#define ENVI_AIRCOND_ONOFF_FLAG         BIT13		// 空调参数读取
#define ENVI_AIRCOND_TEMP_FLAG         	BIT14		// 空调参数读取
#define ENVI_AIRCOND_ALARM_FLAG         BIT15		// 空调参数读取

#define DEV_PARAM_SEND_FLAG_1         	BIT16		/*参数查询*/
#define DEV_PARAM_SEND_FLAG_2         	BIT17		/*参数查询*/

#define DEV_PARAM_SET_FLAG_1          	BIT18	 // 参数设置	--空调开关机
#define DEV_PARAM_SET_FLAG_2       	BIT19	 // 参数设置	--空调温度设置

#define DOOR_OPEN_SET_FLAG          		BIT20	 // 电子锁开
#define DOOR_CLOSE_SET_FLAG          	BIT21	 // 电子锁关


/*********************************************************************************
*                                     读取相关参数起始地址
**********************************************************************************/
#define RSU_REG        		0x40				// 从第一路电压开始读
#define UPS_REG   			0x01
#define SPD_STATUS_REG   	0x00				// 防雷的输入状态从地址0开始
#define SPD_TIMES_REG   	0x0E				// 04码从0E开始读
#define AIR_ONOFF_REG             		0x0801	// 空调开关机
#define AIR_TEMP_REG             		0x0700	// 空调高温低温点

#define ENVI_TEMP_REG             					0x00		// 温湿度
#define ENVI_AIRCOND_ONOFF_REG             		0x0100	// 空调状态
#define ENVI_AIRCOND_TEMP_REG             			0x0501	// 空调温度
#define ENVI_AIRCOND_ALARM_REG             		0x0600	// 空调温度


/*********************************************************************************
*                                         等待帧回复状态
**********************************************************************************/
#define WAIT_NONE			0	/*无等待*/
#define WAIT_PARAM_SET_1		1	/*等待装置参数设置帧回复*/
#define WAIT_PARAM_SET_2		2	/*等待装置参数设置帧回复*/
// 需要参数解析的帧
#define REAL_DATA0_ANALYSE		3	/*实时数据解析,即RSU参数*/
#define REAL_DATA1_ANALYSE		4	/*实时数据解析,即RSU参数*/
#define REAL_DATA2_ANALYSE		5	/*实时数据解析,即RSU参数*/
#define REAL_DATA3_ANALYSE		6	/*实时数据解析,即RSU参数*/

#define ENVI_TEMP_ANALYSE	9	/*温湿度参数解析*/
//#define ENVI_AIR_ANALYSE		10	/*空调参数解析*/
#define ENVI_AIR_ONOFF_ANALYSE		10	/*空调参数解析*/
#define ENVI_AIR_TEMP_ANALYSE		11	/*空调参数解析*/
#define ENVI_AIR_ALARM_ANALYSE		12	/*空调参数解析*/

//#define UPS_DATA_ANALYSE	13	/*UPS参数解析*/
#define SPD_STATUS_ANALYSE	14	/*防雷参数解析*/
#define SPD_TIMES_ANALYSE	15	/*防雷参数解析*/

#define DEVICE_DATA_1_ANALYSE	16	/*装置参数读取解析,空调开关机*/
#define DEVICE_DATA_2_ANALYSE	17	/*装置参数读取解析,空调温度*/

#define UPS_PARAM_ANALYSE		18
#define UPS_IN_ANALYSE			19
#define UPS_OUT_ANALYSE			20
#define UPS_BAT_ANALYSE			21
#define UPS_TIME_ANALYSE			22
#define UPS_STATUS_ANALYSE		23

/*********************************************************************************
*                                     读取相关参数长度
**********************************************************************************/	
#define REAL_DATA_NUM		24  	/*固定为24个字节*/
//#define REAL_TIME_SOE_NUM			37  		/*读取SOE、统计信息等数据长度*/
//#define UPS_DATA_NUM 			4
#define SPD_STATUS_NUM 			0x11		// 测试软件读取了17个长度，其实是17位,共3个字节
#define SPD_TIMES_NUM 			3		// 读3个,分别为当前雷击次数,总雷击次数,最高清零的雷击次数
#define AIR_ONOFF_SET_NUM 		1 		// 空调遥控,只有1个地址
#define AIR_TEMP_SET_NUM 		4 		// 空调遥控,只有4个地址

#define ENVI_TEMP_NUM 			2
#define ENVI_AIRCOND_ONOFF_NUM 			1
#define ENVI_AIRCOND_TEMP_NUM 			3
#define ENVI_AIRCOND_ALARM_NUM 			2


#define FRAME_HEAD_NUM 			3		/*读数据时返回帧有效数据前数据个数*/
#define SET_FRAME_HEAD_NUM 		7		/*写数据时返回帧有效数据前数据个数*/

/*********************************************************************************
*                                    UPS 协议宏定义
**********************************************************************************/
/*状态有效为1, 无效为0*/
typedef union{
	struct
	{
		uint16_t lenid:12;
		uint16_t lchksum:4;
	}lenth_bits;
	/* 必须以数组形式定义,否则连续定义2个变量会放在同一个地址*/
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
/*链路层加应用层数据*/
/*****************************/
typedef struct __PDU_Struct
{
        INT8U 	address;
        INT8U	*PDUBuffPtr;                          /*指向的地址需要有与系统不冲突的空间容纳数据*/
        INT16U   	PDULength;
        INT8U     FrameOK;
}PDU_Struct;
typedef struct __SEND_Struct
{
	INT8U FunctionCode;
	INT8U *SENDBuffPtr;                        /*指向的地址需要有与系统不冲突的空间容纳数据*/
	INT8U SENDLength;
}SEND_Struct;
/*********************************************************************************
*                                     ModbusClient模块全局变量
**********************************************************************************/
extern INT8U g_CommRxFlag; 		/*注意需要初使化为TRUE，方便上电时接收使能*/
extern INT16U g_TxDataCtr;     		/*发送数据发送个数计数*/

extern INT8U Recive_Flag;				/* 接收标志*/
extern INT8U WAIT_response_flag;		/* 0:表示无等待，1:等待设置回复帧，2:等待界面更新查询帧，3:等待定时查询回复 */
extern INT8U Com_err_counter;		/* 通信错误计数器*/
extern INT8U Com_err_flag;			/* 通信错误标志*/
//extern INT8U g_Comm_init;			/* 通信初始化标志*/

extern INT32U CommAppFlag;
extern PDU_Struct  g_PDUData;  

extern INT32U  comm_flag;

/********************************************************************************************************
*                                          ModbusClient模块接口函数
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
**                            文件结束
********************************************************************************************************/



