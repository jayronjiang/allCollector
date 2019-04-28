/**
  * @file    uart_driver.c
  * @version V1.0
  * @date    2018-xx-xx
  * @brief   usartӦ��
  * ******************************************************************************
  */
  
#include "include.h"

/*UART���ݻ�����*/
UART_BUF	 UARTBuf[UART_NUM];

/* ����printf��ȱ��, �������û��ʵ��ʹ��*/
USART_LIST uart_sel = PC1_UART; // Ϊ���ض���3������,����һ��ѡ�����.

const USART_LIST pc_com[PC_USART_NUM] = \
{
	PC1_UART
	#if (PC_USART_NUM == 2)
	,PC2_UART
	#endif
};

/******************************************************************************
 * ������:	UARTProcessTickEvents 
 * ����: 
 *            -�жϴ���ͨ��һ֡�����Ƿ������
 *		  ��������1ms��ʱ�ж�ÿ�������һ��,������
 *		  UART[x]Buf.Timerʱ��������timer�Լ�Ϊ0,��������֡�������;
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 *------------------------
 * �޸���:Jerry
 * �޸�����:2018.11.21
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
 * ������:	fputc 
 * ����: 
 *            -�ض���c�⺯��printf��USARTx, ϵͳ�Դ�
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 *------------------------
 * �޸���:Jerry
 * �޸�����:2018.11.21
 ******************************************************************************/
int fputc(int ch, FILE *f)
{
	USART_TypeDef *pusart=USART1;

	if (uart_sel == UART1_COM)
	{
		pusart = USART1;
	}
#if (BD_USART_NUM == 2)
	else if (uart_sel == UART2_COM)
	{
		pusart = USART2;
	}
#endif
	else
	{
		pusart = USART1;
	}


	/* ����һ���ֽ����ݵ�USARTx */
	USART_SendData(pusart, (uint8_t) ch);
		
	/* �ȴ�������� */
	while (USART_GetFlagStatus(pusart, USART_FLAG_TXE) == RESET);		
	
	return (ch);
}


/******************************************************************************
 * ������:	fputc 
 * ����: 
 *            -�ض���c�⺯��scanf��USARTx, ϵͳ�Դ�
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 *------------------------
 * �޸���:Jerry
 * �޸�����:2018.11.21
 ******************************************************************************/
int fgetc(FILE *f)
{
	USART_TypeDef *pusart=USART1;

	if (uart_sel == UART1_COM)
	{
		pusart = USART1;
	}
#if (BD_USART_NUM == 2)
	else if (uart_sel == UART2_COM)
	{
		pusart = USART2;
	}
#endif
	else
	{
		pusart = USART1;
	}
	
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(pusart, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(pusart);
}

#ifdef SPEAKER_ENABLE
/******************************************************************************
 * ������:	UART_fputc 
 * ����: 
 *            -��ֲATE_SHELL������ӿں���
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 *------------------------
 * �޸���:Jerry
 * �޸�����:2018.12.03
 ******************************************************************************/
int UART_fputc(int ch)
{
	/* ����һ���ֽ����ݵ�USARTx */
	USART_SendData(SHELL_USART, (uint8_t) ch);
		
	/* �ȴ�������� */
	while (USART_GetFlagStatus(SHELL_USART, USART_FLAG_TXE) == RESET);		
	
	return (ch);
}

/******************************************************************************
 * ������:	UART_fgetc 
 * ����: 
 *            -��ֲATE_SHELL������ӿں���
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 *------------------------
 * �޸���:Jerry
 * �޸�����:2018.11.21
 ******************************************************************************/
int  UART_fgetc(void)
{
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(SHELL_USART, USART_FLAG_RXNE) == RESET);

	return (int)USART_ReceiveData(SHELL_USART);
}
#endif

/*********************************************END OF FILE**********************/
