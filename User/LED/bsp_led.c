 /**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   ����������ָʾ�йص�����.
  *
  ******************************************************************************
  */

#include "include.h"

/******************************************************************************
 * ������:	LED_GPIO_Config 
 * ����:  	LED�Ƶĳ�ʼ��
 * 			LED1: PB5,     LED2:PE5
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void LED_GPIO_Config(void)	
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); // ʹ��PC�˿�ʱ��  
  RCC_Clock_Set(LED1_GRP, ENABLE);

  // LED1	
  GPIO_InitStructure.GPIO_Pin = (LED1_POUT);	//ѡ���Ӧ������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED1_GRP, &GPIO_InitStructure);  //��ʼ��PC�˿�

  // LED2
  //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); // ʹ��PC�˿�ʱ��  
  RCC_Clock_Set(LED2_GRP, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = (LED2_POUT);	//ѡ���Ӧ������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED2_GRP, &GPIO_InitStructure);  //��ʼ��PC�˿�

  /* �ر�����LED*/
  LED_Set(LED_1, OFF);
  LED_Set(LED_2, OFF);
}

/******************************************************************************
 * ������:	LED_Flashing 
 * ����:  	LED_No��˸flashtime��,ÿ����mtime,��mtime.
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
void LED_Flashing(uint16_t LED_No,uint16_t mtime, uint16_t flashtime)
{
	do
	{
		switch (LED_No)
		{
		case (LED_1): 
	     		LED1(ON);
			DelayAndFeedDog(mtime);
			LED1(OFF);
			DelayAndFeedDog(mtime);
	      		break;

		case (LED_2): 
	     		LED2(ON);
			DelayAndFeedDog(mtime);
			LED2(OFF);
			DelayAndFeedDog(mtime);
	      		break;

		default: 
			break;
		}
	}while(--flashtime>0);
}

/******************************************************************************
 * ������:	LED_Set 
 * ����:  	����LED_No��������
 *
 * �������: iState: ENABLE, ��,   DISABLE: ��
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
void LED_Set(uint16_t LED_No, FunctionalState iState)
{
	switch (LED_No)
	{
	case (LED_1): 
	     	LED1(iState);
	      	break;

	case (LED_2): 
	     	LED2(iState);
	      	break;

	//case (LED_3): 
	//     	LED3(iState);
	//      	break;

	default: 
		break;
	}
}


