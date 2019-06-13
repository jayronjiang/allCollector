/********************************************************************************
*  版权所有: 广东利通科技				
*
*  文件名:       YDN23.c
*
*
*  当前版本: 0.1
*  
*  作者:   Jerry	                    
*
*  完成日期:  20190613
*
*  描述: 电总协议YDN023, 读取艾默生的UPS数据
*                           
********************************************************************************/

/*
*********************************************************************************************************
*                                         包含文件
*********************************************************************************************************
*/
#include "include.h"


/*根据lenth计算电总协议的LCHKSUM*/
/*发送的时候计算*/
 UINT16 lchkSumCalc(UINT16 len_value)
{
	INT16U re_value = 0;

	/*先对D0D1D2D3+D4D5D6D7+D8D9D10D11求和*/
	re_value = ((len_value&0x0F) + ((len_value>>4)&0x0F) + ((len_value>>8)&0x0F));
	re_value = (~(re_value&0x0F))+1;	// 再模16余数取反加1
	re_value = re_value&0x0F;

	return re_value;
}


/*根据lenth计算电总协议的CHECKSUM*/
 UINT16 checkSumCalc(UINT8 *buffer, UINT8 len)
{
	INT16U re_value = 0;
	INT8U i = 0;

	/*先对所有除SIO之外的求和*/
	for (i = 0; i<len; i++)
	{
		re_value = re_value+buffer[i];
	}
	// 模65535的余数+1
	re_value = (~(re_value&0xFFFF))+1;	// 再模65536余数取反加1

	return re_value;
}

/******************************************************************************
 * 函数名:	comm_UPSParam_ANALYSE 
 * 描述: 		UPS概况数据解析
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:JERRY
 * 创建日期:
 ******************************************************************************/
 #define UPS_PARAM_RES_LEN		72			 // 0x42返回数据长度72
 #define VOLT_AIN_POS				15			// 15字节开始是A相输入电压数值
 #define VOLT_BIN_POS				(VOLT_AIN_POS+4)			// 19字节开始是B相电压数值
 #define VOLT_CIN_POS				(VOLT_BIN_POS+4)			// 23字节开始是C相电压数值

 #define VOLT_AOUT_POS			(VOLT_CIN_POS+4)			// 27字节开始是A相电压数值
 #define VOLT_BOUT_POS			(VOLT_AOUT_POS+4)		// 31字节开始是B相电压数值
 #define VOLT_COUT_POS			(VOLT_BOUT_POS+4)		// 35字节开始是C相电压数值

 #define AMP_AOUT_POS			(VOLT_COUT_POS+4)			// 27字节开始是A相电压数值
 #define AMP_BOUT_POS			(AMP_AOUT_POS+4)		// 31字节开始是B相电压数值
 #define AMP_COUT_POS			(AMP_BOUT_POS+4)		// 35字节开始是C相电压数值

 #define FREQ_OUT_POS			(AMP_COUT_POS+8)		// 35字节开始是C相电压数值
 
void comm_UPSParam_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.ups_in.volt_Ain;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_PARAM_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_PARAM_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_PARAM_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_AIN_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_BIN_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_CIN_POS);

		pointer = &UPSParams.ups_out.volt_Aout;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_AOUT_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_BOUT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VOLT_COUT_POS);

		pointer = &UPSParams.ups_out.amp_Aout;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_AOUT_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_BOUT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_COUT_POS);

		// 输出频率
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FREQ_OUT_POS);
	}
}


/******************************************************************************
 * 函数名:	comm_UPSIn_ANALYSE 
 * 描述: 		UPS输入数据解析
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 ******************************************************************************/
 #define UPS_IN_RES_LEN			96			 // 0x42返回数据长度72
 #define UPS_PHASE_NUM_IN_POS	19			// 输入相数
 #define AMP_AIN_POS				35			// 35字节开始是A相输入电流数值
 #define AMP_BIN_POS				(AMP_AIN_POS+4)			// 19字节开始是B相电压数值
 #define AMP_CIN_POS				(AMP_BIN_POS+4)			// 23字节开始是C相电压数值
 #define FREQ_IN_POS				(AMP_CIN_POS+4)

 #define FACT_AIN_POS				(FREQ_IN_POS+4)
 #define FACT_BIN_POS				(FACT_AIN_POS+4)
 #define FACT_CIN_POS				(FACT_BIN_POS+4)

 #define BYPASS_AIN_POS			(FACT_CIN_POS+4)
 #define BYPASS_BIN_POS			(BYPASS_AIN_POS+4)
 #define BYPASS_CIN_POS			(BYPASS_BIN_POS+4)
 #define BYPASS_FREQ_POS			(BYPASS_CIN_POS+16)	//跳过了线电压部分
 
