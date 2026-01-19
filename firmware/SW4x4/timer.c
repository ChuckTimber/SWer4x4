/*
 * timer.c
 *
 * Created: 2026/01/14 10:16:26
 *  Author: Chuck
 */ 

/*
 * Int every 2msec on 1MHz system clock
 */

#include "config.h"

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

volatile static bool OVF_flag = DISABLE;
volatile static uint8_t OVF_cnt = 0;

ISR(TIMER0_OVF_vect)
{
	OVF_flag = ENABLE;
	OVF_cnt++;
}

/*
8MHz	1 scale       8bit(256) ovf -> 32uS
		64 prescale,  8bit(256) ovf -> 2.048 mS
		256 prescale, 8bit(256) ovf -> 8.192 mS
*/
void timer0Init(void)
{
	// start timer0
	TCNT0 = 0;
	TCCR0B |= DIV256;		// set prescaler by 256; 8MHz/256/256 -> 8msec
	TIMSK0 |= (1<<TOIE0);

	sei();
}

bool getTM0Flg( void )
{
	bool c;
	
	cli();
	c = OVF_flag;
	OVF_flag = DISABLE;
	sei();
	
	return c;
	
}

uint8_t getTM0Cnt( void )
{
	uint8_t t;
	
	cli();
	t = OVF_cnt;
	sei();
	
	return t;
}
