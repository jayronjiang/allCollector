
#ifndef _DO_H
#define _DO_H

/*Low level, e.g. if want to up the BAR, set: BAR_UP = 0 */
#define DO1_OUT_GRP			GPIOA
#define DO1_POUT				GPIO_Pin_0	//栏杆起

#define DO2_OUT_GRP		GPIOA
#define DO2_POUT			GPIO_Pin_1 	// 栏杆落

/*雨棚灯改到O7引脚*/
#define DO7_OUT_GRP		GPIOC
#define DO7_POUT			GPIO_Pin_8	// 雨棚灯

/*声光报警统一到O6引脚*/
#define DO6_OUT_GRP			GPIOB
#define DO6_POUT			GPIO_Pin_1	// 声音报警器

#define DO5_OUT_GRP		GPIOB
#define DO5_POUT			GPIO_Pin_1	// 光报警器

/*O8引脚不变*/
#define DO8_OUT_GRP		GPIOC
#define DO8_POUT			GPIO_Pin_9	// 车道通信灯

/*新的电路板备用1,2移到了原来的TTL和VOX位置,TTL/VOX顺势后移*/
/*注意: 因为和上位机的协议没有变,所以结构体的位定义不能改变*/
/*只需要改变物理层的输出引脚定义就可以了*/
#define DO3_OUT_GRP		GPIOA   		
#define DO3_POUT			GPIO_Pin_6  	// 备用1

#define DO4_OUT_GRP		GPIOA		
#define DO4_POUT			GPIO_Pin_7	// 备用2



#define RELAY_REMOTE_PRESET_CANCEL	0
#define RELAY_REMOTE_ACT_PRESET		1
#define RELAY_REMOTE_ACT				2
#define RELAY_REMOTE_RETURN_PRESET	3
#define RELAY_REMOTE_RETURN			4
#define RELAY_ALARM_ACT				5
#define RELAY_ALARM_RETURN			6
#define RELAY_FORCE_ACT				7
#define RELAY_FORCE_RETURN				8

#define REMOTEACT	BIT0
#define PROACT	BIT1
#define FORCEACT		BIT2

#define REMOTE_ACTED	BIT3		/*标识遥控操作继电器动作过*/
#define PRO_ACTED		BIT4		/*动作过标识*/


#define PRESET_15S	750		/*15秒,20ms中断*/


typedef struct device_out_struct
{
	GPIO_TypeDef* gpio_grp;
	uint16_t gpio_pin;
}DEVICE_CTRL;

/*类型枚举,可扩展*/
typedef enum 
{	
	DO_1 = 0,
	DO_2,		// bit1
	DO_3,		// bit2
	DO_4,		// bit3
	DO_5,		// bit4, 和VOX共用了,这里保留,占有一个位
	DO_6,		// bit5
	DO_7,		//这个备用的已经用来作为降杠或者抬杠的选择逻辑开关
	DO_8,		// bit7

	DO_NUM
}DEVICE_CTRL_LIST;


#define VOX_ALM	DO_4


typedef struct relay_status_struct
{
	UINT32 ActPresetCounter;
	UINT16 ActPresetFlag;

	UINT32 ReturnPresetCounter;
	UINT16 ReturnPresetFlag;

	UINT16 ActFlag;		/*BIT0 遥控 BIT1定值越限,BIT2,面板强制*/
	UINT16 PulseCounter;
}RELAY_STATUS;

extern UINT16 DO_Status;
//tern UINT8 DO_OpStatus[DO_NUM];
extern RELAY_STATUS RelayStatus[DO_NUM];

void DO_Init(void);
UINT8 Relay_Act(UINT8 num);
UINT8 Relay_Return(UINT8 num);
void DOProcessTickEvents(void);
UINT8 RelayOperate(UINT8 num, UINT8 mode);
//void Output_Control(void);
void DEVICE_GPIO_OUT_Config(DEVICE_CTRL_LIST dev);
void DO_Queue_Init(void);
void DI_Queue_Init(void);
void DeviceX_Activate(DEVICE_CTRL_LIST dev);
void DeviceX_Deactivate(DEVICE_CTRL_LIST dev);

#endif


