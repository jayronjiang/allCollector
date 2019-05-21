#ifndef __TIME_H
#define	__TIME_H

#define KEY_CHANGED   		BIT0		/* �ⲿ�豸�����ֱ仯,��Ҫ����λ���㱨*/
#define SYS_ERR_CHK			BIT1		/*3s����һ��ϵͳ���*/
#define CAR_WATCHING		BIT2		/*����Ȧ��ʼ����*/
#define SYSTEM_200MS          	BIT3		/*��ʱδʹ��*/
#define COMM1_MODIFIED		BIT4
#define COMM2_MODIFIED		BIT5
#define COMM3_MODIFIED		BIT6
#define COMM4_MODIFIED		BIT7
#define DEV_MODIFIED			BIT8
#define PARAM_ERR			BIT9

extern uint16_t system_flag;
extern volatile uint32_t system_time_ms;
extern uint8_t  reset_flag;

/* ����20ms�ж���,���1s����50����������*/
#define MS_200 				(10)		// 100ms����5������
#define ONE_SECOND 			(50)
#define SECOND_5 				(250)
#define SECOND_10 			(500)


#define AUTO_DETCET_TIME	(3  *ONE_SECOND)
#define ALARM_TIME			(10  *ONE_SECOND)

/*�������ʱֻ����72Mϵͳʱ��*/
#define	NOP()	__ASM ("nop")
#define	DELAY_100nS()	{NOP();NOP();NOP();NOP();NOP();NOP();NOP();}
/*500ns ��36��NOP*/
#define	DELAY_500nS()	{DELAY_100nS();DELAY_100nS();DELAY_100nS();DELAY_100nS();DELAY_100nS();NOP();}
#define	DELAY_1uS()	{DELAY_500nS();DELAY_500nS();}
#define	DELAY_2uS()	{DELAY_1uS();DELAY_1uS();}
#define	DELAY_4uS()	{DELAY_2uS();DELAY_2uS();}


// xxʱ��Ƭ��������������
#define TIME_SPLIT_1US 	((SystemCoreClock)/1000000)		// 1΢����������1.8432@22.118,400MHz
#define TIME_SPLIT_1MS 	((SystemCoreClock)/1000)	   		// 1������������1843.2@22.118,400MHz

extern uint8_t wdt_counter;
extern volatile uint32_t system_time_s;

void Delay_Xus(uint16_t us);
void Delay_Xms(uint16_t ms);
void Delay_Ms(uint32_t myMs);
void Delay_clk( uint16_t time);

void Start_Timer_x(TIM_TypeDef* TIMx);
void Stop_Timer_x(TIM_TypeDef* TIMx);
void Timer_Start(void);

void Time_Configuration(void);
//uint64_t readnowtime(void);
void DelayAndFeedDog(uint32_t myMs);
uint32_t time_interval(uint32_t time0);

#endif /* __TMIE_H */

