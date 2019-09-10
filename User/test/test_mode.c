 /**
  ******************************************************************************
  * @file    main.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   Main ����, ϵͳ���.
  *
  ******************************************************************************
  */

#include "include.h"

static bool test_entry(void);
static bool test_485(void);
static bool test_232(void);
static bool test_flash(void);

extern UINT8 Write_SingleCoil(UINT16  nStartRegNo, const UINT8  *pdatabuf, UINT8  *perr);
extern void read_fm_memory(UINT16 addr,UINT8 *dest_ptr, UINT16 length);
extern void write_fm_memory(UINT16 addr, UINT8 *src_ptr, UINT16 length);
extern uint16_t user_settings_key_get(void);

typedef bool (*test_state_t)(void);

const test_state_t sub_tests[TEST_NUMBER_OF_TESTS] = {
    [TEST_ENTRY]          = test_entry,
    [TEST_232]             =  test_232,
    [TEST_485]              = test_485,
    [TEST_FLASH]         = test_flash,
};

enum {
    DIP_ENTER               = 0x00,
    DIP_232                = 0x01,
    DIP_485                = 0x03,
    DIP_FLASH            = 0x02,
};

//******************************************************************************
// Module variables
//******************************************************************************
static uint8_t current_test = TEST_NUMBER_OF_TESTS;
static uint16_t dip_setting = 0;
static uint16_t last_dip_setting = 0;
bool test485Flag = FALSE;
bool test232Flag = FALSE;

USART_LIST testTxUt = UART4_COM;
USART_LIST testRxUt = UART2_COM;
uint16_t testAlarm = 0;
static volatile uint32_t temp_time_s = 0;
uint16_t finish_flag = 0;		// ��־4��������Ŀȫ�����

//******************************************************************************
// Function definitions
//******************************************************************************

//  ----------------------------------------------------------------------------
/// \brief
//  ----------------------------------------------------------------------------
void test_mode_start(void)
{
	current_test = TEST_ENTRY;
}


//  ----------------------------------------------------------------------------
/// \brief
//  ----------------------------------------------------------------------------
void test_indication(void)
{
	if (system_flag&TEST_DISPLAY)
	{
		system_flag &= ~ TEST_DISPLAY;
		if (testAlarm&ERR_232)
		{
			LED_Flashing(LED_COM, 100, 1);
		}
		else if (testAlarm&ERR_485)
		{
			LED_Flashing(LED_COM, 100, 2);
		}
		else if (testAlarm&ERR_FLASH)
		{
			LED_Flashing(LED_COM, 100, 3);
		}
		else
		{
			LED_Set(LED_COM, ON);
		}
	}
}

// ����ָʾ
static void test_ind_item(uint16_t item)
{
	if (system_flag&TEST_DISPLAY)
	{
		system_flag &= ~ TEST_DISPLAY;
		switch (item)
		{
		case ERR_485:
			LED_Flashing(LED_COM, 100, 2);
			break;
		case ERR_232:
			LED_Flashing(LED_COM, 100, 1);
			break;
		case ERR_FLASH:
			LED_Flashing(LED_COM, 100, 3);
			break;
		default:
			break;
		}
	}
}



