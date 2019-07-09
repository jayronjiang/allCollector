/**************************Copyright (c)****************************************************
 * 								
 * 					
 * 	
 * ------------------------文件信息---------------------------------------------------
 * 文件名:
 * 版本:
 * 描述:modbus协议处理模块
 * 
 * --------------------------------------------------------------------------------------- 
 * 作者:
 * 创建日期:
 * --------------------------------------------------------------------------------------- 
 * 修改人:
 * 修改日期:
 * 版本:
 * 描述:
 ****************************************************************************************/

#include "include.h"


/******************************************************************************
 * 函数名:	uint16 lo_hi(word) 
 * 描述: 
 *            -短整形数的高低位字节交换
 *
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
UINT16 lo_hi(const UINT8 *word)
{
	UINT16  temp;
	UINT8 *addr;

	addr = (UINT8 *)&temp;
	*addr = *(word + 1);
	*(addr+1) = *word;
	return(temp);
}
								    


/******************************************************************************
 * 函数名:	ModbusInit 
 * 描述: 
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:汪治国 
 * 创建日期:2008.8.13
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void ModbusInit(void)
{
	
}
			    

/******************************************************************************
 * 函数名:	SendException 
 * 描述: 
 *            -异常报文响应
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
UINT8 SendException(UINT8 *send_buf_mb,UINT8 fnc_code, UINT8 except_code)
{

	send_buf_mb[0] = (fnc_code|0x80) ;
	send_buf_mb[1] = except_code;     /*直接将异常码写入发送缓冲的第2个字节，也即组装id号后的第3个字节*/

	return 2;
}



/********************************************************************************************************
** 函数名：    Rep_Read_Registers
** 功能描述：  对读寄存器的响应
** 输入：	regStartAddr--读寄存器开始地址，regNum--寄存器数量, funcCode--功能码
** 输出：	uSendlen--返回包体的长度,send_buf--返回回应包
** 全局变量：	
** 调用模块：
**
** 作者：
** 日期：
**********************************************************************************************************/
UINT8 Rep_Read_Registers( UINT16 regStartAddr, UINT8 regNum, UINT8 funcCode, UINT8 *send_buf_mb )
{
	UINT8 uErr = REG_OK;			        /*错误码*/
	UINT8 byte_count = regNum << 1;			/*数据字节数*/
	UINT8 send_len = 2;                             /*分离出功能码和字节数*/
	
	send_len += Read_Register(regStartAddr, regNum, &send_buf_mb[2], &uErr);/*send_buf_mb已经是发送缓存的第一个字节了*/
	
        if ( uErr == DATA_ERR )
	{
		return SendException(send_buf_mb, funcCode, ILLEGAL_VALUE);
	}
	else if ( uErr == REGADDR_ERR )
	{
		return SendException(send_buf_mb, funcCode, ILLEGAL_ADDRESS);
	}
	else
	{
		send_buf_mb[0] = funcCode;		/*组装功能码*/
		send_buf_mb[1] = byte_count;		/*组装字节数*/

	
		return send_len;
	}
}


/********************************************************************************************************
** 函数名：    Rep_Write_Registers
** 功能描述：  对写寄存器的响应
** 输入：	regStartAddr--读寄存器开始地址，regNum--寄存器数量, funcCode--功能码
** 输出：	uSendlen--返回包体的长度,send_buf--返回回应包
** 全局变量：	
** 调用模块：
**
** 作者：
** 日期：
**
** 修改：
** 日期：2011.03.25
**
**********************************************************************************************************/
UINT8 Rep_Write_Registers(UINT16 regStartAddr, UINT8 regNum, const UINT8 *databuf, UINT8 funcCode, UINT8 *send_buf_mb)
{
	UINT8 send_len = 0;
	UINT8 uErr = 0;					/*错误码*/
	UINT8 byte_count = databuf[0];			/*数据字节数*/	
	
	Write_Register( regStartAddr, regNum, &databuf[1], byte_count, &uErr);
	
        if (uErr ==DATA_ERR)
	{
		return SendException(send_buf_mb, funcCode, ILLEGAL_VALUE);
	}
	else if (uErr == REGADDR_ERR)
	{
		return SendException(send_buf_mb, funcCode, ILLEGAL_ADDRESS);
	}
	else if (uErr == OPERATION_ERR)
	{
		return SendException(send_buf_mb, funcCode, ILLEGAL_OPERATION);
	}
        /*组装除数据以外的其它数据,send_buf_mb已经是send_buf的第一个字节了*/
	send_buf_mb[send_len++] = funcCode;					/*function*/
	send_buf_mb[send_len++] = (UINT8)(regStartAddr>>8);	/*Starting Register Addr Hi*/
	send_buf_mb[send_len++] = (UINT8)regStartAddr;			/*Starting Register Addr Lo*/
	send_buf_mb[send_len++] = (UINT8)(regNum>>8);			/*No.of Registers Hi*/
	send_buf_mb[send_len++] = (UINT8)regNum;				/*No.of Registers Lo*/
		
	return send_len;
}

