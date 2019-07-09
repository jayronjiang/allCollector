/********************************************************************************
*  版权所有: 	广东利通科技		
*
*  文件名:       ModbusServer.c
*
*
*  当前版本:                       0.1
*  
*  作者:     JERRY                     
*
*  完成日期:  20190613
*                 
*  作者:    无
*
*  完成日期:                       无
*
*
*  描述: 
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
#define	WRITEFILE_COMMAND				0x15
#define 	READFILE_COMMAND				0x14
#define	WRITESECRATEREG_COMMAND		0x47 
#define	READSECRATEREG_COMMAND		0x46
#define	WRITESINGLECOIL_COMMAND		0x05 
#define 	REF_TYPE_CODE   					0x06


#ifdef RENDA
#define   LOCK_NUM			3			// 3把锁

#define BRK_NUM				2			//断路器数目
#define ARD_NUM				11			//重合闸
#define SPD_NUM				2			//防雷器个数

#define 	SMOKE_STATION_ADDRESS  		50		/*烟感地址*/
#define 	WATERIN_STATION_ADDRESS  		51		/*默认水浸地址*/
#define 	TEMP_STATION_ADDRESS  			52		/*默认温湿度从站从站地址*/
#define 	AIR_STATION_ADDRESS  			60		/*默认空调从站从站地址*/

#define 	SPD_STATION_1_ADDRESS			35
#define 	SPD_STATION_2_ADDRESS			36

#define 	BREAKER_STATION_1_ADDRESS  	1	/*断路器1地址*/
#define 	BREAKER_STATION_2_ADDRESS  	2	/*断路器2地址*/

#define 	ARD_STATION_1_ADDRESS  	3	/*自动重合闸1地址*/
#define 	ARD_STATION_2_ADDRESS  	4	/*自动重合闸2地址*/
#define 	ARD_STATION_3_ADDRESS  	5	/*自动重合闸3地址*/
#define 	ARD_STATION_4_ADDRESS  	6	/*自动重合闸4地址*/
#define 	ARD_STATION_5_ADDRESS  	7	/*自动重合闸5地址*/
#define 	ARD_STATION_6_ADDRESS  	8	/*自动重合闸6地址*/
#define 	ARD_STATION_7_ADDRESS  	9	/*自动重合闸7地址*/
#define 	ARD_STATION_8_ADDRESS  	10	/*自动重合闸8地址*/
#define 	ARD_STATION_9_ADDRESS  	11	/*自动重合闸9地址*/
// 重合闸10空出
#define 	ARD_STATION_10_ADDRESS  	13	/*自动重合闸11地址*/
#define 	ARD_STATION_11_ADDRESS  	14	/*自动重合闸12地址*/

#define 	UPS_STATION_ADDRESS  	01		/*默认UPS从站地址*/

/*********************************************************************************
*                                    断路器协议宏定义
**********************************************************************************/
#define BRK_REMOTE_ADDR 		0x6802		// 远程分合闸地址
#define BRK_SINGLE_WRITE 		0x06			// 单个寄存器写命令

#define BRK_OPEN_LOCK				0x03
#define BRK_OPEN_WITHOUT_LOCK		0x13
#define BRK_OPEN_UNLOCK				0x23		// 分闸解锁
#define BRK_CLOSE					0x33

#define BREAKER_REG             					0x5000	// 断路器状态renda
/*********************************************************************************
*                                   自动重合闸协议宏定义
**********************************************************************************/
#define ARD_REMOTE_ADDR 	0x11			// 远程分合闸地址
#define ARD_SINGLE_WRITE 	0x06			// 单个寄存器写命令

#define ARD_OPEN				0x01
#define ARD_CLOSE			0x02

#define ARD_REG             						0x10		// 重合闸状态renda
/*********************************************************************************
*                                   SPD协议宏定义
**********************************************************************************/
#define SPD_TIMES_REG   	2001				// 雷击计数