//  ----------------------------------------------------------------------------
/// \brief
//  ----------------------------------------------------------------------------
bool test_mode_update(void)
{
	bool exit_sub_mode = FALSE;
	static bool entry = FALSE;

	dip_setting = user_settings_key_get();	// �ȵõ�DIP_SETTING, ���溯����Ҫ��
	if (entry == FALSE)
	{
		if (dip_setting == DIP_ENTER)
		{
			entry = TRUE;
			test_mode_start();
		}
	}
	#if 0
	// Execute test
	// ��ʽ����,���Ҫ����
	if (system_flag &TEST_SEQ)
	{
		system_flag &= ~TEST_SEQ;
		dip_setting++;
		if (dip_setting >= TEST_FLASH)
		{
			dip_setting = TEST_FLASH;
		}
	}
	#endif
	// ���ε�����,time.c��TEST_SEQҲҪ����
	//////////////////////////////////////////////////////////////////
	if ((current_test < TEST_NUMBER_OF_TESTS) && (sub_tests[current_test] != NULL)) 
	{
		exit_sub_mode = sub_tests[current_test]();
	} 
	else 
	{
		return 1;
	}

        switch (dip_setting)
	{
                case DIP_ENTER:
                    current_test = TEST_ENTRY;
                    break;

                case DIP_485:
                    current_test = TEST_485;
                    break;

                case DIP_232:
                    current_test = TEST_232;
                    break;

                case DIP_FLASH:
                    current_test = TEST_FLASH;
                    break;

                default:
                    current_test = TEST_ENTRY;
                    break;
        }
	return 0;
}

//  ----------------------------------------------------------------------------
/// \brief
/// \b Purpose:
/// Test mode entry point and initial test for BUTTON-B.
///
/// \b Execution:
/// Starting from any state, toggle switches to "0x000". LED indication is
/// red until "0x000" is set. When indication turns green the test will accept
/// pushbutton-B. Press "B" to enter the actual test mode.
///
/// \return 'true' on exit condition met.
//  ----------------------------------------------------------------------------
static bool test_entry(void)
{
	bool exit = FALSE;
	static bool mode = FALSE;
	UINT8 uErr = 0;			/*������*/
	static UINT16 regStartAddr = 1500;
	static UINT8 databuf[2] = {0xff,0x01};
	static uint16_t count = 0;
	static uint8_t entry = FALSE;
	static uint16_t sub_count = 0;

	if (dip_setting == DIP_ENTER)
	{
		if (finish_flag == (BIT(TEST_NUMBER_OF_TESTS)-1))
		{
			return exit;
		}
		if (entry == FALSE)
		{
			entry = TRUE;
			last_dip_setting = dip_setting;
		}
		
		if (system_flag & TEST_LED)
		{
			system_flag &= ~TEST_LED;
			if (!mode)
			{
				count++;
				Write_SingleCoil(regStartAddr, databuf, &uErr);
				regStartAddr++;
				if (regStartAddr >= 1500+ACTRUL_DO_NUM)
				{
					regStartAddr = 1500;
					databuf[1]++;
					if (databuf[1] >=2)
					{
						databuf[1] = 0;
					}
				}
			}
			else
			{
				sub_count++;
				// ��ɺ�10sһ�β���ѭ���ߵ��¿���
				if (sub_count >= 20)
				{
					sub_count = 0;
					Write_SingleCoil(regStartAddr, databuf, &uErr);
					regStartAddr++;
					if (regStartAddr >= 1500+ACTRUL_DO_NUM)
					{
						regStartAddr = 1500;
						databuf[1]++;
						if (databuf[1] >=2)
						{
							databuf[1] = 0;
						}
					}
				}
			}
		}
		if (count > 24)
		{
			exit = TRUE;	// 36��ѭ���Ѿ������
			// ���������Ŀ��������,����
			//if (finish_flag != (BIT(TEST_NUMBER_OF_TESTS)-1))
			{
				LED_Set(LED_COM, ON);
			}
		}
		if (count > 96)
		{
			mode = TRUE;
		}
	} 
	else if (dip_setting != last_dip_setting) 
	{
		finish_flag |= BIT(TEST_ENTRY);
		last_dip_setting = dip_setting;
		if (finish_flag == (BIT(TEST_NUMBER_OF_TESTS)-1))
		{
			return exit;
		}
		LED_Set(LED_COM, OFF);	// ��һ������,Ҫ��
		for (UINT8 i =0; i<ACTRUL_DO_NUM;i++)
		{
			Relay_Act(i);
		}
		/*
		// ������һ��,���־
		if (finish_flag == (BIT(TEST_NUMBER_OF_TESTS)-1))
		{
			finish_flag = 0;
		}
		*/
		entry = FALSE;
		mode =FALSE;
		count = 0;
	}

	return exit;
}

