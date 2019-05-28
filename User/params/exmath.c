/**************************Copyright (c)****************************************************
 * 	
 * ------------------------文件信息---------------------------------------------------
 * 文件名:
 * 版本:
 * 描述:
 * 
 * --------------------------------------------------------------------------------------- 
 * 作者           :
 * 创建日期:
 * --------------------------------------------------------------------------------------- 
 * 修改人:
 * 修改日期:
 * 版本:
 * 描述:
 ****************************************************************************************/
 #include "include.h"

 UINT16 int_abs(INT16 int_value)
 {
     UINT16 value;
     value=(int_value>0?int_value:(-int_value));
     return(value);
 }
/****************************************************************
 函数名：bcd_to_hex()																	
 																								
 输入参数:无															
																									
 输出参数： UINT8 bcd_data															
																								
 返回值:		UINT8 hex															
																									
 函数说明:把bcd数据转换为16进制数 hex 
 **************************************************************/

UINT8 bcd_to_hex(UINT8 bcd_data)
{
	UINT8 hex;
	hex=(bcd_data/16)*10+bcd_data%16;
	return(hex);
}

/****************************************************************
 函数名：bcd_to_hex()																	
 																								
 输入参数:无															
																									
 输出参数： UINT8 bcd_data															
																								
 返回值:		UINT8 hex															
																									
 函数说明:把16进制数 hex转换为bcd数据 
 **************************************************************/
UINT8 hex_to_bcd(UINT8 hex_data)
{
	UINT8 bcd;
	bcd=(hex_data/10)*16+hex_data%10;
	return(bcd);
}

/*********************************************************
函数名:int32 lAbs(int32 num)	int16 Abs( int16 num ) fp32 fAbs( fp32 num )

输入参数:
	int32(int16,fp32) num--待求绝对值的变量

输出参数:	无

返回值:		
	int32(int16,fp32)--返回变量num的绝对值

功能说明:求输入变量的绝对值。
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
 * 函数名:	lsign 
 * 描述: 
 *            -求输入数的正负符号。
 * 
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:汪治国 
 * 创建日期:2006.4.10
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/
INT32 double_sign(double num)
{
	 return( num >0 ? 1l:-1l);
}

/*************************************************************
函数名: 	int32 lround()                         

输入参数:
	int32 div--被除数，长整形
	int32 by--除数，长整形

输出参数:	无

返回值:
	int32--计算结果

功能说明:求div/by的四舍五入的结果

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
	return(temp); /* 返回四舍五入的结果*/ 
} /* end lRound() */

/******************************************************************************
*  函数名:llround()
*
*  描述: 求div/by的四舍五入的结果
*
*  输入: 除数，被除数
*
*  输出: 
*
*  返回值:有符号长长整形 四舍五入的结果
*
*  其它: 
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
	return(temp); /* 返回四舍五入的结果*/ 
}

