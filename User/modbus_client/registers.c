

/**************************Copyright (c)****************************************************
 * 								
 * 						
 * 	
 * ------------------------文件信息---------------------------------------------------
 * 文件名:registers.c 
 * 版本:
 * 描述:modbus协议的寄存器处理，用户接口
 * 
 * --------------------------------------------------------------------------------------- 
 * 作者:
 * 创建日期:
 * --------------------------------------------------------------------------------------- 
 * 修改人:
 * 修改日期:
 * 版本:
 * 描述:
 ****************************************************************************************/

#include "include.h"

UINT16  System_Reset = 0;   /*系统复位*/

static UINT16 RegValue16 = 0; 


/*寄存器索引表*/
const static Map_Reg_Table Reg_Table[] = 
{
	/*实时数据寄存器*/
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

	/*预留*/
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

	 // 状态
	{DATA_START_ADDR+72, (UINT16*)&UPSParams.status.supply_in_status, READONLY, 1},
	{DATA_START_ADDR+73, (UINT16*)&UPSParams.status.supply_out_status, READONLY, 1},
	{DATA_START_ADDR+74, (UINT16*)&UPSParams.status.battery_status, READONLY, 1},

	 // 告警
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

	// 防雷器
	{DATA_START_ADDR+101, (UINT16*)&SPDParams[0].status, READONLY, 1},
	{DATA_START_ADDR+102, (UINT16*)&SPDParams[0].struck_times, READONLY, 1},
	{DATA_START_ADDR+103, (UINT16*)&SPDParams[1].status, READONLY, 1},
	{DATA_START_ADDR+104, (UINT16*)&SPDParams[1].struck_times, READONLY, 1},
	

	//环境数据
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
	


	/*装置信息寄存器*/
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
	
	
	/*装置参数寄存器*/
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
							/*实时数据寄存器*/
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,
							/*环境数据寄存器*/
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,

							/*装置信息寄存器*/
							0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,

							/*装置参数寄存器*/
							240,5,5,5,5,1,1,
							0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,
							0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,0XFFFF,
							1,50,10,15,10
						};

const INT32 Reg_min[] = {	
							/*实时数据寄存器*/
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,
							/*环境数据寄存器*/
							0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,

							/*装置信息寄存器*/
							0,0,0,0,0,0,0,0,0,0,0,0,  0,0,0,0,0,0,0,0,0,0,0,

							/*装置参数寄存器*/
							1,0,0,0,0,0,0,
							0,0,0,0,0,0,0,
							0,0,0,0,0,0,
							0,15,1,-15,1
						};


#define REG_NUM  (sizeof(Reg_Table)/sizeof(Map_Reg_Table))


/******************************************************************************
 * 函数名:	CheckRegValue 
 * 描述: 
 *           			 -检查所写寄存器值是否超出范围，超范围返回1。
 * 输入参数: pReg:所写寄存器信息 value：所写的值 index：要写的寄存器索引
 *
 * 输出参数: 无
 *
 * 返回值: 检查情况，1:寄存器值超限，0:正常
 * 
 * 作	者:		
 * 创建日期:
 * 
 *------------------------
 * 修改人:
 * 修改日期:
 ******************************************************************************/

