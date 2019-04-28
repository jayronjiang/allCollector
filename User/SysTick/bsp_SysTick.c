/**
  ******************************************************************************
  * @file    bsp_SysTick.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   SysTick ϵͳ�δ�ʱ��1ms�жϺ�����,�ж�ʱ�����������.
  *          ���õ��� 1us 10us 1ms �ж�.
  ******************************************************************************
  */
  
#include "include.h"

/******************************************************************************
 * ������:	SysTick_start 
 * ����:  	����ϵͳ�δ�ʱ�� SysTick
 *
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void SysTick_start(void)
{
	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;
}

/******************************************************************************
 * ������:	SysTick_stop 
 * ����:  	ֹͣϵͳ�δ�ʱ�� SysTick
 *
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void SysTick_stop(void)
{
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Init(void)
{
	/* SystemFrequency / 1000    1ms�ж�һ��
	 * SystemFrequency / 100000	 10us�ж�һ��
	 * SystemFrequency / 1000000 1us�ж�һ��
	 */
	//if (SysTick_Config(SystemFrequency / 100000))	// ST3.0.0��汾
	if (SysTick_Config(SystemCoreClock /1000))	// ST3.5.0��汾
	{ 
		/* Capture error */ 
		while (1);
	}
		
	SysTick->CTRL &= ~ SysTick_CTRL_ENABLE_Msk;	// �رյδ�ʱ��  
}


////************* �˺�Ϊ��ʱ���򣬶�Ӧ�жϼ��Ϊ10us *********************************************************

//static __IO u32 TimingDelay;

///**
//  * @brief   us��ʱ����,10usΪһ����λ
//  * @param  
//  *		@arg nTime: Delay_us( 1 ) ��ʵ�ֵ���ʱΪ 1 * 10us = 10us
//  * @retval  ��
//  */
//void Delay_us(__IO u32 nTime)
//{ 
//	TimingDelay = nTime;	

//	// ʹ�ܵδ�ʱ��  
//	SysTick->CTRL |=  SysTick_CTRL_ENABLE_Msk;

//	while(TimingDelay != 0);
//}

///**
//  * @brief  ��ȡ���ĳ���
//  * @param  ��
//  * @retval ��
//  * @attention  �� SysTick �жϺ��� SysTick_Handler()����
//  */
//void TimingDelay_Decrement(void)
//{
//	if (TimingDelay != 0x00)
//	{ 
//		TimingDelay--;
//	}
//}
/*********************************************END OF FILE**********************/
