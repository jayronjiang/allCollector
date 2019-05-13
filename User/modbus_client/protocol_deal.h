/********************************************************************************
*  文件名:      Display.h				
*
*  版权所有: 深圳市中电电力技术有限公司
*
*
*  当前版本:V1.0
*  
*  模块功能: 定义声明通讯协议处理模块用到的函数、变量和宏
*
*           
*  作	者:       杨甫勇
*
*  完成日期: 2010.09.25
*
*  修改日志: 无
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


/*全局函数定义*/
void Init_CommUnit(void);		/*根据硬件配置初始化通讯接口*/
void CommSetTime();
//void InitProtocol(void);			/*初始化规约*/
void reset();
void CommProc(void);
void GatewayProc(void);
void Rs485StartSend(void);

#endif

