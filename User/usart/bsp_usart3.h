#ifndef __USART3_H
#define	__USART3_H

#if (BD_USART_NUM >= 3)
void USART3_Config(uint32_t baudrate);
void NVIC_USART3_Configuration(void);
void USART3_Init(uint32_t baudrate);
#endif

#endif /* __USART1_H */

