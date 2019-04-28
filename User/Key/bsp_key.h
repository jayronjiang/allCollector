#ifndef __BSP_KEY_H
#define	__BSP_KEY_H

#define KEY_ON		0
#define KEY_OFF		1


/*----------------------------------------------------------------------------*
 *  ��Ҫע����Ǵ���������߼��ж��Ѿ��ǵ���Ч
 *  ����, �ж��Ƿ�������:
 *  if(Status_Get(ALARM) == 0) {}
 *
 *  �����ڼ���ʱ��Ҫʹ�ø���Ч,��Ҫ��ƽȡ��!
 *  ��û�нӵ�·,����ʱ����Ϊ�Ѿ�����Ϊ��������,
 *  ���ҲҪ��Ϊ����Ч
 *----------------------------------------------------------------------------
 */

/*ʵ�ʵ�·�߼�Ҳ�ǵ���Ч*/
/*��������Ҫѡ����Ч*/
#define HIGH_POLAR	0	// ����Ч
#define LOW_POLAR	1	// ����Ч

#define DEBOUNCE_TIME	2 	// ��������

void EXTI_PE4_Config(void);
uint16_t GPIO_ReadInputAll(uint8_t polar);
void ReadKey(void);

#endif /* __BSP_KEY_H */
