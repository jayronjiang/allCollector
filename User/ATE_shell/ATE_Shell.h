#ifndef __ATE_SHELL_H
#define	__ATE_SHELL_H

#ifdef SPEAKER_ENABLE

#define SHELL_COM		UART1_COM
#define SHELL_USART		USART1

void ATE_main(void);
uint8_t xm_load_tx_record(void);

#endif	/*__ATE_SHELL_H*/
#endif

