/*
 * lcd.c
 *
 *  Library for Interfacing with Nokia 1100 LCD based on PCF8814 driver.
 *
 *  Init sequence "inspired" by http://sunbizhosting.co.uk/~spiral/1100/
 *
 */

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

#include <stdlib.h>
#include <string.h>
#include "lcd.h"

#if LCD_USE_FONT== 1
#include "font5x8.h"
#endif

#define GLUE(a, b) a##b

#define SET_(what, p, m) GLUE(what, p) |= (1 << (m))
#define CLR_(what, p, m) GLUE(what, p) &= ~(1 << (m))
#define GET_(p, m) GLUE(PIN, p) & (1 << (m))
#define SET(what, x) SET_(what, x)
#define CLR(what, x) CLR_(what, x)

#define LCD_CMD 0 // command
#define LCD_DATA 1 //data to display

#define LCD_MEM_SIZE 864

#if LCD_USE_BUFFER == 1
#define LCD_BUF_SIZE	 ((LCD_X_RES * LCD_Y_RES) / 8)//-12

static uint8_t displayBuf[LCD_BUF_SIZE];

volatile uint16_t markerLow=LCD_BUF_SIZE;
volatile uint16_t markerHigh = 0;

#endif

static uint8_t cursorX;
static uint8_t cursorY;

static struct {
	uint8_t invert:1;
	uint8_t rotate:1;
	uint8_t mirror:1;
} lcdStatus;


//static void lcdCmd(uint8_t command, uint8_t type);
void lcdClr(void);

inline void lcdDelay()
{
	asm volatile ("nop"); //symbolic delay for 1 clock cycle
}

void lcdInit()
{
	// line directions
	SET(DDR, LCD_CE);
	SET(DDR, LCD_MOSI);
	SET(DDR, LCD_RST);
	SET(DDR, LCD_CLK);

	// select lcd
	SET(PORT, LCD_CE);

	// reset lcd
	CLR(PORT,LCD_RST);
	lcdDelay();
	//_delay_ms(15);
	lcdDelay();
	SET(PORT,LCD_RST);

	lcdCmd(0x23, LCD_CMD); // write VOP register [contrast]
	lcdCmd(0x90, LCD_CMD); //
	lcdCmd(0xA4, LCD_CMD); // all on/normal display
	lcdCmd(0x2F, LCD_CMD); // Power control set(charge pump on/off)
	lcdCmd(0x40, LCD_CMD); // set start row address = 0
	lcdCmd(0xb0, LCD_CMD); // set Y-address = 0
	lcdCmd(0x10, LCD_CMD); // set X-address, upper 3 bits =0
	lcdCmd(0x00, LCD_CMD); //
	lcdCmd(0xac, LCD_CMD); // set initial row (R0) of the display
	lcdCmd(0x07, LCD_CMD); //
	lcdCmd(0xaf, LCD_CMD); // display ON/OFF

	lcdCls();				// clear display ram and buffer
}

void lcdCmd(uint8_t command, uint8_t type)
{
	CLR(PORT, LCD_CE); // enable


	CLR(DDR, LCD_CLK);

	lcdDelay();

	// if sending data Control bit must be 1
	if (type == LCD_DATA) {
		SET(PORT, LCD_MOSI);
	} else {
		CLR(PORT, LCD_MOSI);
	}
	SET(DDR, LCD_CLK);
	lcdDelay();

	//send byte
	//MOST significant bit first (MSB -> LSB)
	uint8_t mask;
	for (uint8_t i = 8; i > 0;)
	{
		mask = (1 << --i);

		CLR(PORT, LCD_CLK);
		lcdDelay();

		if (command & mask) {
			SET(PORT, LCD_MOSI);
		} else {
			CLR(PORT, LCD_MOSI);
		}
		SET(PORT, LCD_CLK);
		lcdDelay();
	}
	SET(PORT, LCD_CE);
}

void lcdHome(void)
{
	cursorX = 0;
	cursorY = 0;
	lcdSetX(0);
	lcdSetY(0);
	/*
	 * lcdCmd(0xb0, LCD_CMD); // 1011 0000 set Y-address = 0
	lcdCmd(0x10, LCD_CMD); // 0001 0000 set X-address, upper 3 bits =0
	lcdCmd(0x00, LCD_CMD); // 0000 0000 set X-address, lower 4 bits =0
	*/
}

