#ifndef __MYIIC_H
#define __MYIIC_H

/* ����I2C�������ӵ�GPIO�˿�, �û�ֻ��Ҫ�޸�����4�д��뼴������ı�SCL��SDA������ */
#define I2C_GPIO_PORT			GPIOB			/* GPIO�˿� */
#define I2C_RCC_PORT 		RCC_APB2Periph_GPIOB		/* GPIO�˿�ʱ�� */
#define I2C_SCL_PIN		GPIO_Pin_10			/* ���ӵ�SCLʱ���ߵ�GPIO */
#define I2C_SDA_PIN		GPIO_Pin_11			/* ���ӵ�SDA�����ߵ�GPIO */

/*IO��������, PIN11�Ѿ���CRH��������*/
#define SDA_IN()  {I2C_GPIO_PORT->CRH &= 0XFFFFFFFF; I2C_GPIO_PORT->CRH |=((u32)0x88888888&(GPIO_CRH_MODE11 |GPIO_CRH_CNF11));} 
#define SDA_OUT() {I2C_GPIO_PORT->CRH &= 0XFFFFFFFF; I2C_GPIO_PORT->CRH |=((u32)0x33333333&(GPIO_CRH_MODE11 |GPIO_CRH_CNF11));}

//#define SDA_IN()  {GPIOB->CRH &= 0XFFFFFFFF; GPIOB->CRH |=(u32)8<<12;} 
//#define SDA_OUT() {GPIOB->CRH &= 0XFFFFFFFF; GPIOB->CRH |=(u32)3<<12;}

//IO��������	 
#define I2C_SCL_1()  (I2C_GPIO_PORT->BSRR = I2C_SCL_PIN)				/* SCL = 1 */
#define I2C_SCL_0()  (I2C_GPIO_PORT->BRR = I2C_SCL_PIN)				/* SCL = 0 */
	
#define I2C_SDA_1()  (I2C_GPIO_PORT->BSRR = I2C_SDA_PIN)				/* SDA = 1 */
#define I2C_SDA_0()  (I2C_GPIO_PORT->BRR = I2C_SDA_PIN)				/* SDA = 0 */
	
#define I2C_SDA_READ()  ((I2C_GPIO_PORT->IDR & I2C_SDA_PIN) != 0)		/* ��SDA����״̬ */

// ���Է�����2�ֲ�����ʽʱ�䶼��һ����
#define IIC_SCL    PBout(10) //SCL = 0; SCL = 1; 
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //����SDA 


//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�	  
#endif

