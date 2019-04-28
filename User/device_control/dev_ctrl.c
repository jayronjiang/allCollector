/**
  ******************************************************************************
  * @file    dev_ctrl.c
  * @author  Jerry
  * @date    22-Nov-2018
  *
  * @brief   ����Ķ�ȡ�Ϳ��ƴ���
  *
  ******************************************************************************
  */
  
#include "include.h"

/* ÿһλ����ϸ������ṹ�嶨��*/
DEVICE_CONTROL_BITS device_control_used;  // �豸������,���ݴ��־����ֵ�����豸

// 0λ: �����״̬      --0Ϊ��ƣ�1Ϊ�̵�
// 1λ: �������״̬--0Ϊ�򿪣�1Ϊ�ر�
// 2λ: ��̤����״̬--0Ϊ�б�����1Ϊ�ޱ���
// 3λ: �Զ�����״̬--0Ϊ���ˣ�1Ϊ̧��
// 4λ: ǰ��Ȧ                 --0Ϊ�г���1Ϊ�޳�
// 5λ: ����Ȧ                 --0Ϊ�г���1Ϊ�޳�
// 6λ: ����ͨ�е�      --0Ϊ��ƣ�1Ϊ�̵�
// 7λ: ����                       --0Ϊ������1Ϊ��
DEVICE_STATUS_BITS device_status_used;	// �ⲿ״̬��,����ⲿ�豸��״̬

DEVICE_CTRL	device_ctrl_queue[DEV_CTRL_NUM];	// �豸����������,���ÿ��ƿڵľ��������ַ
DEVICE_STATUS device_status_queue[DEV_STATUS_NUM]; //�豸״̬������

uint8_t bLastLaneRedGreenOperateState = RED; // �����ϴε�ͨ�е�״̬
bool ALG_down_flag_bit = FALSE;			// ָʾ�����Ƿ�ʼTRUE:���ܿ�ʼ
bool ALG_up_flag_bit = FALSE;			// ָʾ̧���Ƿ�ʼTRUE:̧�ܿ�ʼ
bool LastLaneLampState = FALSE;			// �����ϴ����Ƶ�״̬
bool autoBarEnable = FALSE;				// �Զ����ʹ��
bool dete_bit_recd = FALSE;				// ����Ǳ���BACK_COIL���ϴ�״̬
bool bFeeCleared = FALSE;				// �����Ƿ����?

uint16_t WatchingDelayCount = 0;					// ���Ҽ���ʱ��
uint16_t detect_time_counter = AUTO_DETCET_TIME;	//ϵͳ���������ʱ��
uint16_t alarm_time_counter = 0;					// ��������ʱ��

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
void Device_Ctrl_Queue_Init(void)
{
	DEVICE_CTRL_LIST dev_type = BAR_UP;

	for( dev_type=BAR_UP; dev_type<DEV_CTRL_NUM; dev_type++)
	{
		switch(dev_type)
		{
		case BAR_UP:
			device_ctrl_queue[dev_type].gpio_grp = BAR_UP_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = BAR_UP_POUT;
			break;

		case BAR_DOWN:
			device_ctrl_queue[dev_type].gpio_grp = BAR_DOWN_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = BAR_DOWN_POUT;
			break;

		case TTL_GREEN:
			device_ctrl_queue[dev_type].gpio_grp = TTL_GREEN_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = TTL_GREEN_POUT;
			break;

		case VOX_ALM:
			device_ctrl_queue[dev_type].gpio_grp = VOX_ALM_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = VOX_ALM_POUT;
			break;

		case LIGHT_ALM:
			device_ctrl_queue[dev_type].gpio_grp = LIGHT_ALM_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = LIGHT_ALM_POUT;
			break;

		case LAN_LAMP:
			device_ctrl_queue[dev_type].gpio_grp = LAN_LAMP_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = LAN_LAMP_POUT;
			break;

		case BAK1_USED:
			device_ctrl_queue[dev_type].gpio_grp = BAK1_USED_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = BAK1_USED_POUT;
			break;

		case BAK2_USED:
			device_ctrl_queue[dev_type].gpio_grp = BAK2_USED_OUT_GRP;
			device_ctrl_queue[dev_type].gpio_pin = BAK2_USED_POUT;
			break;

		default:
			break;
		}
		DEVICE_GPIO_OUT_Config(dev_type);
	}
}