/*********************************************************************************
*                                  空调协议宏定义
**********************************************************************************/
#define AIR_ONOFF_REG             		0x0801	// 空调开关机
#define AIR_TEMP_REG             		0x0700	// 空调高温低温点

#define ENVI_AIRCOND_ONOFF_REG             		0x0100	// 空调状态
#define ENVI_AIRCOND_TEMP_REG             			0x0501	// 空调温度
#define ENVI_AIRCOND_ALARM_REG             		0x0600	// 空调温度

/*********************************************************************************
*                                 温湿度/水浸/烟雾协议宏定义
**********************************************************************************/
#define ENVI_TEMP_REG             					0x00		// 温湿度
#define WATER_IN_REG             					0x0010	// 水浸renda
#define SMOKE_REG             						0x0		// 烟感renda


/*********************************************************************************
*                                     读取相关参数长度
**********************************************************************************/	
#define REAL_DATA_NUM			24  		/*固定为24个字节*/
#define SPD_TIMES_NUM 			1		// 读1个,分别为当前雷击次数
#define AIR_ONOFF_SET_NUM 		1 		// 空调遥控,只有1个地址
#define AIR_TEMP_SET_NUM 		4 		// 空调遥控,只有4个地址

#define ENVI_TEMP_NUM 			2
#define ENVI_AIRCOND_ONOFF_NUM 			5	//任达只有5个
#define ENVI_AIRCOND_TEMP_NUM 			7	// 增加了电压电流
#define ENVI_AIRCOND_ALARM_NUM 			17	// 一共17个报警量

#define WATER_IN_NUM 			1
#define SMOKE_EVENT_NUM 			1

/*断路器和重合闸*/
#define BREAKER_STATUS_NUM 		1
#define ARD_REG_NUM             		0x01		// 重合闸renda

#else	// 利通自己的柜子

#define LOCK_NUM			1	// 1把生久锁

#define BRK_NUM			0	//断路器数目
#define ARD_NUM			0	//重合闸
#define SPD_NUM			1	//防雷器个数

#define 	WATERIN_STATION_ADDRESS  		0x04		/*默认水浸地址*/
#define 	TEMP_STATION_ADDRESS  			0x03		/*默认温湿度从站从站地址*/
#define 	AIR_STATION_ADDRESS  			0x02		/*默认空调从站从站地址*/

#define 	SPD_STATION_1_ADDRESS			0x05
#define 	UPS_STATION_ADDRESS  			0x01		/*默认UPS从站地址*/


#define	SPD_DIREAD_COMMAND			0x02		//防雷自己的读功能码为02
#define	SPD_TIMESREAD_COMMAND		0x03		//防雷自己的雷击读功能码为03

/*********************************************************************************
*                                   外设自己的协议地址定义
**********************************************************************************/
#define RSU_REG        		0x40				// 从第一路电压开始读
//#define UPS_REG   			0x01
#define SPD_STATUS_REG   	0x00				// 防雷的输入状态从地址0开始
#define SPD_TIMES_REG   	0x0E				// 04码从0E开始读
#define AIR_ONOFF_REG             		0x0801	// 空调开关机
#define AIR_TEMP_REG             		0x0700	// 空调高温低温点

#define ENVI_AIRCOND_ONOFF_REG             		0x0100	// 空调状态
#define ENVI_AIRCOND_TEMP_REG             			0x0501	// 空调温度
#define ENVI_AIRCOND_ALARM_REG             		0x0600	// 空调温度

#define ENVI_TEMP_REG             					0x00		// 温湿度
#define WATER_IN_REG             					0x0010	// 水浸renda

/*********************************************************************************
*                                     读取相关参数长度
**********************************************************************************/
#define REAL_DATA_NUM			24  	/*固定为24个字节*/
#define SPD_STATUS_NUM 			0x11		// 测试软件读取了17个长度，其实是17位,共3个字节
#define SPD_TIMES_NUM 			3		// 读3个,分别为当前雷击次数,总雷击次数,最高清零的雷击次数