/******************************************************************************
*  函数名: long_sqrt()
*
*  描述: 长整形数开方处理
*
*  输入: 需要开方的无符号长整形数据
*
*  输出: 
*
*  返回值: 16位无符号整形数
*
*  其它: 
*******************************************************************************/
UINT16 long_sqrt(UINT32 M)
{ 
	UINT8 flag = 0; 
	UINT16 n, i; 
	UINT32 tmp, ttp;   /*   结果、循环计数	  */
	
	if (M == 0)               /* 被开方数，开方结果也为0  */
	{
		return 0; 
	}
	
	if (M > 0x3FF001)
	{
		M >>= 2;
		flag = 1;
	}

	n = 0; 
	tmp = (M >> 30);          /* 获取最高位：B[m-1] */
	M <<= 2; 
	
	if (tmp > 1)              /*  最高位为1 */
	{ 
		n++;                 /* 结果当前位为1，否则为默认的0 */
		tmp -= n; 
	} 

	for (i=15; i>0; i--)      /* 求剩余的15位 */
	{ 
		n <<= 1;              /* 左移一位 */
		tmp <<= 2; 
		tmp += (M >> 30);     /* 假设 */
		ttp = n; 
		ttp = (ttp << 1) + 1; 
		M <<= 2; 
		if (tmp >= ttp)       /* 假设成立 */
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
*  函数名: double_sqrt()
*
*  描述: 长整型开方，返回浮点值
*
*  输入: 需要开方的无符号长整形数据
*
*  输出: 
*
*  返回值: 浮点值
*
*  其它: 
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
*  函数名: scale_it()
*
*  描述: 两个乘积数与一个被除数的运算，防止溢出的一种
*			运算,这个函数的前提条件是商小于有符号长整形
*
*  输入: 两个乘积数与一个被除数
*
*  输出: 
*
*  返回值: (in * by ) / div
*
*  其它: 
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
** 函数名：		long_2_bcd(UINT32 x, UINT8 * buffer)
** 功能描述：	将长整形数转换为字符数组
** 输入：		UINT32 x  要转换的长整形数
                                   UINT8 * buffer    字符数组的指针
** 输出：		无	
** 全局变量：	
** 调用模块：
**
** 作者：何学军
** 日期：2007.1.27
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
 * 函数名:	bcd_2_long
 * 描述: 
 *            -把一个bcd码变为一长整型数字
 * 
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:何学军
 * 创建日期:2006.4.10
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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
 * 函数名:	bin_2_bcd16
 * 描述: 
 *            -把一个整型数据转换为BCD码.
 * 
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:汪治国 
 * 创建日期:2006.4.10
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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
 * 函数名:	bcd_2_bin16
 * 描述: 
 *            -把一个bcd码变为一整型数字
 * 
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:汪治国 
 * 创建日期:2006.4.10
 * 
 *------------------------
 * 修改人:
 * 修改日期:
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
*  函数名: get_check_sum()
*
*  描述: 本函数对传递过来的数组值进行累加和计算,
* 			返回累加和结果
*
*  输入: 累加对象的起始地址，个数
*
*  输出: 
*
*  返回值: 累加和结果
*
*  其它: 
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

/*计算三个LONG数据的最小值*/
UINT32 min_three(UINT32 a,UINT32 b,UINT32 c)
{
	UINT32 min;
	
	min = (a < b)? a:b;
	min = (min < c)? min:c;
	return(min);
}

#if 0
/* CRC 高位字节值表 */
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
/* CRC低位字节值表*/
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
	UINT8 uchCRCHi = 0xFF ; /* 高CRC字节初始化 */
	UINT8 uchCRCLo = 0xFF ;/* 低CRC 字节初始化 */
	UINT8 uIndex; /* CRC循环中的索引 */
	while (len--) /* 传输消息缓冲区 */
	{
		uIndex = uchCRCHi ^ * (pBuffer++); /* 计算CRC */
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
*  函数名: void char_to_long(INT8U* buffer,LONG32U* value)
*
*  描述: 字符转化为长整型
*			
*			
*
*  输入: 
*
*  输出: 
*
*  返回值: 
*
*  其它: 
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
*  函数名: void long_to_char(INT8U* buffer,LONG32S value)
*
*  描述: 这个函数是按照MODBUS的数据格式将一个LONG型
*		   转换到CHAR型缓冲区中
*					
*  输入: buffer: 	输出的缓冲区
*		   value: 	要转换的数
*
*  输出: 
*
*  返回值: 
*
*  其它: 
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
*  函数名: void char_to_long(INT8U* buffer,LONG32U* value)
*
*  描述: 字符转化为整型
*			
*			
*
*  输入: 
*
*  输出: 
*
*  返回值: 
*
*  其它: 
*******************************************************************************/
void char_to_int(INT8U* buffer,INT16U* value)
{
	INTEGER_UNION int_value;

	int_value.b[1] = *(buffer);
	int_value.b[0] = *(buffer + 1);
	*value = int_value.i;
}

/******************************************************************************
*  函数名: void int_to_char(INT8U *buffer, INT16U value)
*
*  描述: 这个函数是按照MODBUS的数据格式将一个INT型
*		   转换到CHAR型缓冲区中
*					
*  输入: buffer: 	输出的缓冲区
*		   value: 	要转换的数
*
*  输出: 
*
*  返回值: 
*
*  其它: 
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
		re_value = x+0x30;	// 数字加上0x30，即0的ascii码
	}
	else if ((x >= 0x0A) && (x <= 0x0F))
	{
		re_value =(x-0x0A)+65;	// A的ASCII码是65
	}
	
	return re_value;
}

/*将一个hex转换成2个ascii字符，如0x4B-> 0x34, 0x42*/
 void hex2_to_ascii(UINT8 x, UINT8 * buffer)
{
	buffer[1] = (x>>4)&0x0F;
	buffer[1] = char_to_ascii(buffer[1]);
	
	buffer[0] = x&0x0F;
	buffer[0] = char_to_ascii(buffer[0]);
}

/*将一个16位的hex转换成4个ascii字符，如0x4B-> 0x34, 0x42*/
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

// 把几个ASCII码合成16进制
UINT8 ascii_to_char(UINT8 x)
{
	UINT8 re_value = 0;

	assert_param(((x >= 0x30) && (x <= 0x39)) ||((x >= 65) && (x <= 70)) ||((x >= 97) && (x <= 102)));	// ascii码要在0~9,A~F,a~f之间
	
	if ((x >= 0x30) && (x <= 0x39))
	{
		re_value = x-0x30;	// 数字加上0x30，即0的ascii码
	}
	else if ((x >= 65) && (x <= 70))
	{
		re_value =(x-65)+0x0A;	// A的ASCII码是65
	}
	else if ((x >= 97) && (x <= 102))
	{
		re_value =(x-97)+0x0A;	// a的ASCII码是97
	}
	re_value = re_value&0x0F;	// 高4位清0
	
	return re_value;
}

/*将2个ascii字符转换成一个8位的hex，如 0x34, 0x42->0x4B*/
// 传输时 高位在前,从而buffer0应该是高位
 UINT8 ascii_to_hex2(UINT8 * buffer)
{
	UINT8 re_value=0;
	const UINT8 cmp2[2] = {0x20,0x20};

	if ((memcmp(buffer, cmp2, 2)) == 0)
	{
		return 0xFF;		// 当全部为空格时,返回0xFFFF
	}

	re_value = ascii_to_char(buffer[0]);
	re_value = (re_value<<4) |(ascii_to_char(buffer[1]));

	return re_value;
}

/*将4个ascii字符转换成一个16位的hex，如 0x34, 0x42->0x4B*/
// 传输时 高位在前,从而buffer0应该是高位
 UINT16 ascii_to_hex4(UINT8 * buffer)
{
	UINT16 re_value = 0;
	const UINT8 cmp2[4] = {0x20,0x20,0x20,0x20};

	if ((memcmp(buffer, cmp2, 4)) == 0)
	{
		return 0xFFFF;		// 当全部为空格时,返回0xFFFF
	}

	re_value = ascii_to_char(buffer[0]);
	re_value = (re_value<<4) |(ascii_to_char(buffer[1]));
	re_value = (re_value<<4) |(ascii_to_char(buffer[2]));
	re_value = (re_value<<4) |(ascii_to_char(buffer[3]));

	return re_value;
}


