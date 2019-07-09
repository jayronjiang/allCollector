#ifndef _PARAMS_H
#define _PARAMS_H

/********************铁电地址划分,共8K的铁电*********************************/

#define DEVICE_TYPE 		"SPARK01"		/*装置设备类型的定义*/
#define SOFTWARE_VERSION 10000			/*装置的软件版本1.00.00*/
#define VERSION_DATE		 190604 			/*版本日期*/
#define PROTOCAL_REVISION 10

#define SYSTEM_SYNC		12341231		/*同步标志,只有CPU平台变化才进行修改，否则不要改变*/

#define DEVICE_PARAM_COUNTER 		sizeof(struct device_params_struct)	/*410个字节*/

#define FM_FIRST_WORD    	0X04		/*首次上电标志的地址*/
#define FM_DEVICE                	0X10
#define FM_DEVICE_END     	(FM_DEVICE+DEVICE_PARAM_COUNTER) 	/*+410,留576*/

#define RSU_NUM			6	// RSU天线数目
#define DOOR_TIME_OUT	3600	// 1hour

/*******************************************************************************************/
typedef struct phase_struct
{
	UINT16 vln;
	// 传感器有1个电压参数寄存器和1个K系数
	UINT32 param_v;
	UINT16 k_v;
	// 传感器还有1个电流参数寄存器和1个K系数
	UINT16 amp;
	UINT32 param_a;
	UINT16 k_a;
	// 投切标志
	UINT16 enable;
	
}RSU_PHASE_PARAMS;


typedef struct rsu_struct
{
	RSU_PHASE_PARAMS phase[RSU_NUM];		// 暂时为6路RSU
}RSU_PARAMS;

// USP子结构体--输入
typedef struct ups_in_struct
{
	UINT16 phase_num;	// 相数
	UINT16 volt_Ain;		//功能码42
	UINT16 volt_Bin;
	UINT16 volt_Cin;
	// 功能码E0
	UINT16 amp_Ain;
	UINT16 amp_Bin;
	UINT16 amp_Cin;
	UINT16 freq;			// 频率
	UINT16 fact_Ain;		// 功率因素
	UINT16 fact_Bin;		// 功率因素
	UINT16 fact_Cin;		// 功率因素

	UINT16 bypass_voltA;	// 旁路电压A
	UINT16 bypass_voltB;	// 旁路电压B
	UINT16 bypass_voltC;	// 旁路电压C
	UINT16 bypass_freq;	// 旁路频率
}UPS_IN_PARAMS;


// USP子结构体--输出
typedef struct ups_out_struct
{
	// 功能码42
	UINT16 volt_Aout;
	UINT16 volt_Bout;
	UINT16 volt_Cout;
	UINT16 amp_Aout;
	UINT16 amp_Bout;
	UINT16 amp_Cout;
	UINT16 freq;

	// 功能码E1
	UINT16 phase_num;	// 相数
	UINT16 fact_Aout;
	UINT16 fact_Bout;
	UINT16 fact_Cout;
	UINT16 kw_Aout;		// 有功
	UINT16 kw_Bout;		// 有功
	UINT16 kw_Cout;		// 有功
	UINT16 kva_Aout;		// 视在
	UINT16 kva_Bout;
	UINT16 kva_Cout;	

	UINT16 load_Aout;		// 负载
	UINT16 load_Bout;		// 负载
	UINT16 load_Cout;		// 负载
}UPS_OUT_PARAMS;

// USP子结构体--电池
typedef struct ups_bat_struct
{
	// 功能码E3
	UINT16 running_day;			// UPS运行时间,电池运行时间是E4
	UINT16 battery_volt;
	UINT16 amp_charge;
	UINT16 amp_discharge;
	UINT16 battery_left;
	UINT16 battery_tmp;	// 环境温度
	UINT16 battery_capacity;
	UINT16 battery_dischg_times;
	//UINT32 battery_running_time;	//电池运行时间是E4,不支持
}UPS_BAT_PARAMS;


// USP子结构体--状态
typedef struct ups_status_struct
{
	// 功能码43	
	UINT16 supply_out_status;		// 输出供电状态
	UINT16 supply_in_status;		// 输入供电状态
	UINT16 battery_status;			// UPS运行时间,电池运行时间是E4
}UPS_STATUS_PARAMS;

