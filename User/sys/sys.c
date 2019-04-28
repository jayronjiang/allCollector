 /**
  ******************************************************************************
  * @file    sys.c
  * @author  Jerry
  * @date    03-Dec-2018
  *
  * @brief   ϵͳ��ʼ���ļ�,��ʼ����������ʹ���.
  *
  ******************************************************************************
  */
#include "include.h"


//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
void WFI_SET(void)
{
	__ASM volatile("wfi");		  
}
//�ر������ж�
void INTX_DISABLE(void)
{		  
	__ASM volatile("cpsid i");
}
//���������ж�
void INTX_ENABLE(void)
{
	__ASM volatile("cpsie i");		  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(uint32_t addr) 
{
    MSR MSP, r0 			//set Main Stack value
    BX r14
}
