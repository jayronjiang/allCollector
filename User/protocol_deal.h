#ifndef __PROTOCOL_H
#define __PROTOCOL_H

 #define UART_RXBUF_SIZE	256		//���ջ���Ĵ�С
#define UART_TXBUF_SIZE	256		//���ͻ���Ĵ�С

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

extern  UART_BUF UART0Buf;

void printf_test(void);

#endif /* __USART1_H */

