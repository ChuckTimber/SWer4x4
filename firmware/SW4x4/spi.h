/*
 * spi.h
 *
 * Created: 2026/01/13 12:29:32
 *  Author: Chuck
 */ 

#ifndef SPI_H_

#define SPI_H_

extern const uint8_t Dev0;
extern const uint8_t Dev1;
extern const uint8_t GPA;
extern const uint8_t GPB;

void sndSPI8 ( uint8_t cDev, uint8_t cPort, uint8_t cData );
void sndSPI16 ( uint8_t cDev, uint16_t iData );
void sndSPI32 ( uint32_t lData );
void initSPI (void);

#endif
