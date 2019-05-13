/********************************************************************************
*  �ļ���:      Display.h				
*
*  ��Ȩ����: �������е�����������޹�˾
*
*
*  ��ǰ�汾:V1.0
*  
*  ģ�鹦��: ��������ͨѶЭ�鴦��ģ���õ��ĺ����������ͺ�
*
*           
*  ��	��:       ���
*
*  �������: 2010.09.25
*
*  �޸���־: ��
*
*
********************************************************************************/

#ifndef __PROTOCOL_H
#define __PROTOCOL_H

//#include "base_variable.h"


typedef  struct 
{
	UINT8 *pTxBuf;
	UINT8 *pRxBuf;
	UINT16 TxLen;
	UINT16 RxLen;
	
}PROTOCOL_BUF;


/*ȫ�ֺ�������*/
void Init_CommUnit(void);		/*����Ӳ�����ó�ʼ��ͨѶ�ӿ�*/
void CommSetTime();
//void InitProtocol(void);			/*��ʼ����Լ*/
void reset();
void CommProc(void);
void GatewayProc(void);
void Rs485StartSend(void);

#endif

