

/**************************Copyright (c)****************************************************
 * 								
 * 						
 * 	
 * ------------------------文件信息---------------------------------------------------
 * 文件名:registers.c 
 * 版本:
 * 描述:modbus协议的寄存器处理，用户接口
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

UINT16  System_Reset = 0;   /*系统复位*/

static UINT16 RegValue16 = 0; 


/*寄存器索引表*/
const static Map_Reg_Table Reg_Table[] = 
{
	#ifdef HAS_8I8O
	{ENVI_START_ADDR+0, (UINT16*)&RegValue16, READONLY, 1},
	{ENVI_START_ADDR+1, (UINT16*)&RegValue16, READONLY, 1},
	{ENVI_START_ADDR+2, (UINT16*)&input[0], READONLY, 1},
	{ENVI_START_ADDR+3, (UINT16*)&input[1], READONLY, 1},
	{ENVI_START_ADDR+4, (UINT16*)&input[2], READONLY, 1},
	{ENVI_START_ADDR+5, (UINT16*)&input[3], READONLY, 1},
	{ENVI_START_ADDR+6, (UINT16*)&input[4], READONLY, 1},
	{ENVI_START_ADDR+7, (UINT16*)&input[5], READONLY, 1},
	{ENVI_START_ADDR+8, (UINT16*)&input[6], READONLY, 1},
	{ENVI_START_ADDR+9, (UINT16*)&input[7], READONLY, 1},
	#endif

	/*装置信息寄存器*/
	{DEVICEINFO_START_ADDR+0, (UINT16*)&DevicComInfor.deviceType[0], READONLY, 1},
	{DEVICEINFO_START_ADDR+1, (UINT16*)&DevicComInfor.deviceType[1], READONLY, 1},
	{DEVICEINFO_START_ADDR+2, (UINT16*)&DevicComInfor.deviceType[2], READONLY, 1},
	{DEVICEINFO_START_ADDR+3, (UINT16*)&DevicComInfor.deviceType[3], READONLY, 1},
	{DEVICEINFO_START_ADDR+4, (UINT16*)&DevicComInfor.deviceType[4], READONLY, 1},
	{DEVICEINFO_START_ADDR+5, (UINT16*)&DevicComInfor.deviceType[5], READONLY, 1},
	{DEVICEINFO_START_ADDR+6, (UINT16*)&DevicComInfor.deviceType[6], READONLY, 1},
	{DEVICEINFO_START_ADDR+7, (UINT16*)&DevicComInfor.deviceType[7], READONLY, 1},
	{DEVICEINFO_START_ADDR+8, (UINT16*)&DevicComInfor.deviceType[8], READONLY, 1},
	{DEVICEINFO_START_ADDR+9, (UINT16*)&DevicComInfor.deviceType[9], READONLY, 1},
	{DEVICEINFO_START_ADDR+10, (UINT16*)&DevicComInfor.deviceType[10], READONLY, 1},
	{DEVICEINFO_START_ADDR+11, (UINT16*)&DevicComInfor.deviceType[11], READONLY, 1},
	{DEVICEINFO_START_ADDR+12, (UINT16*)&DevicComInfor.deviceType[12], READONLY, 1},
	{DEVICEINFO_START_ADDR+13, (UINT16*)&DevicComInfor.deviceType[13], READONLY, 1},
	{DEVICEINFO_START_ADDR+14, (UINT16*)&DevicComInfor.deviceType[14], READONLY, 1},
	{DEVICEINFO_START_ADDR+15, (UINT16*)&DevicComInfor.deviceType[15], READONLY, 1},
	{DEVICEINFO_START_ADDR+16, (UINT16*)&DevicComInfor.deviceType[16], READONLY, 1},
	{DEVICEINFO_START_ADDR+17, (UINT16*)&DevicComInfor.deviceType[17], READONLY, 1},
	{DEVICEINFO_START_ADDR+18, (UINT16*)&DevicComInfor.deviceType[18], READONLY, 1},
	{DEVICEINFO_START_ADDR+19, (UINT16*)&DevicComInfor.deviceType[19], READONLY, 1},
	{DEVICEINFO_START_ADDR+20, (UINT16*)&RegValue16, READONLY, 1},
	{DEVICEINFO_START_ADDR+21, (UINT16*)&DevicComInfor.softVersion, READONLY, 1},
	{DEVICEINFO_START_ADDR+22, (UINT16*)&DevicComInfor.protocolVersion, READONLY, 1},
	
	
	/*装置参数寄存器*/
	{PARAMS_START_ADDR+0, (UINT16*)&DevParams.Address, READONLY, 1},
	{PARAMS_START_ADDR+1, (UINT16*)&DevParams.BaudRate_1, WRITEREAD, 1},
	{PARAMS_START_ADDR+2, (UINT16*)&DevParams.BaudRate_2, WRITEREAD, 1},
	{PARAMS_START_ADDR+3, (UINT16*)&DevParams.BaudRate_3, WRITEREAD, 1},
	{PARAMS_START_ADDR+4, (UINT16*)&DevParams.BaudRate_4, WRITEREAD, 1},
	{PARAMS_START_ADDR+5, (UINT16*)&SystemStatus, READONLY, 1}
};

