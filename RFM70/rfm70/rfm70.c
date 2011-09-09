/*
 * rfm70.c
 *
 *  Created on: 08-09-2011
 *      Author: eagle
 */
#include <avr/io.h>
#include <avr/pgmspace.h>

#include <util/delay.h>

#include "../common.h"
#include "../spi/spi.h"

#include "rfm70.h"


static uint8_t PROGMEM cmdSwitchBank[] = {0x50, 0x53};
void radioInit()
{
	SET(DDR, R_CSN);
	SET(DDR, R_CE);
	SET(PORT, R_CSN);

	_delay_ms(500);
	radioSelectBank(0);

}

uint8_t radioReadRegister(uint8_t reg)
{
  uint8_t buf;
  SET(PORT, R_CSN);
  CLR(PORT, R_CSN);
  spiTransfer(reg);
  buf=spiTransfer(0);
  SET(PORT, R_CSN);
  return buf;
}

void radioSelectBank(uint8_t bank)
{
	uint8_t status = radioReadRegister(0x07) & (1<<7);
	if (bank)
	{
		if (!status)
			radioWriteCmd_P(cmdSwitchBank, sizeof(cmdSwitchBank));
	}
	else if (status)
			radioWriteCmd_P(cmdSwitchBank, sizeof(cmdSwitchBank));

}


void radioWriteCmd_P(uint8_t * cmd, uint8_t len)
{
	SET(PORT, R_CSN);
	CLR(PORT, R_CSN);
	while(len--) {
		spiTransfer(pgm_read_byte(cmd++));
	};
	SET(PORT, R_CSN);
}

void radioWriteCmd(uint8_t cmd)
{
	SET(PORT, R_CSN);
	CLR(PORT, R_CSN);
	//spiSetDataOrder(SPI_ORDER_MSBF);
	spiTransfer(cmd);
	SET(PORT, R_CSN);
}
