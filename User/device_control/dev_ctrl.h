#ifndef __DEV_CTRL_H
#define	__DEV_CTRL_H

/* 位域的数量, 使用2个字节*/
typedef enum 
{	
	USED = 0,
	BACKUP,

	BIT_ARR_NUM
}BIT_ARR_LIST;

/*GPIO类型枚举,可扩展*/
typedef enum 
{	
	GPIO_IN = 0,
	GPIO_OUT,

	GPIO_TYPE_NUM
}GPIO_TYPE_LIST;


/*Low level, e.g. if want to up the BAR, set: BAR_UP = 0 */
#define BAR_UP_OUT_GRP			GPIOA
#define BAR_UP_POUT				GPIO_Pin_0	//栏杆起

#define BAR_DOWN_OUT_GRP		GPIOA
#define BAR_DOWN_POUT			GPIO_Pin_1 	// 栏杆落

/*雨棚灯改到O7引脚*/
#define TTL_GREEN_OUT_GRP		GPIOC
#define TTL_GREEN_POUT			GPIO_Pin_8	// 雨棚灯

/*声光报警统一到O6引脚*/
#define VOX_ALM_OUT_GRP			GPIOB
#define VOX_ALM_POUT			GPIO_Pin_1	// 声音报警器

#define LIGHT_ALM_OUT_GRP		GPIOB
#define LIGHT_ALM_POUT			GPIO_Pin_1	// 光报警器

/*O8引脚不变*/
#define LAN_LAMP_OUT_GRP		GPIOC
#define LAN_LAMP_POUT			GPIO_Pin_9	// 车道通信灯

/*新的电路板备用1,2移到了原来的TTL和VOX位置,TTL/VOX顺势后移*/
/*注意: 因为和上位机的协议没有变,所以结构体的位定义不能改变*/
/*只需要改变物理层的输出引脚定义就可以了*/
#define BAK1_USED_OUT_GRP		GPIOA   		
#define BAK1_USED_POUT			GPIO_Pin_6  	// 备用1

#define BAK2_USED_OUT_GRP		GPIOA		
#define BAK2_USED_POUT			GPIO_Pin_7	// 备用2


/*输入部分定义*/
#define TTL_IN_GRP				GPIOC		// PC0
#define TTL_PIN					GPIO_Pin_0	//雨棚灯状态反馈

#define LG_IN_GRP				GPIOC		// PC1
#define LG_PIN					GPIO_Pin_1	//栏杆状态反馈,只反馈给上位机,不参与逻辑

#define ALARM_IN_GRP				GPIOC		// PC2
#define ALARM_PIN				GPIO_Pin_2	// 报警检测

#define ALG_IN_GRP				GPIOC		// PC3
#define ALG_PIN					GPIO_Pin_3	// 自动栏杆状态,反映栏杆落还是抬

#define FRONT_IN_GRP				GPIOC		// PC4
#define FRONT_PIN				GPIO_Pin_4	// 前线圈,只反馈给上位机,不参与逻辑

#define BACK_COIN_IN_GRP			GPIOC		// PC5
#define BACK_COIN_PIN			GPIO_Pin_5	// 后线圈

#define BAK1_IN_GRP				GPIOC
#define BAK1_PIN					GPIO_Pin_6

#define BAK2_IN_GRP				GPIOC
#define BAK2_PIN					GPIO_Pin_7

#define DEVICE_ALL_PIN			(TTL_PIN |LG_PIN |ALARM_PIN |ALG_PIN |FRONT_PIN |BACK_COIN_PIN |BAK1_PIN |BAK2_PIN)


#define GREEN 		1
#define RED 			0

#define OPT_OK		0
#define OPT_FAIL		1


/*状态有效为1, 无效为0*/
typedef union{
	struct
	{
		uint16_t TTL_detect_bit:1;	//雨棚灯状态
		uint16_t LG_detect_bit:1;	// 
		uint16_t Alarm_detect_bit:1;	// 
		uint16_t ALG_detect_bit:1;	// 自动栏杆状态1: 抬杠状态,0:降杠状态
		uint16_t FRONT_coil_bit:1;	// 前线圈
		uint16_t BACK_coil_bit:1;	// 后线圈
		uint16_t bakup_detect1_bit:1;
		uint16_t bakup_detect2_bit:1;
		
		uint16_t unused0:1;
		uint16_t unused1:1;
		uint16_t ALG_work_status:1;	// bit2, 0: 栏杆操作正常,1:栏杆操作失败
		uint16_t unused3:1;
		uint16_t unused4:1;
		uint16_t unused5:1;
		uint16_t TTL_work_status:1;		//bit6, 0: 雨棚灯操作正常,1:雨棚灯操作失败
		uint16_t unused7:1;
	}status_bits;
	/* 必须以数组形式定义,否则连续定义2个变量会放在同一个地址*/
	uint8_t status_word[BIT_ARR_NUM];
}DEVICE_STATUS_BITS;

