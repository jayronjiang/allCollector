/**************************Copyright (c)****************************************************
 * 								
 * 					
 * 	
 * ------------------------�ļ���Ϣ---------------------------------------------------
 * �ļ���:
 * �汾:
 * ����:modbusЭ�鴦��ģ��
 * 
 * --------------------------------------------------------------------------------------- 
 * ����:
 * ��������:
 * --------------------------------------------------------------------------------------- 
 * �޸���:
 * �޸�����:
 * �汾:
 * ����:
 ****************************************************************************************/

#include "include.h"


/******************************************************************************
 * ������:	uint16 lo_hi(word) 
 * ����: 
 *            -���������ĸߵ�λ�ֽڽ���
 *
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:
 * �޸�����:
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
 * ������:	ModbusInit 
 * ����: 
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:���ι� 
 * ��������:2008.8.13
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void ModbusInit(void)
{
	
}
			    

/******************************************************************************
 * ������:	SendException 
 * ����: 
 *            -�쳣������Ӧ
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
UINT8 SendException(UINT8 *send_buf_mb,UINT8 fnc_code, UINT8 except_code)
{

	send_buf_mb[0] = (fnc_code|0x80) ;
	send_buf_mb[1] = except_code;     /*ֱ�ӽ��쳣��д�뷢�ͻ���ĵ�2���ֽڣ�Ҳ����װid�ź�ĵ�3���ֽ�*/

	return 2;
}



/********************************************************************************************************
** ��������    Rep_Read_Registers
** ����������  �Զ��Ĵ�������Ӧ
** ���룺	regStartAddr--���Ĵ�����ʼ��ַ��regNum--�Ĵ�������, funcCode--������
** �����	uSendlen--���ذ���ĳ���,send_buf--���ػ�Ӧ��
** ȫ�ֱ�����	
** ����ģ�飺
**
** ���ߣ�
** ���ڣ�
**********************************************************************************************************/
UINT8 Rep_Read_Registers( UINT16 regStartAddr, UINT8 regNum, UINT8 funcCode, UINT8 *send_buf_mb )
{
	UINT8 uErr = REG_OK;			        /*������*/
	UINT8 byte_count = regNum << 1;			/*�����ֽ���*/
	UINT8 send_len = 2;                             /*�������������ֽ���*/
	
	send_len += Read_Register(regStartAddr, regNum, &send_buf_mb[2], &uErr);/*send_buf_mb�Ѿ��Ƿ��ͻ���ĵ�һ���ֽ���*/
	
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
		send_buf_mb[0] = funcCode;		/*��װ������*/
		send_buf_mb[1] = byte_count;		/*��װ�ֽ���*/

	
		return send_len;
	}
}


/********************************************************************************************************
** ��������    Rep_Write_Registers
** ����������  ��д�Ĵ�������Ӧ
** ���룺	regStartAddr--���Ĵ�����ʼ��ַ��regNum--�Ĵ�������, funcCode--������
** �����	uSendlen--���ذ���ĳ���,send_buf--���ػ�Ӧ��
** ȫ�ֱ�����	
** ����ģ�飺
**
** ���ߣ�
** ���ڣ�
**
** �޸ģ�
** ���ڣ�2011.03.25
**
**********************************************************************************************************/
UINT8 Rep_Write_Registers(UINT16 regStartAddr, UINT8 regNum, const UINT8 *databuf, UINT8 funcCode, UINT8 *send_buf_mb)
{
	UINT8 send_len = 0;
	UINT8 uErr = 0;					/*������*/
	UINT8 byte_count = databuf[0];			/*�����ֽ���*/	
	
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
        /*��װ�������������������,send_buf_mb�Ѿ���send_buf�ĵ�һ���ֽ���*/
	send_buf_mb[send_len++] = funcCode;					/*function*/
	send_buf_mb[send_len++] = (UINT8)(regStartAddr>>8);	/*Starting Register Addr Hi*/
	send_buf_mb[send_len++] = (UINT8)regStartAddr;			/*Starting Register Addr Lo*/
	send_buf_mb[send_len++] = (UINT8)(regNum>>8);			/*No.of Registers Hi*/
	send_buf_mb[send_len++] = (UINT8)regNum;				/*No.of Registers Lo*/
		
	return send_len;
}

/********************************************************************************************************
** ��������    Write_SingleCoil
** ����������  д������Ȧ
** ���룺		nStartRegNo--�Ĵ�����ʼ���,pdatabuf--���ݻ�������ַ
**				
** �����		perr--���������,
** ȫ�ֱ�����	
** ����ģ�飺
**
** ���ߣ�
** ���ڣ�
**
** �޸ģ�
** ���ڣ�
**********************************************************************************************************/

