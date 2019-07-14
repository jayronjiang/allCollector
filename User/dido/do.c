
#include "include.h"

UINT16 DO_Status = 0;		/*DO的动作状态,0返回,1动作,BIT0:do1,BIT1:do2*/
//NT8 DO_OpStatus[DO_NUM] = {0,0};	/*DO面板的操作状态,0:正常,1:动作2:返回*/ 

RELAY_STATUS RelayStatus[DO_NUM]={{0,0,0,0,0,0},};	/*有关DO操作的一些状态变量,在DO_Init中初始化*/

DEVICE_CTRL	device_ctrl_queue[DO_NUM];	// 设备控制配置组,配置控制口的具体物理地址

/***********************************************************************************
 * 函数名:	DEVICE_GPIO_OUT_Config 
 * 描述: 
 *           	-配置指定输出外设.
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
void DEVICE_GPIO_OUT_Config(DEVICE_CTRL_LIST dev)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*开启输出按键端口PX的时钟*/
	RCC_Clock_Set(device_ctrl_queue[dev].gpio_grp, ENABLE);
	//RCC_APB2PeriphClockCmd( DEVICE_RCC_GRP, ENABLE); // 使能PC端口时钟  
	GPIO_InitStructure.GPIO_Pin = device_ctrl_queue[dev].gpio_pin;	//选择对应的引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(device_ctrl_queue[dev].gpio_grp, &GPIO_InitStructure);  //初始化端口
	DeviceX_Activate(dev);	 // 初始化为无效
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
void DO_Init(void)
{
	UINT8 i;
	/*I/O口初始化*/
	DO_Queue_Init();
	
	DO_Status = 0;
	for( i=0; i< DO_NUM;i++ )
	{ 
		RelayStatus[i].ActFlag = 0;
		RelayStatus[i].ActPresetCounter = 0L;
		RelayStatus[i].ActPresetFlag = FALSE;
		RelayStatus[i].ReturnPresetCounter = 0L;
		RelayStatus[i].ReturnPresetFlag = FALSE;
		RelayStatus[i].PulseCounter = 0;
	}
}

/***********************************************************************************
 * 函数名:	DeviceX_Activate 
 * 描述: 
 *           	-激活指定外设,低电平有效,使用复位语句
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
void DeviceX_Activate(DEVICE_CTRL_LIST dev)
{
	assert_param(dev<DO_NUM);
	GPIO_SetBits(device_ctrl_queue[dev].gpio_grp, device_ctrl_queue[dev].gpio_pin);
}

/***********************************************************************************
 * 函数名:	DeviceX_Deactivate 
 * 描述: 
 *           	-无效指定外设,低电平有效,使用置位语句
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
void DeviceX_Deactivate(DEVICE_CTRL_LIST dev)
{
	assert_param(dev<DO_NUM);
	GPIO_ResetBits(device_ctrl_queue[dev].gpio_grp, device_ctrl_queue[dev].gpio_pin);
}


/******************************************************************************
 * 函数名:	DOProcessTickEvents 
 * 描述: 
 *            -DO模块定时事件的处理,10ms处理一次
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:汪治国 
 * 创建日期:2008.12.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void DOProcessTickEvents(void)
{
	UINT16 i;
	
	for(i = 0; i < DO_NUM; i++)
	{
		/*遥控预置的处理: 真正的遥控要在预置后15s内完成*/
		if(RelayStatus[i].ActPresetFlag)
		{
			if( RelayStatus[i].ActPresetCounter++ >= PRESET_15S )
			{
				RelayStatus[i].ActPresetCounter = 0;
				RelayStatus[i].ActPresetFlag = FALSE;
			}
		}

		if(RelayStatus[i].ReturnPresetFlag)
		{
			if( RelayStatus[i].ReturnPresetCounter++ >= PRESET_15S )
			{
				RelayStatus[i].ReturnPresetCounter = 0;
				RelayStatus[i].ReturnPresetFlag = FALSE;
			}
		} 
		if( DO_Status&(BIT(i)))		/*DO已经动作*/
		{
				if( !(RelayStatus[i].ActFlag & PROACT))		/*对应的DO没有保护报警发生*/
				{
					if(reset_flag)	/*出口设置为保持且复归键按下时，继电器出口复归*/
			                {
			                        Relay_Return(i);
						RelayStatus[i].ActFlag &=~ REMOTEACT;
			                }
				}
				else			/*有保护,则DO不能返回*/
				{
					RelayStatus[i].PulseCounter = 0;
				}
		}
		else			/*DO未动作，也不返回*/
		{
				RelayStatus[i].PulseCounter = 0;
		} 
	}
}

