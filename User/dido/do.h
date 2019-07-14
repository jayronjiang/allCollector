
#ifndef _DO_H
#define _DO_H

/*Low level, e.g. if want to up the BAR, set: BAR_UP = 0 */
#define DO1_OUT_GRP			GPIOA
#define DO1_POUT				GPIO_Pin_0	// 输出1

#define DO2_OUT_GRP			GPIOA
#define DO2_POUT				GPIO_Pin_1 

#define DO3_OUT_GRP			GPIOA   		
#define DO3_POUT				GPIO_Pin_6  	// 输出3

#define DO4_OUT_GRP			GPIOA		
#define DO4_POUT				GPIO_Pin_7

#define DO5_OUT_GRP			GPIOB
#define DO5_POUT				GPIO_Pin_0	// 输出5

#define DO6_OUT_GRP			GPIOB
#define DO6_POUT				GPIO_Pin_1

#define DO7_OUT_GRP			GPIOC
#define DO7_POUT				GPIO_Pin_8	// 输出7

#define DO8_OUT_GRP			GPIOC
#define DO8_POUT				GPIO_Pin_9

#ifndef HAS_8I8O
#define DO9_OUT_GRP			GPIOC
#define DO9_POUT				GPIO_Pin_0

#define DO10_OUT_GRP		GPIOC
#define DO10_POUT			GPIO_Pin_1

#define DO11_OUT_GRP		GPIOC
#define DO11_POUT			GPIO_Pin_2

#define DO12_OUT_GRP		GPIOC
#define DO12_POUT			GPIO_Pin_3
#endif

/*485输出配置*/
#define RS485_1_OUT_GRP			GPIOB
#define RS485_1_POUT				GPIO_Pin_7	// 485控制1

#define RS485_2_OUT_GRP			GPIOB
#define RS485_2_POUT				GPIO_Pin_6	// 485控制2

#define CHANNEL_0		0
#define CHANNEL_1		1
#define CHANNEL_2		2
#define CHANNEL_3		3


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
#define PROACT		BIT1
#define FORCEACT		BIT2

#define REMOTE_ACTED		BIT3		/*标识遥控操作继电器动作过*/
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
	DO_5,		// bit4
	DO_6,		// bit5
	DO_7,		// bit6
	DO_8,		// bit7

#ifndef HAS_8I8O
	DO_9,		// bit8
	DO_10,		// bit9
	DO_11,		// bit10
	DO_12,		// bit11
#endif

	RS485_CTRL_1,
	RS485_CTRL_2,

	DO_NUM
}DEVICE_CTRL_LIST;


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
void rs485FuncSelect(DEVICE_CTRL_LIST seq,bool value);

#endif


