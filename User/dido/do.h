
#ifndef _DO_H
#define _DO_H

/*正向,打开,  负向-关上*/
#define DIR_CLOSE			0
#define DIR_OPEN			1

#define PULSE_LEN			(5)		// 100/20,最长100ms, 20ms定时器递减

/*Low level, e.g. if want to up the BAR, set: BAR_UP = 0 */
#define DO1_OUT_POS_GRP			GPIOC
#define DO1_POS_POUT			GPIO_Pin_14	// 正向输出1
#define DO1_OUT_NEG_GRP			GPIOC
#define DO1_NEG_POUT				GPIO_Pin_15	//反向输出1

#define DO2_OUT_POS_GRP			GPIOC
#define DO2_POS_POUT			GPIO_Pin_0	// 正向输出2
#define DO2_OUT_NEG_GRP			GPIOC
#define DO2_NEG_POUT				GPIO_Pin_1	//反向输出2

#define DO3_OUT_POS_GRP			GPIOC
#define DO3_POS_POUT			GPIO_Pin_2	// 正向输出3
#define DO3_OUT_NEG_GRP			GPIOC
#define DO3_NEG_POUT				GPIO_Pin_3	//反向输出3

#define DO4_OUT_POS_GRP			GPIOA
#define DO4_POS_POUT			GPIO_Pin_0	// 正向输出4
#define DO4_OUT_NEG_GRP			GPIOA
#define DO4_NEG_POUT				GPIO_Pin_1	//反向输出4

#define DO5_OUT_POS_GRP			GPIOA
#define DO5_POS_POUT			GPIO_Pin_6	// 正向输出5
#define DO5_OUT_NEG_GRP			GPIOA
#define DO5_NEG_POUT				GPIO_Pin_7	//反向输出5

#define DO6_OUT_POS_GRP			GPIOC
#define DO6_POS_POUT			GPIO_Pin_4	// 正向输出6
#define DO6_OUT_NEG_GRP			GPIOC
#define DO6_NEG_POUT				GPIO_Pin_5	//反向输出6

#define DO7_OUT_POS_GRP			GPIOB
#define DO7_POS_POUT			GPIO_Pin_0	// 正向输出7
#define DO7_OUT_NEG_GRP			GPIOB
#define DO7_NEG_POUT				GPIO_Pin_1	//反向输出7

#define DO8_OUT_POS_GRP			GPIOC
#define DO8_POS_POUT			GPIO_Pin_6	// 正向输出8
#define DO8_OUT_NEG_GRP			GPIOC
#define DO8_NEG_POUT				GPIO_Pin_7	//反向输出8

#ifndef HAS_8I8O
#define DO9_OUT_POS_GRP			GPIOC
#define DO9_POS_POUT			GPIO_Pin_8	// 正向输出9
#define DO9_OUT_NEG_GRP			GPIOC
#define DO9_NEG_POUT				GPIO_Pin_9	//反向输出9

#define DO10_OUT_POS_GRP		GPIOA
#define DO10_POS_POUT			GPIO_Pin_11	// 正向输出10
#define DO10_OUT_NEG_GRP		GPIOA
#define DO10_NEG_POUT			GPIO_Pin_12	//反向输出10

#define DO11_OUT_POS_GRP		GPIOB
#define DO11_POS_POUT			GPIO_Pin_6	// 正向输出11
#define DO11_OUT_NEG_GRP		GPIOB
#define DO11_NEG_POUT			GPIO_Pin_7	//反向输出11

#define DO12_OUT_POS_GRP		GPIOB
#define DO12_POS_POUT			GPIO_Pin_8	// 正向输出12
#define DO12_OUT_NEG_GRP		GPIOB
#define DO12_NEG_POUT			GPIO_Pin_9	//反向输出12
#endif

/*485输出配置，已经芯片自动选择了*/
//#define RS485_1_OUT_GRP			GPIOB
//#define RS485_1_POUT				GPIO_Pin_7	// 485控制1

//#define RS485_2_OUT_GRP			GPIOB
//#define RS485_2_POUT				GPIO_Pin_6	// 485控制2

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
	/*正向或者负向都会赋值过来*/
	GPIO_TypeDef* gpio_grp;
	uint16_t gpio_pin;
	/*正向的端口*/
	GPIO_TypeDef* gpio_p_grp;
	uint16_t gpio_p_pin;
	/*负向的端口*/
	GPIO_TypeDef* gpio_n_grp;
	uint16_t gpio_n_pin;
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
void DO_Queue_Init(UINT8 direction);
void DeviceX_Activate(DEVICE_CTRL_LIST dev, UINT8 direction);
void DeviceX_Deactivate(DEVICE_CTRL_LIST dev,UINT8 direction);
//void rs485FuncSelect(DEVICE_CTRL_LIST seq,bool value);
void Do_Status_Save(void);
void Do_Status_Update(void);
void Get_Do_Status_And_Output(void);
void  Do_Output_Start_End(UINT8 start_num, UINT8 end_num, UINT8 out_st);
void DO_All_Output(UINT8 direction);

#endif


