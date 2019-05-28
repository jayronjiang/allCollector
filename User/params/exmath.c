/**************************Copyright (c)****************************************************
 * 	
 * ------------------------�ļ���Ϣ---------------------------------------------------
 * �ļ���:
 * �汾:
 * ����:
 * 
 * --------------------------------------------------------------------------------------- 
 * ����           :
 * ��������:
 * --------------------------------------------------------------------------------------- 
 * �޸���:
 * �޸�����:
 * �汾:
 * ����:
 ****************************************************************************************/
 #include "include.h"

 UINT16 int_abs(INT16 int_value)
 {
     UINT16 value;
     value=(int_value>0?int_value:(-int_value));
     return(value);
 }
/****************************************************************
 ��������bcd_to_hex()																	
 																								
 �������:��															
																									
 ��������� UINT8 bcd_data															
																								
 ����ֵ:		UINT8 hex															
																									
 ����˵��:��bcd����ת��Ϊ16������ hex 
 **************************************************************/

UINT8 bcd_to_hex(UINT8 bcd_data)
{
	UINT8 hex;
	hex=(bcd_data/16)*10+bcd_data%16;
	return(hex);
}

/****************************************************************
 ��������bcd_to_hex()																	
 																								
 �������:��															
																									
 ��������� UINT8 bcd_data															
																								
 ����ֵ:		UINT8 hex															
																									
 ����˵��:��16������ hexת��Ϊbcd���� 
 **************************************************************/
UINT8 hex_to_bcd(UINT8 hex_data)
{
	UINT8 bcd;
	bcd=(hex_data/10)*16+hex_data%10;
	return(bcd);
}

/*********************************************************
������:int32 lAbs(int32 num)	int16 Abs( int16 num ) fp32 fAbs( fp32 num )

�������:
	int32(int16,fp32) num--�������ֵ�ı���

�������:	��

����ֵ:		
	int32(int16,fp32)--���ر���num�ľ���ֵ

����˵��:����������ľ���ֵ��
*********************************************************/
INT32 lAbs(INT32 num)
{
	return ( num >= 0 ? num : -num);
}

INT16 Abs( INT16 num )
{
	 return ( num >= 0 ? num : -num);
}


/******************************************************************************
 * ������:	lsign 
 * ����: 
 *            -�����������������š�
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:���ι� 
 * ��������:2006.4.10
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
INT32 double_sign(double num)
{
	 return( num >0 ? 1l:-1l);
}

/*************************************************************
������: 	int32 lround()                         

�������:
	int32 div--��������������
	int32 by--������������

�������:	��

����ֵ:
	int32--������

����˵��:��div/by����������Ľ��

*************************************************************/
INT32 lRound(INT32 div,INT32 by)
{
	INT32 temp;
	if (by)
	{
		temp = (div + (by>>1))/by;
	}
	else
	{
		temp=div;
	}
	return(temp); /* ������������Ľ��*/ 
} /* end lRound() */

/******************************************************************************
*  ������:llround()
*
*  ����: ��div/by����������Ľ��
*
*  ����: ������������
*
*  ���: 
*
*  ����ֵ:�з��ų������� ��������Ľ��
*
*  ����: 
*******************************************************************************/
INT32 llround(INT64 div,INT32 by)/*2009.04.21*/
{
	INT32 temp;
	if (by)
	{
		temp = (div + (by>>1))/by;
	}
	else
	{
		temp=div;
	}
	return(temp); /* ������������Ľ��*/ 
}

/******************************************************************************
*  ������: long_sqrt()
*
*  ����: ����������������
*
*  ����: ��Ҫ�������޷��ų���������
*
*  ���: 
*
*  ����ֵ: 16λ�޷���������
*
*  ����: 
*******************************************************************************/
UINT16 long_sqrt(UINT32 M)
{ 
	UINT8 flag = 0; 
	UINT16 n, i; 
	UINT32 tmp, ttp;   /*   �����ѭ������	  */
	
	if (M == 0)               /* �����������������ҲΪ0  */
	{
		return 0; 
	}
	
	if (M > 0x3FF001)
	{
		M >>= 2;
		flag = 1;
	}

	n = 0; 
	tmp = (M >> 30);          /* ��ȡ���λ��B[m-1] */
	M <<= 2; 
	
	if (tmp > 1)              /*  ���λΪ1 */
	{ 
		n++;                 /* �����ǰλΪ1������ΪĬ�ϵ�0 */
		tmp -= n; 
	} 

	for (i=15; i>0; i--)      /* ��ʣ���15λ */
	{ 
		n <<= 1;              /* ����һλ */
		tmp <<= 2; 
		tmp += (M >> 30);     /* ���� */
		ttp = n; 
		ttp = (ttp << 1) + 1; 
		M <<= 2; 
		if (tmp >= ttp)       /* ������� */
		{ 
			tmp -= ttp; 
			n++; 
		} 
	} 

	if (flag)
	{
		n <<= 1;
	}
	return n; 
}

