#ifndef __TIME_H
#define	__TIME_H

#define KEY_CHANGED   		BIT0		/* 外部设备检测出现变化,需要向上位机汇报*/
#define SYS_ERR_CHK			BIT1		/*3s进行一次系统检测*/
#define CAR_WATCHING		BIT2		/*后线圈开始监视*/
#define SYSTEM_200MS          	BIT3		/*暂时未使用*/
#define COMM1_MODIFIED		BIT4
#define COMM2_MODIFIED		BIT5
#define COMM3_MODIFIED		BIT6
#define COMM4_MODIFIED		BIT7
#define DEV_MODIFIED			BIT8
#define PARAM_ERR			BIT9

extern uint16_t system_flag;
extern volatile uint32_t system_time_ms;
extern uint8_t  reset_flag;

/* 用在20ms中断中,因此1s就是50个计数周期*/
#define MS_200 				(10)		// 100ms就是5个周期
#define ONE_SECOND 			(50)
#define SECOND_5 				(250)
#define SECOND_10 			(500)


#define AUTO_DETCET_TIME	(3  *ONE_SECOND)
#define ALARM_TIME			(10  *ONE_SECOND)

/*下面的延时只适用72M系统时钟*/
#define	NOP()	__ASM ("nop")
#define	DELAY_100nS()	{NOP();NOP();NOP();NOP();NOP();NOP();NOP();}
/*500ns 是36个NOP*/
#define	DELAY_500nS()	{DELAY_100nS();DELAY_100nS();DELAY_100nS();DELAY_100nS();DELAY_100nS();NOP();}
#define	DELAY_1uS()	{DELAY_500nS();DELAY_500nS();}
#define	DELAY_2uS()	{DELAY_1uS();DELAY_1uS();}
#define	DELAY_4uS()	{DELAY_2uS();DELAY_2uS();}


// xx时间片（机器周期数）
#define TIME_SPLIT_1US 	((SystemCoreClock)/1000000)		// 1微妙周期数，1.8432@22.118,400MHz
#define TIME_SPLIT_1MS 	((SystemCoreClock)/1000)	   		// 1毫秒周期数，1843.2@22.118,400MHz

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

