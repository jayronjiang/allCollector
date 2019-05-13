#ifndef __USART4_H
#define	__USART4_H

#if (BD_USART_NUM >= 3)
void USART4_Config(uint32_t baudrate);
void NVIC_USART4_Configuration(void);
void USART4_Init(uint32_t baudrate);
#endif

#endif /* __USART4_H */

