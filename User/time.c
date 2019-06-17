 /**
  ******************************************************************************
  * @file    time.c
  * @author  Jerry
  * @date    21-Nov-2018
  *
  * @brief   ����ʱ���жϲ���,��������2����ʱ��TIM2(20ms), TIM4(��̬).
  *          ms��ʱ����stm32f10x_it.c�б�����.
  *
  ******************************************************************************
  */

#include "include.h"

/*ϵͳ�ĺ��뼶�ۼ�ʱ��,������Ϊ�޷�����,*/
/* ��ʱδ��ʹ��*/
uint8_t wdt_counter = 0;
uint8_t  reset_flag=0;	/*�����ĸ����־,��ʱδ��*/

volatile uint32_t system_time_ms = 0;
volatile uint32_t system_time_s = 0;

/******************************************************************************
 * ������:	Delay_Ms 
 * ����:  	������ʱ����������ms��ʱ,���ֶ�ʱ�����ڱ�������
 * 			��̫��ȷ
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
void Delay_Ms(uint32_t myMs)
{
	uint32_t i;
	while(myMs--)
	{
		// ���Ϊ9�����, ���Ϊ72M/9 = 8K.
		i=10256;
		while(i--);
	}
}

uint16_t go_onflag;
/******************************************************************************
 * ������:	Delay_Xus 
 * ����:  	ʹ�ö�ʱ����ʽ��������us��ʱ,��Delay_Xmsһ��
 * 			ʵ�����岻��,���ܾ�����΢��һ��
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
void Delay_Xus(uint16_t us)
{   
	TIM_SetCounter(TIM4,0);
	TIM_SetAutoreload(TIM4,us);
	TIM_Cmd(TIM4, ENABLE);
	while(go_onflag==0);
	TIM_Cmd(TIM4, DISABLE);
	go_onflag=0;
}

/******************************************************************************
 * ������:	Delay_Xms 
 * ����:  	����ʱ����������ms��ʱ
 * 
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
void Delay_Xms (uint16_t ms)
{
	uint16_t icount = ms;
    	for ( ; icount > 0; icount--)
    	{
	 	Delay_Xus(1000);
	 }
}


/******************************************************************************
 * ������:	DelayAndFeedDog 
 * ����:  	��ι�����ܵ���ʱ,��ʱ��ÿ20msι��һ��
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
void DelayAndFeedDog(uint32_t myMs)
{
	#if 0
	// Do not let the myMs be greater than the watchdog period.
	//  Feed the Watchdog every 20ms.
	uint32_t wcount;
	for ( wcount=0; wcount<myMs/20; wcount++)
	{
		Ext_Wdt_Feed();
		// Feed the Watchdog every 20ms.
		Delay_Ms(20);
	}
	// when jump out the loop, feed the last watchdog.
	Ext_Wdt_Feed();
	#endif
	// û���ⲿ���Ź������ÿ��ǿ��Ź����
	Delay_Ms(myMs);
}


/******************************************************************************
 * ������:	delay 
 * ����:  	delay���ٸ�ʱ������
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
void Delay_clk( uint16_t time)
{
  	while(time--)
  	{
     		 NOP();
  	}
}

 /******************************************************************************
*  ������: time_interval()
*
*  ����: ����һ�������ʱ�������뵱ǰʱ�̵�ʱ����
*
*  ����: ���Ƚϵ�ʱ�������
*
*  ���: 
*
*  ����ֵ: ʱ���ֵ
*
*  ����: 
*******************************************************************************/
uint32_t time_interval(uint32_t time0)
{
	uint32_t now_ms;

	now_ms = system_time_s;
	now_ms = now_ms - time0;

	return now_ms;
}

//*****************************************************************************
//                                Timerʵ��
//*****************************************************************************