/***********************************************************************************
 * ������:	Device_Stat_Queue_Init 
 * ����: 
 *           	-���贫������GPIO��ʼ��,��ʼ����IO���йصĵ�һ���ֽ�.
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
void Device_Stat_Queue_Init(void)
{
	DEVICE_STATUS_LIST dev_type = TTL;

	for( dev_type=TTL; dev_type<DEV_STATUS_NUM; dev_type++)
	{
		switch(dev_type)
		{
		case TTL:
			device_status_queue[dev_type].gpio_grp = TTL_IN_GRP;
			device_status_queue[dev_type].gpio_pin = TTL_PIN;
			break;

		case LG:
			device_status_queue[dev_type].gpio_grp = LG_IN_GRP;
			device_status_queue[dev_type].gpio_pin = LG_PIN;
			break;

		case ALARM:
			device_status_queue[dev_type].gpio_grp = ALARM_IN_GRP;
			device_status_queue[dev_type].gpio_pin = ALARM_PIN;
			break;

		case ALG:
			device_status_queue[dev_type].gpio_grp = ALG_IN_GRP;
			device_status_queue[dev_type].gpio_pin = ALG_PIN;
			break;

		case FRONT_COIL:
			device_status_queue[dev_type].gpio_grp = FRONT_IN_GRP;
			device_status_queue[dev_type].gpio_pin = FRONT_PIN;
			break;

		case BACK_COIL:
			device_status_queue[dev_type].gpio_grp = BACK_COIN_IN_GRP;
			device_status_queue[dev_type].gpio_pin = BACK_COIN_PIN;
			break;

		case BAK1:
			device_status_queue[dev_type].gpio_grp = BAK1_IN_GRP;
			device_status_queue[dev_type].gpio_pin = BAK1_PIN;
			break;

		case BAK2:
			device_status_queue[dev_type].gpio_grp = BAK2_IN_GRP;
			device_status_queue[dev_type].gpio_pin = BAK2_PIN;
			break;

		default:
			break;
		}
		DEVICE_GPIO_IN_Config(dev_type);
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
	assert_param(dev<DEV_CTRL_NUM);
	GPIO_ResetBits(device_ctrl_queue[dev].gpio_grp, device_ctrl_queue[dev].gpio_pin);
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
	assert_param(dev<DEV_CTRL_NUM);
	GPIO_SetBits(device_ctrl_queue[dev].gpio_grp, device_ctrl_queue[dev].gpio_pin);
}

/***********************************************************************************
 * ������:	Drop_LG_Start 
 * ����: 
 *           	-���ܶ�����ʼ
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
void Drop_LG_Start(void)
{
	// BAK1_USED�ǽ��ܺ�̧��ѡ���߼�����
	// high: ����	low: ̧��
	// high/low���߼��ߵ�ƽ/�͵�ƽ�����������ƽ
	DeviceX_Activate(BAK1_USED);
	DeviceX_Activate(BAR_DOWN);
	ALG_down_flag_bit = TRUE;		//ָʾ��ʼ����
	ALG_up_flag_bit = FALSE;		//���̧��ָʾ
	/*����,���������*/
	device_control_used.control_bits.Lane_lamp_bit = 0;
}

/***********************************************************************************
 * ������:	Rise_LG_Start 
 * ����: 
 *           	-̧�ܶ�����ʼ
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
void Rise_LG_Start(void)
{
	// BAK1_USED�ǽ��ܺ�̧��ѡ���߼�����
	// high: ����	low: ̧��
	// high/low���߼��ߵ�ƽ/�͵�ƽ�����������ƽ
	DeviceX_Deactivate(BAK1_USED);
	DeviceX_Activate(BAR_UP);
	ALG_down_flag_bit = FALSE;		//�������ָʾ
	ALG_up_flag_bit = TRUE;		//̧�ܿ�ʼ
	/*̧��,�������̵�*/
	device_control_used.control_bits.Lane_lamp_bit = 1;
}

/***********************************************************************************
 * ������:	LG_Act_End 
 * ����: 
 *           	-̧�ܻ򽵸��������
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
void LG_Act_End(void)
{
	DeviceX_Deactivate(BAR_UP);
	DeviceX_Deactivate(BAR_DOWN);
}

/***********************************************************************************
 * ������:	RCC_Clock_Set 
 * ����: 
 *           	-���������ʱ��
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
void RCC_Clock_Set(GPIO_TypeDef* GPIOx, FunctionalState iState)
{
	assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
	
	if (GPIOx == GPIOA)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, iState); 
	}
	else if (GPIOx == GPIOB)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, iState); 
	}
	else if (GPIOx == GPIOC)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, iState); 
	}
	else if (GPIOx == GPIOD)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, iState); 
	}
	else if (GPIOx == GPIOE)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, iState); 
	}
	else if (GPIOx == GPIOF)
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOF, iState); 
	}
	else
	{
		RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOG, iState); 
	}
}

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
	DeviceX_Deactivate(dev);	 // ��ʼ��Ϊ��Ч
}

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
void DEVICE_GPIO_IN_Config(DEVICE_STATUS_LIST dev)	
{
	GPIO_InitTypeDef GPIO_InitStructure;

	  /*�������밴���˿ڣ�PX����ʱ��*/
	RCC_Clock_Set(device_status_queue[dev].gpio_grp, ENABLE);
 	GPIO_InitStructure.GPIO_Pin = device_status_queue[dev].gpio_pin;; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;  // ��������
	GPIO_Init(device_status_queue[dev].gpio_grp, &GPIO_InitStructure);
}

