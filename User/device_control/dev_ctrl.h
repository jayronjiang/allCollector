#ifndef __DEV_CTRL_H
#define	__DEV_CTRL_H

/* λ�������, ʹ��2���ֽ�*/
typedef enum 
{	
	USED = 0,
	BACKUP,

	BIT_ARR_NUM
}BIT_ARR_LIST;

/*GPIO����ö��,����չ*/
typedef enum 
{	
	GPIO_IN = 0,
	GPIO_OUT,

	GPIO_TYPE_NUM
}GPIO_TYPE_LIST;


/*Low level, e.g. if want to up the BAR, set: BAR_UP = 0 */
#define BAR_UP_OUT_GRP			GPIOA
#define BAR_UP_POUT				GPIO_Pin_0	//������

#define BAR_DOWN_OUT_GRP		GPIOA
#define BAR_DOWN_POUT			GPIO_Pin_1 	// ������

/*����Ƹĵ�O7����*/
#define TTL_GREEN_OUT_GRP		GPIOC
#define TTL_GREEN_POUT			GPIO_Pin_8	// �����

/*���ⱨ��ͳһ��O6����*/
#define VOX_ALM_OUT_GRP			GPIOB
#define VOX_ALM_POUT			GPIO_Pin_1	// ����������

#define LIGHT_ALM_OUT_GRP		GPIOB
#define LIGHT_ALM_POUT			GPIO_Pin_1	// �ⱨ����

/*O8���Ų���*/
#define LAN_LAMP_OUT_GRP		GPIOC
#define LAN_LAMP_POUT			GPIO_Pin_9	// ����ͨ�ŵ�

/*�µĵ�·�屸��1,2�Ƶ���ԭ����TTL��VOXλ��,TTL/VOX˳�ƺ���*/
/*ע��: ��Ϊ����λ����Э��û�б�,���Խṹ���λ���岻�ܸı�*/
/*ֻ��Ҫ�ı�������������Ŷ���Ϳ�����*/
#define BAK1_USED_OUT_GRP		GPIOA   		
#define BAK1_USED_POUT			GPIO_Pin_6  	// ����1

#define BAK2_USED_OUT_GRP		GPIOA		
#define BAK2_USED_POUT			GPIO_Pin_7	// ����2


/*���벿�ֶ���*/
#define TTL_IN_GRP				GPIOC		// PC0
#define TTL_PIN					GPIO_Pin_0	//�����״̬����

#define LG_IN_GRP				GPIOC		// PC1
#define LG_PIN					GPIO_Pin_1	//����״̬����,ֻ��������λ��,�������߼�

#define ALARM_IN_GRP				GPIOC		// PC2
#define ALARM_PIN				GPIO_Pin_2	// �������

#define ALG_IN_GRP				GPIOC		// PC3
#define ALG_PIN					GPIO_Pin_3	// �Զ�����״̬,��ӳ�����仹��̧

#define FRONT_IN_GRP				GPIOC		// PC4
#define FRONT_PIN				GPIO_Pin_4	// ǰ��Ȧ,ֻ��������λ��,�������߼�

#define BACK_COIN_IN_GRP			GPIOC		// PC5
#define BACK_COIN_PIN			GPIO_Pin_5	// ����Ȧ

#define BAK1_IN_GRP				GPIOC
#define BAK1_PIN					GPIO_Pin_6

#define BAK2_IN_GRP				GPIOC
#define BAK2_PIN					GPIO_Pin_7

#define DEVICE_ALL_PIN			(TTL_PIN |LG_PIN |ALARM_PIN |ALG_PIN |FRONT_PIN |BACK_COIN_PIN |BAK1_PIN |BAK2_PIN)


#define GREEN 		1
#define RED 			0

#define OPT_OK		0
#define OPT_FAIL		1


