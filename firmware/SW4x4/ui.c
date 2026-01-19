/*
 * ui.c
 *
 * Created: 2026/01/13 12:27:47
 *  Author: Chuck
 */ 

#include "config.h"

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "ui.h"

volatile static key_t key_buf;

void keyInit(void)
{
	// pull-up settings
	PORT_SW |= (1<<PIN_SW0);
	PORT_SW |= (1<<PIN_SW1);
	PORT_SW |= (1<<PIN_SW2);
	// input settings
	DDR_SW &= ~(1<<PIN_SW0);
	DDR_SW &= ~(1<<PIN_SW1);
	DDR_SW &= ~(1<<PIN_SW2);
}

void keyScan(void)
{
	uint8_t a, b;

	//a = (~PIND & 0x0F);
	a = SENSE_SW();
	if (a == key_buf.filt) {
		b = key_buf.stat;
		key_buf.stat = a;
		b = (b ^ a) & a;
		if (b) {
			key_buf.cmd = b;
		}
	}
	key_buf.filt = a;

}

bool keyGetEvent(key_event_t *ev)
{
	uint8_t c;

	if (key_buf.cmd == 0) {
		return false;
	} else {
		cli();
		c = key_buf.cmd;
		key_buf.cmd = 0;
		sei();
		ev->key = c;
	}
	
	return true;
}

/*
uint8_t keyGetCmd(void)
{
	uint8_t c;

	cli();
	c = key_buf.cmd;
	key_buf.cmd = 0;
	sei();

	return c;
}
*/

uint8_t keyGetStat(void)
{
	uint8_t c;

	cli();
	c = key_buf.stat;
	sei();
	return c;
}