/********************************************************************************************************
** 函数名：    Write_SingleCoil
** 功能描述：  写单个线圈
** 输入：		nStartRegNo--寄存器开始编号,pdatabuf--数据缓冲区地址
**				
** 输出：		perr--操作结果码,
** 全局变量：	
** 调用模块：
**
** 作者：
** 日期：
**
** 修改：
** 日期：
**********************************************************************************************************/

UINT8 Write_SingleCoil(UINT16  nStartRegNo, const UINT8  *pdatabuf, UINT8  *perr)
{
	UINT16  reg_value;
	UINT16  coil_num;
	UINT16  coil_action;

	reg_value = (pdatabuf[0] << 8) + pdatabuf[1];

	if((nStartRegNo >= DO_START_ADDR) && (nStartRegNo < (DO_START_ADDR + DO_REG_MAX)))
	{
		/*遥合和遥分*/
		if((reg_value != REMOTE_CLOSE) && (reg_value != REMOTE_OPEN))
		{
			*perr = DATA_ERR;		/*如果不是FF00是报04还是03?,应该是帧错误,报03*/
			return(0);
		}

		if( nStartRegNo < DO_START_ADDR+ACTRUL_DO_NUM)
		{
			coil_num = (nStartRegNo - DO_START_ADDR);		/*第几个DO*/
			coil_action = reg_value;			/*当前DO的操作*/
			switch(coil_action)
			{
				case REMOTE_CLOSE:		/*遥合执行*/
					if(!RelayOperate(coil_num, RELAY_REMOTE_ACT))
					{
						*perr = OPERATION_ERR;
						return(0);
					}
					break;

				case REMOTE_OPEN:		/*遥分执行*/	
					if( !RelayOperate(coil_num, RELAY_REMOTE_RETURN))
					{
						*perr = OPERATION_ERR;
						return(0);
					}
					break;
				default:
					break;
			}
		}

		else if( nStartRegNo == REMOTE_RESET_REG )
		{
			System_Reset = 1;
		}
	#if (LOCK_NUM >= 1)
		else if( nStartRegNo == DOOR_1_REMOTE_REG )
		{
			if (reg_value == REMOTE_OPEN)
			{
				control_flag |= LBIT(DOOR1_OPEN_SET_FLAG);
			}
			else if (reg_value == REMOTE_CLOSE)
			{
				control_flag |= LBIT(DOOR1_CLOSE_SET_FLAG);
			}
		}
	#endif
	#if (LOCK_NUM >= 2)
		else if( nStartRegNo == DOOR_2_REMOTE_REG )
		{
			if (reg_value == REMOTE_OPEN)
			{
				control_flag |= LBIT(DOOR2_OPEN_SET_FLAG);
			}
			else if (reg_value == REMOTE_CLOSE)
			{
				control_flag |= LBIT(DOOR2_CLOSE_SET_FLAG);
			}
		}
	#endif
	#if (LOCK_NUM >= 3)
		else if( nStartRegNo == DOOR_3_REMOTE_REG )
		{
			if (reg_value == REMOTE_OPEN)
			{
				control_flag |= LBIT(DOOR3_OPEN_SET_FLAG);
			}
			else if (reg_value == REMOTE_CLOSE)
			{
				control_flag |= LBIT(DOOR3_CLOSE_SET_FLAG);
			}
		}
	#endif

	#if (BRK_NUM > 0)
		else if ((nStartRegNo >= BRK1_REMOTE_REG )&&(nStartRegNo < (BRK1_REMOTE_REG +BRK_NUM) ))
		{
			coil_num = (nStartRegNo - BRK1_REMOTE_REG);		/*第几个DO*/
			coil_action = reg_value;			/*当前DO的操作*/
			switch(coil_action)
			{
				case REMOTE_CLOSE:		/*遥合执行*/
					control_flag |= LBIT(BRK1_CLOSE_SET_FLAG+(4*coil_num));
					break;

				case REMOTE_OPEN:		/*遥分执行*/	
					control_flag |= LBIT(BRK1_OPEN_SET_FLAG+(4*coil_num));
					break;

				case REMOTE_OPEN_LOCK:		/*遥分执行锁死*/	
					control_flag |= LBIT(BRK1_OPEN_LOCK_SET_FLAG+(4*coil_num));
					break;

				case REMOTE_OPEN_UNLOCK:		/*遥分执行解锁*/	
					control_flag |= LBIT(BRK1_OPEN_UNLOCK_SET_FLAG+(4*coil_num));
					break;

				default:
					break;
			}
		}
	#endif

	#if (ARD_NUM > 0)
		else if ((nStartRegNo >= ARD1_REMOTE_REG )&&(nStartRegNo < (ARD1_REMOTE_REG +ARD_NUM) ))
		{
			coil_num = (nStartRegNo - ARD1_REMOTE_REG);		/*第几个DO*/
			coil_action = reg_value;			/*当前DO的操作*/
			switch(coil_action)
			{
				case REMOTE_CLOSE:		/*遥合执行*/
					control_flag |= LBIT(ARD1_CLOSE_SET_FLAG+(2*coil_num));
					break;

				case REMOTE_OPEN:		/*遥分执行*/	
					control_flag |= LBIT(ARD1_OPEN_SET_FLAG+(2*coil_num));
					break;

				default:
					break;
			}
		}
	#endif
		else
		{
			*perr = REGADDR_ERR;
			return(0);
		}
	}
	else
	{
		*perr = REGADDR_ERR;
		return(0);
	}

	*perr = REG_OK;	
	return(1);
}