/*逻辑值, 使能输出为1，不使能为0*/
typedef union{
	struct
	{
		uint16_t ALG_up_bit:1;		/*抬栏杆*/
		uint16_t ALG_down_bit:1;	/* 落栏杆*/
		uint16_t TTL_green_bit:1;	/* 雨棚灯*/
		uint16_t VOX_alarm_bit:1;	/* 语音报警*/
		uint16_t Light_alarm_bit:1;	/* 光报警*/
		uint16_t Lane_lamp_bit:1;	/* 车道通信灯,0:红灯,1:绿灯*/
		uint16_t control_bak1_bit:1;
		uint16_t control_bak2_bit:1;

		uint16_t ALG_control_mode:1;	/*bit0,自动或者人工栏杆*/
		uint16_t FEE_display_bit:1;		/*bit1,1:费额显示,0:费额不显示*/
		uint16_t SPEAK_control_bit:1;	/*bit2,1:报价,0:不报价*/
		uint16_t FEE_displaykeep_bit:1; /*bit3,用于功能：绿灯后，费显仍然保持*/
		uint16_t bFeeDispSelection:1;	/*bit4,费显选择，1-无保持功能的广东用版本，0-有保持功能的山西版本*/
		uint16_t bControlOption:1;		/*bit5,车控器使用属性: 1-出口, 0-入口*/
		uint16_t bFeeDispReserved2:1;	/*bit6*/
		uint16_t unused:1;
	}control_bits;
	uint8_t control_word[BIT_ARR_NUM];
}DEVICE_CONTROL_BITS;

typedef struct device_out_struct
{
	GPIO_TypeDef* gpio_grp;
	uint16_t gpio_pin;
}DEVICE_CTRL;

typedef struct device_in_struct
{
	GPIO_TypeDef* gpio_grp;
	uint16_t gpio_pin;
}DEVICE_STATUS;

/*设备类型枚举,可扩展*/
typedef enum 
{	
	TTL = 0,
	LG,				// bit1
	ALARM,			// bit2
	ALG,				// bit3
	FRONT_COIL,		// bit4
	BACK_COIL,		// bit5
	BAK1,			// bit6
	BAK2,			// bit7

	DEV_STATUS_NUM
}DEVICE_STATUS_LIST;

/*类型枚举,可扩展*/
typedef enum 
{	
	BAR_UP = 0,
	BAR_DOWN,		// bit1
	TTL_GREEN,		// bit2
	VOX_ALM,		// bit3
	LIGHT_ALM,		// bit4, 和VOX共用了,这里保留,占有一个位
	LAN_LAMP,		// bit5
	BAK1_USED,		//这个备用的已经用来作为降杠或者抬杠的选择逻辑开关
	BAK2_USED,		// bit7

	DEV_CTRL_NUM
}DEVICE_CTRL_LIST;


extern DEVICE_STATUS_BITS device_status_used;
extern DEVICE_CONTROL_BITS device_control_used;
//extern DEVICE_CONTROL_BITS device_control_backup;
extern DEVICE_CTRL device_ctrl_queue[DEV_CTRL_NUM];
extern DEVICE_STATUS device_status_queue[DEV_STATUS_NUM];

/* 指示现在费显是亮红灯还是绿灯*/
extern uint8_t bLastLaneRedGreenOperateState;
extern bool LastLaneLampState;
//extern bool watching_car_passing;		// 后线圈监视
extern bool autoBarEnable;
extern bool bFeeCleared;
extern bool dete_bit_recd;
extern bool ALG_down_flag_bit;
extern bool ALG_up_flag_bit;

extern uint16_t WatchingDelayCount;
extern uint16_t detect_time_counter;		//系统参数检查间隔时间
extern uint16_t alarm_time_counter;

void RCC_Clock_Set(GPIO_TypeDef* GPIOx, FunctionalState iState);
void DeviceX_Activate(DEVICE_CTRL_LIST dev);
void DeviceX_Deactivate(DEVICE_CTRL_LIST dev);
void DEVICE_GPIO_OUT_Config(DEVICE_CTRL_LIST dev);
void DEVICE_GPIO_IN_Config(DEVICE_STATUS_LIST dev);
void Device_Ctrl_Queue_Init(void);
void Device_Stat_Queue_Init(void);
uint8_t Command_Get(DEVICE_CTRL_LIST dev);
bool detect_ALG_TTL_working(void);
uint8_t Status_Get(DEVICE_STATUS_LIST dev);
void params_modify_deal(void);
void control_device_expt_lg(void);
void control_device(void);
void Command_Act(uint8_t act_word);

#endif /* __DEV_CTRL_H */

