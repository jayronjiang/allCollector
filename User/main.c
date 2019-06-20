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

const uint8_t TEXT_Buffer[]={"WarShipSTM32 SPI TEST\n"};
const uint32_t FLASH_SIZE=32*1024*1024;		//FLASH 大小为2M字节
#define SIZE (sizeof(TEXT_Buffer))

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
	static uint32_t front_door_timeout_t=0;
	static uint32_t back_door_timeout_t=0;
	static uint8_t front_flag = FALSE;
	static uint8_t back_flag = FALSE;
	/* 记住上次栏杆状态检测的错误状态,以便返回的时候上报*/
	//static bool TTL_ALG_Wrong = FALSE;
	//USART_LIST i = PC_UART;

	//uint8_t datatemp[64];
	
	/* 检测开关量,(门磁,烟雾,漏水)*/
	if (system_flag&KEY_CHANGED)
	{
		system_flag &= ~KEY_CHANGED;
		 // di是低电平有效,flag是高电平有效
		 // 2个门磁开关，任何一个打开就认为柜门被打开
		ENVIParms.front_door_flag = di_status.status_bits.di_1;
		ENVIParms.back_door_flag = di_status.status_bits.di_2;
		ENVIParms.smoke_event_flag = !di_status.status_bits.di_3;	 // 烟雾传感器状态
	}

	/* di低电平有效,但是标志位为高电平逻辑*/
	if (ENVIParms.front_door_flag == 1)
	{
		// 超过1个小时,超时,测试的时候为10s
		if(time_interval(front_door_timeout_t) >= DOOR_TIME_OUT)		//超时计数器开始
		{
			front_door_timeout_t = system_time_s;
			ENVIParms.door_overtime = TRUE;		// 任何一个门打开超时就算超时
			front_flag = TRUE;
		}
	}
	else
	{
		//ENVIParms.door_overtime = FALSE;
		front_flag = FALSE;
		front_door_timeout_t = system_time_s;
		if( !back_flag )		// 只有都不超时才不算超时
		{
			ENVIParms.door_overtime = FALSE;
		}
	}

	/* di低电平有效,但是标志位为高电平逻辑*/
	if (ENVIParms.back_door_flag == 1)
	{
		// 超过1个小时,超时,测试的时候为10s
		if(time_interval(back_door_timeout_t) >= DOOR_TIME_OUT)		//超时计数器开始
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


	/* 设备每隔3s自动检测下状态*/
	if (system_flag&SYS_ERR_CHK)				//设备错误状态自动检测部分	
	{
		system_flag &= ~SYS_ERR_CHK;
		//这里要加参数保存的任务
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
		comm_polling_process();	//SERVER
		Task_Schedule();			// 系统任务列表
	}
}
/*********************************************END OF FILE**********************/

