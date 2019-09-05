#include "include.h"


DEVICE_PARAMS DevParams;
/*װ�������Ϣ*/
DeviceInfoParams  DevicComInfor;
UINT16 SystemStatus = 0;	// 0:���ã�1: �й���

#if 0
RSU_PARAMS RSUParams;

UPS_PARAMS UPSParams;

SPD_PARAMS SPDParams[SPD_NUM];

ARD_PARAMS ARDParams[ARD_NUM];

BREAKER_PARAMS BRKParams[BRK_NUM];

ENVI_PARAMS ENVIParms = {267, 582, };
#endif
			
/*Ĭ��װ�ò���*/
const DEVICE_PARAMS Init_DevParams=
                                 {
                                    	0,				/*���ֽ��ۼӺ�*/
					( DEVICE_PARAM_COUNTER -4 )>>1,	/*�ṹ����Ч����UINT16*/

					DEV_ADDR,
					BAUD_9600,
					BAUD_9600,
					BAUD_9600,
					BAUD_9600,
					0,			// DO Ԥ��
					#ifdef HAS_8I8O
					0xFF			// DO_STATUS, 8·,��ʼ״̬�Ǳպ�
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

/*�����ʼ��ͳһ����*/
void Save_Param_To_Fm(UINT16 device_param_size1,UINT16 *device_p1,UINT16 start_addr1 )
{
	*device_p1 = get_crc16((UINT8 *)device_p1+4,device_param_size1-4);
	/*д��BLOCK1*/
	write_fm_memory( start_addr1,(UINT8*)device_p1,device_param_size1 );
}


/******************************************************************************
 * ������:	UINT8  Read_Fm_Param() 
 * ����: ֻ���ǲ�������һ�µ����������޸Ľṹ����Ҫ
 * 		�޸��ϵ��־
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:		yfy
 * ��������:	2012.8.12
 * 
 *------------------------
 * �޸���:	CZH
 * �޸�����:	2012.8.29
 ******************************************************************************/

UINT8  Read_Fm_Param(UINT16 *device_p,const UINT16  *device_init_p, UINT16 device_param_size, UINT8 first_flag,UINT16 start_addr )
{
	UINT16 crc_code;
	UINT16 i;

	if(first_flag)  	/*�״��ϵ��־,��ǰ����Ҫ����Ƿ����״��ϵ�*/
	{
		for(i = 0 ;i < device_param_size/2; i++)
		{	
			*(device_p+i) =*(device_init_p+i);
		}
		//*(device_p+1) = device_param_size/2 -2; 	/*��ʼ����ʱ��Գ��ȿ��ܸ�ֵ��׼,�������¼���*/
		Save_Param_To_Fm(device_param_size,device_p,start_addr);	
	}
	else
	{
		//��һ��
		read_fm_memory( start_addr, (UINT8*)device_p,device_param_size );
		crc_code = get_crc16( (UINT8 *)device_p+4,device_param_size-4);

		if(*device_p != crc_code) /*CRC ����ȷ*/
		{
			//�ٶ�һ��
			read_fm_memory(start_addr, (UINT8*)device_p,device_param_size);
			crc_code = get_crc16((UINT8 *)device_p+4, device_param_size-4);
			if(*device_p != crc_code)
			{
				/*�����ָ�Ĭ��*/
				for(i = 0 ;i < device_param_size/2; i++)
				{	
					*(device_p+i) =*(device_init_p+i);
				}
				//*(device_p+1) = (device_param_size -4)/2;	/*��ʼ����ʱ��Գ��ȿ��ܸ�ֵ��׼,�������¼���*/
				/*���������������*/
				Save_Param_To_Fm(device_param_size,device_p,start_addr);	
				return 1;
			}
		}
	}
	return 0;
}

/******************************************************************************
 * ������:	Init_Params 
 * ����: 
 *            -�ϵ�ʱ��ʼ��������ͬʱ������洢���еĲ��������Լ�
 * �������: ��
 * �������: ��
 * ����ֵ: ��
 * 
 * ����:���ι� 
 * ��������:2010.1.11
 * 
 *------------------------
 * �޸���:Jerry
 * �޸�����:2013-02-01
 ******************************************************************************/
bool  Init_Params(void)
{
	UINT16 err_sign = 0;
	UINT32 sync_sign = 0;
	UINT16 first_run_flag;
	
	read_fm_memory(FM_FIRST_WORD, (UINT8 *)&sync_sign, 4);		/*��ͬ����־,��3��*/
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
		first_run_flag = 0XFF;			/*�״��ϵ�,�������ݴ�д��*/
		sync_sign = SYSTEM_SYNC;		/*����ͬ����־*/
		write_fm_memory(FM_FIRST_WORD, (UINT8 *)&sync_sign, 4);
	}

	/*��ȡװ�ò���*/
	err_sign = Read_Fm_Param((UINT16 *)&DevParams, (UINT16 *)&Init_DevParams, DEVICE_PARAM_COUNTER, first_run_flag, FM_DEVICE);
	if(err_sign)
	{
		//��������
		SystemStatus = 1;
	}

	// װ����Ϣ��ʼ��
	ComDeviceInfoInit();
	return (first_run_flag?1:0);
}   

/***********************************************************************************
 * ������:	void ComDeviceInfoInit(void)
 * ����: 
 *           	 --��ʼ��װ����Ϣ:����汾 ����Լ�汾�����кŵ�
 * �������: ��
 *
 * �������: ��
 *
 * ����ֵ: 	��
 * 
 * ��	��:		���
 * ��������:2012.02.20
 * 
 *------------------------
 * �޸���:Jerry,��ֲ��2612D��
 * �޸�����:2013-03-12
 ***********************************************************************************/
void ComDeviceInfoInit(void)
{
	UINT8   i=0;
	UINT8   j;
	UINT8 *pDeviceType=DEVICE_TYPE;

	while((*pDeviceType !='\0') && (i<20))/*��ȡװ������*/
	{
		DevicComInfor.deviceType[i] = *pDeviceType;
		pDeviceType++;
		i++;   
	}

	if(i<20)                         /*����20�ַ����Կո�(ASCII��Ϊ20)����*/
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
 * ������:	Self_Check 
 * ����: 
 *            -ϵͳ�Լ�
 * �������: ��
 * �������: ��
 * ����ֵ: ��
 * 
 * ����:Jerry
 * ��������:2019.08.08
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
void Self_Check(void)
{
	DEVICE_PARAMS DevParamRead;

	memset(&DevParamRead, 0, DEVICE_PARAM_COUNTER);	// ��ʼ��Ϊ0
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