/******************************************************************************
 * 函数名:	Rep_Write_SingleCoils 
 * 描述: 
 *            -对写单个寄存器的响应
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
UINT8 Rep_Write_SingleCoil(UINT16 regStartAddr, const UINT8 *databuf, UINT8 funcCode, UINT8 *send_buf_mb)
{
	UINT8 send_len = 0;
	UINT8 uErr = 0;							/*错误码*/
	
	Write_SingleCoil(regStartAddr, databuf, &uErr);
	if (uErr==DATA_ERR)
	{
		return SendException(send_buf_mb, funcCode, ILLEGAL_VALUE);
	}
	else if (uErr == REGADDR_ERR)
	{
		return SendException(send_buf_mb, funcCode, ILLEGAL_ADDRESS);
	}
	else if (uErr == OPERATION_ERR)
	{
		return SendException(send_buf_mb, funcCode, ILLEGAL_OPERATION);
	}
	
	send_buf_mb[send_len++] = funcCode;					/*function*/
	send_buf_mb[send_len++] = (UINT8)(regStartAddr>>8);	/*Starting Register Addr Hi*/
	send_buf_mb[send_len++] = (UINT8)regStartAddr;			/*Starting Register Addr Lo*/
	send_buf_mb[send_len++] = databuf[0];			/*Register Value Hi*/
	send_buf_mb[send_len++] = databuf[1];				/*Register Value Lo*/

	
	return send_len;
}


/******************************************************************************
 * 函数名:	ReadHoldingRegisters 
 * 描述: 
 *            -读保持寄存器
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *修改说明:将no_of_regs变量的定义修改为字节
 *
 *
 ******************************************************************************/