/*״̬��ЧΪ1, ��ЧΪ0*/
typedef union{
	struct
	{
		uint16_t TTL_detect_bit:1;	//�����״̬
		uint16_t LG_detect_bit:1;	// 
		uint16_t Alarm_detect_bit:1;	// 
		uint16_t ALG_detect_bit:1;	// �Զ�����״̬1: ̧��״̬,0:����״̬
		uint16_t FRONT_coil_bit:1;	// ǰ��Ȧ
		uint16_t BACK_coil_bit:1;	// ����Ȧ
		uint16_t bakup_detect1_bit:1;
		uint16_t bakup_detect2_bit:1;
		
		uint16_t unused0:1;
		uint16_t unused1:1;
		uint16_t ALG_work_status:1;	// bit2, 0: ���˲�������,1:���˲���ʧ��
		uint16_t unused3:1;
		uint16_t unused4:1;
		uint16_t unused5:1;
		uint16_t TTL_work_status:1;		//bit6, 0: ����Ʋ�������,1:����Ʋ���ʧ��
		uint16_t unused7:1;
	}status_bits;
	/* ������������ʽ����,������������2�����������ͬһ����ַ*/
	uint8_t status_word[BIT_ARR_NUM];
}DEVICE_STATUS_BITS;

/*�߼�ֵ, ʹ�����Ϊ1����ʹ��Ϊ0*/
typedef union{
	struct
	{
		uint16_t ALG_up_bit:1;		/*̧����*/
		uint16_t ALG_down_bit:1;	/* ������*/
		uint16_t TTL_green_bit:1;	/* �����*/
		uint16_t VOX_alarm_bit:1;	/* ��������*/
		uint16_t Light_alarm_bit:1;	/* �ⱨ��*/
		uint16_t Lane_lamp_bit:1;	/* ����ͨ�ŵ�,0:���,1:�̵�*/
		uint16_t control_bak1_bit:1;
		uint16_t control_bak2_bit:1;

		uint16_t ALG_control_mode:1;	/*bit0,�Զ������˹�����*/
		uint16_t FEE_display_bit:1;		/*bit1,1:�Ѷ���ʾ,0:�Ѷ��ʾ*/
		uint16_t SPEAK_control_bit:1;	/*bit2,1:����,0:������*/
		uint16_t FEE_displaykeep_bit:1; /*bit3,���ڹ��ܣ��̵ƺ󣬷�����Ȼ����*/
		uint16_t bFeeDispSelection:1;	/*bit4,����ѡ��1-�ޱ��ֹ��ܵĹ㶫�ð汾��0-�б��ֹ��ܵ�ɽ���汾*/
		uint16_t bControlOption:1;		/*bit5,������ʹ������: 1-����, 0-���*/
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

/*�豸����ö��,����չ*/
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

/*����ö��,����չ*/
typedef enum 
{	
	BAR_UP = 0,
	BAR_DOWN,		// bit1
	TTL_GREEN,		// bit2
	VOX_ALM,		// bit3
	LIGHT_ALM,		// bit4, ��VOX������,���ﱣ��,ռ��һ��λ
	LAN_LAMP,		// bit5
	BAK1_USED,		//������õ��Ѿ�������Ϊ���ܻ���̧�ܵ�ѡ���߼�����
	BAK2_USED,		// bit7

	DEV_CTRL_NUM
}DEVICE_CTRL_LIST;


extern DEVICE_STATUS_BITS device_status_used;
extern DEVICE_CONTROL_BITS device_control_used;
//extern DEVICE_CONTROL_BITS device_control_backup;
extern DEVICE_CTRL device_ctrl_queue[DEV_CTRL_NUM];
extern DEVICE_STATUS device_status_queue[DEV_STATUS_NUM];

/* ָʾ���ڷ���������ƻ����̵�*/
extern uint8_t bLastLaneRedGreenOperateState;
extern bool LastLaneLampState;
//extern bool watching_car_passing;		// ����Ȧ����
extern bool autoBarEnable;
extern bool bFeeCleared;
extern bool dete_bit_recd;
extern bool ALG_down_flag_bit;
extern bool ALG_up_flag_bit;

extern uint16_t WatchingDelayCount;
extern uint16_t detect_time_counter;		//ϵͳ���������ʱ��
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