/******************************************************************************
 * 函数名:	Relay_Act 
 * 描述: 
 *            -继电器动作
 * 输入参数: 继电器对应的DO序号0~1
 * 输出参数: 无
 * 返回值: 1:继电器操作成功0:无效操作
 * 
 * 作者:汪治国 
 * 创建日期:2008.9.18
 * 
 *------------------------
 * 修改人:Jerry
 * 修改日期:2013-02-20,移植至2612D
 ******************************************************************************/
UINT8 Relay_Act(UINT8 num)
{
	if(num < DO_NUM)
	{
		if(DO_Status & BIT(num))		/*处于闭合状态时动作无效*/
		{
			return 0;
		}		
	}
	else
	{
		return 0;
	}

	DO_Status |= BIT(num);
	// 低电平动作
	DeviceX_Deactivate((DEVICE_CTRL_LIST)num);
	return 1;
}



/******************************************************************************
 * 函数名:	Relay_Return
 * 描述: 
 *            -继电器返回
 * 输入参数: 继电器对应的DO序号0~1
 * 输出参数: 无
 * 返回值: 1:继电器操作成功0:无效操作
 * 
 * 作者:汪治国 
 * 创建日期:2008.9.18
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
UINT8 Relay_Return(UINT8 num)
{
	if(num < DO_NUM)
	{
		if(!(DO_Status & BIT(num)))		/*处于打开状态时动作无效*/
		{
			return 0;
		}		
	}
	else
	{
		return 0;
	}
	DO_Status &= ~ BIT(num);
	DeviceX_Activate((DEVICE_CTRL_LIST)num);

	return 1;
}


/******************************************************************************
 * 函数名:	Relay_Act_Preset 
 * 描述: 
 *            -遥控预置,不执行则15秒后无效
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:汪治国 
 * 创建日期:2010.1.13
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
UINT8 Relay_Act_Preset(UINT8 num)
{
	if(num < DO_NUM)
	{
		if(DO_Status & BIT(num))		/*处于闭合状态时动作无效*/
		{
			return 0;
		}		
	}
	else
	{
		return 0;
	}

	RelayStatus[num].ActPresetFlag = TRUE;
	RelayStatus[num].ActPresetCounter = 0;
	return 1;
}


/******************************************************************************
 * 函数名:	Relay_Return_Preset 
 * 描述: 
 *            -遥控预置,不执行则15秒后无效
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:汪治国 
 * 创建日期:2010.1.13
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
UINT8 Relay_Return_Preset(UINT8 num)
{
	if(num < DO_NUM)
	{
		if(!(DO_Status & BIT(num)))		/*处于打开状态时动作无效*/
		{
			return 0;
		}		
	}
	else
	{
		return 0;
	}

	RelayStatus[num].ReturnPresetFlag = TRUE;
	RelayStatus[num].ReturnPresetCounter = 0;
	
	return 1;
}


/******************************************************************************
 * 函数名:	RelayOperate 
 * 描述: 
 *            -继电器操作函数
 * 输入参数: num--继电器号, mode--操作模式
 * 输出参数: 
 * 返回值: 0代表操作无效  1代表操作正常
 * 
 * 作者:汪治国 
 * 创建日期:2009.2.2
 * 
 *------------------------
 * 修改人:Jerry
 * 修改日期:2013.2.20移植到2612D
 ******************************************************************************/
