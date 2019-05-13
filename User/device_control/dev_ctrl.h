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






#define GREEN 		1
#define RED 			0

#define OPT_OK		0
#define OPT_FAIL		1


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
}DEVICE_CONTROL_WORD;





extern DEVICE_STATUS_BITS device_status_used;
//extern DEVICE_CONTROL_BITS device_control_used;
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





uint8_t Command_Get(DEVICE_CTRL_LIST dev);
bool detect_ALG_TTL_working(void);
uint8_t Status_Get(DEVICE_STATUS_LIST dev);
void params_modify_deal(void);
void control_device_expt_lg(void);
void control_device(void);
void Command_Act(uint8_t act_word);

#endif /* __DEV_CTRL_H */

