/*
 * rfm70.h
 *
 *  Created on: 08-09-2011
 *      Author: eagle
 */

#ifndef RFM70_H_
#define RFM70_H_

#define R_MISO	B, 4
#define R_MOSI	B, 3
#define R_SCK	B, 5
#define R_CE	D, 1
#define R_CSN	D, 0
#define R_IRQ	D, 2



enum {R_REG = 0, W_REG = 0x20, R_PAYLOAD = 0x21, W_PAYLOAD = 0xa0,
		FLUSH_TX = 0xe1, FLUSH_RX = 0xe2, REUSE_TX_PL = 0xe3,
		ACTIVATE = 0x50, R_RX_PL_WID=0x60, W_ACK_PAYLOAD=0xa8,
		W_TX_PAYLOAD_NOACK=0xb0, NOP= 0xff
} radioSpi;

void radioInit(void);

uint8_t radioReadRegister(uint8_t reg);
void radioSelectBank(uint8_t bank);

void radioWriteCmd_P(uint8_t * cmd, uint8_t len);
void radioWriteCmd(uint8_t cmd);


#endif /* RFM70_H_ */
