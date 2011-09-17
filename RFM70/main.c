/*
 * main.c
 *
 *  Created on: 08-09-2011
 *      Author: eagle
 */

#include <string.h>

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "lcd1100/lcd.h"
#include "rfm70/rfm70.h"
#include "spi/spi.h"

#include "common.h"

volatile int8_t waiting;
volatile uint8_t flags;

uint8_t packetBuf[MAX_PACKET_LEN];

ISR(INT0_vect)//, ISR_NOBLOCK)
{
//	//waiting = waiting+1%3;
//	waiting++;
//	if (waiting > 1) return;
//
//	rReadPacket(packetBuf);
//	waiting--;
//	flags = 1;
//	lcdChar('w',LCD_BLACK);
}

uint8_t setup(void)
{
	SET(DDR,LED_1);
	SET(DDR,LED_2);
	SET(DDR,LED_3);

	spiMasterInit();
	uint8_t rinit = rInit();
	//rSetAddrLen(3);
	if (!rinit)
	{
		PORTD = 0xff;
	}


	uint8_t addr[5] = {0xa0, 0xaa, 0xab, 0xac, 0xad};

	uint8_t RX0_Address[]={0x34,0x43,0x10,0x10,0x01};//Receive address data pipe 0
	uint8_t RX1_Address[]={0x39,0x38,0x37,0x36,0xc2};////Receive address data pipe 1

	if (PINC & (1<<PC0))
	{
		SET(PORT,LED_1);
		rSetTXAddr(RX0_Address,sizeof(RX0_Address));
		rSetRXAddr(0,RX0_Address,sizeof(RX0_Address));
		addr[0]++;
		rSetRXAddr(1,RX1_Address,sizeof(RX1_Address));
		if (!rinit)
		{
			PORTD = 0xff;
		}
	}
	else
	{
		lcdInit();

		if (!rinit)
		{
			PORTD = 0xff;
			lcdStr_P(PSTR("ERR!"), LCD_BLACK);
		}
		rSetRXAddr(1,RX0_Address,sizeof(RX0_Address));

		addr[0]++;
		rSetTXAddr(RX1_Address,sizeof(RX1_Address));
		rSetRXAddr(0,RX1_Address,sizeof(RX1_Address));
		SET(PORT,LED_2);
	}



	//EICRA |= (1<<ISC01);
	EIMSK |= (1<<INT0);

	return rinit;
}


int main()
{
	uint8_t set = setup();

	rSelectBank(0);

	//TX
	if (PINC & (1<<PC0))
	{
		rTXmode();
		rPowerUp();
		rSetCE(1);
		uint8_t packet[] = {0x30,0x31,0x32,0x33,0x34,0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,0x3f,0x78};
		for(;set > 0;)
		{
			rSendPacket(packet,sizeof(packet));
			_delay_ms(1000);
		}
	}
	//RX
	else
	{

		lcdInt(rReadRegister(0x1d), LCD_BLACK);
		lcdStr_P(PSTR("RX:"), LCD_BLACK);

		rPowerUp();
		rSetCE(1);
		rRXmode();
		for(;set > 0;)
		{

//			if (flags)
//			{
//				rReadPacket(packetBuf);
//				//process packet
//				lcdInt(packetBuf[0], LCD_BLACK);
//				lcdChar(',', LCD_BLACK);
//				flags=0;
//			}
//			if (waiting--)
//			{
//				//read next packet
//			}
			if (rReadPacket(packetBuf))
			{
				lcdInt(packetBuf[0], LCD_BLACK);
				lcdInt(packetBuf[1], LCD_BLACK);
				lcdChar(',',LCD_BLACK);
			}

			TOG(PORT, LED_1);
			_delay_ms(100);

		}
	}
	return 1;

}