void lcdCls(void)
{
	lcdHome();
	lcdCmd(0xae, LCD_CMD); // disable display;
	for (int i = 0; i < LCD_MEM_SIZE; i++) {
		lcdCmd(0x00, LCD_DATA);
	}
	lcdCmd(0xaf, LCD_CMD); // enable display;

	lcdHome();
#if LCD_USE_BUFFER == 1
	lcdClr();
	markerLow = LCD_BUF_SIZE;
	markerHigh = 0;
#endif

}

#if LCD_USE_INVERT == 1
void lcdInvert(void)
{
	lcdCmd((lcdStatus.invert)? 0xA6: 0xA7, LCD_CMD);
	lcdStatus.invert = !lcdStatus.invert;

}
#endif // LCD_USE_INVERT

#if LCD_USE_MIRROR == 1
void lcdMirror()
{
	// 1100 A00B mirror Y axis (about X axis) A- mirror, B- mirror last row
	lcdCmd( (lcdStatus.mirror ) ? 0xC0:0xC9, LCD_CMD );
	lcdStatus.mirror = !lcdStatus.mirror;
}
#endif // LCD_USE_MIRROR

#if LCD_USE_ROTATE == 1
void lcdRotate()
{
	//FIXME: on rotate last row is first
	// 1010 000X - Invert screen in horizontal axis
	lcdCmd( (lcdStatus.rotate)? 0xA0 : 0xA1, LCD_CMD );
	lcdStatus.rotate = !lcdStatus.rotate;
}
#endif // LCD_USE_ROTATE

#if LCD_USE_BUFFER == 1
void lcdPositionMem()
{
	uint8_t xaddr =  markerLow % LCD_X_RES;

	if (markerLow > LCD_BUF_SIZE - (LCD_X_RES/8)-1)
		xaddr *=8;

	lcdCmd(0x40, LCD_CMD); // set start row address = 0

	lcdSetX(xaddr);
	lcdSetY(markerLow / LCD_X_RES);
}
#else
void lcdPositionMem()
{
	lcdCmd(0x40, LCD_CMD); // set start row address = 0

	lcdSetX(cursorX);
	lcdSetY(cursorY / 8);
}

#endif

inline void lcdSetX(uint8_t x)
{
	lcdCmd(0x10 | ((x>>4)&0x7), LCD_CMD); // 0001 0000 set X-address, upper 3 bits
	lcdCmd( 0x0 | (x & 0x0f) , LCD_CMD); // 0000 0000 set X-address, lower 4 bits
}

inline void lcdSetY(uint8_t y)
{
	lcdCmd(0xb0 | (y & 0x0f), LCD_CMD); // 1011 0000 set Y-address
}

void lcdUpdate()
{
#if LCD_USE_BUFFER == 1
	if (markerLow > markerHigh)
	{
		return;
	}

	if (markerHigh > LCD_BUF_SIZE-1) markerHigh = LCD_BUF_SIZE-1;
	if (markerLow > LCD_BUF_SIZE-1) markerLow = LCD_BUF_SIZE-1;

	if ((markerLow%LCD_X_RES) == 95)
	{
		//bugfix for a very strange error in addressing 95th column, do not remove
		--markerLow;
	}

	lcdPositionMem();
	for (volatile uint16_t i = markerLow; i <= markerHigh; i++)
	{
		if (i < LCD_BUF_SIZE - (LCD_X_RES/8))
		{
			lcdCmd(displayBuf[i], LCD_DATA);
		}
		else
		{
			/* for last row addressing is a bit different
			only LSB in every byte are used to display data
			*/
			for (uint8_t bit = 0; bit <8; ++bit)
			{
				lcdCmd (  ( displayBuf[i] & (1<<bit)) ? 255:0 , LCD_DATA);
			}

		}

	}
	markerLow = LCD_BUF_SIZE;
	markerHigh = 0;
#else
	return;
#endif
}



static inline char readPGMbyte(char *ptr) {return pgm_read_byte(ptr);};

#if LCD_USE_FONT== 1

#if LCD_USE_BUFFER == 1

void lcdChar(char c, uint8_t mode)
{
	char *fontPtr=(char*)&font[FONT_WIDTH*(c-0x20)];
	char px;
	if (cursorX > LCD_X_RES - FONT_WIDTH)
	{
			cursorX = 0;
			cursorY +=FONT_HEIGHT;
	}

	if (cursorY > LCD_Y_RES - 8)
		cursorY =0;

	for (int x = 0; x < FONT_WIDTH+1; x++)
	{
		if (x == FONT_WIDTH)
			px = 0; // one pixel distance between letters
		else
			px= readPGMbyte(fontPtr++);

		for (uint8_t y = 0; y < 8; ++y)
		{
			if (mode == LCD_XOR)
			{
				if (px & (1<<y))
					lcdPixel(cursorX, cursorY+y, LCD_PIXEL_XOR);
			}
			else
			{
				if (px & (1<<y)) lcdPixel(cursorX, cursorY+y, (mode == LCD_WHITE) ? LCD_PIXEL_OFF : LCD_PIXEL_ON);
				else lcdPixel(cursorX, cursorY+y, (mode == LCD_WHITE) ? LCD_PIXEL_ON : LCD_PIXEL_OFF);
			}
		}
		cursorX++;
	}

}