/******************************************************************************
*  ������: double_sqrt()
*
*  ����: �����Ϳ��������ظ���ֵ
*
*  ����: ��Ҫ�������޷��ų���������
*
*  ���: 
*
*  ����ֵ: ����ֵ
*
*  ����: 
*******************************************************************************/
double double_sqrt(UINT32 M)
{
	UINT16 scale;

	scale = 1;

	if (M == 0)
	{
		return 0;
	}
	
	if (M < 0xffff )
	{
		M *= 0x10000;
		scale = 0x100;
	}

	while (M < 0x3fffffff )
	{
		M <<= 2;
		scale <<=1;
	}
	
	return ((double)long_sqrt(M) / scale);
}


/******************************************************************************
*  ������: scale_it()
*
*  ����: �����˻�����һ�������������㣬��ֹ�����һ��
*			����,���������ǰ����������С���з��ų�����
*
*  ����: �����˻�����һ��������
*
*  ���: 
*
*  ����ֵ: (in * by ) / div
*
*  ����: 
*******************************************************************************/
INT32 scale_it(INT32 in, INT32 by, INT32 div)
{
	if ((in == 0L) || (div == 0L) || (by == 0L))
	{
		return(0L);
	}
	return(llround(((INT64)in * by), div));
} 

/*******************************************************************************************************
** ��������		long_2_bcd(UINT32 x, UINT8 * buffer)
** ����������	����������ת��Ϊ�ַ�����
** ���룺		UINT32 x  Ҫת���ĳ�������
                                   UINT8 * buffer    �ַ������ָ��
** �����		��	
** ȫ�ֱ�����	
** ����ģ�飺
**
** ���ߣ���ѧ��
** ���ڣ�2007.1.27
*******************************************************************************************************/
void long_2_bcd(UINT32 x, UINT8 * buffer)
{
      buffer[9] = x/1000000000L;
	x = x%1000000000;
	buffer[8] = x/100000000L;
	x = x%100000000;
	buffer[7] = x/10000000L;
	x = x%10000000;
	buffer[6] = x/1000000L;
       x = x%1000000;
       buffer[5] = x/100000L;
       x = x%100000;  
	buffer[4] = x/10000;
	x = x%10000;
	buffer[3] = x/1000;
	x = x%1000;
	buffer[2] = x/100;
	x = x%100;
	buffer[1] = x/10;
	buffer[0] = x%10;
}

/******************************************************************************
 * ������:	bcd_2_long
 * ����: 
 *            -��һ��bcd���Ϊһ����������
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:��ѧ��
 * ��������:2006.4.10
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
UINT32  bcd_2_long(UINT8 * buf)        
{
	UINT32 itemp;
	
	itemp = (UINT32)buf[9]*1000000000L;
	itemp += (UINT32)buf[8]*100000000L;
	itemp += (UINT32)buf[7]*10000000L;
	itemp += (UINT32)buf[6]*1000000L;
	itemp += (UINT32)buf[5]*100000L;
	itemp += (UINT32)buf[4]*10000L;
	itemp += (UINT32)buf[3]*1000L;
	itemp += (UINT32)buf[2]*100L;
	itemp += buf[1]*10;
	itemp += buf[0];
	return(itemp);

}
/******************************************************************************
 * ������:	bin_2_bcd16
 * ����: 
 *            -��һ����������ת��ΪBCD��.
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:���ι� 
 * ��������:2006.4.10
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
 void bin_2_bcd16(UINT16 x, UINT8 * buffer)
{
	buffer[4] = x/10000;
	x = x%10000;
	buffer[3] = x/1000;
	x = x%1000;
	buffer[2] = x/100;
	x = x%100;
	buffer[1] = x/10;
	buffer[0] = x%10;
}

/******************************************************************************
 * ������:	bcd_2_bin16
 * ����: 
 *            -��һ��bcd���Ϊһ��������
 * 
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:���ι� 
 * ��������:2006.4.10
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/
 UINT16  bcd_2_bin16(UINT8 * buf)        
{
	UINT16 itemp;
	
	itemp = (UINT16)buf[4]*10000;
	itemp += (UINT16)buf[3]*1000;
	itemp += (UINT16)buf[2]*100;
	itemp += (UINT16)buf[1]*10;
	itemp += (UINT16)buf[0];
	return(itemp);

}

UINT16 bcd_to_bin2(UINT8 * buf)
{
        UINT16 itemp=0;
	itemp += (UINT16)buf[1]*10;
	itemp += (UINT16)buf[0];
	return(itemp);
}

 void bin_to_bcd2(UINT8 x, UINT8 * buffer)
{
	buffer[1] = x/10;
	buffer[0] = x%10;
}

double double_fabs( double num )
{
	 return ( (num+1.000000) >=1.000000 ? num : -num );
}


/******************************************************************************
*  ������: get_check_sum()
*
*  ����: �������Դ��ݹ���������ֵ�����ۼӺͼ���,
* 			�����ۼӺͽ��
*
*  ����: �ۼӶ������ʼ��ַ������
*
*  ���: 
*
*  ����ֵ: �ۼӺͽ��
*
*  ����: 
*******************************************************************************/
UINT16 get_check_sum(UINT8 *addr_start,UINT8 counter)
{
	UINT8 i;
	UINT16 check_sum = 0;

	for(i=0;i<counter;i++)
	{
		check_sum += *addr_start;
		addr_start++;
	}

	return check_sum;
}