UINT8 CheckRegValue( const Map_Reg_Table *pReg,INT32 value,UINT16 index )
{
	UINT16 reg_addr = pReg->nRegNo;
	//UINT8 i = 0;
	//UINT16 temp_value = 0;/*专门用来判断输入配置的临时变量*/


	if(( reg_addr >= PARAMS_START_ADDR ) && ( reg_addr < (PARAMS_START_ADDR + PARAMS_REG_MAX)))
	{
		if( reg_addr == ( PARAMS_START_ADDR + 1 ))  /*串口1波特率*/
		{
			/*检查所写的值是否在最大值和最小值之间*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM1_MODIFIED;   /*检查数据合格，装置参数改变*/
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 2 ))
		{
			/*检查所写的值是否在最大值和最小值之间*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM2_MODIFIED;   /*检查数据合格，装置参数改变*/
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 3 ))
		{
			/*检查所写的值是否在最大值和最小值之间*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM3_MODIFIED;   /*检查数据合格，装置参数改变*/
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 4 ))
		{
			/*检查所写的值是否在最大值和最小值之间*/
                        if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |= COMM4_MODIFIED;   /*检查数据合格，装置参数改变*/
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
                        }
		}
		else if ( reg_addr == ( PARAMS_START_ADDR + 20 ))	// 空调开关
		{
			if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |=DEV_MODIFIED;	/*一起改变*/
				control_flag |= LBIT(DEV_PARAM_SET_FLAG_1);
				return 0;
			}
		}
		else if (( reg_addr == ( PARAMS_START_ADDR + 21 )) ||	( reg_addr == ( PARAMS_START_ADDR + 22 ))\
				|| ( reg_addr == ( PARAMS_START_ADDR + 24)))
		{
			if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag |=DEV_MODIFIED;	/*一起改变*/
				control_flag |= LBIT(DEV_PARAM_SET_FLAG_2);
				return 0;
			}
		}
		
		else if ( reg_addr == ( PARAMS_START_ADDR + 23 ))
		{
			/*有负数，要单独处理*/
                        if(( (INT16)value <=*( Reg_max+index )) && ( (INT16)value>=*( Reg_min+index )))
			{
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
                        }
		}
		else
		{
			 if(( value <=*( Reg_max+index )) && ( value>=*( Reg_min+index )))
			{
				system_flag|=DEV_MODIFIED;	/*一起改变*/
				return 0;
			 }
		}
	}		
	return 1;
}

/********************************************************************************************************
** 函数名：    Search_Reg_Index
** 功能描述：  根据寄存器编号查找在寄存器索引表中查找对应的索引
** 输入：		nRegNo--寄存器编号
** 输出：		pReg--寄存器指针
** 全局变量：	
** 调用模块：
**
**
** 修改：
** 日期：
**
**说明 :寄存器查找由顺序算法修改为二分法查找算法，由于寄存器
**		  比较多，采用顺序查找时间较长，因此采用二分法查找；
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
		if( Reg_Table[mid].nRegNo == nRegNo )/*找到*/
		{
			*pReg = &Reg_Table[mid];     /*返回索引值,*pReg还是个指针,它指向的是Reg_Table[mid]的地址*/
			return  mid;  
		}
        	/*要找的地址大于中间值，将中间值作为小值继续寻找，否则作为大值*/
                else if( Reg_Table[mid].nRegNo< nRegNo ) 
		{
			low = mid + 1;    
		}
                else  
		{
			high = mid - 1; 
		}
 	}  
	
	pReg = NULL;      /*没找到*/
	return REG_NUM;   /*将索引地址超出点表地址*/
}

