 /**
  ******************************************************************************
  * @file    bsp_led.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   处理与亮灯指示有关的任务.
  *
  ******************************************************************************
  */

#include "include.h"

/******************************************************************************
 * 函数名:	LED_GPIO_Config 
 * 描述:  	LED灯的初始化
 * 			LED1: PB5,     LED2:PE5
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
void LED_GPIO_Config(void)	
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); // 使能PC端口时钟  
  RCC_Clock_Set(LED1_GRP, ENABLE);

  // LED1	
  GPIO_InitStructure.GPIO_Pin = (LED1_POUT);	//选择对应的引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED1_GRP, &GPIO_InitStructure);  //初始化PC端口

  // LED2
  //RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE); // 使能PC端口时钟  
  RCC_Clock_Set(LED2_GRP, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = (LED2_POUT);	//选择对应的引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(LED2_GRP, &GPIO_InitStructure);  //初始化PC端口

  /* 关闭所有LED*/
  LED_Set(LED_1, OFF);
  LED_Set(LED_2, OFF);
}

/******************************************************************************
 * 函数名:	LED_Flashing 
 * 描述:  	LED_No闪烁flashtime次,每次亮mtime,灭mtime.
 *			注意mtime不要超过500ms,否则看门狗会溢出
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.16
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void LED_Flashing(uint16_t LED_No,uint16_t mtime, uint16_t flashtime)
{
	do
	{
		switch (LED_No)
		{
		case (LED_1): 
	     		LED1(ON);
			wdt_counter = 0;
			Delay_Ms(mtime);
			LED1(OFF);
			wdt_counter = 0;
			Delay_Ms(mtime);
	      		break;

		case (LED_2): 
	     		LED2(ON);
			wdt_counter = 0;	// 延时之前喂狗1次,防止flashtime多次会溢出
			Delay_Ms(mtime);
			LED2(OFF);
			wdt_counter = 0;
			Delay_Ms(mtime);
	      		break;

		default: 
			break;
		}
	}while(--flashtime>0);
}

/******************************************************************************
 * 函数名:	LED_Set 
 * 描述:  	控制LED_No亮或者灭
 *
 * 输入参数: iState: ENABLE, 亮,   DISABLE: 灭
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.16
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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

	default: 
		break;
	}
}