UINT32 max_three(UINT32 a,UINT32 b,UINT32 c)
{
	UINT32 max;
	
	max = (a > b)? a:b;
	max = (max > c)? max:c;
	return(max);
}

/*��������LONG���ݵ���Сֵ*/
UINT32 min_three(UINT32 a,UINT32 b,UINT32 c)
{
	UINT32 min;
	
	min = (a < b)? a:b;
	min = (min < c)? min:c;
	return(min);
}

#if 0
/* CRC ��λ�ֽ�ֵ�� */
const  UINT8 auchCRCHi[] = {
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40, 0x00, 0xC1, 0x81, 0x40,
0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0, 0x80, 0x41, 0x00, 0xC1,
0x81, 0x40, 0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41,
0x00, 0xC1, 0x81, 0x40, 0x01, 0xC0, 0x80, 0x41, 0x01, 0xC0,
0x80, 0x41, 0x00, 0xC1, 0x81, 0x40
};
/* CRC��λ�ֽ�ֵ��*/
const UINT8 auchCRCLo[] = {
0x00, 0xC0, 0xC1, 0x01, 0xC3, 0x03, 0x02, 0xC2, 0xC6, 0x06,
0x07, 0xC7, 0x05, 0xC5, 0xC4, 0x04, 0xCC, 0x0C, 0x0D, 0xCD,
0x0F, 0xCF, 0xCE, 0x0E, 0x0A, 0xCA, 0xCB, 0x0B, 0xC9, 0x09,
0x08, 0xC8, 0xD8, 0x18, 0x19, 0xD9, 0x1B, 0xDB, 0xDA, 0x1A,
0x1E, 0xDE, 0xDF, 0x1F, 0xDD, 0x1D, 0x1C, 0xDC, 0x14, 0xD4,
0xD5, 0x15, 0xD7, 0x17, 0x16, 0xD6, 0xD2, 0x12, 0x13, 0xD3,
0x11, 0xD1, 0xD0, 0x10, 0xF0, 0x30, 0x31, 0xF1, 0x33, 0xF3,
0xF2, 0x32, 0x36, 0xF6, 0xF7, 0x37, 0xF5, 0x35, 0x34, 0xF4,
0x3C, 0xFC, 0xFD, 0x3D, 0xFF, 0x3F, 0x3E, 0xFE, 0xFA, 0x3A,
0x3B, 0xFB, 0x39, 0xF9, 0xF8, 0x38, 0x28, 0xE8, 0xE9, 0x29,
0xEB, 0x2B, 0x2A, 0xEA, 0xEE, 0x2E, 0x2F, 0xEF, 0x2D, 0xED,
0xEC, 0x2C, 0xE4, 0x24, 0x25, 0xE5, 0x27, 0xE7, 0xE6, 0x26,
0x22, 0xE2, 0xE3, 0x23, 0xE1, 0x21, 0x20, 0xE0, 0xA0, 0x60,
0x61, 0xA1, 0x63, 0xA3, 0xA2, 0x62, 0x66, 0xA6, 0xA7, 0x67,
0xA5, 0x65, 0x64, 0xA4, 0x6C, 0xAC, 0xAD, 0x6D, 0xAF, 0x6F,
0x6E, 0xAE, 0xAA, 0x6A, 0x6B, 0xAB, 0x69, 0xA9, 0xA8, 0x68,
0x78, 0xB8, 0xB9, 0x79, 0xBB, 0x7B, 0x7A, 0xBA, 0xBE, 0x7E,
0x7F, 0xBF, 0x7D, 0xBD, 0xBC, 0x7C, 0xB4, 0x74, 0x75, 0xB5,
0x77, 0xB7, 0xB6, 0x76, 0x72, 0xB2, 0xB3, 0x73, 0xB1, 0x71,
0x70, 0xB0, 0x50, 0x90, 0x91, 0x51, 0x93, 0x53, 0x52, 0x92,
0x96, 0x56, 0x57, 0x97, 0x55, 0x95, 0x94, 0x54, 0x9C, 0x5C,
0x5D, 0x9D, 0x5F, 0x9F, 0x9E, 0x5E, 0x5A, 0x9A, 0x9B, 0x5B,
0x99, 0x59, 0x58, 0x98, 0x88, 0x48, 0x49, 0x89, 0x4B, 0x8B,
0x8A, 0x4A, 0x4E, 0x8E, 0x8F, 0x4F, 0x8D, 0x4D, 0x4C, 0x8C,
0x44, 0x84, 0x85, 0x45, 0x87, 0x47, 0x46, 0x86, 0x82, 0x42,
0x43, 0x83, 0x41, 0x81, 0x80, 0x40
} ;

