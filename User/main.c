 /**
  ******************************************************************************
  * @file    main.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   Main ����, ϵͳ���.
  *
  ******************************************************************************
  */

#include "include.h"

/*����ϵͳ��־��,ʹ�����е�λ�����д�������¼�.*/
uint16_t system_flag = 0;

/******************************************************************************
 * ������:	Task_Schedule 
 * ����: ������������, ��������¼�������
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.11.21
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
static void Task_Schedule(void)
{
	/* ��ס�ϴ�����״̬���Ĵ���״̬,�Ա㷵�ص�ʱ���ϱ�*/
	static bool TTL_ALG_Wrong = FALSE;
	USART_LIST i = PC1_UART;

	//uint8_t datatemp[64];
	
	#ifdef TEST
	/*�����д�����ɽ��̫����ʹ�ã�����֪������ʲôĿ��*/
		if(Status_Get(TTL)==0)
		{
			message_send_printf(PC1_UART, TRUE, TEST_MSG);
		}
	#endif

	/* ��⵽�ⲿ��������λ,���߿����߼��ͼ���߼������*/
	if (system_flag&SYS_CHANGED)
	{
		system_flag &= ~SYS_CHANGED;
		if (Status_Get(ALG) == 0)	// ����ǽ���״̬,��ȡ������
		{
			system_flag &= ~CAR_WATCHING;
		}
		/* �ȿ����Ƿ��ж����쳣*/
		detect_ALG_TTL_working();
		/*֪ͨ������λ���б�λ*/
		for (i = pc_com[0]; i <= pc_com[PC_USART_NUM-1]; i++)
		{
			message_send_printf(i, TRUE, B_RES_MSG);
		}
		
		if(Status_Get(BACK_COIL) == 0)	 	/* ������μ�⵽�г�*/
		{
			/*���浽�ϴ�״̬*/
			dete_bit_recd = TRUE;
		}
	}

/*******************���ҹ����߼�************************************************/
	/* ���ܹ����з��ֺ���Ȧ�г�, ̧��,���ٴν����Զ����ܹ���*/
	if (system_flag&CAR_WATCHING)
	{
		/*����Ȧ�г�*/
		if(Status_Get(BACK_COIL) == 0)	
		{
			/*���ڽ���...*/
			if(ALG_down_flag_bit&Status_Get(ALG))
			{
				device_control_used.control_bits.ALG_up_bit = 1; /*̧��*/
				device_control_used.control_bits.ALG_down_bit = 0;
				control_device();
				autoBarEnable = TRUE; 
			}
		}
	}

	/*�����Զ�����*/
	if (autoBarEnable)
	{
		/* ����ϴ��г�, ����޳�,��ʾ���Ѿ��뿪*/
		if(Status_Get(BACK_COIL))				//Back Coil Flag =1, �����޳�
		{
			if(dete_bit_recd == TRUE)			//�ϴ��г�
			{
				dete_bit_recd = FALSE;
				autoBarEnable = FALSE;		//��λΪ�ֶ�������ģʽ	
				device_control_used.control_bits.ALG_up_bit = 0; 
				device_control_used.control_bits.ALG_down_bit = 1;		/*��ʼ����*/
				device_control_used.control_bits.Lane_lamp_bit = 0;		/*�����*/
				control_device();
				system_flag |= CAR_WATCHING;
			}
		}
	}
/*******************���ҹ����߼�����********************************************/

	/*�ֳ����˰�����,����10s,һֱ���ͻ�һֱ��*/
	if(Status_Get(ALARM) == 0)		//valid is 0
	{
		alarm_time_counter = ALARM_TIME;
		device_control_used.control_bits.VOX_alarm_bit = 1;
		device_control_used.control_bits.Light_alarm_bit = 1;
		control_device_expt_lg();
	}

	/* �豸ÿ��3s�Զ������״̬*/
	if (system_flag&SYS_ERR_CHK)				//�豸����״̬�Զ���ⲿ��	
	{
		system_flag &= ~SYS_ERR_CHK;
    		if(detect_ALG_TTL_working())
		{
			TTL_ALG_Wrong = TRUE;			//����Ǵ���״̬��ÿ���ñ�־��λʱ����λ���㱨
			system_flag |= SYS_CHANGED;
		}
		else
		{
			/*�Ӵ���״̬���ص�����״̬ʱ����ʱ����λ���㱨*/
			if(TTL_ALG_Wrong) 
			{
				system_flag |= SYS_CHANGED;
			}
			TTL_ALG_Wrong = FALSE;
		}
	}

}


/******************************************************************************
 * ������:	main 
 * ����: ����ʼ���.
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:2018.10.21
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
int main(void)
{
	Init_System();
	while(1)
	{
		/*��������ʼ��,��ʱ�ж�ι��,��ͬʱ��ֹ���������жϳ����ܷ�*/
		wdt_counter = 0; 
		Comm_Proc();
		Task_Schedule();			// ϵͳ�����б�
	}
}
/*********************************************END OF FILE**********************/

