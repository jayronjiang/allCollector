/********************************************************************************
*  ��Ȩ����: �㶫��ͨ�Ƽ�				
*
*  �ļ���:       YDN23.c
*
*
*  ��ǰ�汾: 0.1
*  
*  ����:   Jerry	                    
*
*  �������:  20190613
*
*  ����: ����Э��YDN023, ��ȡ��Ĭ����UPS����
*                           
********************************************************************************/

/*
*********************************************************************************************************
*                                         �����ļ�
*********************************************************************************************************
*/
#include "include.h"


/*����lenth�������Э���LCHKSUM*/
/*���͵�ʱ�����*/
 UINT16 lchkSumCalc(UINT16 len_value)
{
	INT16U re_value = 0;

	/*�ȶ�D0D1D2D3+D4D5D6D7+D8D9D10D11���*/
	re_value = ((len_value&0x0F) + ((len_value>>4)&0x0F) + ((len_value>>8)&0x0F));
	re_value = (~(re_value&0x0F))+1;	// ��ģ16����ȡ����1
	re_value = re_value&0x0F;

	return re_value;
}


/*����lenth�������Э���CHECKSUM*/
 UINT16 checkSumCalc(UINT8 *buffer, UINT8 len)
{
	INT16U re_value = 0;
	INT8U i = 0;

	/*�ȶ����г�SIO֮������*/
	for (i = 0; i<len; i++)
	{
		re_value = re_value+buffer[i];
	}
	// ģ65535������+1
	re_value = (~(re_value&0xFFFF))+1;	// ��ģ65536����ȡ����1

	return re_value;
}

/******************************************************************************
 * ������:	comm_UPSParam_ANALYSE 
 * ����: 		UPS�ſ����ݽ���
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:JERRY
 * ��������:
 ******************************************************************************/
 #define UPS_PARAM_RES_LEN		72			 // 0x42�������ݳ���72
 #define VOLT_AIN_POS				15			// 15�ֽڿ�ʼ��A�������ѹ��ֵ
 #define VOLT_BIN_POS				(VOLT_AIN_POS+4)			// 19�ֽڿ�ʼ��B���ѹ��ֵ
 #define VOLT_CIN_POS				(VOLT_BIN_POS+4)			// 23�ֽڿ�ʼ��C���ѹ��ֵ

 #define VOLT_AOUT_POS			(VOLT_CIN_POS+4)			// 27�ֽڿ�ʼ��A���ѹ��ֵ
 #define VOLT_BOUT_POS			(VOLT_AOUT_POS+4)		// 31�ֽڿ�ʼ��B���ѹ��ֵ
 #define VOLT_COUT_POS			(VOLT_BOUT_POS+4)		// 35�ֽڿ�ʼ��C���ѹ��ֵ

 #define AMP_AOUT_POS			(VOLT_COUT_POS+4)			// 27�ֽڿ�ʼ��A���ѹ��ֵ
 #define AMP_BOUT_POS			(AMP_AOUT_POS+4)		// 31�ֽڿ�ʼ��B���ѹ��ֵ
 #define AMP_COUT_POS			(AMP_BOUT_POS+4)		// 35�ֽڿ�ʼ��C���ѹ��ֵ

 #define FREQ_OUT_POS			(AMP_COUT_POS+8)		// 35�ֽڿ�ʼ��C���ѹ��ֵ
 
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

		// ���Ƶ��
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FREQ_OUT_POS);
	}
}


/******************************************************************************
 * ������:	comm_UPSIn_ANALYSE 
 * ����: 		UPS�������ݽ���
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 ******************************************************************************/
 #define UPS_IN_RES_LEN			96			 // 0x42�������ݳ���72
 #define UPS_PHASE_NUM_IN_POS	19			// ��������
 #define AMP_AIN_POS				35			// 35�ֽڿ�ʼ��A�����������ֵ
 #define AMP_BIN_POS				(AMP_AIN_POS+4)			// 19�ֽڿ�ʼ��B���ѹ��ֵ
 #define AMP_CIN_POS				(AMP_BIN_POS+4)			// 23�ֽڿ�ʼ��C���ѹ��ֵ
 #define FREQ_IN_POS				(AMP_CIN_POS+4)

 #define FACT_AIN_POS				(FREQ_IN_POS+4)
 #define FACT_BIN_POS				(FACT_AIN_POS+4)
 #define FACT_CIN_POS				(FACT_BIN_POS+4)

 #define BYPASS_AIN_POS			(FACT_CIN_POS+4)
 #define BYPASS_BIN_POS			(BYPASS_AIN_POS+4)
 #define BYPASS_CIN_POS			(BYPASS_BIN_POS+4)
 #define BYPASS_FREQ_POS			(BYPASS_CIN_POS+16)	//�������ߵ�ѹ����
 
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

		// ��·Ƶ��
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_FREQ_POS);
	}
}