UINT8 ReadHoldingRegisters(const UINT8 *recv_buf_mb, UINT16 recv_len, UINT8 *send_buf_mb)
{
	UINT8 fnc_code;
	UINT8 no_of_regs;	/*修改定义为字节*/
	UINT16 start_addr;	
	

	fnc_code = recv_buf_mb[MBUS_FUNC];
	/*将2个8位的字节数据赋给一个16位的数，需要交换高低位，也可以采用移位操作*/
        start_addr = lo_hi( &recv_buf_mb[MBUS_START] ) ;
	no_of_regs = lo_hi( &recv_buf_mb[MBUS_NOREGS] );

	/*读数据的长度在1和256之间，且总长度必须为8*/
        if(( no_of_regs ) && ( no_of_regs <= READ_REGS_MAX ) && ( recv_len  == 5 ))
	{
		return Rep_Read_Registers(start_addr, no_of_regs, fnc_code, send_buf_mb);
	}
	else
	{
		return SendException(send_buf_mb, fnc_code, ILLEGAL_VALUE);/*否则报长度错误码03*/
	}
}


/******************************************************************************
 * 函数名:	PresetMultipleRegisters 
 * 描述: 
 *            -写多个寄存器
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改日期:
  *
 *
 ******************************************************************************/
UINT8 PresetMultipleRegisters( const UINT8 *recv_buf_mb, UINT16 recv_len, UINT8 *send_buf_mb )
{	
	UINT8 send_len = 0;
	UINT8 fnc_code, byte_count;
	UINT8 no_of_regs;	/*修改定义为字节*/
	UINT16 start_addr;	
	//UINT8 siteid = recv_buf_mb[MBUS_ADDR];	/*广播只能对时,还要进行一次处理*/

	fnc_code = recv_buf_mb[MBUS_FUNC];
	start_addr = lo_hi(&recv_buf_mb[MBUS_START]) ;
	no_of_regs = lo_hi(&recv_buf_mb[MBUS_NOREGS]);
	byte_count = recv_buf_mb[MBUS_BYTECNT];
	
	/*添加判断条件:byte_count == recv_len-6(实际为减9),检查实际要写入的字节数是否等于byte_count*/
	if(( no_of_regs ) && ( no_of_regs <= WRITE_REGS_MAX ) && ( byte_count  == no_of_regs * 2 ) &&( byte_count == recv_len-6 ))
	{
		send_len = Rep_Write_Registers( start_addr, no_of_regs, &recv_buf_mb[MBUS_BYTECNT], fnc_code, send_buf_mb );
	}
	else
	{
		return SendException( send_buf_mb, fnc_code, ILLEGAL_VALUE );/*报长度错误码*/
	}
	
	return send_len;
}


/******************************************************************************
 * 函数名:	ForceSingleCoil 
 * 描述: 
 *            -写单个寄存器
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ******************************************************************************/
UINT8 ForceSingleCoil(const UINT8  *recv_buf_mb, UINT16  recv_len, UINT8 *send_buf_mb)
{	
	if( recv_len != 5 )
	{
		return DATA_ERR;	/*对命令长度也进行一次限定,防止FF 00 后面还有数据也能成功*/
	}
	
	UINT8  fnc_code;
	UINT16  start_addr;

	fnc_code = recv_buf_mb[MBUS_FUNC];
	start_addr = lo_hi(&recv_buf_mb[MBUS_START]) ;
	
	/*写单个线圈*/
	return Rep_Write_SingleCoil(start_addr, &recv_buf_mb[4], fnc_code, send_buf_mb);

}

/******************************************************************************
 * 函数名:	ReadSecretRegisters 
 * 描述: 
 *            -读秘密寄存器，和读普通寄存器操作一致，调用同样的响应函数
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ******************************************************************************/
UINT8 ReadSecretRegisters(const UINT8 *recv_buf_mb, UINT16 recv_len, UINT8 *send_buf_mb)
{
	UINT8 fnc_code;
	UINT8 no_of_regs;
	UINT16 start_addr;

	fnc_code = recv_buf_mb[MBUS_FUNC];
	start_addr = lo_hi( &recv_buf_mb[MBUS_START] );
	no_of_regs = lo_hi( &recv_buf_mb[MBUS_NOREGS] );

	if(( no_of_regs ) && ( no_of_regs <= READ_REGS_MAX ) && ( recv_len  == 5 ))
	{
		return Rep_Read_Registers( start_addr, no_of_regs, fnc_code, send_buf_mb );
	}
	else
	{
		return SendException( send_buf_mb, fnc_code, ILLEGAL_VALUE );
	}
}


