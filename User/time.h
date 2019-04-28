#ifndef __TIME_H
#define	__TIME_H

#define SYS_CHANGED   		BIT0		/* �ⲿ�豸�����ֱ仯,��Ҫ����λ���㱨*/
#define SYS_ERR_CHK			BIT1		/*3s����һ��ϵͳ���*/
#define CAR_WATCHING		BIT2		/*����Ȧ��ʼ����*/
#define SYSTEM_200MS          		BIT3		/*��ʱδʹ��*/

extern uint16_t system_flag;
extern volatile uint32_t system_time_ms;

/* ����20ms�ж���,���1s����50����������*/
#define MS_200 				(10)		// 100ms����5������
#define ONE_SECOND 			(50)

#define AUTO_DETCET_TIME	(3  *ONE_SECOND)
#define ALARM_TIME			(10  *ONE_SECOND)


// xxʱ��Ƭ��������������
#define TIME_SPLIT_1US 	((SystemCoreClock)/1000000)		// 1΢����������1.8432@22.118,400MHz
#define TIME_SPLIT_1MS 	((SystemCoreClock)/1000)	   		// 1������������1843.2@22.118,400MHz

extern uint8_t wdt_counter;

void Delay_Xus(uint16_t us);
void Delay_Xms(uint16_t ms);
void Delay_Ms(uint32_t myMs);

void Start_Timer_x(TIM_TypeDef* TIMx);
void Stop_Timer_x(TIM_TypeDef* TIMx);
void Timer_Start(void);

void Time_Configuration(void);
uint64_t readnowtime(void);
void DelayAndFeedDog(uint32_t myMs);

#endif /* __TMIE_H */

