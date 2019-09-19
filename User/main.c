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

#if 0
const uint8_t TEXT_Buffer[]={"WarShipSTM32 SPI TEST\n"};
const uint32_t FLASH_SIZE=32*1024*1024;		//FLASH ��СΪ2M�ֽ�
#define SIZE (sizeof(TEXT_Buffer))
#endif

/******************************************************************************
 * ������:	DO_Pulse_End_Task 
 * ����: ����DO��������,���ƴű��̵ּ���
 *		�����Ƽ�Ϊ100ms
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
 static void DO_Pulse_End_Task(void)
{
	uint8_t  t_dir = 0, t_id = 0;

	for (t_dir = 0; t_dir < DIRECTION_NUM; t_dir++)
	{
		for (t_id = 0; t_id < MAX_NUMBER_OF_SW_TIMERS; t_id++) 
		{
			/*100ms����ʱ�䵽, ��ʱ����0*/
			if (swt_20_ms_check_and_clear(t_dir,t_id))
			{
				swt_20_ms_release(t_dir,t_id);
				DeviceX_Deactivate((DEVICE_CTRL_LIST)t_id, t_dir);
			}
		}
	}
}
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
#ifdef HAS_8I8O
	uint8_t  i = 0;
	/* ��⿪����,(�Ŵ�,����,©ˮ)*/
	if (system_flag&KEY_CHANGED)
	{
		system_flag &= ~KEY_CHANGED;
		for (i = 0; i < DI_NUM; i++)
		{
			input[i] = ((di_status.status_word&BIT(i)) ? 1:0);
		}
	}
#endif
	/* �豸ÿ��1s�Զ�����²����Ƿ����*/
	if (system_flag&PARAM_UPDATE)
	{
		system_flag &= ~PARAM_UPDATE;
		if (system_flag&DEV_MODIFIED)
		{
			system_flag &= ~DEV_MODIFIED;
			Write_DevParams();
			system_flag |= PARAM_CHECK;	// д�������Ҫ�Լ�һ��flash
		}
	}

	/*ÿ20s����Ƿ���дflash, ����������һ�μ��*/
	if (system_flag&SYS_ERR_CHK)
	{
		system_flag &= ~ SYS_ERR_CHK;
		if (system_flag&PARAM_CHECK)
		{
			system_flag &= ~PARAM_CHECK;
			Self_Check();
		}
		// ÿ20s��˸һ��comm��,��ʾflash�쳣
		if (SystemStatus)
		{
			/*ע����ʱ��Ҫ̫�����ᵼ�¿��Ź����*/
			LED_Flashing(LED_COM, 100, 2);
		}
	}

	DO_Pulse_End_Task();
	if ((finish_flag == (BIT(TEST_NUMBER_OF_TESTS)-1))&&(test_mode_is_entry()))
	{
		test_indication();
	}

#if 0
		/* ����SPI, LG��ӦKEY1, ALARM ��ӦKEY0*/
		if(system_flag &SYSTEM_200MS)		// key1 ����
		{
			debug_puts("Start Write W25Q128...");
			W25QXX_Write((uint8_t*)TEXT_Buffer,FLASH_SIZE-100,SIZE);	//�ӵ�����100����ַ����ʼ,д��SIZE���ȵ�����
			debug_puts("W25Q128 Write Finished!");
			system_flag &= ~SYSTEM_200MS;
			debug_puts("Start Read W25Q128...");
			W25QXX_Read(CS_0,datatemp,FLASH_SIZE-100,32);//�ӵ�����100����ַ����ʼ,����SIZE���ֽ�
			debug_puts("The Data Readed Is:");
			debug_puts(datatemp);
		}
#endif

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
		comm_rec_proc();			// CLIENT
		//comm_polling_process();	//SERVER
		Task_Schedule();			// ϵͳ�����б�
		test_mode_update();		// ����״̬��
	}
}
/*********************************************END OF FILE**********************/

