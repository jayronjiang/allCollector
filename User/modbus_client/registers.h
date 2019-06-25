/**************************Copyright (c)****************************************************
 * 								
 * 						
 * 	
 * ------------------------文件信息---------------------------------------------------
 * 文件名:
 * 版	本:
 * 描	述: modbus协议的寄存器处理宏定义，用户接口
 * 
 * --------------------------------------------------------------------------------------- 
 * 作	者: 
 * 日	期: 
 * 版	本:
 * 描	述:
 ****************************************************************************************/

#ifndef	__REGISTERS_H
#define	__REGISTERS_H


/*寄存器属性定义*/
#define READONLY						0		/*只读寄存器*/
#define WRITEONLY					1		/*只写寄存器*/
#define WRITEREAD					2		/*R/W寄存器*/

/*操作寄存器的错误码*/
#define REG_OK						0x00		/*正确*/
#define FUNCCODE_ERR					0x01		/*不支持的功能码*/
#define REGADDR_ERR					0x02		/*寄存器地址非法*/
#define DATA_ERR						0x03		/*数据错误*/
#define OPERATION_ERR				0x04		/*无效操作*/


/*SPARK01装置寄存器的范围*/
/*实时数据寄存器*/
#define DATA_START_ADDR			0		/*数据寄存器起始地址*/
#define DATA_REG_MAX			93		/*本版本所支持的最大寄存器数*/

/*环境数据寄存器*/
#define ENVI_START_ADDR			300		/*数据寄存器起始地址*/
#define ENVI_REG_MAX				29		/*暂定为29*/

/*装置信息寄存器*/
#define DEVICEINFO_START_ADDR			900
#define DEVICEINFO_REG_MAX				23

/*装置参数寄存器*/
#define PARAMS_START_ADDR		1200		/*设备参数寄存器开始地址*/
#define PARAMS_REG_MAX			25			/*本版本所支持的最大寄存器数*/ 

/*遥控寄存器*/
#define DO_START_ADDR					1500
#define DO_REG_MAX						51
#define REMOTE_RESET_REG					1548		/*遥控复位*/
#define DOOR_OPEN_REG					1549		/*电子门锁开*/
#define DOOR_CLOSE_REG					1550		/*电子门锁关*/

#define ACTRUL_DO_NUM					4		/*实际支持的DO数量*/


/*寄存器对象定义*/
typedef struct __Register__
{
	UINT16 *pAddr;				/*寄存器地址*/
	UINT8 bAtrrib;				/*寄存器属性定义RO,WO,RW*/
	UINT16 nLenth;				/*寄存器长度*/
}Reg_Object;

/*modubs寄存器列表列表定义*/
typedef struct __Modbus_Map_Reg
{
	UINT16 nRegNo;				/*寄存器号*/
	Reg_Object reg;				/*寄存器号所对应的寄存器*/
} Map_Reg_Table;



/*寄存器操作*/
UINT8 Read_Register(UINT16 nStartRegNo, UINT16 nRegNum, UINT8 *pdatabuf, UINT8 *perr);
UINT8 Write_Register(UINT16 nStartRegNo, INT16 nRegNum, const UINT8 *pdatabuf, UINT8 datalen, UINT8 *perr);

extern UINT16  System_Reset;

#endif