/******************************************************************************
 * ������:	Start_Timer_x 
 * ����:  	ʹ�ܶ�ʱ��X
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
void Start_Timer_x(TIM_TypeDef* TIMx)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	TIM_Cmd(TIMx, ENABLE);
}

/******************************************************************************
 * ������:	Stop_Timer_x 
 * ����:  	ֹͣ��ʱ��X
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
void Stop_Timer_x(TIM_TypeDef* TIMx)
{
	TIM_Cmd(TIMx, DISABLE);
}

/******************************************************************************
 * ������:	Timer_Start 
 * ����:  	ʹ�����ж�ʱ��
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
void Timer_Start(void)
{
	Start_Timer_x(TIM2);
	/*�տ�ʼ���õ�50ms��ʱ����û����,ֻ����us��ʱ����*/
	/*���ú�ſ�ʼ��ʹ��,�����Ȳ���*/
	//Start_Timer_x(TIM4);
	IWDG_Enable(); // ʹ�ܿ��Ź�
}

/******************************************************************************
 * ������:	Time_NVIC_Configuration 
 * ����:  	��ʱ�����ȼ�������
 *
 * ��ʱ��2: ��ռ���ȼ�Ϊ0, �����ȼ�Ϊ0
 * ��ʱ��4: ��ռ���ȼ�Ϊ1, �����ȼ�Ϊ1
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
void Time_NVIC_Configuration(void)
{   
	NVIC_InitTypeDef NVIC_InitStructure; 

	/*TIM2����ж�*/
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 	
	// ������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	// ���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	  
	
	/*TIM4����ж�*/
	// �����ж���Դ
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
	// ������ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// ���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	  
}

/******************************************************************************
 * ������:	Time_Mode_Config 
 * ����:  	��ʼ����ʱ��2Ϊ20ms,��ʱ��4Ϊ50ms
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
void Time_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* ���ü�ʱģʽ��ģʽ�Ļ�����ʱ��TIM2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 2000-1;		/* 20ms time interrupt */
	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;		/* The time base is 72M/720 = 100K */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);	/* �������жϱ�־ */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//TIM_Cmd(TIM2, ENABLE);
	
	/* ������ʱ��ʱ��TIM4 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 50000-1;	/* 50ms interrupt */
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;	/* The time base is 72M/720 = 1M */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM4,DISABLE);

	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	/* �������жϱ�־ */
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM4, DISABLE);
}

/******************************************************************************
 * ������:	Time_Configuration 
 * ����:  	��ʼ����ʱ������
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
void Time_Configuration(void)
{
	Time_NVIC_Configuration();
	Time_Mode_Config();
}

/******************************************************************************
 * ������:	TIM4_IRQHandler 
 * ����:  ��̬��ʱ��,��ARR ����Delay_Xus�б�����
 *         			��ʱ����us��ʱ��
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
void TIM4_IRQHandler (void)
{
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	
	go_onflag=1;
}

/******************************************************************************
 * ������:	TIM2_IRQHandler 
 * ����:  20ms ��ʱ�ж�
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
void TIM2_IRQHandler (void)
{   
	static uint16_t t_100ms = 0;
	static uint16_t t_cnt = 0;
	static uint16_t t_1s = 0;
	

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	/*1s,ι��, 200ms��־λ��ʱδʹ��*/
	if( ++t_100ms>= MS_200)
	{
     		t_100ms=0;
		system_flag |= SYSTEM_200MS;
		// 1s�ӻ�û����,����ϵͳ�Ѿ�����,��ι��ֱ�Ӹ�λ
		// �����6�����õ���ʵ�Ƿ�ֹ��ѭ���ܷ�,200*6+1000-200 = 2s
	    	if (++wdt_counter <= 6) 
	    	{
	    		IWDG_Feed();  /*�ж�����ι��,�ڲ����Ź�*/
	    	}
	}

	// 5sʱ�䵽��������ѯ
	if( ++t_1s>= ONE_SECOND)
	{
		t_1s = 0;
		// ϵͳʱ��,1s��λ
		system_time_s++;
	}

	// 5sʱ�䵽��������ѯ,���Ե�ʱ����1s
	if( ++t_cnt>= ONE_SECOND)
	{
		t_cnt = 0;
		start_comm();
	}


	/* ϵͳ���״̬�Ϳ���λ�Ƿ�һ��,ÿ3s���һ��*/
	if(detect_time_counter > 0)
	{
		detect_time_counter--;
	}
	else
	{
		system_flag |= SYS_ERR_CHK;
		detect_time_counter = AUTO_DETCET_TIME;
	}
} 


