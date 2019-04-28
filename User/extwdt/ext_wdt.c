 /**
  ******************************************************************************
  * @file    ext_wdt.c
  * @author  Jerry
  * @date    17-Nov-2018
  *
  * @brief   �ⲿ���Ź������߼�.
  *
  ******************************************************************************
  */
  
#include "include.h"

uint8_t wdt_counter = 0;


/***********************************************************************************
 * ������:	Ext_Wdt_Feed 
 * ����: 
 *           	-ι������, ���һ��խ������.
 *		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:20181109
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 *
 *
 ***********************************************************************************/
 #if 0
void Ext_Wdt_Feed(void)
{
	// ���һ��������
	GPIO_SetBits(EXT_WDT_GRP,EXT_WDT_PIN);
	// ��Ҫ����NOPҪ��һ��
	 __ASM ("nop");
	 __ASM ("nop");
	//__NOP; //�ᱻ�Ż�
	 __ASM ("nop")
    	GPIO_ResetBits(EXT_WDT_GRP,EXT_WDT_PIN);
}
#endif
void Ext_Wdt_Feed(void)
{
	 __ASM ("nop");	// û���ⲿ���Ź�
}

/***********************************************************************************
 * ������:	Ext_Wdt_GPIO_Config 
 * ����: 
 *           	-�ⲿ���Ź���GPIO����.
 *		
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:Jerry
 * ��������:20181109
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 *
 *
 ***********************************************************************************/
void Ext_Wdt_GPIO_Config(void)	
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_APB2PeriphClockCmd( EXT_WDT_RCC_GRP, ENABLE); // ʹ��PC�˿�ʱ��  
	
  GPIO_InitStructure.GPIO_Pin = EXT_WDT_PIN;	//ѡ���Ӧ������
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(EXT_WDT_GRP, &GPIO_InitStructure);  //��ʼ��PC�˿�
	
  GPIO_ResetBits(EXT_WDT_GRP,EXT_WDT_PIN);	 // ��ʼ��Ϊ�͵�ƽ
  Ext_Wdt_Feed();
}