UINT16  get_crc16(UINT16 len,const UINT8 * pBuffer)
{
	UINT8 uchCRCHi = 0xFF ; /* ��CRC�ֽڳ�ʼ�� */
	UINT8 uchCRCLo = 0xFF ;/* ��CRC �ֽڳ�ʼ�� */
	UINT8 uIndex; /* CRCѭ���е����� */
	while (len--) /* ������Ϣ������ */
	{
		uIndex = uchCRCHi ^ * (pBuffer++); /* ����CRC */
		uchCRCHi = uchCRCLo ^ auchCRCHi[uIndex];
		uchCRCLo = auchCRCLo[uIndex] ;
	}
	return (uchCRCLo << 8 | uchCRCHi) ; 
}


#else
UINT16  get_crc16(UINT8 * pBuffer,UINT16 len)
{
	INT16U i;
	INT8U j;
  	INT16U wCRC = 0xFFFF;

	for (i = 0;i < len;i++)
	{
		wCRC ^= pBuffer[i];
		for (j = 0;j < 8;j++)
		{
			if (wCRC & 0x0001)
			{wCRC = (wCRC >> 1) ^ 0xA001;}
			else
			{wCRC = wCRC >> 1;}
		}
	}

	return wCRC;
}   
#endif

/******************************************************************************
*  ������: void char_to_long(INT8U* buffer,LONG32U* value)
*
*  ����: �ַ�ת��Ϊ������
*			
*			
*
*  ����: 
*
*  ���: 
*
*  ����ֵ: 
*
*  ����: 
*******************************************************************************/
void char_to_long(INT8U* buffer,INT32U* value)
{
	LONG_UNION long_value;
	INT8U i;
	
	for(i=0;i<4;i++)
	{
		long_value.b[3 - i] = *(buffer + i);
	}
	*value = long_value.i;
}

/******************************************************************************
*  ������: void long_to_char(INT8U* buffer,LONG32S value)
*
*  ����: ��������ǰ���MODBUS�����ݸ�ʽ��һ��LONG��
*		   ת����CHAR�ͻ�������
*					
*  ����: buffer: 	����Ļ�����
*		   value: 	Ҫת������
*
*  ���: 
*
*  ����ֵ: 
*
*  ����: 
*******************************************************************************/
void long_to_char(INT8U* buffer,INT32S value)
{
	LONG_UNION long_value;
	INT8U i;
	
	long_value.i = value;
	
	for(i = 0; i < 4; i++)
	{
		*(buffer + i) = long_value.b[3 - i];	  
	}    
}

/******************************************************************************
*  ������: void char_to_long(INT8U* buffer,LONG32U* value)
*
*  ����: �ַ�ת��Ϊ����
*			
*			
*
*  ����: 
*
*  ���: 
*
*  ����ֵ: 
*
*  ����: 
*******************************************************************************/
void char_to_int(INT8U* buffer,INT16U* value)
{
	INTEGER_UNION int_value;

	int_value.b[1] = *(buffer);
	int_value.b[0] = *(buffer + 1);
	*value = int_value.i;
}