const INT32 Reg_max[] = {
						#ifdef HAS_8I8O
							0,0,0,0,0,0,0,0,0,0,
						#endif
							/*装置信息寄存器*/
							0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,

							/*装置参数寄存器*/
							240,5,5,5,5,1
						};

const INT32 Reg_min[] = {
						#ifdef HAS_8I8O
							0,0,0,0,0,0,0,0,0,0,
						#endif
							/*装置信息寄存器*/
							0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,

							/*装置参数寄存器*/
							1,0,0,0,0,0
						};


#define REG_NUM  (sizeof(Reg_Table)/sizeof(Map_Reg_Table))


/******************************************************************************
 * 函数名:	CheckRegValue 
 * 描述: 
 *           			 -检查所写寄存器值是否超出范围，超范围返回1。
 * 输入参数: pReg:所写寄存器信息 value：所写的值 index：要写的寄存器索引
 *
 * 输出参数: 无
 *
 * 返回值: 检查情况，1:寄存器值超限，0:正常
 * 
 * 作	者:		
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/

UINT8 CheckRegValue( const Map_Reg_Table *pReg,INT32 value,UINT16 index )
{
	UINT16 reg_addr = pReg->nRegNo;

	if(( reg_addr >= PARAMS_START_ADDR ) && ( reg_addr < (PARAMS_START_ADDR + PARAMS_REG_MAX)))
	{
		if( reg_addr == ( PARAMS_START_ADDR + 1 ))  /*串口1波特率*/
		{
			/*检查所写的值是否在最大值和最小值之间*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM1_MODIFIED;   /*检查数据合格，装置参数改变*/
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 2 ))
		{
			/*检查所写的值是否在最大值和最小值之间*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM2_MODIFIED;   /*检查数据合格，装置参数改变*/
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 3 ))
		{
			/*检查所写的值是否在最大值和最小值之间*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM3_MODIFIED;   /*检查数据合格，装置参数改变*/
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 4 ))
		{
			/*检查所写的值是否在最大值和最小值之间*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM4_MODIFIED;   /*检查数据合格，装置参数改变*/
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
                        }
		}
		else
		{
			 if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
			 }
		}
	}		
	return 1;
}

/********************************************************************************************************
** 函数名：    Search_Reg_Index
** 功能描述：  根据寄存器编号查找在寄存器索引表中查找对应的索引
** 输入：		nRegNo--寄存器编号
** 输出：		pReg--寄存器指针
** 全局变量：	
** 调用模块：
**
**
** 修改：
** 日期：
**
**说明 :寄存器查找由顺序算法修改为二分法查找算法，由于寄存器
**		  比较多，采用顺序查找时间较长，因此采用二分法查找；
**
**
**********************************************************************************************************/


