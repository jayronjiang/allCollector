/********************************************************************************
*  文件名:      protocol_deal.c				
*
*  版权所有: 深圳市中电电力技术有限公司	
*
*
*  当前版本:V1.0
*  
*  模块功能: 通讯协议处理模块
*
*
*
*  主要函数: Comm0Proc Comm1Proc
*
*  函数功能:按照通讯协议处理对应数据
*           
*  作	者:       杨甫勇
*
*  完成日期: 2010.09.25
*
*  修改日志: Jerry
*  修改时间:2013-03-12,移植至2612D中
*
********************************************************************************/

#include "include.h"


/*定义通信缓冲区*/
PROTOCOL_BUF	ProtocolBuf0;

void Init_Comm(void)
{
	UARTInit_RS485();					/*初始化RS485口*/
	ProtocolBuf0.pTxBuf = UART0Buf.TxBuf;
	ProtocolBuf0.pRxBuf = UART0Buf.RxBuf;
	ProtocolBuf0.RxLen = 0;
	ProtocolBuf0.TxLen = 0;
}

/********************************************************************************************************
** 函数名：    InitCommUint
** 功能描述：  通讯硬件设备的初始化
** 输入：	
** 输出：
** 全局变量：	
** 调用模块：
**
** 作者：杨甫勇
** 日期：2010.09.26
**********************************************************************************************************/
void Init_CommUnit(void)
{
	Init_Comm();			/*初始化通信口*/
	ComDeviceInfoInit();		/*装置版本 规约等初始化2012.02.17*/
}

void reset()
{
	WDTCTL = 0x0000;/*WDTCTL(0120)的高字节中写入除05Ah外的其它值将产生系统复位(PUC)*/
}

/***********************************************************************************
 * 函数名:	CommProc 
 * 描述: 
 *           			 -通讯处理函数,主程序中调用,主要处理RS 485口数据,
 *				   若是本装置的数据则进入Modbus处理,否则置数据帧
 *				   标示为MBUS(需要转发给MBUS口)
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:杨甫勇
 * 创建日期:2012.02.20
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ***********************************************************************************/
void CommProc(void)
{
	
	if( UART0Buf.RecFlag )		                      /*RS485口有数据*/
	{	
		ProtocolBuf0.pTxBuf = UART0Buf.TxBuf;         /*地址置换*/
		ProtocolBuf0.pRxBuf = UART0Buf.RxBuf;
		ProtocolBuf0.RxLen = UART0Buf.RxLen;
		ProtocolBuf0.TxLen = 0;	

		modbus_rtu_process(&ProtocolBuf0, DevParams.Address);	/*MODBUS通信协议处理*/

		UART0Buf.TxLen = ProtocolBuf0.TxLen;  /*置换回来，处理物理层数据*/
		if(UART0Buf.TxLen >0)
		{
			Rs485StartSend();
		}
		delay(50);
		UART0Buf.RxLen = 0;	        /*接收数据已处理，清除相关标志*/
		UART0Buf.RecFlag = 0;
	}
	params_modify_deal();	/*后续的数据改变处理,注意它不但是通信的改变，显示的改变也在这里处理*/
	CommSetTime();				/*判断是否有通讯校时*/
}

