 /**
  ******************************************************************************
  * @file    sw_timer.h
  * @author  Jerry
  * @date    17-Oct-2018
  *
  * @brief   �����ʱ������.
  *
  ******************************************************************************
  */
#ifndef SW_TIMER_H_INCLUDED
#define SW_TIMER_H_INCLUDED

//���Ҫ���ڵ���DO������
#define MAX_NUMBER_OF_SW_TIMERS (12U)
#define DIRECTION_NUM			(2)

#define SW_TIMER_UNASSIGNED ((uint8_t) ~0U)

void swt_20_ms_update(void);

uint8_t swt_20_ms_setup( uint8_t direction, uint8_t index, uint16_t timer_start_value);

bool swt_20_ms_check_and_clear(uint8_t dir,const uint8_t t_id);

void swt_20_ms_release(uint8_t dir,uint8_t const t_id);

void swt_20_ms_set(uint8_t dir,uint8_t const t_id, const uint16_t new_timer_value);

#endif // SW_TIMER_H_INCLUDED
