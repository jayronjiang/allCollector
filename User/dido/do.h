
#ifndef _DO_H
#define _DO_H

/*����,��,  ����-����*/
#define DIR_CLOSE			0
#define DIR_OPEN			1

#define PULSE_LEN			(5)		// 100/20,�100ms, 20ms��ʱ���ݼ�

/*Low level, e.g. if want to up the BAR, set: BAR_UP = 0 */
#define DO1_OUT_POS_GRP			GPIOC
#define DO1_POS_POUT			GPIO_Pin_14	// �������1
#define DO1_OUT_NEG_GRP			GPIOC
#define DO1_NEG_POUT				GPIO_Pin_15	//�������1

#define DO2_OUT_POS_GRP			GPIOC
#define DO2_POS_POUT			GPIO_Pin_0	// �������2
#define DO2_OUT_NEG_GRP			GPIOC
#define DO2_NEG_POUT				GPIO_Pin_1	//�������2

#define DO3_OUT_POS_GRP			GPIOC
#define DO3_POS_POUT			GPIO_Pin_2	// �������3
#define DO3_OUT_NEG_GRP			GPIOC
#define DO3_NEG_POUT				GPIO_Pin_3	//�������3

#define DO4_OUT_POS_GRP			GPIOA
#define DO4_POS_POUT			GPIO_Pin_0	// �������4
#define DO4_OUT_NEG_GRP			GPIOA
#define DO4_NEG_POUT				GPIO_Pin_1	//�������4

#define DO5_OUT_POS_GRP			GPIOA
#define DO5_POS_POUT			GPIO_Pin_6	// �������5
#define DO5_OUT_NEG_GRP			GPIOA
#define DO5_NEG_POUT				GPIO_Pin_7	//�������5

#define DO6_OUT_POS_GRP			GPIOC
#define DO6_POS_POUT			GPIO_Pin_4	// �������6
#define DO6_OUT_NEG_GRP			GPIOC
#define DO6_NEG_POUT				GPIO_Pin_5	//�������6

#define DO7_OUT_POS_GRP			GPIOB
#define DO7_POS_POUT			GPIO_Pin_0	// �������7
#define DO7_OUT_NEG_GRP			GPIOB
#define DO7_NEG_POUT				GPIO_Pin_1	//�������7

#define DO8_OUT_POS_GRP			GPIOC
#define DO8_POS_POUT			GPIO_Pin_6	// �������8
#define DO8_OUT_NEG_GRP			GPIOC
#define DO8_NEG_POUT				GPIO_Pin_7	//�������8

#ifndef HAS_8I8O
#define DO9_OUT_POS_GRP			GPIOC
#define DO9_POS_POUT			GPIO_Pin_8	// �������9
#define DO9_OUT_NEG_GRP			GPIOC
#define DO9_NEG_POUT				GPIO_Pin_9	//�������9

#define DO10_OUT_POS_GRP		GPIOA
#define DO10_POS_POUT			GPIO_Pin_11	// �������10
#define DO10_OUT_NEG_GRP		GPIOA
#define DO10_NEG_POUT			GPIO_Pin_12	//�������10

#define DO11_OUT_POS_GRP		GPIOB
#define DO11_POS_POUT			GPIO_Pin_6	// �������11
#define DO11_OUT_NEG_GRP		GPIOB
#define DO11_NEG_POUT			GPIO_Pin_7	//�������11

#define DO12_OUT_POS_GRP		GPIOB
#define DO12_POS_POUT			GPIO_Pin_8	// �������12
#define DO12_OUT_NEG_GRP		GPIOB
#define DO12_NEG_POUT			GPIO_Pin_9	//�������12
#endif

/*485������ã��Ѿ�оƬ�Զ�ѡ����*/
//#define RS485_1_OUT_GRP			GPIOB
//#define RS485_1_POUT				GPIO_Pin_7	// 485����1

//#define RS485_2_OUT_GRP			GPIOB
//#define RS485_2_POUT				GPIO_Pin_6	// 485����2

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

#define REMOTE_ACTED		BIT3		/*��ʶң�ز����̵���������*/
#define PRO_ACTED		BIT4		/*��������ʶ*/


#define PRESET_15S	750		/*15��,20ms�ж�*/


typedef struct device_out_struct
{
	/*������߸��򶼻ḳֵ����*/
	GPIO_TypeDef* gpio_grp;
	uint16_t gpio_pin;
	/*����Ķ˿�*/
	GPIO_TypeDef* gpio_p_grp;
	uint16_t gpio_p_pin;
	/*����Ķ˿�*/
	GPIO_TypeDef* gpio_n_grp;
	uint16_t gpio_n_pin;
}DEVICE_CTRL;

/*����ö��,����չ*/
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

	UINT16 ActFlag;		/*BIT0 ң�� BIT1��ֵԽ��,BIT2,���ǿ��*/
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


