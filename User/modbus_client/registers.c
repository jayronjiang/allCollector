

/**************************Copyright (c)****************************************************
 * 								
 * 						
 * 	
 * ------------------------�ļ���Ϣ---------------------------------------------------
 * �ļ���:registers.c 
 * �汾:
 * ����:modbusЭ��ļĴ��������û��ӿ�
 * 
 * --------------------------------------------------------------------------------------- 
 * ����:
 * ��������:
 * --------------------------------------------------------------------------------------- 
 * �޸���:
 * �޸�����:
 * �汾:
 * ����:
 ****************************************************************************************/

#include "include.h"

UINT16  System_Reset = 0;   /*ϵͳ��λ*/

static UINT16 RegValue16 = 0; 


/*�Ĵ���������*/
const static Map_Reg_Table Reg_Table[] = 
{
	/*ʵʱ���ݼĴ���*/
	{DATA_START_ADDR+0, (UINT16*)&RSUParams.phase[0].vln, READONLY, 1},
	{DATA_START_ADDR+1, (UINT16*)&RSUParams.phase[0].amp, READONLY, 1},
	{DATA_START_ADDR+2, (UINT16*)&RSUParams.phase[1].vln, READONLY, 1},
	{DATA_START_ADDR+3, (UINT16*)&RSUParams.phase[1].amp, READONLY, 1},
	{DATA_START_ADDR+4, (UINT16*)&RSUParams.phase[2].vln, READONLY, 1},
	{DATA_START_ADDR+5, (UINT16*)&RSUParams.phase[2].amp, READONLY, 1},
	{DATA_START_ADDR+6, (UINT16*)&RSUParams.phase[3].vln, READONLY, 1},
	{DATA_START_ADDR+7, (UINT16*)&RSUParams.phase[3].amp, READONLY, 1},
	{DATA_START_ADDR+8, (UINT16*)&RSUParams.phase[4].vln, READONLY, 1},
	{DATA_START_ADDR+9, (UINT16*)&RSUParams.phase[4].amp, READONLY, 1},
	{DATA_START_ADDR+10, (UINT16*)&RSUParams.phase[5].vln, READONLY, 1},
	{DATA_START_ADDR+11, (UINT16*)&RSUParams.phase[5].amp, READONLY, 1},

	/*Ԥ��*/
	{DATA_START_ADDR+12, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+13, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+14, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+15, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+16, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+17, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+18, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+19, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+20, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+21, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+22, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+23, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+24, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+25, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+26, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+27, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+28, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+29, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+30, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+31, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+32, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+33, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+34, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+35, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+36, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+37, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+38, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+39, (UINT16*)&RegValue16, READONLY, 1},

	{DATA_START_ADDR+40, (UINT16*)&UPSParams.ups_in.phase_num, READONLY, 1},
	{DATA_START_ADDR+41, (UINT16*)&UPSParams.ups_in.freq, READONLY, 1},
	{DATA_START_ADDR+42, (UINT16*)&UPSParams.ups_in.volt_Ain, READONLY, 1},
	{DATA_START_ADDR+43, (UINT16*)&UPSParams.ups_in.volt_Bin, READONLY, 1},
	{DATA_START_ADDR+44, (UINT16*)&UPSParams.ups_in.volt_Cin, READONLY, 1},
	{DATA_START_ADDR+45, (UINT16*)&UPSParams.ups_in.amp_Ain, READONLY, 1},
	{DATA_START_ADDR+46, (UINT16*)&UPSParams.ups_in.amp_Bin, READONLY, 1},
	{DATA_START_ADDR+47, (UINT16*)&UPSParams.ups_in.amp_Cin, READONLY, 1},

	{DATA_START_ADDR+48, (UINT16*)&UPSParams.ups_out.phase_num, READONLY, 1},
	{DATA_START_ADDR+49, (UINT16*)&UPSParams.ups_out.freq, READONLY, 1},
	{DATA_START_ADDR+50, (UINT16*)&UPSParams.ups_out.volt_Aout, READONLY, 1},
	{DATA_START_ADDR+51, (UINT16*)&UPSParams.ups_out.volt_Bout, READONLY, 1},
	{DATA_START_ADDR+52, (UINT16*)&UPSParams.ups_out.volt_Cout, READONLY, 1},
	{DATA_START_ADDR+53, (UINT16*)&UPSParams.ups_out.amp_Aout, READONLY, 1},
	{DATA_START_ADDR+54, (UINT16*)&UPSParams.ups_out.amp_Bout, READONLY, 1},
	{DATA_START_ADDR+55, (UINT16*)&UPSParams.ups_out.amp_Cout, READONLY, 1},

	{DATA_START_ADDR+56, (UINT16*)&UPSParams.ups_out.kw_Aout, READONLY, 1},
	{DATA_START_ADDR+57, (UINT16*)&UPSParams.ups_out.kw_Bout, READONLY, 1},
	{DATA_START_ADDR+58, (UINT16*)&UPSParams.ups_out.kw_Cout, READONLY, 1},
	{DATA_START_ADDR+59, (UINT16*)&UPSParams.ups_out.kva_Aout, READONLY, 1},
	{DATA_START_ADDR+60, (UINT16*)&UPSParams.ups_out.kva_Bout, READONLY, 1},
	{DATA_START_ADDR+61, (UINT16*)&UPSParams.ups_out.kva_Cout, READONLY, 1},
	{DATA_START_ADDR+62, (UINT16*)&UPSParams.ups_out.load_Aout, READONLY, 1},
	{DATA_START_ADDR+63, (UINT16*)&UPSParams.ups_out.load_Bout, READONLY, 1},
	{DATA_START_ADDR+64, (UINT16*)&UPSParams.ups_out.load_Cout, READONLY, 1},

	{DATA_START_ADDR+65, (UINT16*)&UPSParams.battery.running_day, READONLY, 1},
	{DATA_START_ADDR+66, (UINT16*)&UPSParams.battery.battery_volt, READONLY, 1},
	{DATA_START_ADDR+67, (UINT16*)&UPSParams.battery.amp_charge, READONLY, 1},
	{DATA_START_ADDR+68, (UINT16*)&UPSParams.battery.battery_left, READONLY, 1},
	{DATA_START_ADDR+69, (UINT16*)&UPSParams.battery.battery_tmp, READONLY, 1},
	{DATA_START_ADDR+70, (UINT16*)&UPSParams.battery.battery_capacity, READONLY, 1},
	{DATA_START_ADDR+71, (UINT16*)&UPSParams.battery.battery_dischg_times, READONLY, 1},

	 // ״̬
	{DATA_START_ADDR+72, (UINT16*)&UPSParams.status.supply_in_status, READONLY, 1},
	{DATA_START_ADDR+73, (UINT16*)&UPSParams.status.supply_out_status, READONLY, 1},
	{DATA_START_ADDR+74, (UINT16*)&UPSParams.status.battery_status, READONLY, 1},

	 // �澯
	{DATA_START_ADDR+75, (UINT16*)&UPSParams.alarm.main_abnormal, READONLY, 1},
	{DATA_START_ADDR+76, (UINT16*)&UPSParams.alarm.system_overtemp, READONLY, 1},
	{DATA_START_ADDR+77, (UINT16*)&UPSParams.alarm.sysbat_low_prealarm, READONLY, 1},
	{DATA_START_ADDR+78, (UINT16*)&UPSParams.alarm.rectifier_overload, READONLY, 1},
	{DATA_START_ADDR+79, (UINT16*)&UPSParams.alarm.inverter_overload, READONLY, 1},
	{DATA_START_ADDR+80, (UINT16*)&UPSParams.alarm.bypass_abnomal, READONLY, 1},
	{DATA_START_ADDR+81, (UINT16*)&UPSParams.alarm.battery_low_prealarm, READONLY, 1},
	{DATA_START_ADDR+82, (UINT16*)&UPSParams.alarm.battery_abnomal, READONLY, 1},
	{DATA_START_ADDR+83, (UINT16*)&UPSParams.alarm.battery_overtemp, READONLY, 1},
	{DATA_START_ADDR+84, (UINT16*)&UPSParams.alarm.fan_abnormal, READONLY, 1},
	{DATA_START_ADDR+85, (UINT16*)&UPSParams.alarm.shutdown_alarm, READONLY, 1},
	{DATA_START_ADDR+86, (UINT16*)&UPSParams.alarm.maintain_status, READONLY, 1},
	{DATA_START_ADDR+87, (UINT16*)&UPSParams.alarm.inverter_supply, READONLY, 1},
	{DATA_START_ADDR+88, (UINT16*)&UPSParams.alarm.bypass_supply, READONLY, 1},
	{DATA_START_ADDR+89, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+90, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+91, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+92, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+93, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+94, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+95, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+96, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+97, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+98, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+99, (UINT16*)&RegValue16, READONLY, 1},
	{DATA_START_ADDR+100, (UINT16*)&RegValue16, READONLY, 1},

	// ������
	{DATA_START_ADDR+101, (UINT16*)&SPDParams[0].status, READONLY, 1},
	{DATA_START_ADDR+102, (UINT16*)&SPDParams[0].struck_times, READONLY, 1},
	{DATA_START_ADDR+103, (UINT16*)&SPDParams[1].status, READONLY, 1},
	{DATA_START_ADDR+104, (UINT16*)&SPDParams[1].struck_times, READONLY, 1},
	

	//��������
	{ENVI_START_ADDR+0, (UINT16*)&ENVIParms.temp, READONLY, 1},
	{ENVI_START_ADDR+1, (UINT16*)&ENVIParms.moist, READONLY, 1},
	{ENVI_START_ADDR+2, (UINT16*)&ENVIParms.water_flag, READONLY, 1},
	{ENVI_START_ADDR+3, (UINT16*)&ENVIParms.front_door_flag, READONLY, 1},
	{ENVI_START_ADDR+4, (UINT16*)&ENVIParms.back_door_flag, READONLY, 1},
	{ENVI_START_ADDR+5, (UINT16*)&ENVIParms.door_overtime, READONLY, 1},
	{ENVI_START_ADDR+6, (UINT16*)&ENVIParms.smoke_event_flag, READONLY, 1},
	{ENVI_START_ADDR+7, (UINT16*)&RegValue16, READONLY, 1},
	{ENVI_START_ADDR+8, (UINT16*)&RegValue16, READONLY, 1},
	{ENVI_START_ADDR+9, (UINT16*)&RegValue16, READONLY, 1},
	{ENVI_START_ADDR+10, (UINT16*)&DevParams.AirCondSet, READONLY, 1},
	{ENVI_START_ADDR+11, (UINT16*)&ENVIParms.air_cond_fan_in_status, READONLY, 1},
	{ENVI_START_ADDR+12, (UINT16*)&ENVIParms.air_cond_fan_out_status, READONLY, 1},
	{ENVI_START_ADDR+13, (UINT16*)&ENVIParms.air_cond_comp_status, READONLY, 1},
	{ENVI_START_ADDR+14, (UINT16*)&ENVIParms.air_cond_heater_status, READONLY, 1},
	{ENVI_START_ADDR+15, (UINT16*)&ENVIParms.air_cond_fan_emgy_status, READONLY, 1},
	
	{ENVI_START_ADDR+16, (UINT16*)&ENVIParms.air_cond_temp_in, READONLY, 1},
	{ENVI_START_ADDR+17, (UINT16*)&ENVIParms.air_cond_temp_out, READONLY, 1},
	{ENVI_START_ADDR+18, (UINT16*)&ENVIParms.air_cond_amp, READONLY, 1},
	{ENVI_START_ADDR+19, (UINT16*)&ENVIParms.air_cond_volt, READONLY, 1},
	
	{ENVI_START_ADDR+20, (UINT16*)&ENVIParms.air_cond_hightemp_alarm, READONLY, 1},
	{ENVI_START_ADDR+21, (UINT16*)&ENVIParms.air_cond_lowtemp_alarm, READONLY, 1},
	{ENVI_START_ADDR+22, (UINT16*)&ENVIParms.air_cond_highmoist_alarm, READONLY, 1},
	{ENVI_START_ADDR+23, (UINT16*)&ENVIParms.air_cond_lowmoist_alarm, READONLY, 1},
	{ENVI_START_ADDR+24, (UINT16*)&ENVIParms.air_cond_infan_alarm, READONLY, 1},
	{ENVI_START_ADDR+25, (UINT16*)&ENVIParms.air_cond_outfan_alarm, READONLY, 1},
	{ENVI_START_ADDR+26, (UINT16*)&ENVIParms.air_cond_comp_alarm, READONLY, 1},
	{ENVI_START_ADDR+27, (UINT16*)&ENVIParms.air_cond_heater_alarm, READONLY, 1},
	{ENVI_START_ADDR+28, (UINT16*)&ENVIParms.air_cond_emgyfan_alarm, READONLY, 1},
	


	/*װ����Ϣ�Ĵ���*/
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
	
	
	/*װ�ò����Ĵ���*/
	{PARAMS_START_ADDR+0, (UINT16*)&DevParams.Address, WRITEREAD, 1},
	{PARAMS_START_ADDR+1, (UINT16*)&DevParams.BaudRate_1, WRITEREAD, 1},
	{PARAMS_START_ADDR+2, (UINT16*)&DevParams.BaudRate_2, WRITEREAD, 1},
	{PARAMS_START_ADDR+3, (UINT16*)&DevParams.BaudRate_3, WRITEREAD, 1},
	{PARAMS_START_ADDR+4, (UINT16*)&DevParams.BaudRate_4, WRITEREAD, 1},
	{PARAMS_START_ADDR+5, (UINT16*)&DevParams.Pre_Remote, WRITEREAD, 1},
	{PARAMS_START_ADDR+6, (UINT16*)&DevParams.AutoSend, WRITEREAD, 1},

	{PARAMS_START_ADDR+7, (UINT16*)&RegValue16, WRITEREAD, 1}, 
	{PARAMS_START_ADDR+8, (UINT16*)&RegValue16, WRITEREAD, 1},
	{PARAMS_START_ADDR+9, (UINT16*)&RegValue16, WRITEREAD, 1},
	{PARAMS_START_ADDR+10, (UINT16*)&RegValue16, WRITEREAD, 1}, 
	{PARAMS_START_ADDR+11, (UINT16*)&RegValue16, WRITEREAD, 1}, 
	{PARAMS_START_ADDR+12, (UINT16*)&RegValue16, WRITEREAD, 1}, 
	{PARAMS_START_ADDR+13, (UINT16*)&RegValue16, WRITEREAD, 1},
	{PARAMS_START_ADDR+14, (UINT16*)&RegValue16, WRITEREAD, 1},
	{PARAMS_START_ADDR+15, (UINT16*)&RegValue16, WRITEREAD, 1}, 
	{PARAMS_START_ADDR+16, (UINT16*)&RegValue16, WRITEREAD, 1}, 
	{PARAMS_START_ADDR+17, (UINT16*)&RegValue16, WRITEREAD, 1},
	{PARAMS_START_ADDR+18, (UINT16*)&RegValue16, WRITEREAD, 1}, 
	{PARAMS_START_ADDR+19, (UINT16*)&RegValue16, WRITEREAD, 1}, 
	
	{PARAMS_START_ADDR+20, (UINT16*)&DevParams.AirCondSet, WRITEREAD, 1},
	{PARAMS_START_ADDR+21, (UINT16*)&DevParams.AirColdStartPoint, WRITEREAD, 1},
	{PARAMS_START_ADDR+22, (UINT16*)&DevParams.AirColdLoop, WRITEREAD, 1},
	{PARAMS_START_ADDR+23, (UINT16*)&DevParams.AirHotStartPoint, WRITEREAD, 1},
	{PARAMS_START_ADDR+24, (UINT16*)&DevParams.AirHotLoop, WRITEREAD, 1},
};

