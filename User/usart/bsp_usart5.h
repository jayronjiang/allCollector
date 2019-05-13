#ifndef __USART5_H
#define	__USART5_H

#if (BD_USART_NUM >= 4)
void USART5_Config(uint32_t baudrate);
void NVIC_USART5_Configuration(void);
void USART5_Init(uint32_t baudrate);
#endif

#endif /* __USART4_H */

