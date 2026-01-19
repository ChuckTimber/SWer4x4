/*
 * SW4x4 main.c
 *
 * Created: 2026/01/09 14:40:51
 * Author : Chuck
 */ 

#include "config.h"

#include <inttypes.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/sleep.h>

#include "timer.h"
#include "spi.h"
#include "ui.h"
#include "renc.h"



typedef enum { 
	COL_0 = 0, 
	COL_R, 
	COL_G, 
	COL_Y, 
	COL_B, 
	COL_M, 
	COL_C, 
	COL_W
} LedColor_t;

/*
Dev0 16bit { GPA[7:0], GPB[7:0] } = 
	{  nc,  S3A, S3B, S3D, S1A, S1B, S1C, S1D, 
	   nc,  S2D, S2C, S2A, nc,  S4C, S4B, S4A  }

Dev1 16bit { GPA[7:0], GPB[7:0] } = 
	{  GTA, GTB, GTC, GTD, nc,  P_B, P_G, P_R, 
	   nc,  R_B, R_G, R_R, nc,  F_B, F_G, F_R  }
*/

typedef enum {
	GTA = 31, 
	GTB = 30,
	GTC = 29,
	GTD = 28,
	nc0 = 27,
	P_B = 26,
	P_G = 25,
	P_R = 24,	// LED slot1
	nc1 = 23,
	R_B = 22,
	R_G = 21,
	R_R = 20,	// LED slot2
	nc2 = 19,
	F_B = 18,
	F_G = 17,
	F_R = 16,	// LED slot3
	nc3 = 15,
	S3A = 14,
	S3B = 13,
	S3D = 12,
	S1A = 11,
	S1B = 10,
	S1C = 9,
	S1D = 8,
	nc4 = 7,
	S2D = 6,
	S2C = 5,
	S2A = 4,
	nc5 = 3,
	S4C = 2,
	S4B = 1,
	S4A = 0,
	LED1 = 24,	// LED slot1
	LED2 = 20,	// LED slot2
	LED3 = 16,	// LED slot3
} BitPos;

#define LED(d, p)	((long)d << p)

#define BIT(pos) (1UL << (pos))

const uint32_t EMPTY = BIT(nc0)|BIT(nc1)|BIT(nc2)|BIT(nc3)|BIT(nc4);
const uint32_t _0AWO = BIT(S1A) | BIT(GTA);
const uint32_t _1AWO = BIT(S2A) | BIT(GTA);
const uint32_t _2AWO = BIT(S3A) | BIT(GTA);
const uint32_t _3AWO = BIT(S4A) | BIT(GTA);
const uint32_t _0BX1 = BIT(S1B) | BIT(GTB);
const uint32_t _2BX1 = BIT(S3B) | BIT(GTB);
const uint32_t _3BX1 = BIT(S4B) | BIT(GTB);
const uint32_t _0CY2 = BIT(S1C) | BIT(GTC);
const uint32_t _1CY2 = BIT(S2C) | BIT(GTC);
const uint32_t _3CY2 = BIT(S4C) | BIT(GTC);
const uint32_t _0DZ3 = BIT(S1D) | BIT(GTD);
const uint32_t _1DZ3 = BIT(S2D) | BIT(GTD);
const uint32_t _2DZ3 = BIT(S3D) | BIT(GTD);

#define BYPASS (_0AWO| LED(COL_G, LED1) | LED(COL_G, LED2) | LED(COL_G, LED3))
#define XLIMIT 6
#define YLIMIT 3

void timer_service(void)
{
	uint8_t tmcnt = getTM0Cnt();
	
	if( tmcnt & 0x01 )
		keyScan();
	else
		reScan();

}

int clamp (int v, int min, int max)
{
	if (v < min) return min;
	if (v > max) return max;
	return v;
}

