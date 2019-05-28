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
#ifndef _MATH_H
#define _MATH_H

INT32 lRound(INT32 div,INT32 by);
INT32 lAbs(INT32 num);
INT16 Abs( INT16 num );

UINT8 bcd_to_hex(UINT8 bcd_data);

UINT8 hex_to_bcd(UINT8 hex_data);

UINT32 long_abs(INT32 num);

UINT16 int_abs(INT16 int_value);

double double_sqrt(UINT32 M);

INT32 double_sign(double num);

INT32 llround(INT64 div,INT32 by);

UINT8 BITS(INT8 bit);

INT32 scale_it(INT32 in,INT32 by,INT32 div);

void long_2_bcd(UINT32 x, UINT8 * buffer);

UINT32  bcd_2_long(UINT8 * buf);

 void bin_2_bcd16(UINT16 x, UINT8 * buffer);
 
  UINT16  bcd_2_bin16(UINT8 * buf);

 UINT16 bcd_to_bin2(UINT8 * buf);

  void bin_to_bcd2(UINT8 x, UINT8 * buffer);
  
 double double_fabs( double num );
 
 UINT16 get_check_sum(UINT8 *addr_start,UINT8 counter);

 UINT32 max_three(UINT32 a,UINT32 b,UINT32 c);

 UINT32 min_three(UINT32 a,UINT32 b,UINT32 c);

UINT16  get_crc16(INT8U * pBuffer,INT16U len);

 void char_to_long(INT8U* buffer,INT32U* value);

 void long_to_char(INT8U* buffer,INT32S value);

 void char_to_int(INT8U* buffer,INT16U* value);

 void int_to_char(INT8U *buffer, INT16U value);

 void hex2_to_ascii(UINT8 x, UINT8 * buffer);

 void hex4_to_ascii(UINT16 x, UINT8 * buffer);

UINT8 ascii_to_hex2(UINT8 * buffer);

UINT16 ascii_to_hex4(UINT8 * buffer);
 
#endif
