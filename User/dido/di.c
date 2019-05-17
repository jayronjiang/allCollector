
#include "include.h"

DEVICE_STATUS device_status_queue[DI_NUM]; //设备状态配置组
DEVICE_STATUS_BITS di_status;	// 外部状态字,检测外部设备的状态

uint16_t detect_time_counter = AUTO_DETCET_TIME;	//系统参数检查间隔时间
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
void DEVICE_GPIO_IN_Config(DEVICE_STATUS_LIST dev)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	  /*开启输入按键端口（PX）的时钟*/
	RCC_Clock_Set(device_status_queue[dev].gpio_grp, ENABLE);
 	GPIO_InitStructure.GPIO_Pin = device_status_queue[dev].gpio_pin;; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // 上拉输入
	GPIO_Init(device_status_queue[dev].gpio_grp, &GPIO_InitStructure);
}

/******************************************************************************
 * 函数名:	DO_Init 
 * 描述: 
 *            -DO模块初始化操作
 * 输入参数: 无
 * 输出参数: 无
 * 返回值: 无
 * 
 * 作者:Jerry
 * 创建日期:2013.02.20
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void DI_Init(void)
{
	/*I/O口初始化*/
	DI_Queue_Init();
	//y_First_Read(); 
}

/***********************************************************************************
 * 函数名:	Device_Stat_Queue_Init 
 * 描述: 
 *           	-外设传感器的GPIO初始化,初始化和IO口有关的第一个字节.
 *		这个函数移植时需要改变
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

