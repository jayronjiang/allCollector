 /**
  ******************************************************************************
  * @file    time.c
  * @author  Jerry
  * @date    21-Nov-2018
  *
  * @brief   处理时钟中断部分,这里用了2个定时器TIM2(20ms), TIM4(动态).
  *          ms定时器在stm32f10x_it.c中被处理.
  *
  ******************************************************************************
  */

#include "include.h"

/*系统的毫秒级累加时间,由于其为无符号型,*/
/* 暂时未被使用*/
uint8_t wdt_counter = 0;
uint8_t  reset_flag=0;	/*报警的复归标志,暂时未用*/

volatile uint32_t system_time_ms = 0;
volatile uint32_t system_time_s = 0;

/******************************************************************************
 * 函数名:	Delay_Ms 
 * 描述:  	不带定时器的阻塞型ms延时,这种定时依赖于编译条件
 * 			不太精确
 *			
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Delay_Ms(uint32_t myMs)
{
	uint32_t i;
	while(myMs--)
	{
		// 汇编为9个语句, 因此为72M/9 = 8K.
		i=10256;
		while(i--);
	}
}

uint16_t go_onflag;
/******************************************************************************
 * 函数名:	Delay_Xus 
 * 描述:  	使用定时器方式的阻塞型us延时,和Delay_Xms一样
 * 			实际意义不大,可能精度稍微高一点
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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
 * 函数名:	Delay_Xms 
 * 描述:  	带定时器的阻塞型ms延时
 * 
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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
 * 函数名:	DelayAndFeedDog 
 * 描述:  	带喂狗功能的延时,延时中每20ms喂狗一次
 * 
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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
	// 没有外部看门狗，不用考虑看门狗溢出
	Delay_Ms(myMs);
}


/******************************************************************************
 * 函数名:	delay 
 * 描述:  	delay多少个时钟周期
 * 
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Delay_clk( uint16_t time)
{
  	while(time--)
  	{
     		 NOP();
  	}
}

 /******************************************************************************
*  函数名: time_interval()
*
*  描述: 计算一个输入的时间量，与当前时刻的时间间隔
*
*  输入: 被比较的时间毫秒数
*
*  输出: 
*
*  返回值: 时间差值
*
*  其它: 
*******************************************************************************/
uint32_t time_interval(uint32_t time0)
{
	uint32_t now_ms;

	now_ms = system_time_s;
	now_ms = now_ms - time0;

	return now_ms;
}

//*****************************************************************************
//                                Timer实现
//*****************************************************************************

/******************************************************************************
 * 函数名:	Start_Timer_x 
 * 描述:  	使能定时器X
 *
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Start_Timer_x(TIM_TypeDef* TIMx)
{
	/* Check the parameters */
	assert_param(IS_TIM_ALL_PERIPH(TIMx));
	TIM_Cmd(TIMx, ENABLE);
}

/******************************************************************************
 * 函数名:	Stop_Timer_x 
 * 描述:  	停止定时器X
 *
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Stop_Timer_x(TIM_TypeDef* TIMx)
{
	TIM_Cmd(TIMx, DISABLE);
}

/******************************************************************************
 * 函数名:	Timer_Start 
 * 描述:  	使能所有定时器
 *
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Timer_Start(void)
{
	Start_Timer_x(TIM2);
	/*刚开始配置的50ms定时器并没有用,只有在us定时器被*/
	/*配置后才开始被使用,这里先不打开*/
	//Start_Timer_x(TIM4);
	IWDG_Enable(); // 使能看门狗
}

/******************************************************************************
 * 函数名:	Time_NVIC_Configuration 
 * 描述:  	定时器优先级的配置
 *
 * 定时器2: 抢占优先级为0, 子优先级为0
 * 定时器4: 抢占优先级为1, 子优先级为1
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Time_NVIC_Configuration(void)
{   
	NVIC_InitTypeDef NVIC_InitStructure; 

	/*TIM2溢出中断*/
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 	
	// 设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	 
	// 设置子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);	  
	
	/*TIM4溢出中断*/
	// 设置中断来源
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;	
	// 设置抢占优先级
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;	 
	// 设置子优先级
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;	
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	  
}

/******************************************************************************
 * 函数名:	Time_Mode_Config 
 * 描述:  	初始化定时器2为20ms,定时器4为50ms
 * 
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Time_Mode_Config(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	/* 配置计时模式的模式的基本定时器TIM2 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 2000-1;		/* 20ms time interrupt */
	TIM_TimeBaseStructure.TIM_Prescaler = 720-1;		/* The time base is 72M/720 = 100K */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);	/* 清除溢出中断标志 */
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//TIM_Cmd(TIM2, ENABLE);
	
	/* 配置延时定时器TIM4 */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
	TIM_TimeBaseStructure.TIM_Period = 50000-1;	/* 50ms interrupt */
	TIM_TimeBaseStructure.TIM_Prescaler = 72-1;	/* The time base is 72M/720 = 1M */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	TIM_ARRPreloadConfig(TIM4,DISABLE);

	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	/* 清除溢出中断标志 */
	TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
	TIM_Cmd(TIM4, DISABLE);
}

/******************************************************************************
 * 函数名:	Time_Configuration 
 * 描述:  	初始化定时器配置
 * 
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Time_Configuration(void)
{
	Time_NVIC_Configuration();
	Time_Mode_Config();
}

/******************************************************************************
 * 函数名:	TIM4_IRQHandler 
 * 描述:  动态定时器,其ARR 是在Delay_Xus中被更新
 *         			暂时用作us定时器
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void TIM4_IRQHandler (void)
{
	TIM_ClearFlag(TIM4, TIM_FLAG_Update);	
	go_onflag=1;
}

/******************************************************************************
 * 函数名:	TIM2_IRQHandler 
 * 描述:  20ms 定时中断
 *         		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void TIM2_IRQHandler (void)
{   
	static uint16_t t_100ms = 0;
	static uint16_t t_cnt = 0;
	static uint16_t t_1s = 0;
	

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	/*1s,喂狗, 200ms标志位暂时未使用*/
	if( ++t_100ms>= MS_200)
	{
     		t_100ms=0;
		system_flag |= SYSTEM_200MS;
		// 1s钟还没清零,表明系统已经卡死,不喂狗直接复位
		// 这里的6起作用的其实是防止主循环跑飞,200*6+1000-200 = 2s
	    	if (++wdt_counter <= 6) 
	    	{
	    		IWDG_Feed();  /*中断里面喂狗,内部看门狗*/
	    	}
	}

	// 5s时间到，参数轮询
	if( ++t_1s>= ONE_SECOND)
	{
		t_1s = 0;
		// 系统时间,1s单位
		system_time_s++;
	}

	// 5s时间到，参数轮询,调试的时候先1s
	if( ++t_cnt>= ONE_SECOND)
	{
		t_cnt = 0;
		start_comm();
	}


	/* 系统检测状态和控制位是否一致,每3s检测一次*/
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