void comm_UPSIn_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.ups_in.phase_num;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_IN_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_IN_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_IN_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ UPS_PHASE_NUM_IN_POS);
	
		pointer = &UPSParams.ups_in.amp_Ain;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_AIN_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_BIN_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ AMP_CIN_POS);
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FREQ_IN_POS);

		pointer = &UPSParams.ups_in.fact_Ain;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_AIN_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_BIN_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_CIN_POS);

		pointer = &UPSParams.ups_in.bypass_voltA;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_AIN_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_BIN_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_CIN_POS);

		// 旁路频率
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_FREQ_POS);
	}
}


/******************************************************************************
 * 函数名:	comm_UPSOut_ANALYSE 
 * 描述: 		UPS输出数据解析0xE1
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 ******************************************************************************/
 #define UPS_OUT_RES_LEN			88			 // 0xE1返回数据长度72
 #define UPS_PHASE_NUM_OUT_POS	19			// 输入相数
 #define FACT_AOUT_POS			(UPS_PHASE_NUM_OUT_POS+4)			// 35字节开始是A相输入电流数值
 #define FACT_BOUT_POS			(FACT_AOUT_POS+4)			// 19字节开始是B相电压数值
 #define FACT_COUT_POS			(FACT_BOUT_POS+4)			// 23字节开始是C相电压数值

 #define KWH_AOUT_POS			(FACT_COUT_POS+16)
 #define KWH_BOUT_POS			(KWH_AOUT_POS+4)
 #define KWH_COUT_POS			(KWH_BOUT_POS+4)

 #define VAH_AOUT_POS			(KWH_COUT_POS+4)
 #define VAH_BOUT_POS			(VAH_AOUT_POS+4)
 #define VAH_COUT_POS			(VAH_BOUT_POS+4)

 #define LOAD_AOUT_POS			(VAH_COUT_POS+4)
 #define LOAD_BOUT_POS			(LOAD_AOUT_POS+4)
 #define LOAD_COUT_POS			(LOAD_BOUT_POS+4)
 
void comm_UPSOut_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.ups_out.phase_num;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_OUT_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_OUT_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_OUT_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ UPS_PHASE_NUM_OUT_POS);

		pointer = &UPSParams.ups_out.fact_Aout;
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_AOUT_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_BOUT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FACT_COUT_POS);

		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ KWH_AOUT_POS);
		*(pointer+4) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ KWH_BOUT_POS);
		*(pointer+5) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ KWH_COUT_POS);

		*(pointer+6) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VAH_AOUT_POS);
		*(pointer+7) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VAH_BOUT_POS);
		*(pointer+8) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ VAH_COUT_POS);

		*(pointer+9) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ LOAD_AOUT_POS);
		*(pointer+10) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ LOAD_BOUT_POS);
		*(pointer+11) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ LOAD_COUT_POS);
	}
}


/******************************************************************************
 * 函数名:	comm_UPSBat_ANALYSE 
 * 描述: 		UPS电池数据解析0xE3
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 * 
 ******************************************************************************/
#define UPS_BAT_RES_LEN			72			 // 0x42返回数据长度72
#define BAT_RUNNING_DAY_POS		19			// 35字节开始是A相输入电流数值
#define BAT_VOLT_POS				(BAT_RUNNING_DAY_POS+4)			// 19字节开始是电池运行天数
#define BAT_CHARGE_AMP_POS		(BAT_VOLT_POS+4)
#define BAT_DISCHG_AMP_POS		(BAT_CHARGE_AMP_POS+4)
#define BAT_BACKUP_TIME_POS		(BAT_DISCHG_AMP_POS+16)
#define BAT_TMP_POS				(BAT_BACKUP_TIME_POS+4)	//电池温度,不支持
#define BAT_ENVI_TMP_POS			(BAT_TMP_POS+4)
#define BAT_VOLUMN_POS			(BAT_ENVI_TMP_POS+4)
#define BAT_DISCHG_TIMES_POS	(BAT_VOLUMN_POS+4)		//放电次数
#define BAT_CHKSUM_POS			(BAT_DISCHG_TIMES_POS+4)
void comm_UPSBat_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.battery.running_day;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_BAT_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_BAT_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_BAT_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_RUNNING_DAY_POS);
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_VOLT_POS);
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_CHARGE_AMP_POS);
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_DISCHG_AMP_POS);
		*(pointer+4) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_BACKUP_TIME_POS);
		*(pointer+5) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_ENVI_TMP_POS);	// 环境温度
		*(pointer+6) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_VOLUMN_POS);		// 电池容量
		*(pointer+7) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_DISCHG_TIMES_POS);		// 放电次数
	}
}