#define AIR_ONOFF_SET_NUM 		1 		// 空调遥控,只有1个地址
#define AIR_TEMP_SET_NUM 		4 		// 空调遥控,只有4个地址

#define ENVI_AIRCOND_ONOFF_NUM 			6	//扩展到6个
#define ENVI_AIRCOND_TEMP_NUM 			7	// 增加了电压电流
#define ENVI_AIRCOND_ALARM_NUM 			17	// 一共17个报警量

#define ENVI_TEMP_NUM 			2
#define WATER_IN_NUM 			1
#endif


#define Wait_max_time  		200 		/* 发送后等待接收帧的最长时间为200ms */

/*********************************************************************************
*                                      通信轮询参数标志 comm_flag
**********************************************************************************/
//485轮询类型枚举,可扩展
typedef enum 
{	
	DEV_PARAM_SEND_FLAG_2 = 0,
	DEV_PARAM_SEND_FLAG_1,

	REAL_DATA0_SEND_FLAG,		/*实时数据查询,传感器2*/
	REAL_DATA1_SEND_FLAG,
	REAL_DATA2_SEND_FLAG,
	REAL_DATA3_SEND_FLAG,

	UPS_PARAM_SEND_FLAG,		// 6	/*USP的总体参数*/
	UPS_IN_SEND_FLAG,			/*USP的输入侧参数*/
	UPS_OUT_SEND_FLAG,
	UPS_BAT_SEND_FLAG,
	UPS_STATUS_SEND_FLAG,
	UPS_ALARM_SEND_FLAG,		// 11 /*USP的报警信息参数*/

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

	WATER_IN_FLAG,			// 18,水浸
#ifdef RENDA
	SMOKE_FLAG,			// 烟雾
#endif

#if (BRK_NUM >= 1)
	BREAKER_OPEN_CLOSE_STATUS_1,  // 20断路器1合分闸状态
#endif
#if (BRK_NUM >= 2)
	BREAKER_OPEN_CLOSE_STATUS_2,
#endif

#if (ARD_NUM >= 1)
	ARD_STS_FLAG_1,		// 22 自动重合闸1
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
#define REAL_DATA0_SEND_FLAG    	BIT0		/*实时数据查询,传感器0*/
#define REAL_DATA1_SEND_FLAG    	BIT1		/*实时数据查询,传感器1*/
#define REAL_DATA2_SEND_FLAG    	BIT2		/*实时数据查询,传感器2*/
#define REAL_DATA3_SEND_FLAG    	BIT3		/*实时数据查询,传感器3*/

#define UPS_PARAM_SEND_FLAG		BIT4		/*USP的总体参数*/
#define UPS_IN_SEND_FLAG			BIT5		/*USP的输入侧参数*/
#define UPS_OUT_SEND_FLAG		BIT6		/*USP的输出侧参数*/
#define UPS_BAT_SEND_FLAG		BIT7		/*USP的电池参数*/
#define UPS_STATUS_SEND_FLAG	BIT8		/*USP的运行状态参数*/
#define UPS_ALARM_SEND_FLAG		BIT9		/*USP的报警信息参数*/


//#define SPD_STATUS_SEND_FLAG		BIT10
#define SPD_TIMES_SEND_FLAG_1		BIT10
#define SPD_TIMES_SEND_FLAG_2		BIT11
//#define SPD_TIMES_SEND_FLAG_3		BIT24

#define ENVI_TEMP_SEND_FLAG         		BIT12		// 温湿度
#define ENVI_AIRCOND_ONOFF_FLAG         BIT13		// 空调参数读取
#define ENVI_AIRCOND_TEMP_FLAG         	BIT14		// 空调参数读取
#define ENVI_AIRCOND_ALARM_FLAG         BIT15		// 空调参数读取

#define DEV_PARAM_SEND_FLAG_1         	BIT16		/*参数查询*/
#define DEV_PARAM_SEND_FLAG_2         	BIT17		/*参数查询*/


#define WATER_IN_FLAG          	BIT22	 // 水进也是485的了renda
#define SMOKE_FLAG          		BIT23	 // 烟感也是485的了renda

#define BREAKER_OPEN_CLOSE_STATUS_1          	BIT24	 // 断路器1合分闸状态
#define BREAKER_OPEN_CLOSE_STATUS_2          	BIT25	 // 断路器2合分闸状态

#define ARD_STS_FLAG_1          					BIT26	 	// 自动重合闸1
#define ARD_STS_FLAG_2          					BIT27	 	// 自动重合闸2
#define ARD_STS_FLAG_3          					BIT28	 	// 自动重合闸3
#define ARD_STS_FLAG_4          					BIT29	 	// 自动重合闸4
#define ARD_STS_FLAG_5          					BIT30	 	// 自动重合闸5
#define ARD_STS_FLAG_6          					BIT31	 	// 自动重合闸6
#define ARD_STS_FLAG_7          					LBIT(32)	 	// 自动重合闸7
#define ARD_STS_FLAG_8          					LBIT(33)	 	// 自动重合闸8
#define ARD_STS_FLAG_9          					LBIT(34)	 	// 自动重合闸9
#define ARD_STS_FLAG_10          					LBIT(35)	 	// 自动重合闸10
#define ARD_STS_FLAG_11          					LBIT(36)	 	// 自动重合闸11
#endif

/*********************************************************************************
*                                      通信设置参数标志control_flag
**********************************************************************************/
//485参数写类型枚举,可扩展
typedef enum 
{	
	DEV_PARAM_SET_FLAG_1 = 0,	// 参数设置	--空调开关机
	DEV_PARAM_SET_FLAG_2,		// 参数设置	--空调温度设置

#if (LOCK_NUM >= 1)
	DOOR1_OPEN_SET_FLAG,		// 1 ,电子锁开renda-生久
	DOOR1_CLOSE_SET_FLAG,		// 电子锁关
#endif
#if (LOCK_NUM >= 2)
	DOOR2_OPEN_SET_FLAG,		// 2 ,电子锁开renda-生久
	DOOR2_CLOSE_SET_FLAG,		// 电子锁关
#endif
#if (LOCK_NUM >= 3)
	DOOR3_OPEN_SET_FLAG,		// 4,电子锁开renda-生久
	DOOR3_CLOSE_SET_FLAG,		// 电子锁关
#endif

#if (BRK_NUM >= 1)
	BRK1_CLOSE_SET_FLAG,		// 6,断路器1关
	BRK1_OPEN_SET_FLAG,		// 断路器1开不锁死
	BRK1_OPEN_LOCK_SET_FLAG,	// 断路器1开锁死
	BRK1_OPEN_UNLOCK_SET_FLAG,	// 断路器1开解锁
#endif
#if (BRK_NUM >= 2)
	BRK2_CLOSE_SET_FLAG,			// 10, 断路器2关
	BRK2_OPEN_SET_FLAG,			// 11断路器2开不锁死
	BRK2_OPEN_LOCK_SET_FLAG,		// 12断路器2开锁死
	BRK2_OPEN_UNLOCK_SET_FLAG,	// 13断路器2开解锁
#endif

#if (ARD_NUM >= 1)
	ARD1_CLOSE_SET_FLAG,			// 14,自动重合闸1关
	ARD1_OPEN_SET_FLAG,			// 15自动重合闸1开
#endif
#if (ARD_NUM >= 2)
	ARD2_CLOSE_SET_FLAG,			// 自动重合闸2关
	ARD2_OPEN_SET_FLAG,			// 自动重合闸2开
#endif
#if (ARD_NUM >= 3)
	ARD3_CLOSE_SET_FLAG,			// 自动重合闸3关
	ARD3_OPEN_SET_FLAG,			// 自动重合闸3开
#endif
#if (ARD_NUM >= 4)
	ARD4_CLOSE_SET_FLAG,			// 20自动重合闸4关
	ARD4_OPEN_SET_FLAG,			// 21自动重合闸4开
#endif
#if (ARD_NUM >= 5)
	ARD5_CLOSE_SET_FLAG,			// 22自动重合闸5关
	ARD5_OPEN_SET_FLAG,			// 23自动重合闸5开
#endif
#if (ARD_NUM >= 6)
	ARD6_CLOSE_SET_FLAG,			// 24自动重合闸6关
	ARD6_OPEN_SET_FLAG,			// 25自动重合闸6开
#endif
#if (ARD_NUM >= 7)
	ARD7_CLOSE_SET_FLAG,			// 26自动重合闸7关
	ARD7_OPEN_SET_FLAG,			// 27自动重合闸7开
#endif
#if (ARD_NUM >= 8)
	ARD8_CLOSE_SET_FLAG,			// 自动重合闸8关
	ARD8_OPEN_SET_FLAG,			// 自动重合闸8开
#endif
#if (ARD_NUM >= 9)
	ARD9_CLOSE_SET_FLAG,			// 自动重合闸9关
	ARD9_OPEN_SET_FLAG,			// 自动重合闸9开
#endif
#if (ARD_NUM >= 10)
	ARD10_CLOSE_SET_FLAG,			// 32自动重合闸10关
	ARD10_OPEN_SET_FLAG,			// 33自动重合闸10开
#endif
#if (ARD_NUM >= 11)
	ARD11_CLOSE_SET_FLAG,			// 34自动重合闸11关
	ARD11_OPEN_SET_FLAG,			// 35自动重合闸11开
#endif	
	CONTROL_NUM		// 36
}CONTROL_LIST;

#if 0
#define DEV_PARAM_SET_FLAG_1          	BIT0	 	// 参数设置	--空调开关机
#define DEV_PARAM_SET_FLAG_2       	BIT1	 	// 参数设置	--空调温度设置

#define DOOR2_OPEN_SET_FLAG          	BIT2	 	// 电子锁开renda-生久
#define DOOR2_CLOSE_SET_FLAG          	BIT3	 	// 电子锁关

#define DOOR3_OPEN_SET_FLAG          	BIT4	 	// 电子锁开renda-生久
#define DOOR3_CLOSE_SET_FLAG          	BIT5	 	// 电子锁关

#define BRK1_CLOSE_SET_FLAG          	BIT6	 	// 断路器1关
#define BRK1_OPEN_SET_FLAG          		BIT7 		// 断路器1开不锁死
#define BRK1_OPEN_LOCK_SET_FLAG          	BIT8 		// 断路器1开锁死
#define BRK1_OPEN_UNLOCK_SET_FLAG          	BIT9 		// 断路器1开解锁


#define BRK2_CLOSE_SET_FLAG          	BIT10	 	// 断路器1关
#define BRK2_OPEN_SET_FLAG          		BIT11 		// 断路器1开不锁死
#define BRK2_OPEN_LOCK_SET_FLAG          	BIT12 		// 断路器1开锁死
#define BRK2_OPEN_UNLOCK_SET_FLAG          	BIT13 		// 断路器1开解锁

#define ARD1_CLOSE_SET_FLAG          	BIT14	 	// 自动重合闸1关
#define ARD1_OPEN_SET_FLAG          		BIT15	 	// 自动重合闸1开

#define ARD2_CLOSE_SET_FLAG          	BIT16	 	// 自动重合闸2关
#define ARD2_OPEN_SET_FLAG          		BIT17	 	// 自动重合闸2开

#define ARD3_CLOSE_SET_FLAG          	BIT18	 	// 自动重合闸3关
#define ARD3_OPEN_SET_FLAG          		BIT19	 	// 自动重合闸3开

#define ARD4_CLOSE_SET_FLAG          	BIT20	 	// 自动重合闸4关
#define ARD4_OPEN_SET_FLAG          		BIT21	 	// 自动重合闸4开

#define ARD5_CLOSE_SET_FLAG          	BIT22	 	// 自动重合闸5关
#define ARD5_OPEN_SET_FLAG          		BIT23	 	// 自动重合闸5开

#define ARD6_CLOSE_SET_FLAG          	BIT24	 	// 自动重合闸6关
#define ARD6_OPEN_SET_FLAG          		BIT25	 	// 自动重合闸6开

#define ARD7_CLOSE_SET_FLAG          	BIT26	 	// 自动重合闸7关
#define ARD7_OPEN_SET_FLAG          		BIT27	 	// 自动重合闸7开

#define ARD8_CLOSE_SET_FLAG          	BIT28	 	// 自动重合闸8关
#define ARD8_OPEN_SET_FLAG          		BIT29	 	// 自动重合闸8开

#define ARD9_CLOSE_SET_FLAG          	BIT30	 	// 自动重合闸9关
#define ARD9_OPEN_SET_FLAG          		BIT31	 	// 自动重合闸9开

#define ARD10_CLOSE_SET_FLAG          	LBIT(32)	 	// 自动重合闸10关
#define ARD10_OPEN_SET_FLAG          	LBIT(33)	 	// 自动重合闸10开

#define ARD11_CLOSE_SET_FLAG          	LBIT(34)	 	// 自动重合闸11关
#define ARD11_OPEN_SET_FLAG          	LBIT(35)	 	// 自动重合闸11开
#endif


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
#define SPD_TIMES_ANALYSE_1	14	/*防雷参数解析*/
#define SPD_TIMES_ANALYSE_2	15	/*防雷参数解析*/

#define DEVICE_DATA_1_ANALYSE	16	/*装置参数读取解析,空调开关机*/
#define DEVICE_DATA_2_ANALYSE	17	/*装置参数读取解析,空调温度*/

#define UPS_PARAM_ANALYSE		18
#define UPS_IN_ANALYSE			19
#define UPS_OUT_ANALYSE			20
#define UPS_BAT_ANALYSE			21
#define UPS_ALARM_ANALYSE		22
#define UPS_STATUS_ANALYSE		23

#define WATER_IN_ANALYSE		24	/*水浸参数解析*/
#define SMOKE_ANALYSE		25	/*烟感参数解析*/

#define WAIT_DOOR_OPEN		26	/*等待开锁设置帧回复*/
#define WAIT_DOOR_CLOSE		27	/*等待关锁设置帧回复*/

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


#define FRAME_HEAD_NUM 			3		/*读数据时返回帧有效数据前数据个数*/
#define SET_FRAME_HEAD_NUM 		7		/*写数据时返回帧有效数据前数据个数*/


/*********************************************************************************
*                                    生久锁 协议宏定义
**********************************************************************************/

#define   LOCK_ADDR_1		0x01
#define   LOCK_SOI		0x7E

#define   LOCK_DES_THREAD	0x00
#define   LOCK_SRC_THREAD	0x00

#define   LOCK_EOI1		0x7E
#define   LOCK_EOI2		0x7E

/* 协议字节顺序号定义*/
#define   LOCK_SQU_SOI		0
#define   LOCK_SQU_DES_STA	1	// 目的地址
#define   LOCK_SQU_SRC_STA	2	// 源地址
#define   LOCK_SQU_DES_THRD	3	// 目的进程
#define   LOCK_SQU_SRC_THRD	4	// 源进程号
#define   LOCK_SQU_LENTH_HIGH	5	// 长度高字节
#define   LOCK_SQU_LENTH_LOW		6	// 长度高字节
#define   LOCK_SQU_SIGNAL		7	// 长度高字节


#define   LOCK_OPEN		0x0005
#define   LOCK_CLOSE		0x0006

#if (LOCK_NUM >= 2)
#define   LOCK_ADDR_2		0x02
#endif

#if (LOCK_NUM >= 3)
#define   LOCK_ADDR_3		0x03
#endif

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

extern INT64U  comm_flag;
extern INT64U  control_flag;

/********************************************************************************************************
*                                          ModbusClient模块接口函数
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
**                            文件结束
********************************************************************************************************/



