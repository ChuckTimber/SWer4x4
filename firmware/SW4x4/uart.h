/*
    Title:    UART library
    Author:   Volker Oth
    Date:     5/1999
    Purpose:  Sending a block of data to uart.
    needed
    Software: AVR-GCC to compile, AVA to assemble and link
    needed
    Hardware: ATS90S8515 on STK200 board
    Note:     To contact me, mail to
                  volkeroth@gmx.de
              You might find more AVR related stuff at my homepage:
                  http://members.xoom.com/volkeroth
*/

#ifndef UART_H
#define UART_H

#ifndef UBRR
	#define UBRR	UBRR0
	#define UBRRH	UBRR0H
	#define UBRRL	UBRR0L
#endif

#ifndef UCSRA
	#define UCSRA	UCSR0A
	#define RXC		RXC0
	#define TXC		TXC0
	#define UDRE	UDRE0
	#define FE		FE0
	#define DOR		DOR0
	#define UPE		UPE0
	#define U2X		U2X0
	#define MPCM	MPCM0
#endif

#ifndef UCSRB
	#define UCSRB	UCSR0B
	#define RXCIE	RXCIE0
	#define TXCIE	TXCIE0
	#define UDRIE	UDRIE0
	#define RXEN	RXEN0
	#define TXEN	TXEN0
	#define CHR9	CHR90
	#define RXB8	RXB80
	#define TXB8	TXB80
#endif

#ifndef USCSRC
	#define UCSRC	UCSR0C
	#define UMSEL	UMSEL0
	#define UPM		UPM0
	#define USBS	USBS0
	#define UCSZ	UCSZ0
	#define UCPOL	UCPOL0
#endif

#ifndef UDR
	#define UDR		UDR0
#endif

/* set baud rate here */		// for MIDI
#define UART_BAUD_RATE 31250
#define ESC            0x1b
#define UART_BUF_SIZE    16


/* automatically calcuate baud register value */
#define UART_BAUD_SELECT (F_CPU/(UART_BAUD_RATE*16l)-1)


/* prototypes */
extern void uart_init(void);
#ifdef USE_TX
extern void uart_clr(void);
extern void uart_nl(void);
extern uint8_t uart_putstr(uint8_t s[]);
extern uint8_t uart_putchar(uint8_t c);
#endif
extern int16_t  uart_getchar(void);

#endif 
