/**
  * @file    uart_driver.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   usart应用
  * ******************************************************************************
  */
  
#include "include.h"

const UINT32  Baud[BAUD_NUM] = { 1200,2400,4800,9600,19200,115200};

/*UART数据缓冲区*/
UART_BUF	 UARTBuf[UART_NUM];

/* 由于printf有缺陷, 这个变量没有实质使用*/
USART_LIST uart_sel = PC_UART; // 为了重定向到3个串口,定义一个选择变量.

const USART_LIST bd_com[BD_USART_NUM] = \
{
	BD1_UART,
#if (BD_USART_NUM >= 2)
	BD2_UART,
#endif
#if (BD_USART_NUM >= 3)
	BD3_UART,
#endif
#if (BD_USART_NUM >= 4)
	BD4_UART
#endif
};

/******************************************************************************
 * 函数名:	UARTProcessTickEvents 
 * 描述: 
 *            -判断串口通信一帧数据是否结束。
 *		  本函数由1ms定时中断每毫秒调用一次,当超过
 *		  UART[x]Buf.Timer时间无数据timer自减为0,表明数据帧接收完毕;
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 *------------------------
 * 修改人:Jerry
 * 修改日期:2018.11.21
 ******************************************************************************/
void UARTProcessTickEvents(void)
{
	USART_LIST i = UART1_COM;

	for (i = UART1_COM; i < UART_NUM; i++)
	{
		if( UARTBuf[i].Timer > 0 )
		{
			UARTBuf[i].Timer--;
			if( UARTBuf[i].Timer == 0 )
			{
				UARTBuf[i].RecFlag = 1;
			}
		}
	}
}

/******************************************************************************
 * 函数名:	fputc 
 * 描述: 
 *            -重定向c库函数printf到USARTx, 系统自带
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 *------------------------
 * 修改人:Jerry
 * 修改日期:2018.11.21
 ******************************************************************************/
int fputc(int ch, FILE *f)
{
	USART_TypeDef *pusart=USART1;

	if (uart_sel == UART1_COM)
	{
		pusart = USART1;
	}
#if (BD_USART_NUM >= 2)
	else if (uart_sel == UART2_COM)
	{
		pusart = USART2;
	}
#endif
#if (BD_USART_NUM >= 3)
	else if (uart_sel == UART4_COM)
	{
		pusart = UART4;
	}
#endif
	else
	{
		pusart = UART5;
	}

	/* 发送一个字节数据到USARTx */
	USART_SendData(pusart, (uint8_t) ch);
		
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(pusart, USART_FLAG_TXE) == RESET);		
	
	return (ch);
}


/******************************************************************************
 * 函数名:	fputc 
 * 描述: 
 *            -重定向c库函数scanf到USARTx, 系统自带
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 *------------------------
 * 修改人:Jerry
 * 修改日期:2018.11.21
 ******************************************************************************/
int fgetc(FILE *f)
{
	USART_TypeDef *pusart=USART1;

	if (uart_sel == UART1_COM)
	{
		pusart = USART1;
	}
#if (BD_USART_NUM >= 2)
	else if (uart_sel == UART2_COM)
	{
		pusart = USART2;
	}
#endif
#if (BD_USART_NUM >= 3)
	else if (uart_sel == UART4_COM)
	{
		pusart = UART4;
	}
#endif
	else
	{
		pusart = UART5;
	}
	
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(pusart, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(pusart);
}

#ifdef SPEAKER_ENABLE
/******************************************************************************
 * 函数名:	UART_fputc 
 * 描述: 
 *            -移植ATE_SHELL的输出接口函数
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 *------------------------
 * 修改人:Jerry
 * 修改日期:2018.12.03
 ******************************************************************************/
int UART_fputc(int ch)
{
	/* 发送一个字节数据到USARTx */
	USART_SendData(SHELL_USART, (uint8_t) ch);
		
	/* 等待发送完毕 */
	while (USART_GetFlagStatus(SHELL_USART, USART_FLAG_TXE) == RESET);		
	
	return (ch);
}

/******************************************************************************
 * 函数名:	UART_fgetc 
 * 描述: 
 *            -移植ATE_SHELL的输入接口函数
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 *------------------------
 * 修改人:Jerry
 * 修改日期:2018.11.21
 ******************************************************************************/
int  UART_fgetc(void)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(SHELL_USART, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(SHELL_USART);
}
#endif

/*********************************************END OF FILE**********************/