/********************************************************************************************************
** 函数名：    Read_Register
** 功能描述：  读一定数据的寄存器内容到缓冲区中
** 输入：		nStartRegNo--寄存器开始编号,nRegNum--读取的寄存器个数,
**				pdatabuf--数据缓冲区地址,maxlen--缓冲区最大长度
** 输出：		perr--操作结果码,UINT16--实际已经读取的数据长度
** 全局变量：	
** 调用模块：
**
** 作者：
** 日期：
**
** 修改：Jerry
** 日期：2013.03.12
**
**
**********************************************************************************************************/
UINT8 Read_Register(UINT16 nStartRegNo, UINT16 nRegNum, UINT8 *pdatabuf, UINT8 *perr)
{
	UINT16 i;
	const Map_Reg_Table *pCurReg = NULL;
	UINT16 *ptmpAddr = NULL;	/*yfy修改INT16为UINT16*/
	UINT16 nNeedReadNum;
	UINT16 nReadRegNo;
	UINT16 nReadLen = 0;
	UINT16 nEndRegNo;

	nEndRegNo = nStartRegNo + nRegNum - 1;
	/*下面的几个判断语句与空操作是检查寄存器是否在范围内*/
	/*读数据寄存器--1s更新一次*/
	if((nStartRegNo >= DATA_START_ADDR) && (nEndRegNo < (DATA_START_ADDR + DATA_REG_MAX)))
	{
		
	}
	/*读装置参数寄存器*/
	else if((nStartRegNo >= PARAMS_START_ADDR) && (nEndRegNo < (PARAMS_START_ADDR + PARAMS_REG_MAX )))
	{
		
	}
	
	/*读装置信息寄存器*/
	else if((nStartRegNo >= DEVICEINFO_START_ADDR) && (nEndRegNo < (DEVICEINFO_START_ADDR + DEVICEINFO_REG_MAX)))
	{
		
	}

	/*读环境寄存器*/
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
	
	/*寻找开始地址的索引*/
	Search_Reg_Index(nReadRegNo, &pCurReg);	/*pCurReg找出的是开始读的地址,即第一个地址*/
	if (pCurReg == NULL)	/*pCurReg是一个指针,函数取其地址,是指向指针的指针,若不用&pCurReg，将为形参传递,是错误的*/
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	while(nNeedReadNum > 0)
	{
		/*读的寄存器长度错误*/
		if (nNeedReadNum < pCurReg->reg.nLenth)
		{
			*perr = DATA_ERR;
			return(0);
		}
		
		if ((pCurReg->reg.bAtrrib!=READONLY) && (pCurReg->reg.bAtrrib!=WRITEREAD))
		{
			*perr = REGADDR_ERR;/*该寄存器不能读*/
			return(0);
		}
		
		ptmpAddr = pCurReg->reg.pAddr;	/*取出指定位置的寄存器名称,注意ptmpAddr是指针,它指向的是实际的寄存器*/
		
		if (ptmpAddr == NULL)	/*当寄存器为保留时，将无法找到，返回NULL*/
		{
			for (i=0; i<pCurReg->reg.nLenth; i++)
			{
				pdatabuf[nReadLen++] = 0x00;
				pdatabuf[nReadLen++] = 0x00;	/*保留寄存器,返回0X0000*/
			}
		}
		else
		{
			for(i=0; i<pCurReg->reg.nLenth; i++)
			{
				INT16 rccode = 0x0000;
				
				if (ptmpAddr != NULL)
				{
					rccode = *(ptmpAddr + pCurReg->reg.nLenth - i - 1);	/*表示从寄存器的高位开始读*/
				}
								
				pdatabuf[nReadLen++]  = (UINT8)((rccode>>8)&0x00FF);/*高位*/
				pdatabuf[nReadLen++]  = (UINT8)(rccode&0x00FF);		/*低位*/
			}
		}
		
		nNeedReadNum -= pCurReg->reg.nLenth;	/*调整需要读取的寄存器数目*/
		pCurReg++;		/*读下一个索引*/
	}
	
	*perr = REG_OK;
	return nReadLen;
}

