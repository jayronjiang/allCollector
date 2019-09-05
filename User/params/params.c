#include "include.h"


DEVICE_PARAMS DevParams;
/*装置相关信息*/
DeviceInfoParams  DevicComInfor;
UINT16 SystemStatus = 0;	// 0:良好，1: 有故障

#if 0
RSU_PARAMS RSUParams;

UPS_PARAMS UPSParams;

SPD_PARAMS SPDParams[SPD_NUM];

ARD_PARAMS ARDParams[ARD_NUM];

BREAKER_PARAMS BRKParams[BRK_NUM];

ENVI_PARAMS ENVIParms = {267, 582, };
#endif
			
/*默认装置参数*/
const DEVICE_PARAMS Init_DevParams=
                                 {
                                    	0,				/*各字节累加和*/
					( DEVICE_PARAM_COUNTER -4 )>>1,	/*结构体有效长度UINT16*/

					DEV_ADDR,
					BAUD_9600,
					BAUD_9600,
					BAUD_9600,
					BAUD_9600,
					0,			// DO 预置
					#ifdef HAS_8I8O
					0xFF			// DO_STATUS, 8路,初始状态是闭合
					#else
					0xFFF
					#endif
                                 };

void read_fm_memory(UINT16 addr,UINT8 *dest_ptr, UINT16 length)
{
	W25QXX_Read(CS_0,dest_ptr, addr, length ); 
}

void write_fm_memory(UINT16 addr, UINT8 *src_ptr, UINT16 length)
{
	W25QXX_Write( src_ptr, addr, length);
}

void Write_DevParams(void)
{
	DevParams.Crc = get_crc16((UINT8 *)&DevParams+4, DEVICE_PARAM_COUNTER-4);

	write_fm_memory( FM_DEVICE, (UINT8 *)&DevParams, DEVICE_PARAM_COUNTER);
}

/*铁电初始化统一函数*/
void Save_Param_To_Fm(UINT16 device_param_size1,UINT16 *device_p1,UINT16 start_addr1 )
{
	*device_p1 = get_crc16((UINT8 *)device_p1+4,device_param_size1-4);
	/*写入BLOCK1*/
	write_fm_memory( start_addr1,(UINT8*)device_p1,device_param_size1 );
}


/******************************************************************************
 * 函数名:	UINT8  Read_Fm_Param() 
 * 描述: 只考虑参数定义一致的情况，如果修改结构体需要
 * 		修改上电标志
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:		yfy
 * 创建日期:	2012.8.12
 * 
 *------------------------
 * 修改人:	CZH
 * 修改日期:	2012.8.29
 ******************************************************************************/

UINT8  Read_Fm_Param(UINT16 *device_p,const UINT16  *device_init_p, UINT16 device_param_size, UINT8 first_flag,UINT16 start_addr )
{
	UINT16 crc_code;
	UINT16 i;

	if(first_flag)  	/*首次上电标志,在前面需要检查是否是首次上电*/
	{
		for(i = 0 ;i < device_param_size/2; i++)
		{	
			*(device_p+i) =*(device_init_p+i);
		}
		//*(device_p+1) = device_param_size/2 -2; 	/*初始化的时候对长度可能赋值不准,长度重新计算*/
		Save_Param_To_Fm(device_param_size,device_p,start_addr);	
	}
	else
	{
		//读一次
		read_fm_memory( start_addr, (UINT8*)device_p,device_param_size );
		crc_code = get_crc16( (UINT8 *)device_p+4,device_param_size-4);

		if(*device_p != crc_code) /*CRC 不正确*/
		{
			//再读一次
			read_fm_memory(start_addr, (UINT8*)device_p,device_param_size);
			crc_code = get_crc16((UINT8 *)device_p+4, device_param_size-4);
			if(*device_p != crc_code)
			{
				/*参数恢复默认*/
				for(i = 0 ;i < device_param_size/2; i++)
				{	
					*(device_p+i) =*(device_init_p+i);
				}
				//*(device_p+1) = (device_param_size -4)/2;	/*初始化的时候对长度可能赋值不准,长度重新计算*/
				/*保存参数至铁电中*/
				Save_Param_To_Fm(device_param_size,device_p,start_addr);	
				return 1;
			}
		}
	}
	return 0;
}

