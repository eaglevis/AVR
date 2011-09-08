/*
 * 1100.c
 *
 *  Created on: 31-08-2011
 *      Author: eagle
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>

#include "lcd1100/lcd.h"

#include <util/delay.h>

void setup(void)
{
	lcdInit();
	//lcdRotate();
	//lcdMirror();
}


void demo()
{
	lcdCls();
	for (uint8_t i = 0x20; i <= 128; ++i)
	{
		lcdChar(i, LCD_XOR);
	}

	lcdUpdate();
	_delay_ms(2000);

	lcdStrPos (3,11);
	lcdStr_P(PSTR("Free string"), LCD_WHITE);
	lcdStrPos (22,37);
	lcdStr_P(PSTR("positioning"), LCD_WHITE);
	lcdUpdate();
	_delay_ms(3000);

	lcdCls();
	lcdStr_P(PSTR("Lines, rectangles, circles."), LCD_BLACK);
	lcdUpdate();
	_delay_ms(1000);
	lcdNewLine();
	lcdStr_P(PSTR("Filled or unfilled."), LCD_BLACK);
	lcdUpdate();
	_delay_ms(1000);
	lcdNewLine();
	lcdStr_P(PSTR("Solid or XORed filling, text."), LCD_BLACK);
	lcdUpdate();
	_delay_ms(5000);


	lcdRect(0, 0, LCD_Y_RES, LCD_X_RES, LCD_BLACK, LCD_FILL_BLACK);

	lcdStrPos(36,0);

	lcdStr_P(PSTR("MENU"), LCD_XOR);


	lcdRect(5,7,LCD_Y_RES-8, LCD_X_RES-10,LCD_WHITE, LCD_FILL_WHITE );

	for (int i = 1; i < 8; ++i)
	{
		lcdStrPos(10, i*8);
		lcdInt(i, LCD_BLACK);
		lcdStr_P(PSTR(". Sample"), LCD_BLACK);
		lcdLine(5,8*i-1,LCD_X_RES-10, LCD_HORIZ, LCD_BLACK);
	}

	lcdUpdate();

	_delay_ms(2000);


	lcdCircle(LCD_X_RES/2,LCD_Y_RES/2,30, LCD_XOR, LCD_FILL_XOR);
	lcdUpdate();
	_delay_ms(2000);
	lcdBresenhamLine(0, 0, LCD_X_RES-1, LCD_Y_RES-1, LCD_XOR);
	lcdUpdate();
	_delay_ms(2000);
	lcdBresenhamLine(0, LCD_Y_RES-1, LCD_X_RES-1, 0, LCD_XOR);
	lcdUpdate();
	_delay_ms(5000);

	lcdCls();
	lcdStr_P(PSTR("Per-pixel horizontal fill speed test in:"), LCD_BLACK);lcdUpdate();lcdNewLine();
	_delay_ms(1000);
	lcdStr_P(PSTR("3..."), LCD_BLACK);lcdUpdate();
	_delay_ms(1000);
	lcdStr_P(PSTR("2..."), LCD_BLACK);lcdUpdate();
	_delay_ms(1000);
	lcdStr_P(PSTR("1..."), LCD_BLACK);lcdUpdate();
	_delay_ms(1000);


	for (uint8_t x = 0; x < LCD_X_RES; ++x)
	{
		for (uint8_t y = 0; y < LCD_Y_RES; ++y)
		{

			lcdPixel(x,y,LCD_PIXEL_XOR);
			lcdUpdate();
		}
	}
	_delay_ms(1000);
	lcdNewLine();
	lcdStr_P(PSTR("Vertical in:"), LCD_XOR);lcdUpdate();lcdNewLine();
	_delay_ms(1000);
	lcdStr_P(PSTR("3..."), LCD_XOR);lcdUpdate();
	_delay_ms(1000);
	lcdStr_P(PSTR("2..."), LCD_XOR);lcdUpdate();
	_delay_ms(1000);
	lcdStr_P(PSTR("1..."), LCD_XOR);lcdUpdate();
	_delay_ms(1000);

	for (uint8_t y = 0; y < LCD_Y_RES; ++y)
	{
		for (uint8_t x = 0; x < LCD_X_RES; ++x)
		{

			lcdPixel(x,y,LCD_PIXEL_XOR);
			lcdUpdate();
		}
	}
	_delay_ms(2000);
	lcdCls();
	lcdStr_P(PSTR("Every pixel was filled with it's XORed value and updated."), LCD_XOR);lcdUpdate();
	_delay_ms(5000);
	lcdCls();
	lcdStr_P(PSTR("That's it!"), LCD_XOR);
	lcdNewLine();
	lcdStr_P(PSTR("Thanks!"), LCD_XOR);lcdUpdate();

	_delay_ms(5000);
}
int main()
{
	setup();

	for(;;)
	{
		demo();

	}
	return 1;

}