UINT8 RelayOperate(UINT8 num, UINT8 mode)
{
	if(num >= DO_NUM)
	{
		return 0;
	}

	switch(mode)
	{
		case RELAY_REMOTE_ACT_PRESET:
			if(!(RelayStatus[num].ActFlag & PROACT))	/*保护优先级比遥控高*/
			{
				return Relay_Act_Preset(num);
			}
			return 0;

		case RELAY_REMOTE_ACT:
			if(!(RelayStatus[num].ActFlag & PROACT))	/*保护优先级比遥控高*/
			{
				if(((RelayStatus[num].ActPresetFlag == TRUE)&&(DevParams.Pre_Remote))||( !DevParams.Pre_Remote))
				{
					RelayStatus[num].ActFlag |= REMOTEACT;
					RelayStatus[num].ActPresetFlag = FALSE;
					return Relay_Act(num);
				}
				return 0;
			}
			return 0;
			
		case RELAY_FORCE_ACT:				/*面板强制动作*/
			return Relay_Act(num);

		case RELAY_REMOTE_RETURN_PRESET:
			if(!(RelayStatus[num].ActFlag & PROACT))	/*保护优先级比遥控高*/
			{
				return Relay_Return_Preset(num);
			}
			return 0;

		case RELAY_REMOTE_RETURN:
			if(!(RelayStatus[num].ActFlag & PROACT))	/*保护优先级比遥控高*/
			{
				if(((RelayStatus[num].ReturnPresetFlag == TRUE)&&(DevParams.Pre_Remote))||( !DevParams.Pre_Remote))
				{
					RelayStatus[num].ActFlag &= ~REMOTEACT;
					RelayStatus[num].ReturnPresetFlag = FALSE;
					return Relay_Return(num);
				}
				return 0;	/*遥控未预置*/
			}
			return 0;	/*保护发生*/
			
		case RELAY_FORCE_RETURN:			/*面板强制返回*/
			return Relay_Return(num);

		case RELAY_ALARM_ACT:
			if(!(RelayStatus[num].ActFlag & PROACT))
			{
				RelayStatus[num].ActFlag |= PROACT;
			}
			return Relay_Act(num);	/*放到括号外面,防止当强制操作正常后不能返回到报警模式*/

		case RELAY_ALARM_RETURN:
			if( RelayStatus[num].ActFlag &PROACT)		/*必须要判断,否则总是将遥控的动作返回*/
			{
				RelayStatus[num].ActFlag &= ~PROACT;	/*将保护位清掉，作为遥控展宽计数的条件*/
				return Relay_Return(num);		/*不考虑展宽,直接返回*/	
			}	

		default:
			return 0;
	}
}

/***********************************************************************************
 * 函数名:	Device_Ctrl_Queue_Init 
 * 描述: 
 *           	-控制外设的GPIO初始化,初始化和IO口有关的第一个字节.
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
void DO_Queue_Init(void)
{
	DEVICE_CTRL_LIST dev_type = DO_1;

	for( dev_type=DO_1; dev_type<DO_NUM; dev_type++)
	{
		switch(dev_type)
		{
		case DO_1:
			device_ctrl_queue[dev_type].gpio_grp = DO1_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO1_POUT;
			break;

		case DO_2:
			device_ctrl_queue[dev_type].gpio_grp = DO2_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO2_POUT;
			break;

		case DO_3:
			device_ctrl_queue[dev_type].gpio_grp = DO3_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO3_POUT;
			break;

		case DO_4:
			device_ctrl_queue[dev_type].gpio_grp = DO4_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO4_POUT;
			break;

		case DO_5:
			device_ctrl_queue[dev_type].gpio_grp = DO5_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO5_POUT;
			break;

		case DO_6:
			device_ctrl_queue[dev_type].gpio_grp = DO6_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO6_POUT;
			break;

		case DO_7:
			device_ctrl_queue[dev_type].gpio_grp = DO7_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO7_POUT;
			break;

		case DO_8:
			device_ctrl_queue[dev_type].gpio_grp = DO8_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO8_POUT;
			break;

	#ifndef HAS_8I8O
		/*DO9~DO12占用原来DI1~DI4*/
		case DO_9:
			device_ctrl_queue[dev_type].gpio_grp = DO9_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO9_POUT;
			break;

		case DO_10:
			device_ctrl_queue[dev_type].gpio_grp = DO10_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO10_POUT;
			break;

		case DO_11:
			device_ctrl_queue[dev_type].gpio_grp = DO11_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO11_POUT;
			break;

		case DO_12:
			device_ctrl_queue[dev_type].gpio_grp = DO12_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = DO12_POUT;
			break;
	#endif

		/*这2路DO控制RS485的方向*/
		case RS485_CTRL_1:
			device_ctrl_queue[dev_type].gpio_grp = RS485_1_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = RS485_1_POUT;
			break;

		case RS485_CTRL_2:
			device_ctrl_queue[dev_type].gpio_grp = RS485_2_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = RS485_2_POUT;
			break;

		default:
			break;
		}
		DEVICE_GPIO_OUT_Config(dev_type);
	}
}


/*485发送/接收功能选择: 低电平接收,高电平发送*/
void rs485FuncSelect(DEVICE_CTRL_LIST seq,bool value)
{
	if (value == SEL_S)
	{
		DeviceX_Activate(seq);	// 注意ACtive是高电平
	}
	else
	{
		DeviceX_Deactivate(seq);
	}
}