#else

void lcdChar(char c, uint8_t mode)
{
	char *fontPtr=(char*)&font[5*(c-0x20)];//+(5*(c-32));

	if (cursorX > LCD_X_RES - FONT_WIDTH+1)
	{
			cursorX = 0;
			cursorY +=FONT_HEIGHT;
	}

	if (cursorY > LCD_Y_RES - 8)
		cursorY =0;

	lcdPositionMem();

	char px;

	for (int x = 0; x < FONT_WIDTH+1; x++)
	{
		if (x == FONT_WIDTH)
			px = 0; // one pixel distance between letters
		else
			px= readPGMbyte(fontPtr++);

		if(mode == LCD_WHITE)
		{
			px = ~px;
		}
		lcdCmd(px,LCD_DATA);
		++cursorX;
	}
}

#endif // LCD_USE_BUFFER

void lcdStrPos(uint8_t x, uint8_t y)
{
#if LCD_USE_BUFFER == 0
	y /=8;
#endif
	cursorX = x;
	cursorY = y;
}

// Jump to next line
void lcdNewLine()
{
	cursorY += FONT_HEIGHT;
	cursorX = 0;
}


#if LCD_USE_STR == 1
void lcdStr(char * str, uint8_t mode)
{
	register char chr;
	while ( (chr=*(str++)) )
		lcdChar(chr, mode);
}
#endif // LCD_USE_STR

#if LCD_USE_STR_P == 1
void lcdStr_P(char * str, uint8_t mode)
{
	register char chr;
	while ( (chr=pgm_read_byte(str++)) )
	{
		lcdChar(chr, mode);
	}
}
#endif // LCD_USE_STR_P

#if LCD_USE_INT == 1
void lcdInt(int i, uint8_t mode)
{
	char buf[17];
	lcdStr(itoa(i, buf, 10), mode);
}
#endif // LCD_USE_INT

#endif // LCD_USE_FONT

#if LCD_USE_GRAPHICS == 1

void lcdPixel (uint8_t x, uint8_t y, uint8_t mode)
{
	if ( (x > LCD_X_RES) || (y > LCD_Y_RES))
		return;
#if LCD_USE_BUFFER == 1
	uint16_t index;
	uint8_t offset;
	// every but last row
	if (y < LCD_Y_RES-1)
	{
		index = x+ LCD_X_RES*(y/8);
		offset = y % 8;
	}
	else
	{
		//only 1 bit is used for EVERY column in last row
		index = LCD_X_RES*(y/8) + x / 8;
		offset = x %8;
	}

	uint8_t * ptr = &displayBuf[index];

	switch (mode)
	{
		case LCD_PIXEL_OFF:
			*ptr &= ~(1 << offset);
			break;
		case LCD_PIXEL_XOR:
			*ptr ^= (1 << offset);
			break;
		default:
			*ptr |= (1 << offset);
			break;
	}
	if (markerLow > index)
		markerLow = index;
	if (markerHigh < index)
		markerHigh = index;

#else
	//TODO: implement no buffer // can be done?
#endif
}

#if LCD_USE_LINE == 1

#if LCD_USE_BUFFER == 1
void lcdLine(uint8_t x, uint8_t y, uint8_t len, uint8_t direction, uint8_t mode)
{
	if ( (x >= LCD_X_RES) || (y >= LCD_Y_RES))
		return;

	for (uint8_t i = 0; i < len; ++i)
	{

		lcdPixel(x, y, mode);
		if (direction == LCD_HORIZ)
			++x;
		else ++y;
	}
}

#else

