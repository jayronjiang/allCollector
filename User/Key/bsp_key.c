 /**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   �ⲿ�����ɨ��ͷ���,25msɨ��һ��.
  *
  ******************************************************************************
  */
#include "include.h"

/******************************************************************************
 * ������:	GPIO_ReadInputAll 
 * ����:  	 һ�ΰ����еļ�ֵ������
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.16
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
uint16_t GPIO_ReadInputAll(uint8_t polar)
{
	uint16_t key_val = 0;
	uint16_t tmp = 0;
	DEVICE_STATUS_LIST dev_status = DI_1;

	for( dev_status=DI_1; dev_status < DI_NUM; dev_status++)
	{

		tmp = GPIO_ReadInputDataBit(device_status_queue[dev_status].gpio_grp,
										device_status_queue[dev_status].gpio_pin);

		key_val |= tmp << dev_status;	// �ڼ�����ֵ�����Ƽ�λ.
	}
	if ( polar == LOW_POLAR )
	{
		key_val = key_val^0x00FF;		// �ѵ�8λ�ĵ���Чȫ��תΪ����Ч
	}

	return key_val;
}
 
/******************************************************************************
 * ������:	ReadKey 
 * ����:  	��ȡ�ⲿ����״̬
 * 
 * �������: 
 * �������: key_effective_port,��ӦλΪ1������ȡ���ζ�Ϊ1
 *				��ӦλΪ0������ȡ���ζ�Ϊ0.
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.16
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void ReadKey(void)
{
	static uint16_t debounce_buffer[DEBOUNCE_TIME];	//����һ�ξͿ���
	static uint8_t buffer_index = 0;
	/*��Ϊ��ʼʱ�ⲿĬ��Ϊ�ߵ�ƽ,���Գ�ʼ��Ϊ0xFF */
	/*�����ʼʱ�ⲿΪ�͵�ƽ,Ҫ��ʼ��Ϊ0, ����ʼ����һ�α�λ */
	static uint16_t key_effective_port_last= 0xFF; /*�ϴξ�����������ʵ�����˿�״̬*/
	static uint16_t key_effective_port = 0xFF;	/*��ʾ����������ȷ�ϵõ��İ���״̬*/
	uint16_t constant_high = 0xFF;	/*����ӦλΪ1������ȡ�ļ�����Ӧλ��Ϊ1*/
	uint16_t constant_low = 0xFF;	/*����ӦλΪ1������ȡ�ļ�����Ӧλ��Ϊ0*/

	uint8_t i = 0;

	/*��ȡ���μ�ֵ,�±�����1*/
	debounce_buffer[buffer_index] = GPIO_ReadInputAll(HIGH_POLAR);
	if (++buffer_index >= DEBOUNCE_TIME)
	{
		buffer_index = 0;
	}

	/*ͬʱ��ȡ����Ч�͵���Ч��2��ֵ*/
	for (i = 0; i < DEBOUNCE_TIME; i++) 
	{
		constant_high &= debounce_buffer[i];
		constant_low &= ~debounce_buffer[i];
	}

	/*�ȵõ�ȫΪ1��λ*/
	/*�ٵõ�ȫΪ0��λ*/
	key_effective_port |= (constant_high&0xFF);
	key_effective_port &= ((~constant_low)&0xFF);

	di_status.status_word = key_effective_port;
	
	if (key_effective_port !=key_effective_port_last)
	{
		system_flag |= KEY_CHANGED;	// ���������ر�λ
		key_effective_port_last = key_effective_port;
	}
}

/***********************************************************************************
 * ������:	RCC_Clock_Set 
 * ����: 
 *           	-���������ʱ��
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
void RCC_Clock_Set(GPIO_TypeDef* GPIOx, FunctionalState iState)
{
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	
	if (GPIOx == GPIOA)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, iState); 
	}
	else if (GPIOx == GPIOB)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, iState); 
	}
	else if (GPIOx == GPIOC)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, iState); 
	}
	else if (GPIOx == GPIOD)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, iState); 
	}
	else if (GPIOx == GPIOE)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, iState); 
	}
	else if (GPIOx == GPIOF)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF, iState); 
	}
	else
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG, iState); 
	}
}


#if 0
/******************************************************************************
 * ������:	NVIC_Configuration 
 * ����:  	�ⲿ�жϵ�����
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.16
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* �����ж�Դ */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************
 * ������:	EXTI_PE4_Config 
 * ����:  	����PE4Ϊ�ⲿ�ж�
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.16
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void EXTI_PE4_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure; 
	EXTI_InitTypeDef EXTI_InitStructure;

	/* config the extiline clock and AFIO clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO,ENABLE);
											
	/* config the NVIC */
	NVIC_Configuration();

	/* EXTI line gpio config*/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;       		// PE4
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // ��������
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* EXTI line mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //�½����ж�
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
}

/******************************************************************************
 * ������:	EXTI4_IRQHandler 
 * ����:  	�ⲿ�����ж�, PE4
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.16
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
 // bug:���ﰴ���е�����,�����غ��½��ض��������ж�
void EXTI4_IRQHandler(void)
{
	Delay_Ms(20);
	if(EXTI_GetITStatus(EXTI_Line4) != RESET) //ȷ���Ƿ������EXTI Line�ж�
	{
		// LED1 ȡ��		
		LED1_TOGGLE;
		printf("USART1\n");
		EXTI_ClearITPendingBit(EXTI_Line4);     //����жϱ�־λ
	}  
}
#endif

