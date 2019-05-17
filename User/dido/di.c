
#include "include.h"

DEVICE_STATUS device_status_queue[DI_NUM]; //�豸״̬������
DEVICE_STATUS_BITS di_status;	// �ⲿ״̬��,����ⲿ�豸��״̬

uint16_t detect_time_counter = AUTO_DETCET_TIME;	//ϵͳ���������ʱ��
/***********************************************************************************
 * ������:	DEVICE_GPIO_IN_Config 
 * ����: 
 *           	-����ָ����������.
 *		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:20181109
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 *
 *
 ***********************************************************************************/
void DEVICE_GPIO_IN_Config(DEVICE_STATUS_LIST dev)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	  /*�������밴���˿ڣ�PX����ʱ��*/
	RCC_Clock_Set(device_status_queue[dev].gpio_grp, ENABLE);
 	GPIO_InitStructure.GPIO_Pin = device_status_queue[dev].gpio_pin;; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // ��������
	GPIO_Init(device_status_queue[dev].gpio_grp, &GPIO_InitStructure);
}

/******************************************************************************
 * ������:	DO_Init 
 * ����: 
 *            -DOģ���ʼ������
 * �������: ��
 * �������: ��
 * ����ֵ: ��
 * 
 * ����:Jerry
 * ��������:2013.02.20
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void DI_Init(void)
{
	/*I/O�ڳ�ʼ��*/
	DI_Queue_Init();
	//y_First_Read(); 
}

/***********************************************************************************
 * ������:	Device_Stat_Queue_Init 
 * ����: 
 *           	-���贫������GPIO��ʼ��,��ʼ����IO���йصĵ�һ���ֽ�.
 *		���������ֲʱ��Ҫ�ı�
 *		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:20181109
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 *
 *
 ***********************************************************************************/
void DI_Queue_Init(void)
{
	DEVICE_STATUS_LIST dev_type = DI_1;

	for( dev_type=DI_1; dev_type<DI_NUM; dev_type++)
	{
		switch(dev_type)
		{
		case DI_1:
			device_status_queue[dev_type].gpio_grp = DI1_GRP;
			device_status_queue[dev_type].gpio_pin = DI1_PIN;
			break;

		case DI_2:
			device_status_queue[dev_type].gpio_grp = DI2_GRP;
			device_status_queue[dev_type].gpio_pin = DI2_PIN;
			break;

		case DI_3:
			device_status_queue[dev_type].gpio_grp = DI3_GRP;
			device_status_queue[dev_type].gpio_pin = DI3_PIN;
			break;

		case DI_4:
			device_status_queue[dev_type].gpio_grp = DI4_GRP;
			device_status_queue[dev_type].gpio_pin = DI4_PIN;
			break;

		case DI_5:
			device_status_queue[dev_type].gpio_grp = DI5_GRP;
			device_status_queue[dev_type].gpio_pin = DI5_PIN;
			break;

		case DI_6:
			device_status_queue[dev_type].gpio_grp = DI6_GRP;
			device_status_queue[dev_type].gpio_pin = DI6_PIN;
			break;

		case DI_7:
			device_status_queue[dev_type].gpio_grp = DI7_GRP;
			device_status_queue[dev_type].gpio_pin = DI7_PIN;
			break;

		case DI_8:
			device_status_queue[dev_type].gpio_grp = DI8_GRP;
			device_status_queue[dev_type].gpio_pin = DI8_PIN;
			break;

		default:
			break;
		}
		DEVICE_GPIO_IN_Config(dev_type);
	}
}

