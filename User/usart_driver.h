#ifndef __USART_DRIVER_H
#define	__USART_DRIVER_H

/*һ�������Ҫ��BD_USART_NUM >= PC_USART_NUM*/
#define PC_USART_NUM		1		// �������Ӽ������ػ�?
#define BD_USART_NUM		4		// ���������м�������?,�°�ֻ��3������,UART4Ϊ485

#define BAUD_1200                       		0
#define BAUD_2400				1
#define BAUD_4800				2
#define BAUD_9600				3
#define BAUD_19200				4
#define BAUD_115200				5
#define BAUD_NUM 				6

//��������ö��,����չ
typedef enum 
{	
	UART1_COM = 0,
/*��ʱ�͹��ػ�һһ��Ӧ,�Ժ���ܻ��õ�������;,*/
/*��ʱȡ��������궨��*/
#if (BD_USART_NUM >= 2)
	UART2_COM,
#endif
#if (BD_USART_NUM >= 3)
	UART4_COM,
#endif
#if (BD_USART_NUM >= 4)
	UART5_COM,
#endif

/*ע��: UART_NUM��ö����,������������������*/
/*������#if (UART_NUM == 2) ..., #endif */
/*ԭ����ö�ٶ��巢���ڱ���׶Σ�������*/
/*���뷢����Ԥ����׶�*/
	UART_NUM
}USART_LIST;

// ����Ҫ����/������ô������, ����Ҫ��С��COMM_LENTH
#define UART_RXBUF_SIZE		256		//���ջ���Ĵ�С
#define UART_TXBUF_SIZE		256		//���ͻ���Ĵ�С

typedef struct 
{
	uint8_t TxBuf[UART_TXBUF_SIZE];
	uint8_t RxBuf[UART_RXBUF_SIZE];
	uint16_t TxLen;
	uint16_t TxPoint;
	uint16_t RxLen;
	uint8_t RecFlag;
	uint16_t Timer;		/*T15/T35��ʱ*/
}UART_BUF;

extern  UART_BUF UARTBuf[UART_NUM];;
extern uint8_t uart_sel;
//board com num
extern const USART_LIST bd_com[BD_USART_NUM];
extern const UINT32  Baud[BAUD_NUM];

int UART_fputc(int ch);
int  UART_fgetc(void);

#endif /*__USART_DRIVER_H */