/***********************************************************************************
 * ������:	Command_Get 
 * ����: 
 *           	-�õ�ָ�����������ֵ
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
uint8_t Command_Get(DEVICE_CTRL_LIST dev)
{
	uint8_t re_val = 0;
	uint8_t posdev = (uint8_t)dev;
	uint8_t temp = 0;

	assert_param(dev<DEV_CTRL_NUM);	// ���������ܴ���DEV_CTRL_NUM

	temp = BIT(posdev); // �õ���posdevλ������.
	temp = (device_control_used.control_word[USED]&temp);  // ȡdevice_control_used.control_word��8λ�е�posdevλ
	re_val = temp?TRUE:FALSE;		// ������λ��Ϊ0�ͷ���TRUE.

#if 0
	switch(dev)
	{
	case BAR_UP:
		re_val = device_control_used.control_bits.ALG_up_bit;
		break;

	case BAR_DOWN:
		re_val = device_control_used.control_bits.ALG_down_bit;
		break;

	case TTL_GREEN:
		re_val = device_control_used.control_bits.TTL_green_bit;
		break;

	case VOX_ALM:
		re_val = device_control_used.control_bits.VOX_alarm_bit;
		break;

	case LIGHT_ALM:
		re_val = device_control_used.control_bits.Light_alarm_bit;
		break;

	case LAN_LAMP:
		re_val = device_control_used.control_bits.Lane_lamp_bit;
		break;

	case BAK1_USED:
		re_val = device_control_used.control_bits.control_bak1_bit;
		break;

	case BAK2_USED:
		re_val = device_control_used.control_bits.control_bak2_bit;
		break;

	default:
		break;
	}
#endif

	return re_val;
}

/***********************************************************************************
 * ������:	Command_Act 
 * ����: 
 *           	-���ݿ�����,ֱ�ӿ���IO��
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
void Command_Act(uint8_t act_word)
{
	uint8_t i = 0;

	for (i = 0; i < 8; i++)
	{
		if (act_word & BIT(i))
		{
			GPIO_SetBits(device_ctrl_queue[i].gpio_grp, device_ctrl_queue[i].gpio_pin);
		}
		else
		{
			GPIO_ResetBits(device_ctrl_queue[i].gpio_grp, device_ctrl_queue[i].gpio_pin);
		}
	}
}

/***********************************************************************************
 * ������:	detect_ALG_TTL_working 
 * ����: 
 *           	-�ڻظ�PC����Ϣ��ʱ��,����¶���ִ���Ƿ�ɹ�.
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
bool detect_ALG_TTL_working(void)
{
	// ̧���Ѿ���ʼ
	if(ALG_up_flag_bit)
	{
		if (Status_Get(ALG))
		{
			device_status_used.status_bits.ALG_work_status = OPT_OK;	//̧�˳ɹ�
		}
		else
		{
			device_status_used.status_bits.ALG_work_status = OPT_FAIL;	//̧�ܳ���
		}
	}

	// ���ܶ����Ѿ���ʼ
	if(ALG_down_flag_bit)
	{
		if (Status_Get(ALG))
		{
			device_status_used.status_bits.ALG_work_status = OPT_FAIL;	//���ܳ���
		}
		else
		{
			device_status_used.status_bits.ALG_work_status = OPT_OK;	//���ܳɹ�
		}
	}

	/*����������ǿ��ƻ���״̬����0:���, 1: �̵�*/
	if (Status_Get(TTL) == Command_Get(TTL_GREEN))
	{
		device_status_used.status_bits.TTL_work_status = OPT_OK;
	}
	else
	{
		device_status_used.status_bits.TTL_work_status = OPT_FAIL;
	}

	if (device_status_used.status_bits.ALG_work_status ||device_status_used.status_bits.ALG_work_status)
	{
		return OPT_FAIL;
	}
	else
	{
		return OPT_OK;
	}
}


