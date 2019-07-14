
#include "include.h"

UINT16 DO_Status = 0;		/*DO�Ķ���״̬,0����,1����,BIT0:do1,BIT1:do2*/
//NT8 DO_OpStatus[DO_NUM] = {0,0};	/*DO���Ĳ���״̬,0:����,1:����2:����*/ 

RELAY_STATUS RelayStatus[DO_NUM]={{0,0,0,0,0,0},};	/*�й�DO������һЩ״̬����,��DO_Init�г�ʼ��*/

DEVICE_CTRL	device_ctrl_queue[DO_NUM];	// �豸����������,���ÿ��ƿڵľ��������ַ

/***********************************************************************************
 * ������:	DEVICE_GPIO_OUT_Config 
 * ����: 
 *           	-����ָ���������.
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
void DEVICE_GPIO_OUT_Config(DEVICE_CTRL_LIST dev)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	/*������������˿�PX��ʱ��*/
	RCC_Clock_Set(device_ctrl_queue[dev].gpio_grp, ENABLE);
	//RCC_APB2PeriphClockCmd( DEVICE_RCC_GRP, ENABLE); // ʹ��PC�˿�ʱ��  
	GPIO_InitStructure.GPIO_Pin = device_ctrl_queue[dev].gpio_pin;	//ѡ���Ӧ������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(device_ctrl_queue[dev].gpio_grp, &GPIO_InitStructure);  //��ʼ���˿�
	DeviceX_Activate(dev);	 // ��ʼ��Ϊ��Ч
}


/******************************************************************************
 * ������:	DO_Init 
 * ����: 
 *            -DOģ���ʼ������
 * �������: ��
 * �������: ��
 * ����ֵ: ��
 * 
 * ����:Jerry
 * ��������:2013.02.20
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void DO_Init(void)
{
	UINT8 i;
	/*I/O�ڳ�ʼ��*/
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
 * ������:	DeviceX_Activate 
 * ����: 
 *           	-����ָ������,�͵�ƽ��Ч,ʹ�ø�λ���
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
void DeviceX_Activate(DEVICE_CTRL_LIST dev)
{
	assert_param(dev<DO_NUM);
	GPIO_SetBits(device_ctrl_queue[dev].gpio_grp, device_ctrl_queue[dev].gpio_pin);
}

/***********************************************************************************
 * ������:	DeviceX_Deactivate 
 * ����: 
 *           	-��Чָ������,�͵�ƽ��Ч,ʹ����λ���
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
void DeviceX_Deactivate(DEVICE_CTRL_LIST dev)
{
	assert_param(dev<DO_NUM);
	GPIO_ResetBits(device_ctrl_queue[dev].gpio_grp, device_ctrl_queue[dev].gpio_pin);
}


/******************************************************************************
 * ������:	DOProcessTickEvents 
 * ����: 
 *            -DOģ�鶨ʱ�¼��Ĵ���,10ms����һ��
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:���ι� 
 * ��������:2008.12.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void DOProcessTickEvents(void)
{
	UINT16 i;
	
	for(i = 0; i < DO_NUM; i++)
	{
		/*ң��Ԥ�õĴ���: ������ң��Ҫ��Ԥ�ú�15s�����*/
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
		if( DO_Status&(BIT(i)))		/*DO�Ѿ�����*/
		{
				if( !(RelayStatus[i].ActFlag & PROACT))		/*��Ӧ��DOû�б�����������*/
				{
					if(reset_flag)	/*��������Ϊ�����Ҹ��������ʱ���̵������ڸ���*/
			                {
			                        Relay_Return(i);
						RelayStatus[i].ActFlag &=~ REMOTEACT;
			                }
				}
				else			/*�б���,��DO���ܷ���*/
				{
					RelayStatus[i].PulseCounter = 0;
				}
		}
		else			/*DOδ������Ҳ������*/
		{
				RelayStatus[i].PulseCounter = 0;
		} 
	}
}

/******************************************************************************
 * ������:	Relay_Act 
 * ����: 
 *            -�̵�������
 * �������: �̵�����Ӧ��DO���0~1
 * �������: ��
 * ����ֵ: 1:�̵��������ɹ�0:��Ч����
 * 
 * ����:���ι� 
 * ��������:2008.9.18
 * 
 *------------------------
 * �޸���:Jerry
 * �޸�����:2013-02-20,��ֲ��2612D
 ******************************************************************************/
UINT8 Relay_Act(UINT8 num)
{
	if(num < DO_NUM)
	{
		if(DO_Status & BIT(num))		/*���ڱպ�״̬ʱ������Ч*/
		{
			return 0;
		}		
	}
	else
	{
		return 0;
	}

	DO_Status |= BIT(num);
	// �͵�ƽ����
	DeviceX_Deactivate((DEVICE_CTRL_LIST)num);
	return 1;
}



