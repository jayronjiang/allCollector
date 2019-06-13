/**
  ******************************************************************************
  * @file    bsp_usart5.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   USART3�弶Ӧ��.
  *
  ******************************************************************************
  */
#include "include.h"
  
#if (BD_USART_NUM >= 4)

#define USARTn 		UART5
#define UART_COM 	UART5_COM
 /******************************************************************************
 * ������:	USART2_Config 
 * ����: USART2 GPIO ����,����ģʽ���á�115200 8-N-1
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
void USART5_Config(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART5 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5,ENABLE);
	
	/* USART3 GPIO config */
	/* Configure UART5 Tx (PC.12) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOC, &GPIO_InitStructure);    
	/* Configure UART5 Rx (PD.2) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	/* UART5 mode config */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USARTn, &USART_InitStructure);
	
	/* ʹ�ܴ���5�����ж� */
	USART_ITConfig(USARTn, USART_IT_RXNE, ENABLE);
	/* ʹ�ܴ���2�����ж� */
	USART_ITConfig(USARTn, USART_IT_IDLE, ENABLE);
	
	USART_Cmd(USARTn, ENABLE);
	
	NVIC_USART5_Configuration();
	
	//printf("USART2��ʼ�����\n");
}

 /******************************************************************************
 * ������:	NVIC_USART2_Configuration 
 * ����: ����USART2�ж�
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
void NVIC_USART5_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/******************************************************************************
 * ������:	Param_USART2_Init 
 * ����: �����ʼ��
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
static void Param_USART5_Init(void)
{
	/*��ʼ������㻺��������*/
	UARTBuf[UART_COM].RxLen = 0;
	UARTBuf[UART_COM].TxLen = 0;
	UARTBuf[UART_COM].TxPoint= 0;
	UARTBuf[UART_COM].RecFlag= 0;
	UARTBuf[UART_COM].Timer =0;
}

 /******************************************************************************
 * ������:	USART2_Init 
 * ����: USART2��ʼ��
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
void USART5_Init(uint32_t baudrate)
{
	USART5_Config(baudrate);
	Param_USART5_Init();
}


 /******************************************************************************
 * ������:	USART2_IRQHandler 
 * ����: USART2�жϴ���, ��������жϺͿ����ж�.
 *	USART_IT_RXNE: û����һ���ֽڴ���һ��.
 *	USART_IT_IDLE: ������ϴ���,���Է��ּ�����Ϊ30ms
 *ע��:
 *	���ϸ��֡�ж�Ӧ��ʹ�ö�ʱ��UARTProcessTickEvents.
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
void UART5_IRQHandler(void)
{
	uint8_t ch;
	
	if(USART_GetITStatus(USARTn, USART_IT_RXNE) != RESET)
	{	
		//LED_Set(LED_COM, ON); 	// ��ʼͨ��ָʾ
		ch = USART_ReceiveData(USARTn);

		#if 0
		UARTBuf[UART_COM].RxBuf[UARTBuf[UART_COM].RxLen] = ch ;
		if(UARTBuf[UART_COM].RxLen < UART_RXBUF_SIZE)
		{
			UARTBuf[UART_COM].RxLen++;
			/*��Ҫ���ݲ����������ʱ���жϴ���ͨ��һ֡�����Ƿ����*/
			//UART0Buf.Timer = 50;		// ���50ms��û������,��������֡����
		}
		#endif
		ReceOneChar(ch);
	} 
	else if (USART_GetITStatus(USARTn, USART_IT_IDLE) != RESET)	// ֱ��ʹ�ÿ���֡�ж�
	{
		/* ��Ҫ����2���Ĵ�������жϱ�־*/
		/*USART_GetITStatus �����Ѿ�����SR�Ĵ���*/
		//ch = USART2->SR;
		/*��DR�Ĵ��������־*/
		ch = USART_ReceiveData(USARTn);
		//LED_Set(LED_COM, OFF); 	// ͨ�����

		if (g_PDUData.PDULength == REAL_DATA_NUM)
		{
			UARTBuf[UART_COM].RecFlag = TRUE;
			data_received_handle(UART5_COM);
		}
		else
		{
			g_PDUData.PDULength = 0;	// ׼���ٴν���
		}
	}
}
#endif
/*********************************************END OF FILE**********************/