UINT16 Search_Reg_Index(UINT16 nRegNo, const Map_Reg_Table **pReg)
{	
	UINT16 low = 0; 
	UINT16 high = REG_NUM-1;  
	UINT16 mid; 
	
 	while( low <= high)  
	{ 
       		mid = (low + high) /2;   
		if( Reg_Table[mid].nRegNo == nRegNo )/*找到*/
		{
			*pReg = &Reg_Table[mid];     /*返回索引值,*pReg还是个指针,它指向的是Reg_Table[mid]的地址*/
			return  mid;  
		}
        	/*要找的地址大于中间值，将中间值作为小值继续寻找，否则作为大值*/
                else if( Reg_Table[mid].nRegNo< nRegNo ) 
		{
			low = mid + 1;    
		}
                else  
		{
			high = mid - 1; 
		}
 	}  
	
	pReg = NULL;      /*没找到*/
	return REG_NUM;   /*将索引地址超出点表地址*/
}

/********************************************************************************************************
** 函数名：    Read_Register
** 功能描述：  读一定数据的寄存器内容到缓冲区中
** 输入：		nStartRegNo--寄存器开始编号,nRegNum--读取的寄存器个数,
**				pdatabuf--数据缓冲区地址,maxlen--缓冲区最大长度
** 输出：		perr--操作结果码,UINT16--实际已经读取的数据长度
** 全局变量：	
** 调用模块：
**
** 作者：
** 日期：
**
** 修改：Jerry
** 日期：2013.03.12
**
**
**********************************************************************************************************/
UINT8 Read_Register(UINT16 nStartRegNo, UINT16 nRegNum, UINT8 *pdatabuf, UINT8 *perr)
{
	UINT16 i;
	const Map_Reg_Table *pCurReg = NULL;
	UINT16 *ptmpAddr = NULL;	/*yfy修改INT16为UINT16*/
	UINT16 nNeedReadNum;
	UINT16 nReadRegNo;
	UINT16 nReadLen = 0;
	UINT16 nEndRegNo;

	nEndRegNo = nStartRegNo + nRegNum - 1;

	/*读装置参数寄存器*/
	if((nStartRegNo >= PARAMS_START_ADDR) && (nEndRegNo < (PARAMS_START_ADDR + PARAMS_REG_MAX )))
	{
		;
	}
	#ifdef HAS_8I8O
	/*读装置参数寄存器*/
	else if((nStartRegNo >= ENVI_START_ADDR) && (nEndRegNo < (ENVI_START_ADDR + ENVI_REG_MAX )))
	{
		;
	}
	#endif

	
	/*读装置信息寄存器*/
	else if((nStartRegNo >= DEVICEINFO_START_ADDR) && (nEndRegNo < (DEVICEINFO_START_ADDR + DEVICEINFO_REG_MAX)))
	{
		;
	}
	
	else 
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	nNeedReadNum = nRegNum;
	nReadRegNo = nStartRegNo;
	
	/*寻找开始地址的索引*/
	Search_Reg_Index(nReadRegNo, &pCurReg);	/*pCurReg找出的是开始读的地址,即第一个地址*/
	if (pCurReg == NULL)	/*pCurReg是一个指针,函数取其地址,是指向指针的指针,若不用&pCurReg，将为形参传递,是错误的*/
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	while(nNeedReadNum > 0)
	{
		/*读的寄存器长度错误*/
		if (nNeedReadNum < pCurReg->reg.nLenth)
		{
			*perr = DATA_ERR;
			return(0);
		}
		
		if ((pCurReg->reg.bAtrrib!=READONLY) && (pCurReg->reg.bAtrrib!=WRITEREAD))
		{
			*perr = REGADDR_ERR;/*该寄存器不能读*/
			return(0);
		}
		
		ptmpAddr = pCurReg->reg.pAddr;	/*取出指定位置的寄存器名称,注意ptmpAddr是指针,它指向的是实际的寄存器*/
		
		if (ptmpAddr == NULL)	/*当寄存器为保留时，将无法找到，返回NULL*/
		{
			for (i=0; i<pCurReg->reg.nLenth; i++)
			{
				pdatabuf[nReadLen++] = 0x00;
				pdatabuf[nReadLen++] = 0x00;	/*保留寄存器,返回0X0000*/
			}
		}
		else
		{
			for(i=0; i<pCurReg->reg.nLenth; i++)
			{
				INT16 rccode = 0x0000;
				
				if (ptmpAddr != NULL)
				{
					rccode = *(ptmpAddr + pCurReg->reg.nLenth - i - 1);	/*表示从寄存器的高位开始读*/
				}
								
				pdatabuf[nReadLen++]  = (UINT8)((rccode>>8)&0x00FF);/*高位*/
				pdatabuf[nReadLen++]  = (UINT8)(rccode&0x00FF);		/*低位*/
			}
		}
		
		nNeedReadNum -= pCurReg->reg.nLenth;	/*调整需要读取的寄存器数目*/
		pCurReg++;		/*读下一个索引*/
	}
	
	*perr = REG_OK;
	return nReadLen;
}

