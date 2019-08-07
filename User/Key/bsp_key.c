 /**
  ******************************************************************************
  * @file    bsp_key.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   外部输出的扫描和防抖,25ms扫描一次.
  *
  ******************************************************************************
  */
#include "include.h"


/*定义DIP switch的接口pin*/
static const DEVICE_STATUS dip_switches[NUMBER_OF_DIPKEYS] = {\
    	{DIP2_GRP,DIP2_PIN},		// 先读高位
	{DIP1_GRP,DIP1_PIN}
};

/***********************************************************************************
 * 函数名:	DEVICE_GPIO_IN_Config 
 * 描述: 
 *           	-配置指定输入外设.
 *		
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:20181109
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ***********************************************************************************/
void GPIO_In_Init(DEVICE_STATUS key)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	  /*开启输入按键端口（PX）的时钟*/
	RCC_Clock_Set(key.gpio_grp, ENABLE);
 	GPIO_InitStructure.GPIO_Pin = key.gpio_pin;; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
	GPIO_Init(key.gpio_grp, &GPIO_InitStructure);
}

/******************************************************************************
 * 函数名:	Dip_Switch_Init 
 * 描述: 
 *            -DIP switch 的初始化
 * 输入参数: 无
 * 输出参数: 无
 * 返回值: 无
 * 
 * 作者:Jerry
 * 创建日期:2019.08.20
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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
 * 函数名:	user_settings_key_get 
 * 描述: 
 *            -得到dip_switch的值, 默认上拉的, 当拨到ON则为低电平0
 *            -也就是OFF: 1,  ON: 0
 * 输入参数: 无
 * 输出参数: 无
 * 返回值: 0: OFF,OFF,  1:OFF,ON,  2:ON,OFF, 3:ON,ON
 * 
 * 作者:Jerry
 * 创建日期:2019.08.20
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
uint16_t user_settings_key_get(void)
{
	uint16_t pin_level =0;
	uint16_t tmp =0;
	uint16_t index;

	for (index = 0; index < NUMBER_OF_DIPKEYS; index++)
	{
		pin_level <<= 1;	// 左移1位组成00,01,10,11
		Delay_Ms(1);
	        tmp= GPIO_ReadInputDataBit(dip_switches[index].gpio_grp,dip_switches[index].gpio_pin);
	        pin_level |= (tmp  ? 0u : 1u);	// 低电平则为ON
	}

	return pin_level;
}


/******************************************************************************
 * 函数名:	Address_Get_From_Key 
 * 描述: 
 *            -DIP switch 的初始化
 * 输入参数: 无
 * 输出参数: 无
 * 返回值: 无
 * 
 * 作者:Jerry
 * 创建日期:2019.08.20
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Dev_Address_Init(void)
{
	uint16_t dip_value = 0;
	Dip_Switch_Init();
	Delay_Ms(5);			// 等待一段时间稳定
	dip_value = user_settings_key_get();
	DevParams.Address = DEV_ADDR+dip_value;
	system_flag|=DEV_MODIFIED; 	//是否有必要保存地址参数?
}

/******************************************************************************
 * 函数名:	GPIO_ReadInputAll 
 * 描述:  	 一次把所有的键值读出来
 * 
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
uint16_t GPIO_ReadInputAll(uint8_t polar)
{
	uint16_t key_val = 0;
	uint16_t tmp = 0;
	DEVICE_STATUS_LIST dev_status = DI_1;

	for( dev_status=DI_1; dev_status < DI_NUM; dev_status++)
	{

		tmp = GPIO_ReadInputDataBit(device_status_queue[dev_status].gpio_grp,
										device_status_queue[dev_status].gpio_pin);

		key_val |= tmp << dev_status;	// 第几个键值向左移几位.
	}
	if ( polar == LOW_POLAR )
	{
		key_val = key_val^0x00FF;		// 把低8位的低有效全部转为高有效
	}

	return key_val;
}
 
/******************************************************************************
 * 函数名:	ReadKey 
 * 描述:  	读取外部输入状态
 * 
 * 输入参数: 
 * 输出参数: key_effective_port,相应位为1表明读取几次都为1
 *				相应位为0表明读取几次都为0.
 * 返回值: 
 * 
 * 作者:蒋建荣
 * 创建日期:2018.11.16
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void ReadKey(void)
{
	static uint16_t debounce_buffer[DEBOUNCE_TIME];	//防抖一次就可以
	static uint8_t buffer_index = 0;
	/*因为初始时外部默认为高电平,所以初始化为0xFF */
	/*如果初始时外部为低电平,要初始化为0, 否则开始会有一次变位 */
	static uint16_t key_effective_port_last= 0; /*上次经过防抖的真实按键端口状态*/
	static uint16_t key_effective_port = 0xFF;	/*表示经过消抖后确认得到的按键状态*/
	uint16_t constant_high = 0xFF;	/*当相应位为1表明读取的几次相应位都为1*/
	uint16_t constant_low = 0xFF;	/*当相应位为1表明读取的几次相应位都为0*/

	uint8_t i = 0;

	/*读取本次键值,下标自增1*/
	debounce_buffer[buffer_index] = GPIO_ReadInputAll(HIGH_POLAR);
	if (++buffer_index >= DEBOUNCE_TIME)
	{
		buffer_index = 0;
	}

	/*同时读取高有效和低有效的2个值*/
	for (i = 0; i < DEBOUNCE_TIME; i++) 
	{
		constant_high &= debounce_buffer[i];
		constant_low &= ~debounce_buffer[i];
	}

	/*先得到全为1的位*/
	/*再得到全为0的位*/
	key_effective_port |= (constant_high&0xFF);
	key_effective_port &= ((~constant_low)&0xFF);

	di_status.status_word = key_effective_port;
	
	if (key_effective_port !=key_effective_port_last)
	{
		system_flag |= KEY_CHANGED;	// 按键真正地变位
		key_effective_port_last = key_effective_port;
	}
}


#if 0
/******************************************************************************
 * 函数名:	NVIC_Configuration 
 * 描述:  	外部中断的配置
 * 
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
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置中断源 */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************
 * 函数名:	EXTI_PE4_Config 
 * 描述:  	配置PE4为外部中断
 * 
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
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;	 // 上拉输入
	GPIO_Init(GPIOE, &GPIO_InitStructure);

	/* EXTI line mode config */
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOE, GPIO_PinSource4); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; //下降沿中断
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure); 
}

/******************************************************************************
 * 函数名:	EXTI4_IRQHandler 
 * 描述:  	外部按键中断, PE4
 * 
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
 // bug:这里按键有点问题,上升沿和下降沿都产生了中断
void EXTI4_IRQHandler(void)
{
	Delay_Ms(20);
	if(EXTI_GetITStatus(EXTI_Line4) != RESET) //确保是否产生了EXTI Line中断
	{
		// LED1 取反		
		LED1_TOGGLE;
		printf("USART1\n");
		EXTI_ClearITPendingBit(EXTI_Line4);     //清除中断标志位
	}  
}
#endif

