#ifndef _PARAMS_H
#define _PARAMS_H

/********************�����ַ����,��8K������*********************************/

#define DEVICE_TYPE 		"SPARK01"		/*װ���豸���͵Ķ���*/
#define SOFTWARE_VERSION 10000			/*װ�õ�����汾1.00.00*/
#define VERSION_DATE		 190510 			/*�汾����*/
#define PROTOCAL_REVISION 10

#define SYSTEM_SYNC		12341236		/*ͬ����־,ֻ��CPUƽ̨�仯�Ž����޸ģ�����Ҫ�ı�*/

#define DEVICE_PARAM_COUNTER 		sizeof(struct device_params_struct)	/*410���ֽ�*/

#define DATA_BLOCK_SIZE  	0X450		/*Ԥ��1104���ֽ�*/

#define FM_FIRST_WORD    	0X04		/*�״��ϵ��־�ĵ�ַ*/
#define FM_DEVICE                	0X10
#define FM_DEVICE_END     	(FM_DEVICE+DEVICE_PARAM_COUNTER) 	/*+410,��576*/

#define RSU_NUM		6	// RSU������Ŀ

#define DOOR_TIME_OUT	10	// 10s

/*******************************************************************************************/
typedef struct phase_struct
{
	UINT16 vln;
	UINT16 amp;
	// Ͷ�б�־
	UINT16 enable;
}RSU_PHASE_PARAMS;


typedef struct rsu_struct
{
	RSU_PHASE_PARAMS phase[RSU_NUM];		// ��ʱΪ6·RSU
}RSU_PARAMS;

// USP�ӽṹ��--����
typedef struct ups_in_struct
{
	UINT16 phase_num;	// ����
	UINT16 volt_Ain;		//������42
	UINT16 volt_Bin;
	UINT16 volt_Cin;
	// ������E0
	UINT16 amp_Ain;
	UINT16 amp_Bin;
	UINT16 amp_Cin;
	UINT16 freq;			// Ƶ��
	UINT16 fact_Ain;		// ��������
	UINT16 fact_Bin;		// ��������
	UINT16 fact_Cin;		// ��������

	UINT16 bypass_voltA;	// ��·��ѹA
	UINT16 bypass_voltB;	// ��·��ѹB
	UINT16 bypass_voltC;	// ��·��ѹC
	UINT16 bypass_freq;	// ��·Ƶ��
}UPS_IN_PARAMS;


// USP�ӽṹ��--���
typedef struct ups_out_struct
{
	// ������42
	UINT16 volt_Aout;
	UINT16 volt_Bout;
	UINT16 volt_Cout;
	UINT16 amp_Aout;
	UINT16 amp_Bout;
	UINT16 amp_Cout;
	UINT16 freq;

	// ������E1
	UINT16 phase_num;	// ����
	UINT16 fact_Aout;
	UINT16 fact_Bout;
	UINT16 fact_Cout;
	UINT16 kw_Aout;		// �й�
	UINT16 kw_Bout;		// �й�
	UINT16 kw_Cout;		// �й�
	UINT16 kva_Aout;		// ����
	UINT16 kva_Bout;
	UINT16 kva_Cout;	

	UINT16 load_Aout;		// ����
	UINT16 load_Bout;		// ����
	UINT16 load_Cout;		// ����
}UPS_OUT_PARAMS;

// USP�ӽṹ��--���
typedef struct ups_bat_struct
{
	// ������E3
	UINT16 running_day;			// UPS����ʱ��,�������ʱ����E4
	UINT16 battery_volt;
	UINT16 amp_charge;
	UINT16 amp_discharge;
	UINT16 battery_left;
	UINT16 battery_tmp;	// �����¶�
	UINT16 battery_capacity;
	UINT16 battery_dischg_times;
	//UINT32 battery_running_time;	//�������ʱ����E4,��֧��
}UPS_BAT_PARAMS;


// USP�ӽṹ��--״̬
typedef struct ups_status_struct
{
	// ������43	
	UINT16 supply_out_status;		// �������״̬
	UINT16 supply_in_status;		// ���빩��״̬
	UINT16 battery_status;			// UPS����ʱ��,�������ʱ����E4
}UPS_STATUS_PARAMS;


// USP�ṹ��
typedef struct ups_struct
{
	UPS_IN_PARAMS ups_in;	// ��������
	UPS_OUT_PARAMS ups_out;	// �������
	UPS_BAT_PARAMS  battery;
	UPS_STATUS_PARAMS status;
}UPS_PARAMS;



// �������ṹ��
typedef struct spd_struct
{
	UINT16 status;	// ״̬
	UINT16 DI_status;	// �ӵص���
	UINT16 struck_times;	// �׻�����
}SPD_PARAMS;


// �������ݽṹ��
typedef struct envi_struct
{
	UINT16 temp;	// �¶�
	UINT16 moist;	// ʪ��
	UINT16 water_flag;	// ©ˮ
	UINT16 door_flag;
	UINT16 door_overtime;
	UINT16 fire_move_flag;
	UINT16 smoke_event_flag;
	UINT16 air_cond_status;
	UINT16 air_cond_temp_out;
	UINT16 air_cond_temp_in;
	UINT16 air_cond_hightemp_alarm;
	UINT16 air_cond_lowtemp_alarm;
}ENVI_PARAMS;

typedef struct device_params_struct	/*��384���ֽ�*/
{
	UINT16 Crc;				/*���ֽ��ۼӺ�*/
	UINT16 Number;				/*�ṹ����Ч����INT16*/

	UINT16 Address;
	UINT16 BaudRate_1;	// ����1������
	UINT16 BaudRate_2;	// ����2������
	UINT16 BaudRate_3;	// ����3������
	UINT16 BaudRate_4;	// ����4������

	UINT16 Pre_Remote;		/*ң��Ԥ�ã�0���˳���1��Ͷ��*/
	UINT16 AutoSend;		/*�Զ��ϴ��Ƿ�Ͷ��*/
	UINT16 AirCondSet;		/*�յ������ػ�*/
	UINT16 AirColdStartPoint;		/*�յ������*/
	UINT16 AirColdLoop;		/*�յ�����ز�*/
	UINT16 AirHotStartPoint;		/*�յ������*/
	UINT16 AirHotLoop;		/*�յ�����ز�*/

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
extern RSU_PARAMS RSUParams;
extern UPS_PARAMS UPSParams;
extern SPD_PARAMS SPDParams;
extern ENVI_PARAMS ENVIParms;
extern DeviceInfoParams  DevicComInfor;
extern const DEVICE_PARAMS Init_DevParams;

void Write_DevParams(void);
void Init_Params(void);
void ComDeviceInfoInit(void);

#endif

