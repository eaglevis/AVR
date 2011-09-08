/*
 * lcd.h
 *	Nokia 1100 LCD support for AVR family
 *  Created on: 31-08-2011
 *      Author: eagle
 */

#ifndef LCD_H_
#define LCD_H_
void lcdCmd(uint8_t command, uint8_t type);
#define LCD_X_RES	 96
#define LCD_Y_RES	 65

/*######################### PORTS AND PINS ##########################*/
#define LCD_CE		D, 0
#define LCD_MOSI	D, 7
#define LCD_RST	D, 5
#define LCD_CLK	B, 0

/*####################### FUNCTIONS TO INCLUDE #######################*/

#define OMIT_ROW_95 0

#define LCD_USE_BUFFER			1

#define LCD_USE_INVERT			1
#define LCD_USE_MIRROR			0
#define LCD_USE_ROTATE			0

#define LCD_USE_FONT 			1	// 0 mean no built-in string functions
#define LCD_USE_STR			1
#define LCD_USE_STR_P			1

#define LCD_USE_GRAPHICS		1	// 0 means no built-in graphics functions INCLUDING lcdPixel
#define LCD_USE_INT			1
#define LCD_USE_LINE			1	// Also needed by rect
#define LCD_USE_RECT			1
#define LCD_USE_CIRCLE			1
#define LCD_USE_BRESENHAMLINE	1



/* !----------------! DO NOT TOUCH ANYTHING BELOW !----------------! */


#define LCD_PIXEL_ON 1
#define LCD_PIXEL_OFF 0
#define LCD_PIXEL_XOR 2

#define LCD_HORIZ 0
#define LCD_VERT 1

#define LCD_BLACK 1
#define LCD_WHITE 0
#define LCD_XOR 2

#define LCD_FILL_NONE 5
#define LCD_FILL_WHITE 0
#define LCD_FILL_BLACK 1
#define LCD_FILL_XOR 2
#define LCD_FILL_ALTERNATING 4

void lcdInit(void);

void lcdSendCommand(char* cmd);
void lcdUpdate(void);
void lcdSetX(uint8_t x);
void lcdSetY(uint8_t y);

void lcdPixel (uint8_t x, uint8_t y, uint8_t mode);
void lcdLine(uint8_t x, uint8_t y, uint8_t len, uint8_t direction, uint8_t mode);
void lcdRect(uint8_t x, uint8_t y, uint8_t height, uint8_t width, uint8_t color, uint8_t fill);
void lcdBresenhamLine(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t mode);
void lcdCircle(uint8_t cx, uint8_t cy, uint8_t radius, uint8_t mode, uint8_t fill);

void lcdChar(char c, uint8_t mode);
void lcdStr(char * str, uint8_t mode);
void lcdStr_P(char * str, uint8_t mode);
void lcdInt(int i, uint8_t mode);
void lcdStrPos(uint8_t x, uint8_t y);
void lcdNewLine(void);

void lcdCls(void);
void lcdInvert(void);
void lcdMirror(void);
void lcdRotate(void);

void lcdHome(void);

#endif /* LCD_H_ */