/********************************************************************************************************
** 函数名：    Write_Register
** 功能描述：  写寄存器
** 输入：		nStartRegNo--寄存器开始编号,nRegNum--读取的寄存器个数,
**				pdatabuf--写入的数据缓冲区地址,datalen--缓冲区长度
** 输出：		perr--操作结果码,
** 全局变量：	
** 调用模块：
**
** 修改：
** 日期：
**
**********************************************************************************************************/
UINT8 Write_Register( UINT16 nStartRegNo, INT16 nRegNum, const UINT8 *pdatabuf, UINT8 datalen, UINT8 *perr )
{
	const Map_Reg_Table *pCurReg, *pTmpReg;	          /*索引指针*/
	UINT16 *ptmpAddr = NULL;			  /*yfy修改INT16为UINT16*/
	const UINT8 *temp_pdatabuf =NULL;		  /*缓存接收缓冲区的指针地址*/
	UINT16 nNeedWriteNum;				  /*需要写的寄存器个数*/
	UINT16 nWriteRegNo;				  /*Reg编号*/
	UINT16 nEndRegNo;
	INT32 temp_value=0;		                  /*存储要写入寄存器的值*/
	UINT16 temp_index;			          /*数组Reg_Table元素下标*/
	
	nEndRegNo = nStartRegNo + nRegNum - 1;
	/*写装置参数寄存器*/
	if(( nStartRegNo >= PARAMS_START_ADDR) && (nEndRegNo < (PARAMS_START_ADDR + PARAMS_REG_MAX )))
	{
	
	}
	else 
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	
	/*对指针进行初始化*/
	pCurReg = NULL;
	pTmpReg = NULL;
	
	nNeedWriteNum = nRegNum;
	nWriteRegNo = nStartRegNo;
	
	temp_index = Search_Reg_Index(nWriteRegNo, &pCurReg); /*找地址索引*/

	if (pCurReg == NULL)
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	pTmpReg = pCurReg;
	
	/*保证所写的数据不能只写了一个高字节,而没有写低字节或者只写了必须作为一个整体的数据部分数据(eg:定值)*/
        while( nNeedWriteNum > 0)
	{
		if ( nNeedWriteNum < pTmpReg->reg.nLenth )/*当出现要写入的数量比当前寄存器所占的数量小就报错*/
		{
			*perr = DATA_ERR;
			return(0);
		}
		
		nNeedWriteNum -= pTmpReg->reg.nLenth;	
		pTmpReg++;
	}
	
	/*开始写入数据范围检查*/
	nNeedWriteNum = nRegNum;
	nWriteRegNo = nStartRegNo;

	pTmpReg = pCurReg;		/*缓存指针，用于写入的寄存器范围判断*/
	temp_pdatabuf = pdatabuf;       /*缓存缓冲区地址，用于写入的寄存器范围判断*/
	
        /*对写的寄存器进行检查，并对所写的地址进行分类，置相应的标志位*/
	while( nNeedWriteNum > 0 )
	{

		ptmpAddr = pTmpReg->reg.pAddr;  
		/*只有当寄存器已定义，且可写的时候才取写寄存器的值,否则直接跳过本寄存器*/
		if ( ptmpAddr != (UINT16*)0 && (( pTmpReg->reg.bAtrrib == WRITEONLY ) || ( pTmpReg->reg.bAtrrib == WRITEREAD ))) 
		{	
			if( pTmpReg->reg.nLenth == 1 )
			{
				temp_value= (INT32)((UINT16)temp_pdatabuf[0]<<8 | temp_pdatabuf[1]);
				if( CheckRegValue( pTmpReg,temp_value,temp_index ))//注意这里一次只检查一个寄存器
				{		
					*perr = DATA_ERR;
					return(0);
				}

				ptmpAddr++;
				temp_pdatabuf += 2;
			}
			
			else if ( pTmpReg->reg.nLenth == 2 )
			{
					temp_value = (( UINT16)temp_pdatabuf[0]<<8 | temp_pdatabuf[1] );
					temp_value= ( temp_value &0x0000FFFF );//yfy添加
					temp_value = temp_value<<16;
					temp_value += ( (UINT16)temp_pdatabuf[2]<<8 | temp_pdatabuf[3] );
					if( CheckRegValue( pTmpReg,temp_value,temp_index ))//yfy修改2010.10.11
					{			
						*perr = DATA_ERR;
						return(0);
					}
				ptmpAddr ++;
				ptmpAddr ++;
				temp_pdatabuf += 4;
			}
			temp_index++;/*寄存器范围检查完毕后，准备检查写一个寄存器，这里下标要加1yfy 2010.10.11*/
		}
		else
		{
			*perr = REGADDR_ERR;
			return(0);
		}
		
		nNeedWriteNum -= pTmpReg->reg.nLenth;
		
		pTmpReg++;	/*下一个索引的寄存器*/
		if ( pTmpReg == NULL )
		{
			*perr = DATA_ERR;
			return(0);
		}
	}

	
	/*前面数据检查无误后开始判断写寄存器*/
	nNeedWriteNum = nRegNum;
	nWriteRegNo = nStartRegNo;


	while(nNeedWriteNum > 0)
	{
		ptmpAddr = pCurReg->reg.pAddr;  
		/*只有当寄存器可写的时候才取写寄存器的值,否则直接跳过本寄存器*/
		if ( ptmpAddr != (UINT16*)0 && ((pCurReg->reg.bAtrrib == WRITEONLY)||( pCurReg->reg.bAtrrib == WRITEREAD ))) 
		{	
			if( pCurReg->reg.nLenth == 1 )
			{
				*ptmpAddr =( (UINT16)pdatabuf[0]<<8 | pdatabuf[1] );
				ptmpAddr++;
				pdatabuf += 2;
			}
			else if ( pCurReg->reg.nLenth == 2 )
			{
				/*前面检查寄存器范围正确，这里才写入*/
				*ptmpAddr = (( UINT16)pdatabuf[2]<<8 | pdatabuf[3] );
				ptmpAddr ++;
				*ptmpAddr = (( UINT16)pdatabuf[0]<<8 | pdatabuf[1] );
				ptmpAddr ++;
				pdatabuf += 4;
				
			}
		}
		else
		{
			*perr = REGADDR_ERR;
			return(0);
		}
		
		nNeedWriteNum -= pCurReg->reg.nLenth;	
		pCurReg++;	/*写下一个索引的寄存器*/
		
                /*寄存器未定义*/
                if ( pCurReg == NULL )
		{
			*perr = DATA_ERR;
			return(0);
		}
	}	
	*perr = REG_OK;	
	return(datalen);
}