int main(void)
{

	//static uint16_t	c = 0;
	unsigned char i, j;

	keyInit();
	reInit();
	reEnable();
	initSPI();
	timer0Init();
	

	static uint32_t command_table[XLIMIT+1][YLIMIT+1];
	
	for (i = 0; i < XLIMIT+1; i++ )
		for (j = 0; j < YLIMIT+1; j++)
			command_table[i][j] = EMPTY;
			
	command_table[0][0] = _0AWO                         | LED(COL_G, LED1) | LED(COL_G, LED2) | LED(COL_G, LED3) ;
	command_table[1][0] = _0BX1 | _1CY2 | _2DZ3 | _3AWO | LED(COL_W, LED1) | LED(COL_R, LED2) | LED(COL_G, LED3) ;
	command_table[2][0] = _0BX1 | _1DZ3 | _3CY2 | _2AWO | LED(COL_W, LED1) | LED(COL_G, LED2) | LED(COL_R, LED3) ;
	command_table[3][0] = _0CY2 | _2BX1 | _1DZ3 | _3AWO | LED(COL_R, LED1) | LED(COL_W, LED2) | LED(COL_G, LED3) ;
	command_table[4][0] = _0CY2 | _2DZ3 | _3BX1 | _1AWO | LED(COL_R, LED1) | LED(COL_G, LED2) | LED(COL_W, LED3) ;
	command_table[5][0] = _0DZ3 | _3BX1 | _1CY2 | _2AWO | LED(COL_G, LED1) | LED(COL_W, LED2) | LED(COL_R, LED3) ;
	command_table[6][0] = _0DZ3 | _3CY2 | _2BX1 | _1AWO | LED(COL_G, LED1) | LED(COL_R, LED2) | LED(COL_W, LED3) ;
	command_table[1][1] = _0BX1 | _1CY2 | _2AWO         | LED(COL_W, LED1) | LED(COL_R, LED2) ;
	command_table[2][1] = _0BX1 | _1DZ3 | _3AWO ;
	command_table[3][1] = _0CY2 | _2BX1 | _1AWO ;
	command_table[4][1] = _0CY2 | _2DZ3 | _3AWO ;
	command_table[5][1] = _0DZ3 | _3BX1 | _1AWO ;
	command_table[6][1] = _0DZ3 | _3CY2 | _2AWO ;
	command_table[1][2] = _0BX1 | _1AWO ;
	command_table[2][2] = _0BX1 | _1AWO ;
	command_table[3][2] = _0CY2 | _2AWO ;
	command_table[4][2] = _0CY2 | _2AWO ;
	command_table[5][2] = _0DZ3 | _3AWO ;
	command_table[6][2] = _0DZ3 | _3AWO ;
	command_table[1][3] = _0AWO ;
	command_table[2][3] = _0AWO ;
	command_table[3][3] = _0AWO ;
	command_table[4][3] = _0AWO ;
	command_table[5][3] = _0AWO ;
	command_table[6][3] = _0AWO ;

	typedef struct {
		unsigned char x;
		unsigned char y;
		uint32_t data;
	} Table;
	
	static Table current;

	current.x = 0;
	current.y = 0;
	current.data = BYPASS;


	re_event_t re;
	key_event_t key;

    /* Replace with your application code */
	/*
    while ( c < 0x7ff ) 
    {
		sndSPI8( Dev1, GPB, c & 0x0FF);
		sndSPI8( Dev1, GPA, c++ >> 8);
		for (i = 0; i < 2; ) {	// delay 96msec
			if (getTM0Flg()) {
				i++;
			}
		}
    }
	*/
	sndSPI8( Dev0, GPA, 0x00);
	sndSPI8( Dev0, GPB, 0x00);
	sndSPI8( Dev1, GPA, 0x00);
	sndSPI8( Dev1, GPB, 0x00);

	for (j = 0; j < XLIMIT + 1; j++) {
		sndSPI32( command_table[j][0] );
		for (i = 0; i < 250; ) {	// delay 96msec
			if (getTM0Flg()) {
				i++;
			}
		}

	}


	while (1) {
		sleep_mode();
		timer_service();
		
		if (reGetEvent(&re)) {
			if ( re.rotation == cRE_RightTurn ) {
				current.x = clamp( current.x + 1, 0, XLIMIT );
				current.data = command_table[current.x][current.y];
			} else if ( re.rotation == cRE_LeftTurn ) {
				current.x = clamp( current.x - 1, 0, XLIMIT );
				current.data = command_table[current.x][current.y];
			}
			sndSPI32( current.data );
		}
		if (keyGetEvent(&key)) {
			
		}
	}
}

