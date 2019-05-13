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






#define GREEN 		1
#define RED 			0

#define OPT_OK		0
#define OPT_FAIL		1


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
}DEVICE_CONTROL_WORD;





extern DEVICE_STATUS_BITS device_status_used;
//extern DEVICE_CONTROL_BITS device_control_used;
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





uint8_t Command_Get(DEVICE_CTRL_LIST dev);
bool detect_ALG_TTL_working(void);
uint8_t Status_Get(DEVICE_STATUS_LIST dev);
void params_modify_deal(void);
void control_device_expt_lg(void);
void control_device(void);
void Command_Act(uint8_t act_word);

#endif /* __DEV_CTRL_H */

