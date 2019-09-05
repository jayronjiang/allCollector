#ifndef __TESTMODE_H
#define	__TESTMODE_H

#define ERR_485		BIT0
#define ERR_232		BIT1
#define ERR_FLASH	BIT2

//******************************************************************************
// Module constants
//******************************************************************************
enum {
    TEST_ENTRY = 0,	//Test DO
    TEST_232,			// ≤‚ ‘232
    TEST_485,			// ≤‚ ‘485
    TEST_FLASH,		// ≤‚ ‘FLASH∂¡–¥

    TEST_NUMBER_OF_TESTS
};


extern USART_LIST testTxUt;
extern USART_LIST testRxUt;
extern uint16_t testAlarm;
extern uint16_t finish_flag;

void test_indication(void);
void test_mode_start(void);
bool test_mode_update(void);

#endif