const INT32 Reg_max[] = {	
							/*ʵʱ���ݼĴ���*/
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,
							/*�������ݼĴ���*/
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,

							/*װ����Ϣ�Ĵ���*/
							0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,

							/*װ�ò����Ĵ���*/
							240,5,5,5,5,1,1,
							0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,
							0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,
							1,50,10,15,10
						};

const INT32 Reg_min[] = {	
							/*ʵʱ���ݼĴ���*/
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,
							/*�������ݼĴ���*/
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,

							/*װ����Ϣ�Ĵ���*/
							0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,

							/*װ�ò����Ĵ���*/
							1,0,0,0,0,0,0,
							0,0,0,0,0,0,0,
							0,0,0,0,0,0,
							0,15,1,-15,1
						};


#define REG_NUM  (sizeof(Reg_Table)/sizeof(Map_Reg_Table))


/******************************************************************************
 * ������:	CheckRegValue 
 * ����: 
 *           			 -�����д�Ĵ���ֵ�Ƿ񳬳���Χ������Χ����1��
 * �������: pReg:��д�Ĵ�����Ϣ value����д��ֵ index��Ҫд�ļĴ�������
 *
 * �������: ��
 *
 * ����ֵ: ��������1:�Ĵ���ֵ���ޣ�0:����
 * 
 * ��	��:		
 * ��������:
 * 
 *------------------------
 * �޸���:
 * �޸�����:
 ******************************************************************************/