/******************************************************************************
 * 函数名:	WriteSecretRegisters 
 * 描述: 
 *            -写秘密寄存器，和写普通寄存器一致，调用同样的响应函数
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 *
 *
 ******************************************************************************/
UINT8 WriteSecretRegisters( const UINT8 *recv_buf_mb, UINT16 recv_len, UINT8 *send_buf_mb )
{
	UINT8 fnc_code, byte_count;
	UINT8 no_of_regs;
	UINT16 start_addr;

	fnc_code = recv_buf_mb[MBUS_FUNC];
	start_addr = lo_hi( &recv_buf_mb[MBUS_START] ) ;
	no_of_regs = lo_hi( &recv_buf_mb[MBUS_NOREGS] );
	byte_count = recv_buf_mb[MBUS_BYTECNT];
	
	if(( no_of_regs ) && ( no_of_regs <= WRITE_REGS_MAX ) && ( byte_count  == no_of_regs * 2 ) &&( byte_count == recv_len-6 ))
	{
		return Rep_Write_Registers( start_addr, no_of_regs, &recv_buf_mb[MBUS_BYTECNT], fnc_code, send_buf_mb );	
	}
	else
	{
		return SendException( send_buf_mb, fnc_code, ILLEGAL_VALUE );
	}

}


/******************************************************************************
 * 函数名:	SendException 
 * 描述: 
 *            -MODBUS报文包解析
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:2008.8.13
 * 
 *------------------------
 * 修改人:
 * 修改日期:2010.10.13
 *
 *
 ******************************************************************************/
UINT8 ParsePackage(const UINT8 *recv_buf_mb, UINT16 recv_len, UINT8 *send_buf_mb)
{
	UINT8 send_len = 0;
	UINT8 siteid = recv_buf_mb[MBUS_ADDR];
	UINT8 fnc_code = recv_buf_mb[MBUS_FUNC];

	/*目前广播命令只用于写寄存器中的对时*/
	if(( siteid == 0x00 ) && ( fnc_code != PRESET_MULTIPLE_REGS ))
	{
		return 0;
	}	

	switch(fnc_code)                        /*解析出命令码，转入相应入口*/
	{
		case READ_HOLDING_REGS:         /*读命令码03*/
			/*转入应用层*/
                        send_len = ReadHoldingRegisters( recv_buf_mb, recv_len, send_buf_mb );
			break;

		case PRESET_MULTIPLE_REGS:      /*写命令码10*/	
			send_len = PresetMultipleRegisters( recv_buf_mb, recv_len, send_buf_mb );
			break;

		case FORCE_SINGLE_COIL:		/*DO遥控功能05*/
			send_len = ForceSingleCoil(recv_buf_mb, recv_len, send_buf_mb);
			break;

#if 0
		case READ_SECRET_REGS:	/*秘密读46*/
			send_len = ReadSecretRegisters( recv_buf_mb, recv_len, send_buf_mb );
			break;

		case WRITE_SECRET_REGS:	/*秘密写47*/	
			send_len = WriteSecretRegisters( recv_buf_mb, recv_len, send_buf_mb );
			break;
#endif
			
		/*若是其它的功能码，则报功能错误码01*/
                default:
			send_len = SendException(send_buf_mb, fnc_code, ILLEGAL_FUNCTION);
			break;

	}
        /*广播对时不处理*/
	if (siteid == 0x00)
	{
		send_len = 0;  		/*广播命令不响应数据上传*/	
	}
	return send_len;
}


/******************************************************************************
 * 函数名:	ModbusProcess 
 * 描述: 
 *            -MODBUS主处理函数
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
UINT16 ModbusProcess(const UINT8 recv_buf_mb[],UINT16 recv_len,UINT8 send_buf_mb[])
{

	UINT16 send_len;

	send_len = 0;

	send_len = ParsePackage(recv_buf_mb, recv_len, send_buf_mb);/*转入解包函数*/

	return send_len;
}
