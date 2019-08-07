#ifndef __BSP_KEY_H
#define	__BSP_KEY_H

#define KEY_ON		0
#define KEY_OFF		1

/*DIP switch 功能,设置设备的地址*/
#define NUMBER_OF_DIPKEYS	2

#define DIP1_GRP				GPIOA		// DIP1-PA4
#define DIP1_PIN				GPIO_Pin_4

#define DIP2_GRP				GPIOA		// DIP2-PA5
#define DIP2_PIN				GPIO_Pin_5

/*----------------------------------------------------------------------------*
 *  需要注意的是代码里面的逻辑判断已经是低有效
 *  比如, 判断是否发生报警:
 *  if(Status_Get(ALARM) == 0) {}
 *
 *  所以在检测的时候要使用高有效,不要电平取反!
 *  在没有接电路,测试时候因为已经设置为上拉输入,
 *  因此也要设为高有效
 *----------------------------------------------------------------------------
 */

/*实际电路逻辑也是低有效*/
/*所以这里要选高有效*/
#define HIGH_POLAR	0	// 高有效
#define LOW_POLAR	1	// 低有效

#define DEBOUNCE_TIME	2 	// 防抖次数

void EXTI_PE4_Config(void);
uint16_t GPIO_ReadInputAll(uint8_t polar);
void ReadKey(void);
void Dev_Address_Init(void);

#endif /* __BSP_KEY_H */
