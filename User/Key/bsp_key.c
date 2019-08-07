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


/*����DIP switch�Ľӿ�pin*/
static const DEVICE_STATUS dip_switches[NUMBER_OF_DIPKEYS] = {\
    	{DIP2_GRP,DIP2_PIN},		// �ȶ���λ
	{DIP1_GRP,DIP1_PIN}
};

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
void GPIO_In_Init(DEVICE_STATUS key)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	  /*�������밴���˿ڣ�PX����ʱ��*/
	RCC_Clock_Set(key.gpio_grp, ENABLE);
 	GPIO_InitStructure.GPIO_Pin = key.gpio_pin;; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // ��������
	GPIO_Init(key.gpio_grp, &GPIO_InitStructure);
}

/******************************************************************************
 * ������:	Dip_Switch_Init 
 * ����: 
 *            -DIP switch �ĳ�ʼ��
 * �������: ��
 * �������: ��
 * ����ֵ: ��
 * 
 * ����:Jerry
 * ��������:2019.08.20
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Dip_Switch_Init(void)
{
	uint16_t i = 0;

	for( i=0; i<NUMBER_OF_DIPKEYS; i++)
	{
		GPIO_In_Init(dip_switches[i]);
	}
}

/******************************************************************************
 * ������:	user_settings_key_get 
 * ����: 
 *            -�õ�dip_switch��ֵ, Ĭ��������, ������ON��Ϊ�͵�ƽ0
 *            -Ҳ����OFF: 1,  ON: 0
 * �������: ��
 * �������: ��
 * ����ֵ: 0: OFF,OFF,  1:OFF,ON,  2:ON,OFF, 3:ON,ON
 * 
 * ����:Jerry
 * ��������:2019.08.20
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
uint16_t user_settings_key_get(void)
{
	uint16_t pin_level =0;
	uint16_t tmp =0;
	uint16_t index;

	for (index = 0; index < NUMBER_OF_DIPKEYS; index++)
	{
		pin_level <<= 1;	// ����1λ���00,01,10,11
		Delay_Ms(1);
	        tmp= GPIO_ReadInputDataBit(dip_switches[index].gpio_grp,dip_switches[index].gpio_pin);
	        pin_level |= (tmp  ? 0u : 1u);	// �͵�ƽ��ΪON
	}

	return pin_level;
}


/******************************************************************************
 * ������:	Address_Get_From_Key 
 * ����: 
 *            -DIP switch �ĳ�ʼ��
 * �������: ��
 * �������: ��
 * ����ֵ: ��
 * 
 * ����:Jerry
 * ��������:2019.08.20
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Dev_Address_Init(void)
{
	uint16_t dip_value = 0;
	Dip_Switch_Init();
	Delay_Ms(5);			// �ȴ�һ��ʱ���ȶ�
	dip_value = user_settings_key_get();
	DevParams.Address = DEV_ADDR+dip_value;
	system_flag|=DEV_MODIFIED; 	//�Ƿ��б�Ҫ�����ַ����?
}

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
	static uint16_t key_effective_port_last= 0; /*�ϴξ�����������ʵ�����˿�״̬*/
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

