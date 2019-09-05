 /**
  ******************************************************************************
  * @file    sw_timer.c
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   软件定时器处理.
  *
  ******************************************************************************
  */
#include "include.h"


//******************************************************************************
// Module constants
//******************************************************************************

//******************************************************************************
// Module variables
//******************************************************************************
static uint16_t timer_value[DIRECTION_NUM][MAX_NUMBER_OF_SW_TIMERS] = 
{
	{0,0,0,0,0,0,0,0,0,0,0,0,},{0,0,0,0,0,0,0,0,0,0,0,0,}
};

static bool timer_flag[DIRECTION_NUM][MAX_NUMBER_OF_SW_TIMERS] = 
{
	{0,0,0,0,0,0,0,0,0,0,0,0,},{0,0,0,0,0,0,0,0,0,0,0,0,}
};
static bool timer_assigned[DIRECTION_NUM][MAX_NUMBER_OF_SW_TIMERS] = 
{
	{0,0,0,0,0,0,0,0,0,0,0,0,},{0,0,0,0,0,0,0,0,0,0,0,0,}
};

//------------------------------------------------------------------------------
// Function:    swt_500_ms_setup
//------------------------------------------------------------------------------
// Description: Sets up a software timer to go off after the time passed as
//              parameter.
// Input:       - timer_start_value: time before reaching 0, in ticks of 500 ms.
// Output:      - the timer_id used for future access to the timer allocated.
//                All ones if the timer allocation failed.
//------------------------------------------------------------------------------
uint8_t swt_20_ms_setup( uint8_t dir, uint8_t t_id, uint16_t timer_start_value)
{
	bool slot_found = FALSE;

	if ((dir < DIRECTION_NUM) && (t_id < MAX_NUMBER_OF_SW_TIMERS)) 
	{
		// A slot is empty if the timer value is 0 and the flag is not pending.
		if (timer_assigned[dir][t_id] == FALSE) 
		{
			slot_found = TRUE;
			timer_assigned[dir][t_id] = TRUE;
			timer_value[dir][t_id] = timer_start_value;
			timer_flag[dir][t_id] = FALSE;
		}
	}
	if (slot_found) 
	{
		return (t_id);
	} 
	else 
	{
		return (uint8_t) SW_TIMER_UNASSIGNED;
	}
}


//------------------------------------------------------------------------------
// Function:    swt_500_ms_update
//------------------------------------------------------------------------------
// Description: Update all active software timer on 500 ms. Must be called every
//              500 ms.
//------------------------------------------------------------------------------
void swt_20_ms_update(void)
{
	uint8_t dir = 0, i = 0;
	for (dir = 0; dir < DIRECTION_NUM; dir++)
	{
		for (i = 0; i < MAX_NUMBER_OF_SW_TIMERS; i++) 
		{
			if (timer_value[dir][i] > 0) 
			{
				timer_value[dir][i]--;
				if (timer_value[dir][i] == 0) 
				{
					timer_flag[dir][i] = TRUE;
				}
			}
		}
	}
}


//------------------------------------------------------------------------------
// Function:    swt_500_ms_check_and_clear
//------------------------------------------------------------------------------
// Description: Check if the timer which ID was passed as parameter went off.
//              If it did, reset the flag, thus releasing the timer.
// Input:       - the timer ID returned when allocating the timer.
// Output:      - true if the timer went off, false otherwise.
//------------------------------------------------------------------------------
bool swt_20_ms_check_and_clear(uint8_t dir,const uint8_t t_id)
{
	if ((t_id < MAX_NUMBER_OF_SW_TIMERS) \
	    && (dir < DIRECTION_NUM) &&timer_assigned[dir][t_id]) 
	{
		bool flag = timer_flag[dir][t_id];
		/*测试时会出现脉冲下不来的情况*/
		/*这是因为上一句判断后,小概率执行了中断置位*/
		/*置位后又马上被下一句清清掉了,导致一直无法返回true*/
		/*这里不清，release里面会被清掉*/
		//timer_flag[dir][t_id] = FALSE;
		return flag;
	} 
	else 
	{
		return FALSE;
	}
}


//------------------------------------------------------------------------------
// Function:    swt_500_ms_release
//------------------------------------------------------------------------------
// Description: Release the timer which ID was passed as parameter, by zeroing
//              its value and its flag, and setting the ID to unassigned.
// Input:       - pointer to the ID of the timer to release.
//------------------------------------------------------------------------------
void swt_20_ms_release(uint8_t dir,uint8_t const t_id)
{
	if ((t_id < MAX_NUMBER_OF_SW_TIMERS) \
	     && (dir < DIRECTION_NUM) && timer_assigned[dir][t_id]) 
	{
		timer_value[dir][t_id] = 0;
		timer_flag[dir][t_id] = FALSE;
		timer_assigned[dir][t_id] = FALSE;
		//*p_timer_id = SW_TIMER_UNASSIGNED;
	}
}


//------------------------------------------------------------------------------
// Function:    swt_500_ms_set
//------------------------------------------------------------------------------
// Description: Set the timer value of a timer that is already allocated,
//              or allocate a new timer for that if not done already.
// Input:       - pointer to the ID of the timer to set. Updated to the assigned
//              ID, if the timer was not already assigned.
//              - the new value to set the timer to, in ticks of 500 ms.
//-----------------------------------------------------------------------------
void swt_20_ms_set(uint8_t dir,uint8_t const t_id, const uint16_t new_timer_value)
{
	if ((t_id < MAX_NUMBER_OF_SW_TIMERS) \
	     && (dir < DIRECTION_NUM) && timer_assigned[dir][t_id] )
	{
		timer_value[dir][t_id] = new_timer_value;
		timer_flag[dir][t_id] = FALSE;
	}
	else 
	{
		swt_20_ms_setup(dir, t_id, new_timer_value);
	}
}


//******************************************************************************
// Internal functions
//******************************************************************************
