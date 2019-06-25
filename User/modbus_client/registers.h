/**************************Copyright (c)****************************************************
 * 								
 * 						
 * 	
 * ------------------------�ļ���Ϣ---------------------------------------------------
 * �ļ���:
 * ��	��:
 * ��	��: modbusЭ��ļĴ�������궨�壬�û��ӿ�
 * 
 * --------------------------------------------------------------------------------------- 
 * ��	��: 
 * ��	��: 
 * ��	��:
 * ��	��:
 ****************************************************************************************/

#ifndef	__REGISTERS_H
#define	__REGISTERS_H


/*�Ĵ������Զ���*/
#define READONLY						0		/*ֻ���Ĵ���*/
#define WRITEONLY					1		/*ֻд�Ĵ���*/
#define WRITEREAD					2		/*R/W�Ĵ���*/

/*�����Ĵ����Ĵ�����*/
#define REG_OK						0x00		/*��ȷ*/
#define FUNCCODE_ERR					0x01		/*��֧�ֵĹ�����*/
#define REGADDR_ERR					0x02		/*�Ĵ�����ַ�Ƿ�*/
#define DATA_ERR						0x03		/*���ݴ���*/
#define OPERATION_ERR				0x04		/*��Ч����*/


/*SPARK01װ�üĴ����ķ�Χ*/
/*ʵʱ���ݼĴ���*/
#define DATA_START_ADDR			0		/*���ݼĴ�����ʼ��ַ*/
#define DATA_REG_MAX			93		/*���汾��֧�ֵ����Ĵ�����*/

/*�������ݼĴ���*/
#define ENVI_START_ADDR			300		/*���ݼĴ�����ʼ��ַ*/
#define ENVI_REG_MAX				29		/*�ݶ�Ϊ29*/

/*װ����Ϣ�Ĵ���*/
#define DEVICEINFO_START_ADDR			900
#define DEVICEINFO_REG_MAX				23

/*װ�ò����Ĵ���*/
#define PARAMS_START_ADDR		1200		/*�豸�����Ĵ�����ʼ��ַ*/
#define PARAMS_REG_MAX			25			/*���汾��֧�ֵ����Ĵ�����*/ 

/*ң�ؼĴ���*/
#define DO_START_ADDR					1500
#define DO_REG_MAX						51
#define REMOTE_RESET_REG					1548		/*ң�ظ�λ*/
#define DOOR_OPEN_REG					1549		/*����������*/
#define DOOR_CLOSE_REG					1550		/*����������*/

#define ACTRUL_DO_NUM					4		/*ʵ��֧�ֵ�DO����*/


/*�Ĵ���������*/
typedef struct __Register__
{
	UINT16 *pAddr;				/*�Ĵ�����ַ*/
	UINT8 bAtrrib;				/*�Ĵ������Զ���RO,WO,RW*/
	UINT16 nLenth;				/*�Ĵ�������*/
}Reg_Object;

/*modubs�Ĵ����б��б���*/
typedef struct __Modbus_Map_Reg
{
	UINT16 nRegNo;				/*�Ĵ�����*/
	Reg_Object reg;				/*�Ĵ���������Ӧ�ļĴ���*/
} Map_Reg_Table;



/*�Ĵ�������*/
UINT8 Read_Register(UINT16 nStartRegNo, UINT16 nRegNum, UINT8 *pdatabuf, UINT8 *perr);
UINT8 Write_Register(UINT16 nStartRegNo, INT16 nRegNum, const UINT8 *pdatabuf, UINT8 datalen, UINT8 *perr);

extern UINT16  System_Reset;

#endif



