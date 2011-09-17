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


#define DEVICE_ADDR 16

#define MAX_PACKET_LEN  32// max value is 32


enum {R_REG = 0, W_REG = 0x20, R_PAYLOAD = 0x21, W_PAYLOAD = 0xa0,
		FLUSH_TX = 0xe1, FLUSH_RX = 0xe2, REUSE_TX_PL = 0xe3,
		ACTIVATE = 0x50, R_RX_PL_WID=0x60, W_ACK_PAYLOAD=0xa8,
		W_TX_PAYLOAD_NOACK=0xb0, NOP= 0xff
} rSpi;

#define READ_REG        		0x00  // Define read command to register
#define WRITE_REG       		0x20  // Define write command to register
#define RD_RX_PLOAD     		0x61  // Define RX payload register address
#define WR_TX_PLOAD     		0xA0  // Define TX payload register address
#define FLUSH_TX        		0xE1  // Define flush TX register command
#define FLUSH_RX        		0xE2  // Define flush RX register command
#define REUSE_TX_PL     		0xE3  // Define reuse TX payload register command
#define W_TX_PAYLOAD_NOACK_CMD	0xb0
#define W_ACK_PAYLOAD_CMD		0xa8
#define ACTIVATE_CMD			0x50
#define R_RX_PL_WID_CMD			0x60
#define NOP_NOP            		0xFF  // Define No Operation, might be used to read status register

// SPI(RFM70) registers(addresses)
#define CONFIG          0x00  // 'Config' register address
#define EN_AA           0x01  // 'Enable Auto Acknowledgment' register address
#define EN_RXADDR       0x02  // 'Enabled RX addresses' register address
#define SETUP_AW        0x03  // 'Setup address width' register address
#define SETUP_RETR      0x04  // 'Setup Auto. Retrans' register address
#define RF_CH           0x05  // 'RF channel' register address
#define RF_SETUP        0x06  // 'RF setup' register address
#define STATUS          0x07  // 'Status' register address
#define OBSERVE_TX      0x08  // 'Observe TX' register address
#define CD              0x09  // 'Carrier Detect' register address
#define RX_ADDR_P0      0x0A  // 'RX address pipe0' register address
#define RX_ADDR_P1      0x0B  // 'RX address pipe1' register address
#define RX_ADDR_P2      0x0C  // 'RX address pipe2' register address
#define RX_ADDR_P3      0x0D  // 'RX address pipe3' register address
#define RX_ADDR_P4      0x0E  // 'RX address pipe4' register address
#define RX_ADDR_P5      0x0F  // 'RX address pipe5' register address
#define TX_ADDR         0x10  // 'TX address' register address
#define RX_PW_P0        0x11  // 'RX payload width, pipe0' register address
#define RX_PW_P1        0x12  // 'RX payload width, pipe1' register address
#define RX_PW_P2        0x13  // 'RX payload width, pipe2' register address
#define RX_PW_P3        0x14  // 'RX payload width, pipe3' register address
#define RX_PW_P4        0x15  // 'RX payload width, pipe4' register address
#define RX_PW_P5        0x16  // 'RX payload width, pipe5' register address
#define FIFO_STATUS     0x17  // 'FIFO Status Register' register address
#define PAYLOAD_WIDTH   0x1f  // 'payload length of 256 bytes modes register address

//interrupt status
#define STATUS_RX_DR 	0x40
#define STATUS_TX_DS 	0x20
#define STATUS_MAX_RT 	0x10

#define STATUS_TX_FULL 	0x01

//FIFO_STATUS
#define FIFO_STATUS_TX_REUSE 	0x40
#define FIFO_STATUS_TX_FULL 	0x20
#define FIFO_STATUS_TX_EMPTY 	0x10

#define FIFO_STATUS_RX_FULL 	0x02
#define FIFO_STATUS_RX_EMPTY 	0x01

uint8_t rInit(void);

uint8_t rReadRegister(uint8_t reg);
void rReadLongRegister(uint8_t reg, uint8_t* buffer, uint8_t len);
void rSelectBank(uint8_t bank);

void rWriteCmd_P(uint8_t * cmd, uint8_t len);
void rWriteCmd(uint8_t cmd);

void rWriteRegister(uint8_t reg, uint8_t val);
void rWriteRegister_P(uint8_t reg, uint8_t * cmd, uint8_t len);

uint8_t rReadPacket(uint8_t *buf);
void rSendPacket(uint8_t * payload, uint8_t len);

void rSetDirection(uint8_t dir);
void rSetPower(uint8_t mode);
void rSetAddrLen(uint8_t len);

void rSetTXAddr(uint8_t * address, uint8_t len);
void rSetRXAddr(uint8_t pipe, uint8_t * address, uint8_t len);
void rSetRXPipeMode(uint8_t pipe, uint8_t mode);



inline void rPowerUp()
{
	rSetPower(1);
}
inline void rPowerDown()
{
	rSetPower(0);
}
inline void rTXmode(void)
{
	rSetDirection(0);
}
inline void rRXmode(void)
{
	rSetDirection(1);
}
inline void rSetChannel(uint8_t channel)
{
	rWriteRegister(5,channel&0x7f);
}
inline void rEnablePipe(uint8_t pipe)
{
	rSetRXPipeMode(pipe, 1);
}
inline void rDisablePipe(uint8_t pipe)
{
	rSetRXPipeMode(pipe, 0);
}

void rSetCE(uint8_t mode);



#endif /* RFM70_H_ */