/******************************************************************************
*  ������: void int_to_char(INT8U *buffer, INT16U value)
*
*  ����: ��������ǰ���MODBUS�����ݸ�ʽ��һ��INT��
*		   ת����CHAR�ͻ�������
*					
*  ����: buffer: 	����Ļ�����
*		   value: 	Ҫת������
*
*  ���: 
*
*  ����ֵ: 
*
*  ����: 
*******************************************************************************/
void int_to_char(INT8U *buffer, INT16U value)
{
	*(buffer) = ((INT16U)value&0XFF00)>>8;
	*(buffer+1) = (value)&0x00FF;
}


UINT8 char_to_ascii(UINT8 x)
{
	UINT8 re_value = 0;

	assert_param(((x >= 0) && (x <= 9)) ||((x >= 0x0A) && (x <= 0x0F)));
	
	if ((x >= 0) && (x <= 9))
	{
		re_value = x+0x30;	// ���ּ���0x30����0��ascii��
	}
	else if ((x >= 0x0A) && (x <= 0x0F))
	{
		re_value =(x-0x0A)+65;	// A��ASCII����65
	}
	
	return re_value;
}

/*��һ��hexת����2��ascii�ַ�����0x4B-> 0x34, 0x42*/
 void hex2_to_ascii(UINT8 x, UINT8 * buffer)
{
	buffer[1] = (x>>4)&0x0F;
	buffer[1] = char_to_ascii(buffer[1]);
	
	buffer[0] = x&0x0F;
	buffer[0] = char_to_ascii(buffer[0]);
}

/*��һ��16λ��hexת����4��ascii�ַ�����0x4B-> 0x34, 0x42*/
 void hex4_to_ascii(UINT16 x, UINT8 * buffer)
{
	buffer[3] = (x>>12)&0x0F;
	buffer[3] = char_to_ascii(buffer[3]);

	buffer[2] = (x>>8)&0x0F;
	buffer[2] = char_to_ascii(buffer[2]);

	buffer[1] = (x>>4)&0x0F;
	buffer[1] = char_to_ascii(buffer[1]);
	
	buffer[0] = x&0x0F;
	buffer[0] = char_to_ascii(buffer[0]);
}

// �Ѽ���ASCII��ϳ�16����
UINT8 ascii_to_char(UINT8 x)
{
	UINT8 re_value = 0;

	assert_param(((x >= 0x30) && (x <= 0x39)) ||((x >= 65) && (x <= 70)) ||((x >= 97) && (x <= 102)));	// ascii��Ҫ��0~9,A~F,a~f֮��
	
	if ((x >= 0x30) && (x <= 0x39))
	{
		re_value = x-0x30;	// ���ּ���0x30����0��ascii��
	}
	else if ((x >= 65) && (x <= 70))
	{
		re_value =(x-65)+0x0A;	// A��ASCII����65
	}
	else if ((x >= 97) && (x <= 102))
	{
		re_value =(x-97)+0x0A;	// a��ASCII����97
	}
	re_value = re_value&0x0F;	// ��4λ��0
	
	return re_value;
}

/*��2��ascii�ַ�ת����һ��8λ��hex���� 0x34, 0x42->0x4B*/
// ����ʱ ��λ��ǰ,�Ӷ�buffer0Ӧ���Ǹ�λ
 UINT8 ascii_to_hex2(UINT8 * buffer)
{
	UINT8 re_value=0;
	const UINT8 cmp2[2] = {0x20,0x20};

	if ((memcmp(buffer, cmp2, 2)) == 0)
	{
		return 0xFF;		// ��ȫ��Ϊ�ո�ʱ,����0xFFFF
	}

	re_value = ascii_to_char(buffer[0]);
	re_value = (re_value<<4) |(ascii_to_char(buffer[1]));

	return re_value;
}

/*��4��ascii�ַ�ת����һ��16λ��hex���� 0x34, 0x42->0x4B*/
// ����ʱ ��λ��ǰ,�Ӷ�buffer0Ӧ���Ǹ�λ
 UINT16 ascii_to_hex4(UINT8 * buffer)
{
	UINT16 re_value = 0;
	const UINT8 cmp2[4] = {0x20,0x20,0x20,0x20};

	if ((memcmp(buffer, cmp2, 4)) == 0)
	{
		return 0xFFFF;		// ��ȫ��Ϊ�ո�ʱ,����0xFFFF
	}

	re_value = ascii_to_char(buffer[0]);
	re_value = (re_value<<4) |(ascii_to_char(buffer[1]));
	re_value = (re_value<<4) |(ascii_to_char(buffer[2]));
	re_value = (re_value<<4) |(ascii_to_char(buffer[3]));

	return re_value;
}


