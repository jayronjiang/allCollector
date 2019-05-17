/******************************************************************************
* 版权所有： 
*
* 文 件 名：modbus_rtu.c
*
* 版    本： 1.0
*
* 作    者：
*
* 创建日期：
*
* 描    述： rtu通讯接口函数
*
* 其    他：
*
* 历    史：
*
*
******************************************************************************/

#include "include.h"


/******************************************************************************
 * 函数名:	CheckBuf 
 * 描述: 
 *            -检查接收包是否正确:帧长、CRC校验等
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:2008.8.13
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
UINT8 CheckRTUBuf( UINT8 *pbuf, UINT16 buf_len )
{
	UINT16 crc_code;
	if(( buf_len < 8 ) || ( buf_len > 256 ))
	{
		return LENGTH_ERROR;/*返回长度错误码*/
	}

	crc_code = get_crc16( pbuf, buf_len-2);
	if( crc_code !=  (UINT16)((pbuf[buf_len-1]<<8)|pbuf[buf_len-2] ))
	{
		return CRC_ERROR;/*返回CRC错误码*/
	}
	return BUF_OK;
}

/******************************************************************************
* 函 数 名： protocol_modbus_rtu
* 描    述： 通讯处理函数，根据协议类型调用不同的协议
* 输    入： 无
* 输    出： 无
* 返 回 值： 无
* 全局变量： 无
* 调用模块： 无
* 作    者： 
* 创建日期： 
* 其    他：
* 历    史：
******************************************************************************/
void modbus_rtu_process( PROTOCOL_BUF *buf,UINT8 siteid )
{
	UINT16	cacl_checksum;  /*计算接收到的数据的CRC校验码*/

	UINT8 *recv_buf;
	UINT8 *send_buf;
	UINT16 recv_len;
	UINT16 send_len;
	UINT8 err=0;

	recv_buf = buf->pRxBuf;
	send_buf = buf->pTxBuf;
	recv_len = buf->RxLen;
	buf->TxLen = 0;
	send_len = 0;
        
        /*是否是从站地址或广播地址*/
	if((recv_buf[0] != siteid) && (recv_buf[0] != 0x00))
	{
		err = SITEID_ERROR;                     /*报地址错误码*/
	}
	else
	{
		err = CheckRTUBuf( recv_buf, recv_len );
	}

	/*地址错误，长度不够最小值8，超出最大值255，或检验码错误，装置无反应*/
        if(( err == SITEID_ERROR ) || ( err == LENGTH_ERROR ) ||( err == CRC_ERROR ))
	{
		send_len = 0;                
	}
	else
	{
		send_len = ModbusProcess( recv_buf,recv_len-3,send_buf+1 ); /*分离出响应的id,若出现错误则长度为0*/
		
	}

	/////////////////////////////////////////////////////////////////////
	// 异常响应特殊处理，并计算CRC
	////////////////////////////////////////////////////////////////////
	if(send_len > 0)
	{		
		send_buf[0] = siteid;   /*组装id号*/
		/*计算CRC16，包括首部一个字节；返回长度也要包含两字节CRC值,先低后高*/
		buf->TxLen = send_len + 3;  	/*加上id、CRC校验码*/
		cacl_checksum = get_crc16(send_buf, buf->TxLen - 2 );
		send_buf[buf->TxLen - 2] = cacl_checksum & 0xFF;
		send_buf[buf->TxLen - 1] = (cacl_checksum >> 8) & 0xFF;
	}
	
}


/******************************************************************************
* 结束
******************************************************************************/

