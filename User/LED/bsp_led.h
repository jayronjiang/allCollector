#ifndef __BSP_LED_H
#define	__BSP_LED_H

#include "stm32f10x.h"

/* the macro definition to trigger the led on or off 
 * 1 - ON
 - 0 - OFF
 */

/* LED灯的引脚定义,LED1->PB5, LED2->PC13. */
#define LED1_GRP			GPIOB
#define LED1_POUT		GPIO_Pin_5
#define LED2_GRP			GPIOC
#define LED2_POUT		GPIO_Pin_13

#define digitalToggle(p,i) {p->ODR ^=i;}
#define LED1_TOGGLE 		digitalToggle(LED1_GRP, LED1_POUT)
#define LED2_TOGGLE 		digitalToggle(LED2_GRP, LED2_POUT)

/*灯的定义:低电平输出有效,反逻辑*/
#define ON   DISABLE
#define OFF  ENABLE

#define LED_1		0
#define LED_2		1

#define LED_COM		LED_1
#define LED_RUN		LED_2	// 运行灯

/*带参宏，可以像内联函数一样使用*/
#define LED1(a)	if (a)	\
					GPIO_SetBits(LED1_GRP, LED1_POUT);\
					else		\
					GPIO_ResetBits(LED1_GRP, LED1_POUT)
					
#define LED2(a)	if (a)	\
					GPIO_SetBits(LED2_GRP, LED2_POUT);\
					else		\
					GPIO_ResetBits(LED2_GRP, LED2_POUT)


void LED_GPIO_Config(void);
void LED_Flashing(uint16_t LED_No,uint16_t mtime, uint16_t flashtime);
void LED_Set(uint16_t LED_No, FunctionalState iState);
		

#endif /* __BSP_LED_H */

