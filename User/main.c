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
	/* ��ס�ϴ�����״̬���Ĵ���״̬,�Ա㷵�ص�ʱ���ϱ�*/
	//static bool TTL_ALG_Wrong = FALSE;
	//USART_LIST i = PC_UART;

	//uint8_t datatemp[64];
	
	/* ��⵽�ⲿ��������λ,���߿����߼��ͼ���߼������*/
	if (system_flag&KEY_CHANGED)
	{
		system_flag &= ~KEY_CHANGED;
	}


	/* �豸ÿ��3s�Զ������״̬*/
	if (system_flag&SYS_ERR_CHK)				//�豸����״̬�Զ���ⲿ��	
	{
		system_flag &= ~SYS_ERR_CHK;
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
		Comm_Proc();
		Task_Schedule();			// ϵͳ�����б�
	}
}
/*********************************************END OF FILE**********************/

