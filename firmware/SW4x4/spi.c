/*
 * spi.c
 *
 * Created: 2026/01/13 12:29:32
 *  Author: Chuck
 */ 

#include <inttypes.h>
#include <avr/io.h>
#include "config.h"
#include "spi.h"

#include <util/delay.h>

/* MCP23S17 is running on BANK.1 mode */
const uint8_t Dev0 = 0x40;
const uint8_t Dev1 = 0x42;
const uint8_t GPA = 0x09;
const uint8_t GPB = 0x19;

void SPI_MasterInit(void)
{
	/* Set MOSI and SCK output, all others input */
	DDR_SPI1A = (1<<DD_SCK1)|(1<<DD_XRST);
	DDR_SPI1B = (1<<DD_MOSI1)|(1<<DD_SS1);
	/* Enable SPI, Master, set clock rate fck/xxx */
	//SPCR1 = (1<<SPE1)|(1<<MSTR1)|(1<<SPR1);
	SPCR1 = (1<<SPE1)|(1<<MSTR1);
	PORT_SPI1A |= (1<<DD_XRST);
	PORT_SPI1B |= (1<<DD_SS1);	
}

char SPI_MasterTransmit(uint8_t cData)
{
	/* Start transmission */
	SPDR1 = cData;
	/* Wait for transmission complete */
	while(!(SPSR1 & (1<<SPIF1)))
		;
	
	return SPDR1;
}

void rstMCP23S17( void )
{
	_delay_us(100);
	SPI_Assert_xrst();
	_delay_us(50);
	SPI_Negate_xrst();
}

void sndMCP23S17( uint8_t cmd, uint8_t addr, uint8_t data )
{
	SPI_Assert_ss();
	SPI_MasterTransmit(cmd);
	SPI_MasterTransmit(addr);
	SPI_MasterTransmit(data);
	SPI_Negate_ss();
}

void initMCP23S17( void )
{
	SPI_MasterInit();
	rstMCP23S17();

	sndMCP23S17( 0x40, 0x0A, 0x88 );

	sndMCP23S17( 0x40, 0x05, 0x88 );
	sndMCP23S17( 0x42, 0x05, 0x88 );

	sndMCP23S17( 0x40, 0x09, 0x00 );
	sndMCP23S17( 0x40, 0x00, 0x00 );
	sndMCP23S17( 0x40, 0x19, 0x00 );
	sndMCP23S17( 0x40, 0x10, 0x00 );

	//--------------------------------
		
	sndMCP23S17( 0x42, 0x09, 0x00 );
	sndMCP23S17( 0x42, 0x00, 0x00 );
	sndMCP23S17( 0x42, 0x19, 0x00 );
	sndMCP23S17( 0x42, 0x10, 0x00 );

	/* test data
		sndMCP23S17( 0x42, 0x09, 0xAA );
		sndMCP23S17( 0x42, 0x19, 0x55 );

		sndMCP23S17( 0x40, 0x09, 0xAA );
		sndMCP23S17( 0x40, 0x19, 0x55 );
		sndMCP23S17( 0x40, 0x19, 0xAA );
		sndMCP23S17( 0x42, 0x19, 0xAA );
	*/		
}

void sndSPI8( uint8_t cDev, uint8_t cPort, uint8_t cData )
{
	sndMCP23S17( cDev, cPort, cData );
}

/*
Dev0 16bit { GPA[7:0], GPB[7:0] } = 
	{  nc,  S3A, S3B, S3D, S1A, S1B, S1C, S1D, 
	   nc,  S2D, S2C, S2A, nc,  S4C, S4B, S4A  }

Dev1 16bit { GPA[7:0], GPB[7:0] } = 
	{  GTA, GTB, GTC, GTD, nc,  P_B, P_G, P_R, 
	   nc,  R_B, R_G, R_R, nc,  F_B, F_G, F_R  }

*/
void sndSPI16( uint8_t cDev, uint16_t iData )
{
	sndMCP23S17( cDev, GPB, (0xff & iData) );
	sndMCP23S17( cDev, GPA, (0xff & (iData>>8)) );
}

void sndSPI16X( uint8_t cDev, uint16_t iData )
{
	sndMCP23S17( cDev, GPA, (0xff & iData) );
	sndMCP23S17( cDev, GPB, (0xff & (iData>>8)) );
}

void sndSPI32( uint32_t lData )
{
	sndMCP23S17( Dev0, GPB, (0xff &  lData)      );
	sndMCP23S17( Dev0, GPA, (0xff & (lData>>8))  );
	sndMCP23S17( Dev1, GPB, (0xff & (lData>>16)) );
	sndMCP23S17( Dev1, GPA, (0xff & (lData>>24)) );
}


void initSPI (void)
{
	initMCP23S17();
	
	sndSPI8( Dev0, GPA, 0x00);
	sndSPI8( Dev0, GPB, 0x00);
	sndSPI8( Dev1, GPA, 0x00);
	sndSPI8( Dev1, GPB, 0x00);

}
