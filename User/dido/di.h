
#ifndef _DI_H
#define _DI_H

/*���벿�ֶ���*/
#define DI1_GRP				GPIOC		// PC0
#define DI1_PIN				GPIO_Pin_0

#define DI2_GRP				GPIOC		// PC1
#define DI2_PIN				GPIO_Pin_1	

#define DI3_GRP				GPIOC		// PC2
#define DI3_PIN				GPIO_Pin_2

#define DI4_GRP				GPIOC		// PC3
#define DI4_PIN				GPIO_Pin_3

// DI5-DI8��ʱδʹ��
#define DI5_GRP				GPIOC		// PC4
#define DI5_PIN				GPIO_Pin_4

#define DI6_GRP				GPIOC		// PC5
#define DI6_PIN				GPIO_Pin_5

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

/*����ö��,����չ*/
typedef enum 
{	
	DI_1 = 0,
	DI_2,		// bit1
	DI_3,		// bit2
	DI_4,		// bit3

	DI_NUM
}DEVICE_STATUS_LIST;


/*״̬��ЧΪ1, ��ЧΪ0*/
typedef union{
	struct
	{
		uint16_t di_1:1;	//�°�ֻ��4��DI��
		uint16_t di_2:1;
		uint16_t di_3:1;
		uint16_t di_4:1;
		
		uint16_t unused0:1;
		uint16_t unused1:1;
		uint16_t unused2:1;
		uint16_t unused3:1;
	}status_bits;
	uint8_t status_word;
}DEVICE_STATUS_BITS;

extern uint16_t detect_time_counter;		//ϵͳ���������ʱ��
extern DEVICE_STATUS device_status_queue[DI_NUM]; //�豸״̬������
extern DEVICE_STATUS_BITS di_status;	// �ⲿ״̬��,����ⲿ�豸��״̬

void DI_Init(void);
void DEVICE_GPIO_IN_Config(DEVICE_STATUS_LIST dev);
void DO_Queue_Init(void);
#endif


