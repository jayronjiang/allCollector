 /**
  ******************************************************************************
  * @file    main.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   Main 函数, 系统入口.
  *
  ******************************************************************************
  */

#include "include.h"

/*任务系统标志字,使用其中的位来进行处理各种事件.*/
uint16_t system_flag = 0;

#if 0
const uint8_t TEXT_Buffer[]={"WarShipSTM32 SPI TEST\n"};
const uint32_t FLASH_SIZE=32*1024*1024;		//FLASH 大小为2M字节
#define SIZE (sizeof(TEXT_Buffer))
#endif

/******************************************************************************
 * 函数名:	Task_Schedule 
 * 描述: 任务处理主函数, 处理各种事件和任务
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.11.21
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
static void Task_Schedule(void)
{
#ifdef HAS_8I8O
	uint8_t  i = 0;
	/* 检测开关量,(门磁,烟雾,漏水)*/
	if (system_flag&KEY_CHANGED)
	{
		system_flag &= ~KEY_CHANGED;
		for (i = 0; i < DI_NUM; i++)
		{
			input[i] = ((di_status.status_word&BIT(i)) ? 1:0);
		}
	}
#endif
	/* 设备每隔1s自动检测下参数是否更新*/
	if (system_flag&PARAM_UPDATE)
	{
		system_flag &= ~PARAM_UPDATE;
		if (system_flag&DEV_MODIFIED)
		{
			system_flag &= ~DEV_MODIFIED;
			Write_DevParams();
		}
	}

#if 0
		/* 测试SPI, LG对应KEY1, ALARM 对应KEY0*/
		if(system_flag &SYSTEM_200MS)		// key1 按下
		{
			debug_puts("Start Write W25Q128...");
			W25QXX_Write((uint8_t*)TEXT_Buffer,FLASH_SIZE-100,SIZE);	//从倒数第100个地址处开始,写入SIZE长度的数据
			debug_puts("W25Q128 Write Finished!");
			system_flag &= ~SYSTEM_200MS;
			debug_puts("Start Read W25Q128...");
			W25QXX_Read(CS_0,datatemp,FLASH_SIZE-100,32);//从倒数第100个地址处开始,读出SIZE个字节
			debug_puts("The Data Readed Is:");
			debug_puts(datatemp);
		}
#endif

}


/******************************************************************************
 * 函数名:	main 
 * 描述: 程序开始入口.
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:Jerry
 * 创建日期:2018.10.21
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
int main(void)
{
	Init_System();
	while(1)
	{
		/*主函数初始化,定时中断喂狗,能同时防止主函数和中断程序跑飞*/
		wdt_counter = 0;
		comm_rec_proc();			// CLIENT
		//comm_polling_process();	//SERVER
		Task_Schedule();			// 系统任务列表
	}
}
/*********************************************END OF FILE**********************/