UINT8 Write_SingleCoil(UINT16  nStartRegNo, const UINT8  *pdatabuf, UINT8  *perr)
{
	UINT16  reg_value;
	UINT16  coil_num;
	UINT16  coil_action;

	reg_value = (pdatabuf[0] << 8) + pdatabuf[1];

	if((nStartRegNo >= DO_START_ADDR) && (nStartRegNo < (DO_START_ADDR + DO_REG_MAX)))
	{
		/*ң�Ϻ�ң��*/
		if((reg_value != REMOTE_CLOSE) && (reg_value != REMOTE_OPEN))
		{
			*perr = DATA_ERR;		/*�������FF00�Ǳ�04����03?,Ӧ����֡����,��03*/
			return(0);
		}

		if( nStartRegNo < DO_START_ADDR+ACTRUL_DO_NUM)
		{
			coil_num = (nStartRegNo - DO_START_ADDR);		/*�ڼ���DO*/
			coil_action = reg_value;			/*��ǰDO�Ĳ���*/
			switch(coil_action)
			{
				case REMOTE_CLOSE:		/*ң��ִ��*/
					if(!RelayOperate(coil_num, RELAY_REMOTE_ACT))
					{
						*perr = OPERATION_ERR;
						return(0);
					}
					break;

				case REMOTE_OPEN:		/*ң��ִ��*/	
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
			coil_num = (nStartRegNo - BRK1_REMOTE_REG);		/*�ڼ���DO*/
			coil_action = reg_value;			/*��ǰDO�Ĳ���*/
			switch(coil_action)
			{
				case REMOTE_CLOSE:		/*ң��ִ��*/
					control_flag |= LBIT(BRK1_CLOSE_SET_FLAG+(4*coil_num));
					break;

				case REMOTE_OPEN:		/*ң��ִ��*/	
					control_flag |= LBIT(BRK1_OPEN_SET_FLAG+(4*coil_num));
					break;

				case REMOTE_OPEN_LOCK:		/*ң��ִ������*/	
					control_flag |= LBIT(BRK1_OPEN_LOCK_SET_FLAG+(4*coil_num));
					break;

				case REMOTE_OPEN_UNLOCK:		/*ң��ִ�н���*/	
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
			coil_num = (nStartRegNo - ARD1_REMOTE_REG);		/*�ڼ���DO*/
			coil_action = reg_value;			/*��ǰDO�Ĳ���*/
			switch(coil_action)
			{
				case REMOTE_CLOSE:		/*ң��ִ��*/
					control_flag |= LBIT(ARD1_CLOSE_SET_FLAG+(2*coil_num));
					break;

				case REMOTE_OPEN:		/*ң��ִ��*/	
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
 * ������:	Rep_Write_SingleCoils 
 * ����: 
 *            -��д�����Ĵ�������Ӧ
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
UINT8 Rep_Write_SingleCoil(UINT16 regStartAddr, const UINT8 *databuf, UINT8 funcCode, UINT8 *send_buf_mb)
{
	UINT8 send_len = 0;
	UINT8 uErr = 0;							/*������*/
	
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
 * ������:	ReadHoldingRegisters 
 * ����: 
 *            -�����ּĴ���
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 *
 *�޸�˵��:��no_of_regs�����Ķ����޸�Ϊ�ֽ�
 *
 *
 ******************************************************************************/
UINT8 ReadHoldingRegisters(const UINT8 *recv_buf_mb, UINT16 recv_len, UINT8 *send_buf_mb)
{
	UINT8 fnc_code;
	UINT8 no_of_regs;	/*�޸Ķ���Ϊ�ֽ�*/
	UINT16 start_addr;	
	

	fnc_code = recv_buf_mb[MBUS_FUNC];
	/*��2��8λ���ֽ����ݸ���һ��16λ��������Ҫ�����ߵ�λ��Ҳ���Բ�����λ����*/
        start_addr = lo_hi( &recv_buf_mb[MBUS_START] ) ;
	no_of_regs = lo_hi( &recv_buf_mb[MBUS_NOREGS] );

	/*�����ݵĳ�����1��256֮�䣬���ܳ��ȱ���Ϊ8*/
        if(( no_of_regs ) && ( no_of_regs <= READ_REGS_MAX ) && ( recv_len  == 5 ))
	{
		return Rep_Read_Registers(start_addr, no_of_regs, fnc_code, send_buf_mb);
	}
	else
	{
		return SendException(send_buf_mb, fnc_code, ILLEGAL_VALUE);/*���򱨳��ȴ�����03*/
	}
}


/******************************************************************************
 * ������:	PresetMultipleRegisters 
 * ����: 
 *            -д����Ĵ���
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:
 * �޸�����:
  *
 *
 ******************************************************************************/
UINT8 PresetMultipleRegisters( const UINT8 *recv_buf_mb, UINT16 recv_len, UINT8 *send_buf_mb )
{	
	UINT8 send_len = 0;
	UINT8 fnc_code, byte_count;
	UINT8 no_of_regs;	/*�޸Ķ���Ϊ�ֽ�*/
	UINT16 start_addr;	
	//UINT8 siteid = recv_buf_mb[MBUS_ADDR];	/*�㲥ֻ�ܶ�ʱ,��Ҫ����һ�δ���*/

	fnc_code = recv_buf_mb[MBUS_FUNC];
	start_addr = lo_hi(&recv_buf_mb[MBUS_START]) ;
	no_of_regs = lo_hi(&recv_buf_mb[MBUS_NOREGS]);
	byte_count = recv_buf_mb[MBUS_BYTECNT];
	
	/*����ж�����:byte_count == recv_len-6(ʵ��Ϊ��9),���ʵ��Ҫд����ֽ����Ƿ����byte_count*/
	if(( no_of_regs ) && ( no_of_regs <= WRITE_REGS_MAX ) && ( byte_count  == no_of_regs * 2 ) &&( byte_count == recv_len-6 ))
	{
		send_len = Rep_Write_Registers( start_addr, no_of_regs, &recv_buf_mb[MBUS_BYTECNT], fnc_code, send_buf_mb );
	}
	else
	{
		return SendException( send_buf_mb, fnc_code, ILLEGAL_VALUE );/*�����ȴ�����*/
	}
	
	return send_len;
}


/******************************************************************************
 * ������:	ForceSingleCoil 
 * ����: 
 *            -д�����Ĵ���
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 *
 *
 ******************************************************************************/
UINT8 ForceSingleCoil(const UINT8  *recv_buf_mb, UINT16  recv_len, UINT8 *send_buf_mb)
{	
	if( recv_len != 5 )
	{
		return DATA_ERR;	/*�������Ҳ����һ���޶�,��ֹFF 00 ���滹������Ҳ�ܳɹ�*/
	}
	
	UINT8  fnc_code;
	UINT16  start_addr;

	fnc_code = recv_buf_mb[MBUS_FUNC];
	start_addr = lo_hi(&recv_buf_mb[MBUS_START]) ;
	
	/*д������Ȧ*/
	return Rep_Write_SingleCoil(start_addr, &recv_buf_mb[4], fnc_code, send_buf_mb);

}

/******************************************************************************
 * ������:	ReadSecretRegisters 
 * ����: 
 *            -�����ܼĴ������Ͷ���ͨ�Ĵ�������һ�£�����ͬ������Ӧ����
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:
 * �޸�����:
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
 * ������:	WriteSecretRegisters 
 * ����: 
 *            -д���ܼĴ�������д��ͨ�Ĵ���һ�£�����ͬ������Ӧ����
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 *------------------------
 * �޸���:
 * �޸�����:
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
 * ������:	SendException 
 * ����: 
 *            -MODBUS���İ�����
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:2008.8.13
 * 
 *------------------------
 * �޸���:
 * �޸�����:2010.10.13
 *
 *
 ******************************************************************************/
UINT8 ParsePackage(const UINT8 *recv_buf_mb, UINT16 recv_len, UINT8 *send_buf_mb)
{
	UINT8 send_len = 0;
	UINT8 siteid = recv_buf_mb[MBUS_ADDR];
	UINT8 fnc_code = recv_buf_mb[MBUS_FUNC];

	/*Ŀǰ�㲥����ֻ����д�Ĵ����еĶ�ʱ*/
	if(( siteid == 0x00 ) && ( fnc_code != PRESET_MULTIPLE_REGS ))
	{
		return 0;
	}	

	switch(fnc_code)                        /*�����������룬ת����Ӧ���*/
	{
		case READ_HOLDING_REGS:         /*��������03*/
			/*ת��Ӧ�ò�*/
                        send_len = ReadHoldingRegisters( recv_buf_mb, recv_len, send_buf_mb );
			break;

		case PRESET_MULTIPLE_REGS:      /*д������10*/	
			send_len = PresetMultipleRegisters( recv_buf_mb, recv_len, send_buf_mb );
			break;

		case FORCE_SINGLE_COIL:		/*DOң�ع���05*/
			send_len = ForceSingleCoil(recv_buf_mb, recv_len, send_buf_mb);
			break;

#if 0
		case READ_SECRET_REGS:	/*���ܶ�46*/
			send_len = ReadSecretRegisters( recv_buf_mb, recv_len, send_buf_mb );
			break;

		case WRITE_SECRET_REGS:	/*����д47*/	
			send_len = WriteSecretRegisters( recv_buf_mb, recv_len, send_buf_mb );
			break;
#endif
			
		/*���������Ĺ����룬�򱨹��ܴ�����01*/
                default:
			send_len = SendException(send_buf_mb, fnc_code, ILLEGAL_FUNCTION);
			break;

	}
        /*�㲥��ʱ������*/
	if (siteid == 0x00)
	{
		send_len = 0;  		/*�㲥�����Ӧ�����ϴ�*/	
	}
	return send_len;
}


/******************************************************************************
 * ������:	ModbusProcess 
 * ����: 
 *            -MODBUS��������
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
UINT16 ModbusProcess(const UINT8 recv_buf_mb[],UINT16 recv_len,UINT8 send_buf_mb[])
{

	UINT16 send_len;

	send_len = 0;

	send_len = ParsePackage(recv_buf_mb, recv_len, send_buf_mb);/*ת��������*/

	return send_len;
}
