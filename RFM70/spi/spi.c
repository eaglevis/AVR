/*
 * spi.c
 *
 *  Created on: 09-09-2011
 *      Author: eagle
 */

#include <avr/io.h>

#include "../common.h"
#include "spi.h"

#define SPIPORT PORTB
#define SPIDDR DDRB

#define MISO	PB4
#define MOSI	PB3
#define SCK	PB5
#define SS		PB2

inline void spiSetSS()
{
	SPIPORT |= (1<<SS);
}

inline void spiClrSS()
{
	SPIPORT &= ~(1<<SS);
}

uint8_t spiTransfer(uint8_t byte)
{
	SPDR = byte;
	while( ! (SPSR & (1<<SPIF) ));
	return SPDR;
}

void spiSetDataOrder(uint8_t mode)
{
	if (mode)
		SPCR &= ~(1<<DORD);
	else
		SPCR |= (1<<DORD);
}

void spiMasterInit()
{
	spiSetSS();

	SPIDDR |= ( (1<<SS) | (1<<MOSI) | (1<<SCK) );

	SPCR |= (1<<SPE) | (1<<MSTR); //master, spi enable
	SPSR |= (1<<SPI2X); // SCK 8 MHz
	SPSR;
	SPDR; // flags reset
}
