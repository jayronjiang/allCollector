/**
  ******************************************************************************
  * @file    debug.c
  * @author  Jerry
  * @date    03-Dec-2018
  *
  * @brief   ���Խӿ��ļ�.
  *
  ******************************************************************************
  */

#include "include.h"

void debug_puts(const uint8_t *str)
{
	printf("%s", str);
	uart_sel = PC1_UART;
}