// USP子结构体--告警
typedef struct ups_alarm_struct
{
	// 功能码44	
	UINT16 main_abnormal;			// 主路异常, 9
	UINT16 system_overtemp;		// 系统过温, 10
	UINT16 sysbat_low_prealarm;	// 系统电池电量低预告警,11
	UINT16 rectifier_overload;		// 整流器过载,15
	UINT16 inverter_overload;		// 逆变器过载,17
	UINT16 bypass_abnomal;		// 旁路异常,21
	UINT16 battery_low_prealarm;	// 电池电压低,27
	UINT16 battery_abnomal;		// 电池电压异常,28
	UINT16 battery_overtemp;		// 电池过温,30
	UINT16 fan_abnormal;			// 风扇故障,34
	UINT16 shutdown_alarm;			//紧急关机告警,35
	UINT16 maintain_status;			//维修模式,44
	UINT16 inverter_supply;				//电池逆变供电,45
	UINT16 bypass_supply;				//旁路供电,46
}UPS_ALARM_PARAMS;


// USP结构体
typedef struct ups_struct
{
	UPS_IN_PARAMS ups_in;	// 输入数据
	UPS_OUT_PARAMS ups_out;	// 输出数据
	UPS_BAT_PARAMS  battery;
	UPS_STATUS_PARAMS status;
	UPS_ALARM_PARAMS alarm;
}UPS_PARAMS;



// 防雷器结构体
typedef struct spd_struct
{
	UINT16 status;	// 状态
	UINT16 DI_status;	// 接地电阻
	UINT16 struck_times;	// 雷击次数
}SPD_PARAMS;

//自动重合闸结构体
typedef struct ard_struct
{
	UINT16 address;	// 地址
	UINT16 ard_en;	// 自动重合闸是否使能
	UINT16 status;	// 状态
	UINT16 act;		// 合闸-分闸控制
	UINT16 failure;		// 自动重合闸后动作是否成功
}ARD_PARAMS;


//云断路器结构体
typedef struct breaker_struct
{	
	UINT16 address;	// 地址
	UINT16 drop_opening_en;	// 掉电分闸使能
	UINT16 ard_en;	// 自动重合闸是否使能
	UINT16 act;		// 合闸-分闸控制
	UINT16 status;	// 状态
	UINT16 failure;		// 自动重合闸后动作是否成功
}BREAKER_PARAMS;


// 环境数据结构体
typedef struct envi_struct
{
	UINT16 moist;	// 湿度
	UINT16 temp;	// 温度
	UINT16 water_flag;	// 漏水
	UINT16 front_door_flag;
	UINT16 back_door_flag;
	UINT16 door_overtime;
	UINT16 fire_move_flag;
	UINT16 smoke_event_flag;
	UINT16 air_cond_status;		// 整机状态
	UINT16 air_cond_fan_in_status;	// 内风机
	UINT16 air_cond_fan_out_status;	// 外风机
	UINT16 air_cond_comp_status;	// 压缩机
	UINT16 air_cond_heater_status;	// 电加热
	UINT16 air_cond_fan_emgy_status;	// 应急风机
	
	UINT16 air_cond_temp_out;
	UINT16 air_cond_temp_in;
	UINT16 air_cond_amp;	// 扩大1000倍
	UINT16 air_cond_volt;	// 实际值,不扩大
	
	UINT16 air_cond_hightemp_alarm;
	UINT16 air_cond_lowtemp_alarm;
	UINT16 air_cond_highmoist_alarm;
	UINT16 air_cond_lowmoist_alarm;
	UINT16 air_cond_infan_alarm;	// 内风机
	UINT16 air_cond_outfan_alarm;	// 外风机
	UINT16 air_cond_comp_alarm;	// 压缩机
	UINT16 air_cond_heater_alarm;	// 电加热
	UINT16 air_cond_emgyfan_alarm;	// 应急风机
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
extern SPD_PARAMS SPDParams[SPD_NUM];
extern ENVI_PARAMS ENVIParms;
extern ARD_PARAMS ARDParams[ARD_NUM];
extern BREAKER_PARAMS BRKParams[BRK_NUM];

extern DeviceInfoParams  DevicComInfor;
extern const DEVICE_PARAMS Init_DevParams;

void Write_DevParams(void);
void Init_Params(void);
void ComDeviceInfoInit(void);
void RSUKsInit(void);

#endif