/********************************************************************************************************
** 函数名：    Write_Register
** 功能描述：  写寄存器
** 输入：		nStartRegNo--寄存器开始编号,nRegNum--读取的寄存器个数,
**				pdatabuf--写入的数据缓冲区地址,datalen--缓冲区长度
** 输出：		perr--操作结果码,
** 全局变量：	
** 调用模块：
**
** 修改：
** 日期：
**
**********************************************************************************************************/
UINT8 Write_Register( UINT16 nStartRegNo, INT16 nRegNum, const UINT8 *pdatabuf, UINT8 datalen, UINT8 *perr )
{
	const Map_Reg_Table *pCurReg, *pTmpReg;	          /*索引指针*/
	UINT16 *ptmpAddr = NULL;			  /*yfy修改INT16为UINT16*/
	const UINT8 *temp_pdatabuf =NULL;		  /*缓存接收缓冲区的指针地址*/
	UINT16 nNeedWriteNum;				  /*需要写的寄存器个数*/
	UINT16 nWriteRegNo;				  /*Reg编号*/
	UINT16 nEndRegNo;
	INT32 temp_value=0;		                  /*存储要写入寄存器的值*/
	UINT16 temp_index;			          /*数组Reg_Table元素下标*/
	
	nEndRegNo = nStartRegNo + nRegNum - 1;
	/*写装置参数寄存器*/
	if(( nStartRegNo >= PARAMS_START_ADDR) && (nEndRegNo < (PARAMS_START_ADDR + PARAMS_REG_MAX )))
	{
	
	}
	else 
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	
	/*对指针进行初始化*/
	pCurReg = NULL;
	pTmpReg = NULL;
	
	nNeedWriteNum = nRegNum;
	nWriteRegNo = nStartRegNo;
	
	temp_index = Search_Reg_Index(nWriteRegNo, &pCurReg); /*找地址索引*/

	if (pCurReg == NULL)
	{
		*perr = REGADDR_ERR;
		return(0);
	}
	
	pTmpReg = pCurReg;
	
	/*保证所写的数据不能只写了一个高字节,而没有写低字节或者只写了必须作为一个整体的数据部分数据(eg:定值)*/
        while( nNeedWriteNum > 0)
	{
		if ( nNeedWriteNum < pTmpReg->reg.nLenth )/*当出现要写入的数量比当前寄存器所占的数量小就报错*/
		{
			*perr = DATA_ERR;
			return(0);
		}
		
		nNeedWriteNum -= pTmpReg->reg.nLenth;	
		pTmpReg++;
	}
	
	/*开始写入数据范围检查*/
	nNeedWriteNum = nRegNum;
	nWriteRegNo = nStartRegNo;

	pTmpReg = pCurReg;		/*缓存指针，用于写入的寄存器范围判断*/
	temp_pdatabuf = pdatabuf;       /*缓存缓冲区地址，用于写入的寄存器范围判断*/
	
        /*对写的寄存器进行检查，并对所写的地址进行分类，置相应的标志位*/
	while( nNeedWriteNum > 0 )
	{

		ptmpAddr = pTmpReg->reg.pAddr;  
		/*只有当寄存器已定义，且可写的时候才取写寄存器的值,否则直接跳过本寄存器*/
		if ( ptmpAddr != (UINT16*)0 && (( pTmpReg->reg.bAtrrib == WRITEONLY ) || ( pTmpReg->reg.bAtrrib == WRITEREAD ))) 
		{	
			if( pTmpReg->reg.nLenth == 1 )
			{
				temp_value= (INT32)((UINT16)temp_pdatabuf[0]<<8 | temp_pdatabuf[1]);
				if( CheckRegValue( pTmpReg,temp_value,temp_index ))//注意这里一次只检查一个寄存器
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
					temp_value= ( temp_value &0x0000FFFF );//yfy添加
					temp_value = temp_value<<16;
					temp_value += ( (UINT16)temp_pdatabuf[2]<<8 | temp_pdatabuf[3] );
					if( CheckRegValue( pTmpReg,temp_value,temp_index ))//yfy修改2010.10.11
					{			
						*perr = DATA_ERR;
						return(0);
					}
				ptmpAddr ++;
				ptmpAddr ++;
				temp_pdatabuf += 4;
			}
			temp_index++;/*寄存器范围检查完毕后，准备检查写一个寄存器，这里下标要加1yfy 2010.10.11*/
		}
		else
		{
			*perr = REGADDR_ERR;
			return(0);
		}
		
		nNeedWriteNum -= pTmpReg->reg.nLenth;
		
		pTmpReg++;	/*下一个索引的寄存器*/
		if ( pTmpReg == NULL )
		{
			*perr = DATA_ERR;
			return(0);
		}
	}

	
	/*前面数据检查无误后开始判断写寄存器*/
	nNeedWriteNum = nRegNum;
	nWriteRegNo = nStartRegNo;


	while(nNeedWriteNum > 0)
	{
		ptmpAddr = pCurReg->reg.pAddr;  
		/*只有当寄存器可写的时候才取写寄存器的值,否则直接跳过本寄存器*/
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
				/*前面检查寄存器范围正确，这里才写入*/
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
		pCurReg++;	/*写下一个索引的寄存器*/
		
                /*寄存器未定义*/
                if ( pCurReg == NULL )
		{
			*perr = DATA_ERR;
			return(0);
		}
	}	
	*perr = REG_OK;	
	return(datalen);
}


