#ifndef CONFIG_H_

#define CONFIG_H_

#define F_CPU	8000000L	// 8MHz CPU clok

#define DDR_SPI1A	DDRC
#define DDR_SPI1B	DDRE
#define DD_MISO1	DDRC0
#define DD_MOSI1	DDRE3
#define DD_SCK1		DDRC1
#define DD_SS1		DDRE2
#define DD_XRST		DDRC2
#define PORT_SPI1A	PORTC
#define PORT_SPI1B	PORTE

#define SPI_Assert_xrst()	PORT_SPI1A &= ~(1<<DD_XRST)
#define SPI_Negate_xrst()	PORT_SPI1A |= (1<<DD_XRST)
#define SPI_Assert_ss()		PORT_SPI1B &= ~(1<<DD_SS1)
#define SPI_Negate_ss()		PORT_SPI1B |= (1<<DD_SS1)


#define DDR_RE		DDRD
#define PORT_RE		PORTD
#define PIN_RE		PIND
#define PIN_RE_A	PIND2
#define PIN_RE_B	PIND3

#define SENSE_RE()	((~PIN_RE & ((1<<PIN_RE_A)|(1<<PIN_RE_B))) >> PIN_RE_A) 

#define DDR_SW		DDRD
#define PIN_SW		PIND
#define PORT_SW		PORTD
#define PIN_SW0		PIND4
#define PIN_SW1		PIND5
#define PIN_SW2		PIND6

#define SENSE_SW()	((~PIN_SW & ((1<<PIN_SW0)|(1<<PIN_SW1)|(1<<PIN_SW2))) >>PIN_SW0)

#define FALSE 0
#define TRUE -1
#define DISABLE 0
#define ENABLE -1
#define LOW 0
#define HIGH -1


/*
CS0[2] CS0[1] CS0[0]	Description
0		1		0		clkI/O/8 (from prescaler)
0		1		1		clkI/O/64 (from prescaler)
1		0		0		clkI/O/256 (from prescaler)
1		0		1		clkI/O/1024 (from prescaler)
1		1		0		External clock source on T0 pin. Clock on falling edge.
1		1		1		External clock source on T0 pin. Clock on rising edge.

8MHz	1 scale       8bit(256) ovf -> 32uS
		64 prescale,  8bit(256) ovf -> 2.048 mS
		256 prescale, 8bit(256) ovf -> 8.192 mS
*/

#define DIV1	0
#define DIV8	(1<<CS01)
#define DIV64	(1<<CS01)|(1<<CS00)
#define DIV256	(1<<CS02)

#else


#endif