/******************************************************************************
 * 函数名:	comm_UPSAlarm_ANALYSE 
 * 描述: 		UPS电池报警解析0x44
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者:
 * 创建日期:
 *  
 ******************************************************************************/
#define UPS_ALARM_RES_LEN			146			 // 0xE4返回数据长度146
#define MAIN_ABNOMAL_POS			31			// 31字节开始是主路异常告警
#define SYSTEM_OVERTEMP_POS			(MAIN_ABNOMAL_POS+2)		// 1个状态占1个字节所以+2	
#define SYSTEM_LOWBAT_POS			(SYSTEM_OVERTEMP_POS+2)
#define RECTIFIER_OVERLOAD_POS		(SYSTEM_LOWBAT_POS+8)
#define INVERTER_OVERLOAD_POS		(RECTIFIER_OVERLOAD_POS+4)
#define BYPASS_ABNORMAL_POS		(INVERTER_OVERLOAD_POS+8)
#define BATTERY_LOW_POS				(BYPASS_ABNORMAL_POS+12)
#define BATTERY_ABNORMAL_POS		(BATTERY_LOW_POS+2)
#define BATTERY_OVERTEMP_POS		(BATTERY_ABNORMAL_POS+4)
#define FAN_ABNORMAL_POS			(BATTERY_OVERTEMP_POS+8)
#define SHUTDOWM_POS				(FAN_ABNORMAL_POS+2)
#define MAINTAIN_STATUS_POS			(SHUTDOWM_POS+18)
#define INVERTER_POS					(MAINTAIN_STATUS_POS+2)
#define BYPASS_POS					(INVERTER_POS+2)

void comm_UPSAlarm_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.alarm.main_abnormal;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_ALARM_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_ALARM_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_ALARM_RES_LEN))
	{
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ MAIN_ABNOMAL_POS);		// 主路异常
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ SYSTEM_OVERTEMP_POS);	//系统过温
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ SYSTEM_LOWBAT_POS);	//系统电池低
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ RECTIFIER_OVERLOAD_POS);	//整流器过载
		*(pointer+4) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ INVERTER_OVERLOAD_POS);	//逆变器过载
		*(pointer+5) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_ABNORMAL_POS);	// 旁路异常
		*(pointer+6) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BATTERY_LOW_POS);		// 电池电压低
		*(pointer+7) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BATTERY_ABNORMAL_POS);		// 电池异常
		*(pointer+8) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BATTERY_OVERTEMP_POS);		// 电池过温
		*(pointer+9) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FAN_ABNORMAL_POS);		// 风扇异常
		*(pointer+10) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ SHUTDOWM_POS);		// 紧急关机
		*(pointer+11) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ MAINTAIN_STATUS_POS);		// 维修模式
		*(pointer+12) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ INVERTER_POS);		// 逆变器状态
		*(pointer+13) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_POS);		// 旁路状态
	}
}


/******************************************************************************
 * 函数名:	comm_UPSStatus_ANALYSE 
 * 描述: 		UPS电池数据解析0x43
 *            -
 * 输入参数: 
 * 输出参数: 
 * 返回值: 
 * 
 * 作者: Jerry
 * 创建日期:
 * 
 ******************************************************************************/
#define UPS_STATUS_RES_LEN		40			 // 0x42返回数据长度40
#define BAT_SUPPLY_OUT_POS		15			// 15字节开始是UPS输出供电方式
#define BAT_SUPPLY_IN_POS		(BAT_SUPPLY_OUT_POS+4)		// 19字节开始是UPS输入供电方式
#define BAT_STATUS_POS			(BAT_SUPPLY_IN_POS+2)		// 19字节开始是UPS输入供电方式

void comm_UPSStatus_ANALYSE(void)	
{
	INT16U* pointer = &UPSParams.status.supply_out_status;
	INT16U ups_chksum = 0;

	ups_chksum = ascii_to_hex4(g_PDUData.PDUBuffPtr+UPS_STATUS_RES_LEN-5);

	if((checkSumCalc(g_PDUData.PDUBuffPtr+1, UPS_STATUS_RES_LEN-6) == ups_chksum) \
	    && (g_PDUData.PDULength == UPS_STATUS_RES_LEN))
	{
		*pointer = ascii_to_hex2(g_PDUData.PDUBuffPtr+ BAT_SUPPLY_OUT_POS);
		*(pointer+1) = 	ascii_to_hex2(g_PDUData.PDUBuffPtr+ BAT_SUPPLY_IN_POS);
		*(pointer+2) = 	ascii_to_hex2(g_PDUData.PDUBuffPtr+ BAT_STATUS_POS);
	}
}


