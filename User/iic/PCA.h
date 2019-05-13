#ifndef __PCA_H
#define	__PCA_H

/*ע��,������жϷ���Ƶ�ʾ��ǲ���Ƶ��,8.64KHz,*/
/*�������ʹ�øߵ͵�ƽ��ʾ��ֻ�ܼ������ܲ�*/
#define PWM_8KHZ			(33)		//33����Ч�������ã����ǻ���ʧ��
//#define PWM_8KHZ			(17)
#define PWM_40KHZ		(7)
#define DUTY_50			(0x80)	// 50%������ֵ,��0xFF��һ��

void PCA_init(void);
unsigned char PCA_buf_fill_validate(void);
void PCA_buf_fill_code(uint8_t pcm); 
void PCA_Test_SampleVox(void);
void Dac_Init(void);

#endif

