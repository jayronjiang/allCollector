
#ifndef _DI_H
#define _DI_H

/*输入部分定义*/
#define DI1_GRP				GPIOC		// PC0
#define DI1_PIN				GPIO_Pin_0

#define DI2_GRP				GPIOC		// PC1
#define DI2_PIN				GPIO_Pin_1	

#define DI3_GRP				GPIOC		// PC2
#define DI3_PIN				GPIO_Pin_2

#define DI4_GRP				GPIOC		// PC3
#define DI4_PIN				GPIO_Pin_3

#define DI5_GRP				GPIOC		// PC4
#define DI5_PIN				GPIO_Pin_4

#define DI6_GRP				GPIOC		// PC5
#define DI6_PIN				GPIO_Pin_5

#define DI7_GRP				GPIOC
#define DI7_PIN				GPIO_Pin_6

#define DI8_GRP				GPIOC
#define DI8_PIN				GPIO_Pin_7

typedef struct device_in_struct
{
	GPIO_TypeDef* gpio_grp;
	uint16_t gpio_pin;
}DEVICE_STATUS;

/*类型枚举,可扩展*/
typedef enum 
{	
	DI_1 = 0,
	DI_2,
	DI_3,
	DI_4,
	DI_5,
	DI_6,
	DI_7,
	DI_8,

	DI_NUM
}DEVICE_STATUS_LIST;


/*状态有效为1, 无效为0*/
typedef union{
	struct
	{
		uint16_t di_1:1;	//8个DI
		uint16_t di_2:1;
		uint16_t di_3:1;
		uint16_t di_4:1;
		uint16_t di_5:1;
		uint16_t di_6:1;
		uint16_t di_7:1;
		uint16_t di_8:1;
	}status_bits;
	uint16_t status_word;
}DEVICE_STATUS_BITS;

extern uint16_t detect_time_counter;		//系统参数检查间隔时间
extern DEVICE_STATUS device_status_queue[DI_NUM]; //设备状态配置组
extern DEVICE_STATUS_BITS di_status;	// 外部状态字,检测外部设备的状态
extern uint16_t  input[DI_NUM];	// DI的整形值,用在MODBUS中

void DI_Init(void);
void DEVICE_GPIO_IN_Config(DEVICE_STATUS_LIST dev);
void DI_Queue_Init(void);
#endif