/******************************************************************************
 * ������:	comm_UPSOut_ANALYSE 
 * ����: 		UPS������ݽ���0xE1
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 ******************************************************************************/
 #define UPS_OUT_RES_LEN			88			 // 0xE1�������ݳ���72
 #define UPS_PHASE_NUM_OUT_POS	19			// ��������
 #define FACT_AOUT_POS			(UPS_PHASE_NUM_OUT_POS+4)			// 35�ֽڿ�ʼ��A�����������ֵ
 #define FACT_BOUT_POS			(FACT_AOUT_POS+4)			// 19�ֽڿ�ʼ��B���ѹ��ֵ
 #define FACT_COUT_POS			(FACT_BOUT_POS+4)			// 23�ֽڿ�ʼ��C���ѹ��ֵ

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
 * ������:	comm_UPSBat_ANALYSE 
 * ����: 		UPS������ݽ���0xE3
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 * 
 ******************************************************************************/
#define UPS_BAT_RES_LEN			72			 // 0x42�������ݳ���72
#define BAT_RUNNING_DAY_POS		19			// 35�ֽڿ�ʼ��A�����������ֵ
#define BAT_VOLT_POS				(BAT_RUNNING_DAY_POS+4)			// 19�ֽڿ�ʼ�ǵ����������
#define BAT_CHARGE_AMP_POS		(BAT_VOLT_POS+4)
#define BAT_DISCHG_AMP_POS		(BAT_CHARGE_AMP_POS+4)
#define BAT_BACKUP_TIME_POS		(BAT_DISCHG_AMP_POS+16)
#define BAT_TMP_POS				(BAT_BACKUP_TIME_POS+4)	//����¶�,��֧��
#define BAT_ENVI_TMP_POS			(BAT_TMP_POS+4)
#define BAT_VOLUMN_POS			(BAT_ENVI_TMP_POS+4)
#define BAT_DISCHG_TIMES_POS	(BAT_VOLUMN_POS+4)		//�ŵ����
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
		*(pointer+5) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_ENVI_TMP_POS);	// �����¶�
		*(pointer+6) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_VOLUMN_POS);		// �������
		*(pointer+7) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BAT_DISCHG_TIMES_POS);		// �ŵ����
	}
}


/******************************************************************************
 * ������:	comm_UPSAlarm_ANALYSE 
 * ����: 		UPS��ر�������0x44
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����:
 * ��������:
 *  
 ******************************************************************************/
#define UPS_ALARM_RES_LEN			146			 // 0xE4�������ݳ���146
#define MAIN_ABNOMAL_POS			31			// 31�ֽڿ�ʼ����·�쳣�澯
#define SYSTEM_OVERTEMP_POS			(MAIN_ABNOMAL_POS+2)		// 1��״̬ռ1���ֽ�����+2	
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
		*pointer = ascii_to_hex4(g_PDUData.PDUBuffPtr+ MAIN_ABNOMAL_POS);		// ��·�쳣
		*(pointer+1) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ SYSTEM_OVERTEMP_POS);	//ϵͳ����
		*(pointer+2) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ SYSTEM_LOWBAT_POS);	//ϵͳ��ص�
		*(pointer+3) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ RECTIFIER_OVERLOAD_POS);	//����������
		*(pointer+4) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ INVERTER_OVERLOAD_POS);	//���������
		*(pointer+5) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_ABNORMAL_POS);	// ��·�쳣
		*(pointer+6) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BATTERY_LOW_POS);		// ��ص�ѹ��
		*(pointer+7) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BATTERY_ABNORMAL_POS);		// ����쳣
		*(pointer+8) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ BATTERY_OVERTEMP_POS);		// ��ع���
		*(pointer+9) = 	ascii_to_hex4(g_PDUData.PDUBuffPtr+ FAN_ABNORMAL_POS);		// �����쳣
		*(pointer+10) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ SHUTDOWM_POS);		// �����ػ�
		*(pointer+11) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ MAINTAIN_STATUS_POS);		// ά��ģʽ
		*(pointer+12) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ INVERTER_POS);		// �����״̬
		*(pointer+13) = ascii_to_hex4(g_PDUData.PDUBuffPtr+ BYPASS_POS);		// ��·״̬
	}
}


/******************************************************************************
 * ������:	comm_UPSStatus_ANALYSE 
 * ����: 		UPS������ݽ���0x43
 *            -
 * �������: 
 * �������: 
 * ����ֵ: 
 * 
 * ����: Jerry
 * ��������:
 * 
 ******************************************************************************/
#define UPS_STATUS_RES_LEN		40			 // 0x42�������ݳ���40
#define BAT_SUPPLY_OUT_POS		15			// 15�ֽڿ�ʼ��UPS������緽ʽ
#define BAT_SUPPLY_IN_POS		(BAT_SUPPLY_OUT_POS+4)		// 19�ֽڿ�ʼ��UPS���빩�緽ʽ
#define BAT_STATUS_POS			(BAT_SUPPLY_IN_POS+2)		// 19�ֽڿ�ʼ��UPS���빩�緽ʽ

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


