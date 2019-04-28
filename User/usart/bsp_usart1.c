/**
  ******************************************************************************
  * @file    bsp_usart1.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   USART1�弶Ӧ��.
  *
  ******************************************************************************
  */
#include "include.h"

 /******************************************************************************
 * ������:	USART1_Config 
 * ����: USART1 GPIO ����,����ģʽ���á�115200 8-N-1
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
void USART1_Config(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	/* config USART1 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);
	
	/* USART1 GPIO config */
	/* Configure USART1 Tx (PA.09) as alternate function push-pull */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);    
	/* Configure USART1 Rx (PA.10) as input floating */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/* USART1 mode config */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
	/* ʹ�ܴ���1�����ж� */
	// ע��,ʹ��printf ���Ͳ����жϷ�ʽ����,����Ҫ�򿪷����ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	/* ʹ�ܴ���1�����ж� */
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	
	USART_Cmd(USART1, ENABLE);
	
	//���ô���1��Ӧ���ж�����
	NVIC_USART1_Configuration();
	
	//printf("USART1��ʼ�����\n");
}

 /******************************************************************************
 * ������:	NVIC_USART1_Configuration 
 * ����: ����USART1�ж�
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
void NVIC_USART1_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure; 
	
	/* Configure the NVIC Preemption Priority Bits */  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	
	/* Enable the USARTy Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;	 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

 /******************************************************************************
 * ������:	Param_USART1_Init 
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
void Param_USART1_Init(void)
{
	/*��ʼ������㻺��������*/
	UARTBuf[UART1_COM].RxLen = 0;
	UARTBuf[UART1_COM].TxLen = 0;
	UARTBuf[UART1_COM].TxPoint= 0;
	UARTBuf[UART1_COM].RecFlag= 0;
	UARTBuf[UART1_COM].Timer =0;
}

 /******************************************************************************
 * ������:	USART1_Init 
 * ����: USART1��ʼ��
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
void USART1_Init(uint32_t baudrate)
{
	USART1_Config(baudrate);
	Param_USART1_Init();
}

 /******************************************************************************
 * ������:	USART1_IRQHandler 
 * ����: USART1�жϴ���, ��������жϺͿ����ж�.
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
void USART1_IRQHandler(void)
{
	uint8_t ch;
	
	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)
	{	
		LED_Set(LED_COM, ON); 	// ��ʼͨ��ָʾ
		ch = USART_ReceiveData(USART1);

		UARTBuf[UART1_COM].RxBuf[UARTBuf[UART1_COM].RxLen] = ch ;
		if(UARTBuf[UART1_COM].RxLen < UART_RXBUF_SIZE)
		{
			UARTBuf[UART1_COM].RxLen++;
			/*��Ҫ���ݲ����������ʱ���жϴ���ͨ��һ֡�����Ƿ����*/
			//UART0Buf.Timer = 50;		// ���50ms��û������,��������֡����
		}
	} 
	else if (USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)	// ֱ��ʹ�ÿ���֡�ж�
	{
		UARTBuf[UART1_COM].RecFlag = TRUE;
		/* ��Ҫ����2���Ĵ�������жϱ�־*/
		/*USART_GetITStatus �����Ѿ�����SR�Ĵ���*/
		//ch = USART2->SR;
		/*��DR�Ĵ��������־*/
		ch = USART_ReceiveData(USART1);
		LED_Set(LED_COM, OFF); 	// ͨ�����
	}
}
/*********************************************END OF FILE**********************/