UINT8 CheckRegValue( const Map_Reg_Table *pReg,INT32 value,UINT16 index )
{
	UINT16 reg_addr = pReg->nRegNo;
	//UINT8 i = 0;
	//UINT16 temp_value = 0;/*ר�������ж��������õ���ʱ����*/


	if(( reg_addr >= PARAMS_START_ADDR ) && ( reg_addr < (PARAMS_START_ADDR + PARAMS_REG_MAX)))
	{
		if( reg_addr == ( PARAMS_START_ADDR + 1 ))  /*����1������*/
		{
			/*�����д��ֵ�Ƿ������ֵ����Сֵ֮��*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM1_MODIFIED;   /*������ݺϸ�װ�ò����ı�*/
				system_flag|=DEV_MODIFIED;	/*һ��ı�*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 2 ))
		{
			/*�����д��ֵ�Ƿ������ֵ����Сֵ֮��*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM2_MODIFIED;   /*������ݺϸ�װ�ò����ı�*/
				system_flag|=DEV_MODIFIED;	/*һ��ı�*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 3 ))
		{
			/*�����д��ֵ�Ƿ������ֵ����Сֵ֮��*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM3_MODIFIED;   /*������ݺϸ�װ�ò����ı�*/
				system_flag|=DEV_MODIFIED;	/*һ��ı�*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 4 ))
		{
			/*�����д��ֵ�Ƿ������ֵ����Сֵ֮��*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM4_MODIFIED;   /*������ݺϸ�װ�ò����ı�*/
				system_flag|=DEV_MODIFIED;	/*һ��ı�*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 20 ))	// �յ�����
		{
			if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |=DEV_MODIFIED;	/*һ��ı�*/
				control_flag |= LBIT(DEV_PARAM_SET_FLAG_1);
				return 0;
			}
		}
		else if (( reg_addr == ( PARAMS_START_ADDR + 21 )) ||	( reg_addr == ( PARAMS_START_ADDR + 22 ))\
				|| ( reg_addr == ( PARAMS_START_ADDR + 24)))
		{
			if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |=DEV_MODIFIED;	/*һ��ı�*/
				control_flag |= LBIT(DEV_PARAM_SET_FLAG_2);
				return 0;
			}
		}
		
		else if ( reg_addr == ( PARAMS_START_ADDR + 23 ))
		{
			/*�и�����Ҫ��������*/
                        if(( (INT16)value <=*( Reg_max+index )) && ( (INT16)value>=*( Reg_min+index )))
			{
				system_flag|=DEV_MODIFIED;	/*һ��ı�*/
				return 0;
                        }
		}
		else
		{
			 if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag|=DEV_MODIFIED;	/*һ��ı�*/
				return 0;
			 }
		}
	}		
	return 1;
}

