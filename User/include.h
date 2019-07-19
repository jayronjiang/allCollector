#ifndef __INCLUDE_H
#define	__INCLUDE_H

/*Function defines*/
/*If the board is 8 INPUT &8OUTPUT, enable this*/
/*Or if the board is 12OUTPUT, disable this*/
#define HAS_8I8O

/*头文件包含列表*/
#include "stm32f10x.h"
#include "stdio.h"
#include <string.h>
#include "base_variable.h"
#include "time.h"
#include "init.h"
#include "sys.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"
#include "bsp_key.h"

#include "stm32f10x_it.h"
#include "stm32f10x_iwdg.h"
//#include "dev_ctrl.h"
#include "exmath.h"
#include "usart_driver.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "bsp_usart4.h"
#include "bsp_usart5.h"
#include "ModbusServer.h"
#include "params.h"
#include "message_handle.h"
#include "stm32f10x_spi.h"
#include "spi.h"
#include "w25qxx.h"
#include "myiic.h"
#include "modbus.h"
#include "registers.h"
#include "modbus_rtu.h"
#include "di.h"
#include "do.h"
#include "Ydn23.h"
#include "debug.h"

#endif /* __INCLUDE_H */

