#ifndef _PARAMS_H
#define _PARAMS_H

/********************�����ַ����,��8K������*********************************/
#ifdef HAS_8I8O
#define DEVICE_TYPE 		"LTKJ_I8O8"			/*װ���豸���͵Ķ���*/
#define DEV_ADDR			61
#else
#define DEVICE_TYPE 		"LTKJ_I0O12"			/*װ���豸���͵Ķ���*/
#define DEV_ADDR			71
#endif
#define SOFTWARE_VERSION 10000			/*װ�õ�����汾1.00.00*/
#define VERSION_DATE		 190728 			/*�汾����*/
#define PROTOCAL_REVISION 10

#define SYSTEM_SYNC		12341236		/*ͬ����־,ֻ��CPUƽ̨�仯�Ž����޸ģ�����Ҫ�ı�*/

#define DEVICE_PARAM_COUNTER 		sizeof(struct device_params_struct)	/*410���ֽ�*/

#define FM_FIRST_WORD    	0X04		/*�״��ϵ��־�ĵ�ַ+4*/
#define FM_DEVICE                	0X10
#define FM_DEVICE_END     	(FM_DEVICE+DEVICE_PARAM_COUNTER) 	/*+16*/

/*******************************************************************************************/

typedef struct device_params_struct	/*��16���ֽ�*/
{
	UINT16 Crc;				/*���ֽ��ۼӺ�*/
	UINT16 Number;				/*�ṹ����Ч����INT16*/

	UINT16 Address;
	UINT16 BaudRate_1;	// ����1������
	UINT16 BaudRate_2;	// ����2������
	UINT16 BaudRate_3;	// ����3������
	UINT16 BaudRate_4;	// ����4������

	UINT16 Pre_Remote;	//ң��Ԥ�ã�0���˳���1��Ͷ��
	UINT16 Do_status;		// DO��״̬ҲҪ���б��棬�ϵ��Ҫ��ס�ϴ�״̬
}DEVICE_PARAMS;

typedef  struct  _DeviceInfo_Struct      /*��������д����ʱ�߽��жϣ������ֹ32BIT��ֻд��16BIT*/
{
   UINT16 deviceType[20];
   UINT16 softVersion;
   UINT16 protocolVersion;
   UINT16 softYear;
   UINT16 softMonth;
   UINT16 softDate;
}DeviceInfoParams;

extern DEVICE_PARAMS DevParams;

extern DeviceInfoParams  DevicComInfor;
extern const DEVICE_PARAMS Init_DevParams;

void Write_DevParams(void);
void Init_Params(void);
void ComDeviceInfoInit(void);

#endif