// 
/***********************************************************************************
 * ������:	Status_Get 
 * ����: 
 *           	-��ȡָ�����贫������ֵ.
 * 	�ڸտ�ʼ���Զ�����ʱP2������״̬�ͺ���������ǲ�һ����
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
uint8_t Status_Get(DEVICE_STATUS_LIST dev)
{
	uint8_t re_val = 0;
	
	switch(dev)
	{
	case TTL:
		re_val = device_status_used.status_bits.TTL_detect_bit;
		break;

	case LG:
		re_val = device_status_used.status_bits.LG_detect_bit;
		break;

	case ALARM:
		re_val = device_status_used.status_bits.Alarm_detect_bit;
		break;

	case ALG:
		re_val = device_status_used.status_bits.ALG_detect_bit;
		break;

	case FRONT_COIL:
		re_val = device_status_used.status_bits.FRONT_coil_bit;
		break;

	case BACK_COIL:
		re_val = device_status_used.status_bits.BACK_coil_bit;
		break;

	case BAK1:
		re_val = device_status_used.status_bits.bakup_detect1_bit;
		break;

	case BAK2:
		re_val = device_status_used.status_bits.bakup_detect2_bit;
		break;

	default:
		break;
	}

	return (re_val?TRUE:FALSE);
}

/******************************************************************************
 * ������:	control_device_expt_lg 
 * ����: �豸���ƺ��������Ƴ�������������豸
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.15
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void control_device_expt_lg(void)
{
	// ����ƿ���
	if(Command_Get(TTL_GREEN))
	{
		DeviceX_Activate(TTL_GREEN);
	}
	else
	{
		DeviceX_Deactivate(TTL_GREEN);
	}

	// ���ⱨ������һ��
	if(Command_Get(VOX_ALM) || (Command_Get(LIGHT_ALM)))
	{
		DeviceX_Activate(VOX_ALM);
	}
	else
	{
		DeviceX_Deactivate(VOX_ALM);
	}

#if 0
	// �ƹⱨ��
	if(Command_Get(LIGHT_ALM))
	{
		DeviceX_Activate(LIGHT_ALM);
	}
	else
	{
		DeviceX_Deactivate(LIGHT_ALM);
	}
#endif

	// ����ͨ�е�
	if(Command_Get(LAN_LAMP))
	{
		DeviceX_Activate(LAN_LAMP);
	}
	else
	{
		DeviceX_Deactivate(LAN_LAMP);
	}

	// ����2
	if(Command_Get(BAK2_USED))
	{
		DeviceX_Activate(BAK2_USED);
	}
	else
	{
		DeviceX_Deactivate(BAK2_USED);
	}
}


/******************************************************************************
 * ������:	control_device 
 * ����: �豸���ƺ��������ݿ���״̬��, ������Ӧ�������
 *         		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:2018.11.08
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void control_device(void)
{
	/* �տ�ʼ����Ч*/
	DeviceX_Deactivate(BAR_UP);
	DeviceX_Deactivate(BAR_DOWN);

	/* ALG_down_bit ��ʼ����ʱ��Ϊ1*/
	if(Command_Get(BAR_DOWN))
	{
		// ����Ȧû��,�ߵ�ƽ
		if (Status_Get(BACK_COIL))
		{
			message_send_printf(FEE_UART, TRUE, FEE_R_MSG); // ����, ����ҪΪ��ɫ
			Drop_LG_Start(); 							//����
			system_flag |= CAR_WATCHING;
			WatchingDelayCount = 4*ONE_SECOND;		//���Ҽ���4S
		}
		else
		{
			// ����Ȧ�г�, �����Զ�����߼�
			autoBarEnable = TRUE;
			device_control_used.control_bits.Lane_lamp_bit = 1; //ǿ�иı�ͨ�е�����,��Ϊû����
		}
	}

	// ALG_up_bit ��ʼ����ʱ��Ϊ0
	if(Command_Get(BAR_UP))
	{
		message_send_printf(FEE_UART, TRUE, FEE_G_MSG); // ̧�ˣ���ɫ
		Rise_LG_Start(); 				//̧��
	}

	control_device_expt_lg();

	// ���ݳ���ͨ�е�λ.
	LastLaneLampState = device_control_used.control_bits.Lane_lamp_bit;

	/* ��ʱ250ms, ��Ϊ������Ҫһ���������*/
	DelayAndFeedDog(250);
	/*  ����������*/
	LG_Act_End();

	/* ���������ʱ��*/
	detect_time_counter = AUTO_DETCET_TIME;
}


