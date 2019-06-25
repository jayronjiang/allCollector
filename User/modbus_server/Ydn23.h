/********************************************************************************
*  ��Ȩ����: �������е�����������޹�˾				
*
*  �ļ���:       YDN23.c
*
*
*  ��ǰ�汾: 0.1
*  
*  ����:   Jerry	                    
*
*  �������:  20190613
*
*  ����: ����Э��YDN023, ��ȡ��Ĭ����UPS����
*                           
********************************************************************************/

/*
*********************************************************************************************************
*                                         �����ļ�
*********************************************************************************************************
*/

#ifndef _YDN23_H
#define _YDN23_H

/*********************************************************************************
*                                    UPS Э��궨��
**********************************************************************************/
/*״̬��ЧΪ1, ��ЧΪ0*/
typedef union{
	struct
	{
		uint16_t lenid:12;
		uint16_t lchksum:4;
	}lenth_bits;
	/* ������������ʽ����,������������2�����������ͬһ����ַ*/
	uint16_t lenth_word;
}USP_LENGTH_BITS;


#define   UPS_SOI		0x7E
// ver-0x21
#define   UPS_VER		0x21
// ver-0x2A
#define   UPS_CID1		0x2A

#define   UPS_CID2_ALL	0x42
#define   UPS_CID2_IN		0xE0
#define   UPS_CID2_OUT	0xE1
#define   UPS_CID2_BAT	0xE3
#define   UPS_CID2_TIME	0xE4
#define   UPS_CID2_STATUS	0x43
#define   UPS_CID2_ALARM	0x44

#define   UPS_EOI		0x0D


/********************************************************************************************************
*                                          ydn23ģ��ӿں���
********************************************************************************************************/
 UINT16 lchkSumCalc(UINT16 len_value);
void comm_ask_ups(INT16U station,USART_LIST buf_no,INT16U cid2,INT8U info_len,INT8U command);
UINT16 checkSumCalc(UINT8 *buffer, UINT8 len);
void comm_UPSParam_ANALYSE(void);
void comm_UPSIn_ANALYSE(void);
void comm_UPSOut_ANALYSE(void);
void comm_UPSBat_ANALYSE(void);
void comm_UPSAlarm_ANALYSE(void)	;
void comm_UPSStatus_ANALYSE(void);
#endif
/*********************************************************************************************************
**                            �ļ�����
********************************************************************************************************/