/******************************************************************************
 * 函数名:	Init_Params 
 * 描述: 
 *            -上电时初始化参数，同时对铁电存储器中的参数进行自检
 * 输入参数: 无
 * 输出参数: 无
 * 返回值: 无
 * 
 * 作者:汪治国 
 * 创建日期:2010.1.11
 * 
 *------------------------
 * 修改人:Jerry
 * 修改日期:2013-02-01
 ******************************************************************************/
bool  Init_Params(void)
{
	UINT16 err_sign = 0;
	UINT32 sync_sign = 0;
	UINT16 first_run_flag;
	
	read_fm_memory(FM_FIRST_WORD, (UINT8 *)&sync_sign, 4);		/*读同步标志,读3次*/
	if( sync_sign != SYSTEM_SYNC)
	{
		Delay_Ms(1);
		read_fm_memory(FM_FIRST_WORD, (UINT8 *)&sync_sign, 4);
		if(sync_sign != SYSTEM_SYNC )
		{
			Delay_Ms(2);
			read_fm_memory(FM_FIRST_WORD, (UINT8 *)&sync_sign, 4);
		}
	}

	if(sync_sign == SYSTEM_SYNC)
	{
		first_run_flag = 0;		
	}
	else
	{
		first_run_flag = 0XFF;			/*首次上电,铁电数据待写入*/
		sync_sign = SYSTEM_SYNC;		/*设置同步标志*/
		write_fm_memory(FM_FIRST_WORD, (UINT8 *)&sync_sign, 4);
	}

	/*读取装置参数*/
	err_sign = Read_Fm_Param((UINT16 *)&DevParams, (UINT16 *)&Init_DevParams, DEVICE_PARAM_COUNTER, first_run_flag, FM_DEVICE);
	if(err_sign)
	{
		//参数错误
		SystemStatus = 1;
	}

	// 装置信息初始化
	ComDeviceInfoInit();
	return (first_run_flag?1:0);
}   

/***********************************************************************************
 * 函数名:	void ComDeviceInfoInit(void)
 * 描述: 
 *           	 --初始化装置信息:程序版本 、规约版本、序列号等
 * 输入参数: 无
 *
 * 输出参数: 无
 *
 * 返回值: 	无
 * 
 * 作	者:		杨甫勇
 * 创建日期:2012.02.20
 * 
 *------------------------
 * 修改人:Jerry,移植至2612D中
 * 修改日期:2013-03-12
 ***********************************************************************************/
void ComDeviceInfoInit(void)
{
	UINT8   i=0;
	UINT8   j;
	UINT8 *pDeviceType=DEVICE_TYPE;

	while((*pDeviceType !='\0') && (i<20))/*读取装置类型*/
	{
		DevicComInfor.deviceType[i] = *pDeviceType;
		pDeviceType++;
		i++;   
	}

	if(i<20)                         /*不够20字符，以空格(ASCII码为20)代替*/
	{
		for(j=i; j < 20; j++)
		{
			DevicComInfor.deviceType[j] =0x20; 
		}
	}
	DevicComInfor.softVersion = SOFTWARE_VERSION;
	DevicComInfor.softYear = (UINT32)VERSION_DATE/10000; 
	DevicComInfor.softMonth = (UINT32)VERSION_DATE/100-(UINT16)DevicComInfor.softYear*100;
	DevicComInfor.softDate = (UINT32)VERSION_DATE-(UINT32)DevicComInfor.softYear*10000 
                                  - (UINT16)DevicComInfor.softMonth*100;
	DevicComInfor.protocolVersion = PROTOCAL_REVISION;
}


/******************************************************************************
 * 函数名:	Self_Check 
 * 描述: 
 *            -系统自检
 * 输入参数: 无
 * 输出参数: 无
 * 返回值: 无
 * 
 * 作者:Jerry
 * 创建日期:2019.08.08
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
void Self_Check(void)
{
	DEVICE_PARAMS DevParamRead;

	memset(&DevParamRead, 0, DEVICE_PARAM_COUNTER);	// 初始化为0
	read_fm_memory(FM_DEVICE, (UINT8*)&DevParamRead, DEVICE_PARAM_COUNTER);
	if (memcpy(&DevParams,&DevParamRead,DEVICE_PARAM_COUNTER))
	{
		SystemStatus = 0;
	}
	else
	{
		SystemStatus = 1;
	}
}


