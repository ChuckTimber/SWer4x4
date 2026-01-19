/*
    Title:    UART library
    Author:   Volker Oth
    Date:     5/1999
    Purpose:  Sending a block of data to uart.
    Software: AVR-GCC to compile, AVA to assemble and link
    Hardware: ATtiny2313
    Note:     To contact me, mail to
                  volkeroth@gmx.de
              You might find more AVR related stuff at my homepage:
                  http://members.xoom.com/volkeroth
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "global.h"
#include "uart.h"


/* uart globals */
#ifdef USE_TX
volatile uint8_t uart_txd_buf_cnt;
static uint8_t *uart_txd_in_ptr, *uart_txd_out_ptr;
static uint8_t uart_txd_buffer[UART_BUF_SIZE];
#endif
static volatile uint8_t uart_rxd_buf_cnt;
static uint8_t *uart_rxd_in_ptr, *uart_rxd_out_ptr;
static uint8_t uart_rxd_buffer[UART_BUF_SIZE];
#ifdef USE_TX
static uint8_t UART_CLR[] = {ESC, '[','H', ESC, '[', '2', 'J',0};
static uint8_t UART_NL[] = {0x0d,0x0a,0};
#endif


/* initialize uart */
void uart_init(void)
{
	/* enable RxD/TxD, receive complete interrupt */
//	UCSRA
//	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);
	UCSRB = (1<<RXCIE)|(1<<RXEN);
//	UCSRC
	/* set baud rate */
	UBRRH = (UART_BAUD_SELECT) / 256; 
	UBRRL = (UART_BAUD_SELECT) % 256; 

	/* set pointers */
#ifdef USE_TX
	uart_txd_in_ptr  = uart_txd_out_ptr = uart_txd_buffer;
	uart_txd_buf_cnt = 0;
#endif
	uart_rxd_in_ptr  = uart_rxd_out_ptr = uart_rxd_buffer;
	uart_rxd_buf_cnt = 0;
}


/* signal handler for uart data buffer empty interrupt */
#ifdef USE_TX
ISR(USART_TX_vect)      
{
	if (uart_txd_buf_cnt > 0) {
		UDR = *uart_txd_out_ptr;			/* write byte to data buffer */
		if (++uart_txd_out_ptr >= uart_txd_buffer + UART_BUF_SIZE) /* Pointer wrapping */
			uart_txd_out_ptr = uart_txd_buffer;
		if(--uart_txd_buf_cnt == 0)			/* if buffer is empty: */
			UCSRB &= ~(1<<UDRIE);			/* disable UDRIE int */
	}
}
#endif

ISR(USART_RX_vect)      
/* signal handler for receive complete interrupt */
{
	*uart_rxd_in_ptr = UDR;					/* read byte from receive register */
	uart_rxd_buf_cnt++;
	if (++uart_rxd_in_ptr >= uart_rxd_buffer + UART_BUF_SIZE) /* Pointer wrapping */
		uart_rxd_in_ptr = uart_rxd_buffer;
}


int16_t uart_getchar(void)
{
	uint8_t c;

	if (uart_rxd_buf_cnt>0) {
		cli();
		uart_rxd_buf_cnt--;
		c = *uart_rxd_out_ptr;				/* get character from buffer */
		if (++uart_rxd_out_ptr >= uart_rxd_buffer + UART_BUF_SIZE) /* pointer wrapping */
			uart_rxd_out_ptr = uart_rxd_buffer;
		sei();
		return c;
	} else {
		return -1;							/* buffer is empty */
	}
}

#ifdef USE_TX
uint8_t uart_putchar(uint8_t c)
{
	if (uart_txd_buf_cnt<UART_BUF_SIZE) {
		cli();
		uart_txd_buf_cnt++;
		*uart_txd_in_ptr = c;				/* put character into buffer */
		if (++uart_txd_in_ptr >= uart_txd_buffer + UART_BUF_SIZE) /* pointer wrapping */
			uart_txd_in_ptr = uart_txd_buffer;
		UCSRB |= (1<<UDRIE);				/* enable UDRIE int */
		sei();
		return 1;
	} else {
		return 0;							/* buffer is full */
	}
}

uint8_t uart_putstr(uint8_t s[])
{
	char *c = s;

	while (*c)
		if (uart_putchar(*c))
			c++;

	return 1;
}


/* Send a 'clear screen' to a VT100 terminal */
void uart_clr(void)
{
	uart_putstr(UART_CLR); 
}


/* Send a 'new line' */
void uart_nl(void)
{
	uart_putstr(UART_NL); 
}
#endif
