#ifndef _PARAMS_H
#define _PARAMS_H

/********************铁电地址划分,共8K的铁电*********************************/

#define DEVICE_TYPE 		"SPARK01"		/*装置设备类型的定义*/
#define SOFTWARE_VERSION 10000			/*装置的软件版本1.00.00*/
#define VERSION_DATE		 190506 			/*版本日期*/

#define SYSTEM_SYNC		12341233		/*同步标志,只有CPU平台变化才进行修改，否则不要改变*/

#define DEVICE_PARAM_COUNTER 		sizeof(struct device_params_struct)	/*410个字节*/

#define DATA_BLOCK_SIZE  	0X450		/*预留1104个字节*/

#define FM_FIRST_WORD    	0X04		/*首次上电标志的地址*/
#define FM_DEVICE                	0X10
#define FM_DEVICE_END     	(FM_DEVICE+DEVICE_PARAM_COUNTER) 	/*+410,留576*/

#define RSU_NUM		6	// RSU天线数目

/*******************************************************************************************/
typedef struct phase_struct
{
	UINT16 vln;
	UINT16 amp;
	// 投切标志
	UINT16 enable;
}RSU_PHASE_PARAMS;


typedef struct rsu_struct
{
	RSU_PHASE_PARAMS phase[RSU_NUM];		// 暂时为6路RSU
}RSU_PARAMS;


// USP结构体
typedef struct ups_struct
{
	UINT16 phase_num;	// 相数
	UINT16 volt_Ain;
	UINT16 volt_Bin;
	UINT16 volt_Cin;
	UINT16 amp_Ain;
	UINT16 amp_Bin;
	UINT16 amp_Cin;

	UINT16 volt_Aout;
	UINT16 volt_Bout;
	UINT16 volt_Cout;
	UINT16 amp_Aout;
	UINT16 amp_Bout;
	UINT16 amp_Cout;

	UINT16 running_day;
	UINT16 battery_volt;
	UINT16 amp_charge;
	UINT16 battery_left;
	UINT16 battery_tmp;
	UINT16 battery_capacity;
	UINT16 battery_dischg_times;
	UINT32 battery_running_time;
	UINT32 battery_dischg_time;
	UINT16 battery_status;	
}UPS_PARAMS;

// 防雷器结构体
typedef struct spd_struct
{
	UINT16 status;	// 状态
	UINT16 grd_res;	// 接地电阻
	UINT16 struck_times;	// 雷击次数
}SPD_PARAMS;


// 环境数据结构体
typedef struct envi_struct
{
	UINT16 temp;	// 温度
	UINT16 moist;	// 湿度
	UINT16 water_flag;	// 漏水
	UINT16 door_flag;
	UINT16 door_overtime;
	UINT16 fire_move_flag;
	UINT16 air_cond_status;
	UINT16 air_cond_temp_in;
	UINT16 air_cond_temp_out;
	UINT16 air_cond_hightemp_alarm;
	UINT16 air_cond_lowtemp_alarm;
}ENVI_PARAMS;

typedef struct device_params_struct	/*共384个字节*/
{
	UINT16 Crc;				/*各字节累加和*/
	UINT16 Number;				/*结构体有效长度INT16*/

	UINT16 Address;
	UINT16 BaudRate_1;	// 串口1波特率
	UINT16 BaudRate_2;	// 串口2波特率
	UINT16 BaudRate_3;	// 串口3波特率
	UINT16 BaudRate_4;	// 串口4波特率

	UINT16 Pre_Remote;		/*遥控预置，0：退出，1：投入*/
	UINT16 AutoSend;		/*自动上传是否投入*/
	UINT16 AirCondSet;		/*空调开机关机*/
	UINT16 AirColdStartPoint;		/*空调制冷点*/
	UINT16 AirColdLoop;		/*空调制冷回差*/
	UINT16 AirHotStartPoint;		/*空调制冷点*/
	UINT16 AirHotLoop;		/*空调制冷回差*/
	
	//UINT16 Parity;
	//UINT16 PassWord;
	//UINT16 BackLight;	/* 背光时间*/

	//UINT16 DispType;/*显示模式,0：静态显示，1~60：循环轮显间隔时间(s)*/
	//UINT16 ChlKeyWord;	/*通道投退位*/
	//SETPOINT SetPoint[CHL_NUM][2];	/*温度定值越限,12通道，每通道2组*/
	
	//UINT16 DI_TingleTime;		/*DI去抖时间*/
	//UINT16 DI_Type[DI_NUM];			/*DI类型配置，0:常开，1：常闭 */
	//UINT16 DI_LinkCfg[DI_NUM];	/*DI联动配置，DI_Num宏定义为2*/
	//UINT16 DI_Delay[DI_NUM]; /*DI动作延时，0~999.9s，返回延时不能设置默认为10ms*/
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
extern RSU_PARAMS RSUParams;
extern UPS_PARAMS UPSParams;
extern SPD_PARAMS SPDParams;
extern ENVI_PARAMS ENVIParms;
extern DeviceInfoParams  DevicComInfor;
extern const DEVICE_PARAMS Init_DevParams;

void Write_DevParams(void);
void Init_Params(void);
void ComDeviceInfoInit(void);

#endif

