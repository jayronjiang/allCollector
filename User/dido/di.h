
#ifndef _DI_H
#define _DI_H

/*输入部分定义*/
#define DI1_GRP				GPIOC		// PC0
#define DI1_PIN				GPIO_Pin_0	//雨棚灯状态反馈

#define DI2_GRP				GPIOC		// PC1
#define DI2_PIN				GPIO_Pin_1	//栏杆状态反馈,只反馈给上位机,不参与逻辑

#define DI3_GRP				GPIOC		// PC2
#define DI3_PIN				GPIO_Pin_2	// 报警检测

#define DI4_GRP				GPIOC		// PC3
#define DI4_PIN					GPIO_Pin_3	// 自动栏杆状态,反映栏杆落还是抬

#define DI5_GRP				GPIOC		// PC4
#define DI5_PIN				GPIO_Pin_4	// 前线圈,只反馈给上位机,不参与逻辑

#define DI6_GRP				GPIOC		// PC5
#define DI6_PIN				GPIO_Pin_5	// 后线圈

#define DI7_GRP				GPIOC
#define DI7_PIN				GPIO_Pin_6

#define DI8_GRP				GPIOC
#define DI8_PIN				GPIO_Pin_7

//#define DEVICE_ALL_PIN		(DI1_PIN |DI2_PIN |DI3_PIN |DI4_PIN |DI5_PIN |DI6_PIN |DI7_PIN |DI8_PIN)

typedef struct device_in_struct
{
	GPIO_TypeDef* gpio_grp;
	uint16_t gpio_pin;
}DEVICE_STATUS;

/*类型枚举,可扩展*/
typedef enum 
{	
	DI_1 = 0,
	DI_2,		// bit1
	DI_3,		// bit2
	DI_4,		// bit3
	DI_5,		// bit4, 和VOX共用了,这里保留,占有一个位
	DI_6,		// bit5
	DI_7,		//这个备用的已经用来作为降杠或者抬杠的选择逻辑开关
	DI_8,		// bit7

	DI_NUM
}DEVICE_STATUS_LIST;


/*状态有效为1, 无效为0*/
typedef union{
	struct
	{
		uint16_t di_1:1;	//雨棚灯状态
		uint16_t di_2:1;	// 
		uint16_t di_3:1;	// 
		uint16_t di_4:1;	// 自动栏杆状态1: 抬杠状态,0:降杠状态
		uint16_t di_5:1;	// 前线圈
		uint16_t di_6:1;	// 后线圈
		uint16_t di_7:1;
		uint16_t di_8:1;
		
		uint16_t unused0:1;
		uint16_t unused1:1;
		uint16_t unused2:1;
		uint16_t unused3:1;
		uint16_t unused4:1;
		uint16_t unused5:1;
		uint16_t unused6:1;
		uint16_t unused7:1;
	}status_bits;
	/* 必须以数组形式定义,否则连续定义2个变量会放在同一个地址*/
	uint16_t status_word;
}DEVICE_STATUS_BITS;

extern uint16_t detect_time_counter;		//系统参数检查间隔时间
extern uint16_t alarm_time_counter;
extern DEVICE_STATUS device_status_queue[DI_NUM]; //设备状态配置组
extern DEVICE_STATUS_BITS di_status;	// 外部状态字,检测外部设备的状态

void DI_Init(void);
void DEVICE_GPIO_IN_Config(DEVICE_STATUS_LIST dev);
void DO_Queue_Init(void);
#endif