//  ----------------------------------------------------------------------------
/// \brief
/// \b Purpose:
/// Test mode entry point and initial test for BUTTON-B.
///
/// \b Execution:
/// Starting from any state, toggle switches to "0x000". LED indication is
/// red until "0x000" is set. When indication turns green the test will accept
/// pushbutton-B. Press "B" to enter the actual test mode.
///
/// \return 'true' on exit condition met.
//  ----------------------------------------------------------------------------
static bool test_485(void)
{
	bool exit = FALSE;
	static uint8_t test_status = 0;
	static uint8_t cnt = 0;

	if (dip_setting == DIP_485)
	{
		switch (test_status)
		{
		case 0:
			testTxUt = UART4_COM;
			testRxUt	= UART1_COM;
			message_send_printf(testTxUt, testTxUt, TRUE, TEST_MSG);
			test_status++;
			temp_time_s = system_time_s;
			test485Flag = TRUE;
			break;
		case 1:
			if ((system_time_s -temp_time_s) >=1)
			{
				if (test485Flag)
				{
					testAlarm |= ERR_485;
					test485Flag = 0;
				}
				test_status++;
				temp_time_s = system_time_s;
			}
			break;
		case 2:
			testTxUt = UART1_COM;
			testRxUt	= UART4_COM;
			message_send_printf(testTxUt, testTxUt, TRUE, TEST_MSG);
			test_status++;
			temp_time_s = system_time_s;
			test485Flag = TRUE;
			break;
		case 3:
			if ((system_time_s -temp_time_s) >=1)
			{
				if (test485Flag)
				{
					testAlarm |= ERR_485;
					test485Flag = 0;
				}
				test_status++;
				temp_time_s = system_time_s;
			}
			break;
		case 4:
			if (testAlarm &ERR_485)
			{
				cnt++;
				if (cnt >=3)
				{
					cnt = 3;
					test_ind_item(ERR_485);
				}
				if (cnt < 3)
				{
					test_status = 0;	//������һ��
				}
			}
			else
			{
				// ���������Ŀ��������,����
				//if (finish_flag != (BIT(TEST_NUMBER_OF_TESTS)-1))
				{
					// ��������,�������ڵ�
					LED_Set(LED_COM, ON);	
				}
			}
			exit = TRUE;
			break;
		default:
			break;
		}
	} 
	else if (dip_setting != last_dip_setting) 
	{
		cnt = 0;
		finish_flag |= BIT(TEST_485);
		LED_Set(LED_COM, OFF);	// ��һ������,Ҫ��
		last_dip_setting = dip_setting;
		/*
		if (finish_flag == (BIT(TEST_NUMBER_OF_TESTS)-1))
		{
			finish_flag = 0;
		}
		*/
	}
	return exit;
}


