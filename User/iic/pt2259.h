#ifndef __24CXX_H
#define __24CXX_H   

#define PT2259_ID		0x88

#define MUTE_OFF		0x74		// �رվ���
#define MUTE_ON		0x77		// �򿪾���
#define CHIP_CLEAR	0xf0		// оƬ���

/*ע��-20��0xE0 | 0x02, -2��0xD0 |0x02*/
#define NEG_10DB		0xE0		// ˫��������-10
#define NEG_1DB		0xD0		// ˫��������-1


#define NS4160_OUT_GRP			GPIOA
#define NS4160_POUT			GPIO_Pin_8	//������


void PT2259_Config(u8 Data);
void PT2259_Set( u8 ucData10DB,u8 ucData1DB );
void NS4160_AB_type(void);
void NS4160_Disable(void);
void NS4160_D_type(void);
void NS4160_GPIO_OUT_Config(void);
void Speaker_Init(void);
void Speaker_Test(void);
#endif

