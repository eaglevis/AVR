/*
 * main.c
 *
 *  Created on: 08-09-2011
 *      Author: eagle
 */


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "lcd1100/lcd.h"
#include "rfm70/rfm70.h"
#include "spi/spi.h"

#include "common.h"

#include <util/delay.h>

void setup(void)
{

	lcdInit();
	spiMasterInit();
	radioInit();

	SET(DDR,LED_1);
	SET(DDR,LED_2);
	SET(PORT,LED_1);
}


int main()
{
	setup();
	//lcdStr_P(PSTR("RFM70 test"), LCD_BLACK);

	for(uint8_t i=0; i <0x1e;++i )
	{
		lcdInt(radioReadRegister(i), LCD_BLACK);
		lcdChar(',',LCD_BLACK);
	}

	radioSelectBank(1);
	for(uint8_t i=0; i <0x0f;++i )
	{
		lcdInt(radioReadRegister(i), LCD_BLACK);
		lcdChar(',',LCD_BLACK);
	}

	for(;;)
	{

		_delay_ms(1000);
	}
	return 1;

}