/********************************************************************************************************
** ��������    Search_Reg_Index
** ����������  ���ݼĴ�����Ų����ڼĴ����������в��Ҷ�Ӧ������
** ���룺		nRegNo--�Ĵ������
** �����		pReg--�Ĵ���ָ��
** ȫ�ֱ�����	
** ����ģ�飺
**
**
** �޸ģ�
** ���ڣ�
**
**˵�� :�Ĵ���������˳���㷨�޸�Ϊ���ַ������㷨�����ڼĴ���
**		  �Ƚ϶࣬����˳�����ʱ��ϳ�����˲��ö��ַ����ң�
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
		if( Reg_Table[mid].nRegNo == nRegNo )/*�ҵ�*/
		{
			*pReg = &Reg_Table[mid];     /*��������ֵ,*pReg���Ǹ�ָ��,��ָ�����Reg_Table[mid]�ĵ�ַ*/
			return  mid;  
		}
        	/*Ҫ�ҵĵ�ַ�����м�ֵ�����м�ֵ��ΪСֵ����Ѱ�ң�������Ϊ��ֵ*/
                else if( Reg_Table[mid].nRegNo< nRegNo ) 
		{
			low = mid + 1;    
		}
                else  
		{
			high = mid - 1; 
		}
 	}  
	
	pReg = NULL;      /*û�ҵ�*/
	return REG_NUM;   /*��������ַ��������ַ*/
}