void lcdLine(uint8_t x, uint8_t y, uint8_t len, uint8_t direction, uint8_t mode)
{
	if ( (x >= LCD_X_RES) || (y >= LCD_Y_RES))
		return;

	if (direction == LCD_VERT)
	{
		uint8_t cBuf = 0;
		len += y;
		if (len >= LCD_Y_RES)
			len = LCD_Y_RES-1;

		while (y <= len)
		{
			cBuf |= (1<< (y%8));
			if ( (y %8) == 7)
			{
				lcdSetX(x);
				lcdSetY(y/8);
				lcdCmd(cBuf,LCD_DATA);
				cBuf = 0;
			}
			++y;
		}
		if (cBuf > 0)
		{
			lcdSetX(x);
			lcdSetY(y/8);
			lcdCmd(cBuf,LCD_DATA);
		}

	}
	//horizontal
	else
	{
		lcdSetY(y/8);
		for (uint8_t step=0; step < len; ++step)
		{
			lcdSetX(x++);
			lcdCmd( (1 << y%8), LCD_DATA );
			if (x >= LCD_X_RES)
				return;
		}

	}
}

#endif // LCD_USE_BUFFER
#endif // LCD_USE_LINE

#if LCD_USE_RECT == 1

#if LCD_USE_BUFFER == 1
void lcdRect(uint8_t x, uint8_t y, uint8_t height, uint8_t width, uint8_t color, uint8_t fill)
{
	if ( (x >= LCD_X_RES) || (y >= LCD_Y_RES))
		return;

	if ( (x+width-1 >= LCD_X_RES) || (y+height-1 >= LCD_Y_RES))
		return;

	lcdLine(x,y,width, LCD_HORIZ, color);
	lcdLine(x,y+height,width, LCD_HORIZ, color);

	lcdLine(x,y+1,height-1, LCD_VERT, color);
	lcdLine(x+width-1,y+1,height-1, LCD_VERT, color);
	if (fill != LCD_FILL_NONE)
	{
		for (uint8_t i = 1; i < width-1; ++i )
		{
			lcdLine(x+i, y+1, height-1, LCD_VERT, fill);
		}
	}
}

#else

void lcdRect(uint8_t x, uint8_t y, uint8_t height, uint8_t width, uint8_t color, uint8_t fill)
{
//TODO: Implement
}

#endif // LCD_USE_BUFFER
#endif // LCD_USE_RECT

#if LCD_USE_CIRCLE == 1
void lcdCircle(uint8_t cx, uint8_t cy, uint8_t radius, uint8_t mode, uint8_t fill)
{
	int8_t x, y, xchange, ychange, radiusError, oldX;
	x=radius;
	y=0;
	oldX = x;
	xchange=1-2*radius;
	ychange=1;
	radiusError=0;
	while(x>=y)
	{
		lcdPixel(cx+x, cy+y, mode);
		lcdPixel(cx-x, cy+y, mode);
		lcdPixel(cx-x, cy-y, mode);
		lcdPixel(cx+x, cy-y, mode);

		lcdPixel(cx+y, cy+x, mode);
		lcdPixel(cx-y, cy+x, mode);
		lcdPixel(cx-y, cy-x, mode);
		lcdPixel(cx+y, cy-x, mode);


		if (fill != LCD_FILL_NONE)
		{
			if (y > 1 && oldX != x && x != y)
			{
			lcdLine(cx-x, cy-y+1, 2*y-1, LCD_VERT, fill);
			lcdLine(cx+x, cy-y+1, 2*y-1, LCD_VERT, fill);
			oldX = x;
			}
			lcdLine(cx-y, cy-x+1, 2*x-1, LCD_VERT, fill);
			if (y > 0)lcdLine(cx+y, cy-x+1, 2*x-1, LCD_VERT, fill);

		}

		y++;
		radiusError+=ychange;
		ychange+=2;
		if(2*radiusError+xchange>0)
		{
			x--;
			radiusError+=xchange;
			xchange+=2;
		}
	}
}
#endif // LCD_USE_CIRCLE

#if LCD_USE_BRESENHAMLINE == 1
void lcdBresenhamLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t mode)
{
	int8_t err, e2, sx, sy;
	int8_t dx=abs(x1-x0);
	int8_t dy=abs(y1-y0);
	if(x0<x1) sx=1; else sx=-1;
	if(y0<y1) sy=1; else sy=-1;
	if(dx>dy) err=dx/2; else err=-dy/2;

	while(1)
	{
		lcdPixel(x0, y0, mode);
		if((x0==x1) && (y0==y1)) return;
		e2=err;
		if(e2>-dx)
		{
			err=err-dy;
			x0=x0+sx;
		}
		if(e2<dy)
		{
			err=err+dx;
			y0=y0+sy;
		}
	}
}
#endif // LCD_USE_BRESENHAMLINE
#endif // LCD_USE_GRAPHICS
#if LCD_USE_BUFFER == 1
void lcdClr(void)
{
	memset(displayBuf, 0, LCD_BUF_SIZE);
}
#endif
