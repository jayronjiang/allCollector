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

const uint8_t TEXT_Buffer[]={"WarShipSTM32 SPI TEST\n"};
const uint32_t FLASH_SIZE=32*1024*1024;		//FLASH ��СΪ2M�ֽ�
#define SIZE (sizeof(TEXT_Buffer))

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
	static uint32_t front_door_timeout_t=0;
	static uint32_t back_door_timeout_t=0;
	static uint8_t front_flag = FALSE;
	static uint8_t back_flag = FALSE;
	/* ��ס�ϴ�����״̬���Ĵ���״̬,�Ա㷵�ص�ʱ���ϱ�*/
	//static bool TTL_ALG_Wrong = FALSE;
	//USART_LIST i = PC_UART;

	//uint8_t datatemp[64];
	
	/* ��⿪����,(�Ŵ�,����,©ˮ)*/
	if (system_flag&KEY_CHANGED)
	{
		system_flag &= ~KEY_CHANGED;
		 // di�ǵ͵�ƽ��Ч,flag�Ǹߵ�ƽ��Ч
		 // 2���Ŵſ��أ��κ�һ���򿪾���Ϊ���ű���
		ENVIParms.front_door_flag = di_status.status_bits.di_1;
		ENVIParms.back_door_flag = di_status.status_bits.di_2;
		ENVIParms.smoke_event_flag = !di_status.status_bits.di_3;	 // ��������״̬
	}

	/* di�͵�ƽ��Ч,���Ǳ�־λΪ�ߵ�ƽ�߼�*/
	if (ENVIParms.front_door_flag == 1)
	{
		// ����1��Сʱ,��ʱ,���Ե�ʱ��Ϊ10s
		if(time_interval(front_door_timeout_t) >= DOOR_TIME_OUT)		//��ʱ��������ʼ
		{
			front_door_timeout_t = system_time_s;
			ENVIParms.door_overtime = TRUE;		// �κ�һ���Ŵ򿪳�ʱ���㳬ʱ
			front_flag = TRUE;
		}
	}
	else
	{
		//ENVIParms.door_overtime = FALSE;
		front_flag = FALSE;
		front_door_timeout_t = system_time_s;
		if( !back_flag )		// ֻ�ж�����ʱ�Ų��㳬ʱ
		{
			ENVIParms.door_overtime = FALSE;
		}
	}

	/* di�͵�ƽ��Ч,���Ǳ�־λΪ�ߵ�ƽ�߼�*/
	if (ENVIParms.back_door_flag == 1)
	{
		// ����1��Сʱ,��ʱ,���Ե�ʱ��Ϊ10s
		if(time_interval(back_door_timeout_t) >= DOOR_TIME_OUT)		//��ʱ��������ʼ
		{
			back_door_timeout_t = system_time_s;
			ENVIParms.door_overtime = TRUE;
			back_flag = TRUE;
		}
	}
	else
	{
		//ENVIParms.door_overtime = FALSE;
		back_flag = FALSE;
		back_door_timeout_t = system_time_s;
		if( !front_flag )
		{
			ENVIParms.door_overtime = FALSE;
		}
	}


	/* �豸ÿ��3s�Զ������״̬*/
	if (system_flag&SYS_ERR_CHK)				//�豸����״̬�Զ���ⲿ��	
	{
		system_flag &= ~SYS_ERR_CHK;
		//����Ҫ�Ӳ������������
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
		comm_polling_process();	//SERVER
		Task_Schedule();			// ϵͳ�����б�
	}
}
/*********************************************END OF FILE**********************/

