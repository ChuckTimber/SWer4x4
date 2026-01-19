/*
 * renc.c
 *
 * Created: 2026/01/14 10:59:40
 *  Author: Chuck
 */ 

//----- Include Files ---------------------------------------------------------
#include "config.h"

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "renc.h"

volatile static uint8_t re_buf_cnt;
volatile static re_t *re_in_ptr, *re_out_ptr;
volatile static re_t re_buffer[RE_BUF_SIZE+1];

volatile static bool re_enable = DISABLE;

void reInit(void)
{
	DDR_RE &= ~(1<<PIN_RE_A);	// for Rotary ENC input
	DDR_RE &= ~(1<<PIN_RE_B);	// for Rotary ENC input
	PORT_RE |= (1<<PIN_RE_A);	// for Rotary ENC input pull-up
	PORT_RE |= (1<<PIN_RE_B);	// for Rotary ENC input pull-up

	reDisable();
}


void reScan (void)
{
	static uint8_t renc;
	//re_t a, b;
	re_t a;

	if (re_enable) {
		// rotary encoder sensing 
		renc = (renc << 2) + SENSE_RE();
		renc &= 0x0F;

		switch (renc) {
		case 0x7:
		case 0xe:
			a = cRE_RightTurn;
			break;
		case 0xb:
		case 0xd:
			a = cRE_LeftTurn;
			break;
		default:
			a = cRE_NoTurn;
			break;
		}

		if (a != cRE_NoTurn) {
			*re_in_ptr++ = a;
			if (re_in_ptr >= re_buffer + RE_BUF_SIZE) re_in_ptr = re_buffer; /* Pointer wrapping */
			++re_buf_cnt;
			if(re_buf_cnt > RE_BUF_SIZE) {
				++re_out_ptr;
				if (re_out_ptr >= re_buffer + RE_BUF_SIZE) re_out_ptr = re_buffer; /* Pointer wrapping */
				re_buf_cnt--;
			}
		}

	} // end of if (re_enable)

}

// Rotary Encoder input available
void reEnable(void)
{
	re_enable = ENABLE;
	re_in_ptr  = re_out_ptr = re_buffer;
	re_buf_cnt = 0;
}

void reDisable(void)
{
	re_enable = DISABLE;
}

bool reGetEvent(re_event_t *ev)
{
	re_t c;

	if (re_buf_cnt == 0)	return false;
	
	if (re_buf_cnt > 0) {
		cli();
		re_buf_cnt--;
		c = *re_out_ptr;				/* get character from buffer */
		if (++re_out_ptr >= re_buffer + RE_BUF_SIZE) re_out_ptr = re_buffer; /* pointer wrapping */
		sei();
		ev->rotation = c;
	}	
	return true;
}

/*
re_t reGetCmd(void)
{
	re_t c;

	if (re_buf_cnt > 0) {
		cli();
		re_buf_cnt--;
		c = *re_out_ptr;				// get character from buffer 
		if (++re_out_ptr >= re_buffer + RE_BUF_SIZE) re_out_ptr = re_buffer; // pointer wrapping 
		sei();
		return c;
	} else {
		return cRE_NoTurn;				// buffer is empty
	}
}
*/