/********************************************************************************************************
** ��������    Read_Register
** ����������  ��һ�����ݵļĴ������ݵ���������
** ���룺		nStartRegNo--�Ĵ�����ʼ���,nRegNum--��ȡ�ļĴ�������,
**				pdatabuf--���ݻ�������ַ,maxlen--��������󳤶�
** �����		perr--���������,UINT16--ʵ���Ѿ���ȡ�����ݳ���
** ȫ�ֱ�����	
** ����ģ�飺
**
** ���ߣ�
** ���ڣ�
**
** �޸ģ�Jerry
** ���ڣ�2013.03.12
**
**
**********************************************************************************************************/
UINT8 Read_Register(UINT16 nStartRegNo, UINT16 nRegNum, UINT8 *pdatabuf, UINT8 *perr)
{
	UINT16 i;
	const Map_Reg_Table *pCurReg = NULL;
	UINT16 *ptmpAddr = NULL;	/*yfy�޸�INT16ΪUINT16*/
	UINT16 nNeedReadNum;
	UINT16 nReadRegNo;
	UINT16 nReadLen = 0;
	UINT16 nEndRegNo;

	nEndRegNo = nStartRegNo + nRegNum - 1;
	/*����ļ����ж������ղ����Ǽ��Ĵ����Ƿ��ڷ�Χ��*/
	/*�����ݼĴ���--1s����һ��*/
	if((nStartRegNo >= DATA_START_ADDR) && (nEndRegNo < (DATA_START_ADDR + DATA_REG_MAX)))
	{
		
	}
	/*��װ�ò����Ĵ���*/
	else if((nStartRegNo >= PARAMS_START_ADDR) && (nEndRegNo < (PARAMS_START_ADDR + PARAMS_REG_MAX )))
	{
		
	}
	
	/*��װ����Ϣ�Ĵ���*/
	else if((nStartRegNo >= DEVICEINFO_START_ADDR) && (nEndRegNo < (DEVICEINFO_START_ADDR + DEVICEINFO_REG_MAX)))
	{
		
	}

	/*�������Ĵ���*/
	else if(( nStartRegNo >= ENVI_START_ADDR) && (nEndRegNo < ENVI_START_ADDR + ENVI_REG_MAX))
	{
		
	}
	
	else 
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	nNeedReadNum = nRegNum;
	nReadRegNo = nStartRegNo;
	
	/*Ѱ�ҿ�ʼ��ַ������*/
	Search_Reg_Index(nReadRegNo, &pCurReg);	/*pCurReg�ҳ����ǿ�ʼ���ĵ�ַ,����һ����ַ*/
	if (pCurReg == NULL)	/*pCurReg��һ��ָ��,����ȡ���ַ,��ָ��ָ���ָ��,������&pCurReg����Ϊ�βδ���,�Ǵ����*/
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	while(nNeedReadNum > 0)
	{
		/*���ļĴ������ȴ���*/
		if (nNeedReadNum < pCurReg->reg.nLenth)
		{
			*perr = DATA_ERR;
			return(0);
		}
		
		if ((pCurReg->reg.bAtrrib!=READONLY) && (pCurReg->reg.bAtrrib!=WRITEREAD))
		{
			*perr = REGADDR_ERR;/*�üĴ������ܶ�*/
			return(0);
		}
		
		ptmpAddr = pCurReg->reg.pAddr;	/*ȡ��ָ��λ�õļĴ�������,ע��ptmpAddr��ָ��,��ָ�����ʵ�ʵļĴ���*/
		
		if (ptmpAddr == NULL)	/*���Ĵ���Ϊ����ʱ�����޷��ҵ�������NULL*/
		{
			for (i=0; i<pCurReg->reg.nLenth; i++)
			{
				pdatabuf[nReadLen++] = 0x00;
				pdatabuf[nReadLen++] = 0x00;	/*�����Ĵ���,����0X0000*/
			}
		}
		else
		{
			for(i=0; i<pCurReg->reg.nLenth; i++)
			{
				INT16 rccode = 0x0000;
				
				if (ptmpAddr != NULL)
				{
					rccode = *(ptmpAddr + pCurReg->reg.nLenth - i - 1);	/*��ʾ�ӼĴ����ĸ�λ��ʼ��*/
				}
								
				pdatabuf[nReadLen++]  = (UINT8)((rccode>>8)&0x00FF);/*��λ*/
				pdatabuf[nReadLen++]  = (UINT8)(rccode&0x00FF);		/*��λ*/
			}
		}
		
		nNeedReadNum -= pCurReg->reg.nLenth;	/*������Ҫ��ȡ�ļĴ�����Ŀ*/
		pCurReg++;		/*����һ������*/
	}
	
	*perr = REG_OK;
	return nReadLen;
}

