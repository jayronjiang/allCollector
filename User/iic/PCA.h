#ifndef __PCA_H
#define	__PCA_H

/*注意,这里的中断发生频率就是采样频率,8.64KHz,*/
/*如果测试使用高低电平显示，只能计算半个周波*/
#define PWM_8KHZ			(33)		//33测试效果相对最好，但是还是失真
//#define PWM_8KHZ			(17)
#define PWM_40KHZ		(7)
#define DUTY_50			(0x80)	// 50%的脉宽值,即0xFF的一半

void PCA_init(void);
unsigned char PCA_buf_fill_validate(void);
void PCA_buf_fill_code(uint8_t pcm); 
void PCA_Test_SampleVox(void);
void Dac_Init(void);

#endif