/***********************************************************************************
 * ������:	BarOpRectifyLaneLamp 
 * ����: 
 *           			�Զ������߼����Է����յ�����λ��������ì�ܵ�
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:20181113
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 *
 *
 ***********************************************************************************/
static void BarOpRectifyLaneLamp(void)
{
	if(Command_Get(BAR_UP)&Command_Get(BAR_DOWN)) 	//�������
	{
		device_control_used.control_bits.ALG_down_bit = 0;		// ���ͬʱ���յ�̧�ܺͽ��ܣ���̧��
	}

	if(Command_Get(BAR_UP))
	{
		//�̵�, ���̧�ܣ���������̵�
		device_control_used.control_bits.Lane_lamp_bit = 1;
	}

	if(Command_Get(BAR_DOWN))
	{
		//���, ������ܣ�����������
		device_control_used.control_bits.Lane_lamp_bit = 0;
	}

	// ���̧�ܽ��ܶ�û������,�򱣳��ϴε�����״̬
	if((Command_Get(BAR_UP)|Command_Get(BAR_DOWN)) == FALSE)
	{
		device_control_used.control_bits.Lane_lamp_bit = LastLaneLampState;
	}
}


/***********************************************************************************
 * ������:	ClearFEEdisplay 
 * ����: 
 *           			 -���������ʾ
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:������
 * ��������:20181109
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 *
 *
 ***********************************************************************************/
void ClearFEEdisplay(void)
{
	// �Ѿ��������,�����ٴ����
	if (bFeeCleared)
	{
	 	return;
	}

	bFeeCleared = TRUE;
	if(bLastLaneRedGreenOperateState == GREEN)
	{
		if(device_control_used.control_bits.bFeeDispSelection ==0)
		{
			//���������汾�ķ��ԣ�ֻҪ���̵Ʋ�����Ѷ�
			;
		}
		else
		{
			//���ڹ㶫�汾�ķ��ԣ�ֻҪ���̵ƻ��ƶ�����Ѷ�
			message_send_printf(FEE_UART, TRUE, COST_OFF_MSG);
			
		}
	}
	else
	{
		//�������ĸ��汾�ķ��ԣ�ֻҪ����ƶ�����Ѷ�
		message_send_printf(FEE_UART, TRUE, COST_OFF_MSG);
	}
}


/***********************************************************************************
 * ������:	params_modify_deal 
 * ����: 
 *           	-ͨѶ�����Ĳ�������
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
void params_modify_deal(void)
{
	/*�յ�̧������*/
	if(Command_Get(BAR_UP))
	{
		autoBarEnable = device_control_used.control_bits.ALG_control_mode;		/*��ȡ�Զ�����ģʽ*/
		/*���������λ, �Ƿҳ���վ��������ң�����˻�������*/
		device_control_used.control_bits.ALG_control_mode = 0;
		system_flag &= ~CAR_WATCHING;
		//watching_car_passing=0;
		/*̧�˺�ʼ�ƺ���Ȧ���޳��� P20100603*/
		dete_bit_recd = FALSE;
	}

	/*�����ֳ������豸�߼��������������λ*/
	BarOpRectifyLaneLamp();
	control_device();			/*�յ�������ٴζ�������п���*/

	//����������
	if(device_control_used.control_bits.SPEAK_control_bit == 1)
	{
		/* ����������û�б�����*/
		if((Command_Get(BAR_UP) == 0)&&(Command_Get(BAR_DOWN) == 0))
		{	
			//message_send_printf(FEE_UART, TRUE, SPK_MSG);// ���������,��ʱ�����
			
		}

		/* �����̧������, �ͷ����̵�һ��������*/
		/* ����Ҫע�⴮�ڵ�ѡ��*/
		if(Command_Get(BAR_UP))
		{
			message_send_printf(FEE_UART, TRUE, FEE_G_MSG);
		}
	}
	//�ô���ʱԼ����if�ڵ�ʱ�䣬��֤�����Է��ԵĲ����ļ��ʱ��
	else
	{
		Delay_Ms(50);		// ���ʱ����Ҫ��
	}

	//�Ѷ���ʾ����,��������ַ���,����Ҫ����
	if(device_control_used.control_bits.FEE_displaykeep_bit ==0)
	{
		// �Ѷ���Ҫ��ʾ
		if(device_control_used.control_bits.FEE_display_bit == 1)
		{
			message_send_printf(FEE_UART, TRUE, COST_ON_MSG);
		}
		else
		{
			//����Ѷ����(��Щ���̵�Ҳ����)������Ѷ���ʾ
			ClearFEEdisplay();	
		}
	}
}