/******************************************************************************
 * ������:	Relay_Return
 * ����: 
 *            -�̵�������
 * �������: �̵�����Ӧ��DO���0~1
 * �������: ��
 * ����ֵ: 1:�̵��������ɹ�0:��Ч����
 * 
 * ����:���ι� 
 * ��������:2008.9.18
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
UINT8 Relay_Return(UINT8 num)
{
	if(num < DO_NUM)
	{
		if(!(DO_Status & BIT(num)))		/*���ڴ�״̬ʱ������Ч*/
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
 * ������:	Relay_Act_Preset 
 * ����: 
 *            -ң��Ԥ��,��ִ����15�����Ч
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:���ι� 
 * ��������:2010.1.13
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
UINT8 Relay_Act_Preset(UINT8 num)
{
	if(num < DO_NUM)
	{
		if(DO_Status & BIT(num))		/*���ڱպ�״̬ʱ������Ч*/
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
 * ������:	Relay_Return_Preset 
 * ����: 
 *            -ң��Ԥ��,��ִ����15�����Ч
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:���ι� 
 * ��������:2010.1.13
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
UINT8 Relay_Return_Preset(UINT8 num)
{
	if(num < DO_NUM)
	{
		if(!(DO_Status & BIT(num)))		/*���ڴ�״̬ʱ������Ч*/
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
 * ������:	RelayOperate 
 * ����: 
 *            -�̵�����������
 * �������: num--�̵�����, mode--����ģʽ
 * �������: 
 * ����ֵ: 0���������Ч  1�����������
 * 
 * ����:���ι� 
 * ��������:2009.2.2
 * 
 *------------------------
 * �޸���:Jerry
 * �޸�����:2013.2.20��ֲ��2612D
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
			if(!(RelayStatus[num].ActFlag & PROACT))	/*�������ȼ���ң�ظ�*/
			{
				return Relay_Act_Preset(num);
			}
			return 0;

		case RELAY_REMOTE_ACT:
			if(!(RelayStatus[num].ActFlag & PROACT))	/*�������ȼ���ң�ظ�*/
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
			
		case RELAY_FORCE_ACT:				/*���ǿ�ƶ���*/
			return Relay_Act(num);

		case RELAY_REMOTE_RETURN_PRESET:
			if(!(RelayStatus[num].ActFlag & PROACT))	/*�������ȼ���ң�ظ�*/
			{
				return Relay_Return_Preset(num);
			}
			return 0;

		case RELAY_REMOTE_RETURN:
			if(!(RelayStatus[num].ActFlag & PROACT))	/*�������ȼ���ң�ظ�*/
			{
				if(((RelayStatus[num].ReturnPresetFlag == TRUE)&&(DevParams.Pre_Remote))||( !DevParams.Pre_Remote))
				{
					RelayStatus[num].ActFlag &= ~REMOTEACT;
					RelayStatus[num].ReturnPresetFlag = FALSE;
					return Relay_Return(num);
				}
				return 0;	/*ң��δԤ��*/
			}
			return 0;	/*��������*/
			
		case RELAY_FORCE_RETURN:			/*���ǿ�Ʒ���*/
			return Relay_Return(num);

		case RELAY_ALARM_ACT:
			if(!(RelayStatus[num].ActFlag & PROACT))
			{
				RelayStatus[num].ActFlag |= PROACT;
			}
			return Relay_Act(num);	/*�ŵ���������,��ֹ��ǿ�Ʋ����������ܷ��ص�����ģʽ*/

		case RELAY_ALARM_RETURN:
			if( RelayStatus[num].ActFlag &PROACT)		/*����Ҫ�ж�,�������ǽ�ң�صĶ�������*/
			{
				RelayStatus[num].ActFlag &= ~PROACT;	/*������λ�������Ϊң��չ�����������*/
				return Relay_Return(num);		/*������չ��,ֱ�ӷ���*/	
			}	

		default:
			return 0;
	}
}

/***********************************************************************************
 * ������:	Device_Ctrl_Queue_Init 
 * ����: 
 *           	-���������GPIO��ʼ��,��ʼ����IO���йصĵ�һ���ֽ�.
 *		���������ֲʱ��Ҫ�ı�
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
		/*DO9~DO12ռ��ԭ��DI1~DI4*/
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

		/*��2·DO����RS485�ķ���*/
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


/*485����/���չ���ѡ��: �͵�ƽ����,�ߵ�ƽ����*/
void rs485FuncSelect(DEVICE_CTRL_LIST seq,bool value)
{
	if (value == SEL_S)
	{
		DeviceX_Activate(seq);	// ע��ACtive�Ǹߵ�ƽ
	}
	else
	{
		DeviceX_Deactivate(seq);
	}
}