static bool test_232(void)
{
	bool exit = FALSE;
	static uint8_t test_status = 0;
	static uint8_t cnt = 0;

	if (dip_setting == DIP_232)
	{
		switch (test_status)
		{
		case 0:
			testTxUt = UART1_COM;
			testRxUt	= UART1_COM;
			message_send_printf(testTxUt, testTxUt, TRUE, TEST_MSG);
			test_status++;
			temp_time_s = system_time_s;
			test232Flag = TRUE;
			break;
		case 1:
			if ((system_time_s -temp_time_s) >=1)
			{
				temp_time_s = system_time_s;
				LED_Set(LED_COM, ON);	
				if (test232Flag)
				{
					testAlarm |= ERR_232;
					test232Flag = 0;
					//���ͨ�Ŵ���,����Ϩ��
					LED_Set(LED_COM, OFF);

					cnt++;
					if (cnt >=3)
					{
						cnt = 3;
						test_status = 2;
					}
					if (cnt < 3)
					{
						test_status = 0;	//������һ��
					}
				}
				else
				{
					test_status = 2;
				}
			}
			break;
	/*����һ��232���ⲿ�ӳ�485��,*/ 
		case 2:
			if ((system_time_s -temp_time_s) >=1)
			{
				LED_Set(LED_COM, OFF);
				temp_time_s = system_time_s;
				test_status++;
				cnt = 0;
			}
			break;
		case 3:
			testTxUt = UART2_COM;
			testRxUt	= UART2_COM;
			message_send_printf(testTxUt, testTxUt, TRUE, TEST_MSG);
			test_status++;
			temp_time_s = system_time_s;
			test232Flag = TRUE;
			break;
		case 4:
			if ((system_time_s -temp_time_s) >=1)
			{
				if (test232Flag)
				{
					testAlarm |= ERR_232;
					test232Flag = 0;
				}
				test_status++;
				temp_time_s = system_time_s;
			}
			break;
		case 5:
			if (testAlarm &ERR_232)
			{
				cnt++;
				if (cnt >=3)
				{
					cnt = 3;
					test_ind_item(ERR_232);
				}
				if (cnt < 3)
				{
					test_status = 3;	//������һ��
				}
			}
			else
			{
				// ���������Ŀ��������,����
				//if (finish_flag != (BIT(TEST_NUMBER_OF_TESTS)-1))
				{
					// ��������,�������ڵ�
					LED_Set(LED_COM, ON);	
				}
			}
			exit = TRUE;
			break;
		default:
			break;
		}
	} 
	else if (dip_setting != last_dip_setting) 
	{
		cnt = 0;
		finish_flag |= BIT(TEST_232);
		LED_Set(LED_COM, OFF);	// ��һ������,Ҫ��
		last_dip_setting = dip_setting;
		/*
		if (finish_flag == (BIT(TEST_NUMBER_OF_TESTS)-1))
		{
			finish_flag = 0;
		}
		*/
	}
	return exit;
}

//  ----------------------------------------------------------------------------
/// \brief  Indicate software version
/// \return 'true' if exit condition is met, else false.
//  ----------------------------------------------------------------------------
static bool test_flash(void)
{
	bool exit = FALSE;
	static uint8_t test_status = 0;
	static uint32_t fm_test_value = 0x12341234;
	static uint32_t fm_read_value = 0;
	static uint8_t cnt = 0;

	if (dip_setting == DIP_FLASH)
	{
		switch (test_status)
		{
		case 0:
			fm_test_value++;
			write_fm_memory(FM_TEST_WORD, (UINT8 *)&fm_test_value, 4);
			Delay_Ms(5);
			test_status++;
			break;
		case 1:
			read_fm_memory(FM_TEST_WORD, (UINT8 *)&fm_read_value, 4);
			if (fm_test_value != fm_read_value)
			{
				testAlarm |= ERR_FLASH;
			}
			test_status++;
			break;
		case 2:
			if (testAlarm &ERR_FLASH)
			{
				cnt++;
				if (cnt >=4)
				{
					cnt = 4;
					test_ind_item(ERR_FLASH);
				}
				if (cnt < 4)
				{
					test_status = 0;	//������һ��
				}
			}
			else
			{
				// ���������Ŀ��������,����
				//if (finish_flag != (BIT(TEST_NUMBER_OF_TESTS)-1))
				{
					// ��������,�������ڵ�
					LED_Set(LED_COM, ON);	
				}
			}
			exit = TRUE;
			break;
		default:
			break;
		}
	} 
	else if (dip_setting != last_dip_setting) 
	{
		finish_flag |= BIT(TEST_FLASH);
		/*
		if (finish_flag == (BIT(TEST_NUMBER_OF_TESTS)-1))
		{
			finish_flag = 0;
		}*/
		LED_Set(LED_COM, OFF);	// ��һ������,Ҫ��
		last_dip_setting = dip_setting;
	}
	return exit;
}