/********************************************************************************************************
** ��������    Write_Register
** ����������  д�Ĵ���
** ���룺		nStartRegNo--�Ĵ�����ʼ���,nRegNum--��ȡ�ļĴ�������,
**				pdatabuf--д������ݻ�������ַ,datalen--����������
** �����		perr--���������,
** ȫ�ֱ�����	
** ����ģ�飺
**
** �޸ģ�
** ���ڣ�
**
**********************************************************************************************************/
UINT8 Write_Register( UINT16 nStartRegNo, INT16 nRegNum, const UINT8 *pdatabuf, UINT8 datalen, UINT8 *perr )
{
	const Map_Reg_Table *pCurReg, *pTmpReg;	          /*����ָ��*/
	UINT16 *ptmpAddr = NULL;			  /*yfy�޸�INT16ΪUINT16*/
	const UINT8 *temp_pdatabuf =NULL;		  /*������ջ�������ָ���ַ*/
	UINT16 nNeedWriteNum;				  /*��Ҫд�ļĴ�������*/
	UINT16 nWriteRegNo;				  /*Reg���*/
	UINT16 nEndRegNo;
	INT32 temp_value=0;		                  /*�洢Ҫд��Ĵ�����ֵ*/
	UINT16 temp_index;			          /*����Reg_TableԪ���±�*/
	
	nEndRegNo = nStartRegNo + nRegNum - 1;
	/*дװ�ò����Ĵ���*/
	if(( nStartRegNo >= PARAMS_START_ADDR) && (nEndRegNo < (PARAMS_START_ADDR + PARAMS_REG_MAX )))
	{
	
	}
	else 
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	
	/*��ָ����г�ʼ��*/
	pCurReg = NULL;
	pTmpReg = NULL;
	
	nNeedWriteNum = nRegNum;
	nWriteRegNo = nStartRegNo;
	
	temp_index = Search_Reg_Index(nWriteRegNo, &pCurReg); /*�ҵ�ַ����*/

	if (pCurReg == NULL)
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	pTmpReg = pCurReg;
	
	/*��֤��д�����ݲ���ֻд��һ�����ֽ�,��û��д���ֽڻ���ֻд�˱�����Ϊһ����������ݲ�������(eg:��ֵ)*/
        while( nNeedWriteNum > 0)
	{
		if ( nNeedWriteNum < pTmpReg->reg.nLenth )/*������Ҫд��������ȵ�ǰ�Ĵ�����ռ������С�ͱ���*/
		{
			*perr = DATA_ERR;
			return(0);
		}
		
		nNeedWriteNum -= pTmpReg->reg.nLenth;	
		pTmpReg++;
	}
	
	/*��ʼд�����ݷ�Χ���*/
	nNeedWriteNum = nRegNum;
	nWriteRegNo = nStartRegNo;

	pTmpReg = pCurReg;		/*����ָ�룬����д��ļĴ�����Χ�ж�*/
	temp_pdatabuf = pdatabuf;       /*���滺������ַ������д��ļĴ�����Χ�ж�*/
	
        /*��д�ļĴ������м�飬������д�ĵ�ַ���з��࣬����Ӧ�ı�־λ*/
	while( nNeedWriteNum > 0 )
	{

		ptmpAddr = pTmpReg->reg.pAddr;  
		/*ֻ�е��Ĵ����Ѷ��壬�ҿ�д��ʱ���ȡд�Ĵ�����ֵ,����ֱ���������Ĵ���*/
		if ( ptmpAddr != (UINT16*)0 && (( pTmpReg->reg.bAtrrib == WRITEONLY ) || ( pTmpReg->reg.bAtrrib == WRITEREAD ))) 
		{	
			if( pTmpReg->reg.nLenth == 1 )
			{
				temp_value= (INT32)((UINT16)temp_pdatabuf[0]<<8 | temp_pdatabuf[1]);
				if( CheckRegValue( pTmpReg,temp_value,temp_index ))//ע������һ��ֻ���һ���Ĵ���
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
					temp_value= ( temp_value &0x0000FFFF );//yfy���
					temp_value = temp_value<<16;
					temp_value += ( (UINT16)temp_pdatabuf[2]<<8 | temp_pdatabuf[3] );
					if( CheckRegValue( pTmpReg,temp_value,temp_index ))//yfy�޸�2010.10.11
					{			
						*perr = DATA_ERR;
						return(0);
					}
				ptmpAddr ++;
				ptmpAddr ++;
				temp_pdatabuf += 4;
			}
			temp_index++;/*�Ĵ�����Χ�����Ϻ�׼�����дһ���Ĵ����������±�Ҫ��1yfy 2010.10.11*/
		}
		else
		{
			*perr = REGADDR_ERR;
			return(0);
		}
		
		nNeedWriteNum -= pTmpReg->reg.nLenth;
		
		pTmpReg++;	/*��һ�������ļĴ���*/
		if ( pTmpReg == NULL )
		{
			*perr = DATA_ERR;
			return(0);
		}
	}

	
	/*ǰ�����ݼ�������ʼ�ж�д�Ĵ���*/
	nNeedWriteNum = nRegNum;
	nWriteRegNo = nStartRegNo;


	while(nNeedWriteNum > 0)
	{
		ptmpAddr = pCurReg->reg.pAddr;  
		/*ֻ�е��Ĵ�����д��ʱ���ȡд�Ĵ�����ֵ,����ֱ���������Ĵ���*/
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
				/*ǰ����Ĵ�����Χ��ȷ�������д��*/
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
		pCurReg++;	/*д��һ�������ļĴ���*/
		
                /*�Ĵ���δ����*/
                if ( pCurReg == NULL )
		{
			*perr = DATA_ERR;
			return(0);
		}
	}	
	*perr = REG_OK;	
	return(datalen);
}


