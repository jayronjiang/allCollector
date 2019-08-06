#ifndef _PARAMS_H
#define _PARAMS_H

/********************铁电地址划分,共8K的铁电*********************************/
#ifdef HAS_8I8O
#define DEVICE_TYPE 		"LTKJ_I8O8"			/*装置设备类型的定义*/
#define DEV_ADDR			61
#else
#define DEVICE_TYPE 		"LTKJ_I0O12"			/*装置设备类型的定义*/
#define DEV_ADDR			71
#endif
#define SOFTWARE_VERSION 10000			/*装置的软件版本1.00.00*/
#define VERSION_DATE		 190728 			/*版本日期*/
#define PROTOCAL_REVISION 10

#define SYSTEM_SYNC		12341236		/*同步标志,只有CPU平台变化才进行修改，否则不要改变*/

#define DEVICE_PARAM_COUNTER 		sizeof(struct device_params_struct)	/*410个字节*/

#define FM_FIRST_WORD    	0X04		/*首次上电标志的地址+4*/
#define FM_DEVICE                	0X10
#define FM_DEVICE_END     	(FM_DEVICE+DEVICE_PARAM_COUNTER) 	/*+16*/

/*******************************************************************************************/

typedef struct device_params_struct	/*共16个字节*/
{
	UINT16 Crc;				/*各字节累加和*/
	UINT16 Number;				/*结构体有效长度INT16*/

	UINT16 Address;
	UINT16 BaudRate_1;	// 串口1波特率
	UINT16 BaudRate_2;	// 串口2波特率
	UINT16 BaudRate_3;	// 串口3波特率
	UINT16 BaudRate_4;	// 串口4波特率

	UINT16 Pre_Remote;	//遥控预置，0：退出，1：投入
	UINT16 Do_status;		// DO的状态也要进行保存，上电后要记住上次状态
}DEVICE_PARAMS;

typedef  struct  _DeviceInfo_Struct      /*用于整块写命令时边界判断，例如防止32BIT数只写了16BIT*/
{
   UINT16 deviceType[20];
   UINT16 softVersion;
   UINT16 protocolVersion;
   UINT16 softYear;
   UINT16 softMonth;
   UINT16 softDate;
}DeviceInfoParams;

extern DEVICE_PARAMS DevParams;

extern DeviceInfoParams  DevicComInfor;
extern const DEVICE_PARAMS Init_DevParams;

void Write_DevParams(void);
void Init_Params(void);
void ComDeviceInfoInit(void);

#endif

