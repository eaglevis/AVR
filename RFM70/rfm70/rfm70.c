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
// { register. data to write }
static uint8_t PROGMEM dataBank0[][2] = /*{
		{0, 0x38}, // mask_rt|en_crc|1byte
		//{0, 0x0C}, // en_crc|2byte
		{1, 0x3f}, // auto_ack 5..0
		{2, 0x03}, // enable RX pipes 0..1
		{3, 0x03}, // address field len
		{4, 0x00}, // no retransmit
		{5, 0x15}, // 2421 MHz
		{6, 0x37}, // 1MBps | max power
		{8, 0x00}, // reset counters
		{9, 0x00}, // no carrier detect
//		{11, 32 }, // payload len per pipe
//		{12, 32 },
//		{13, 32 },
//		{14, 32 },
//		{15, 32 },
//		{16, 32 },
		{28, 0x3f}, // dyn payload len for all pipes
		{29, 0x07}, // enable dyn payload len, dyn ack, ack
};*/
		{
{0x00,0x0F},//reflect RX_DR\TX_DS\MAX_RT,Enable CRC ,2byte,POWER UP,PRX
{0x01,0x3F},//Enable auto acknowledgement data pipe5\4\3\2\1\0
{0x02,0x3F},//Enable RX Addresses pipe5\4\3\2\1\0
{0x03,0x03},//RX/TX address field width 5byte
{0x04,0xff},//auto retransmission dalay (4000us),auto retransmission count(15)
{0x05,0x17},//23 channel
{0x06,0x17},//air data rate-1M,out power 0dbm,setup LNA gain
{0x07,0x07},//
{0x08,0x00},//
{0x09,0x00},
{0x0c,0xc3},//only LSB Receive address data pipe 2, MSB bytes is equal to RX_ADDR_P1[39:8]
{0x0d,0xc4},//only LSB Receive address data pipe 3, MSB bytes is equal to RX_ADDR_P1[39:8]
{0x0e,0xc5},//only LSB Receive address data pipe 4, MSB bytes is equal to RX_ADDR_P1[39:8]
{0x0f,0xc6},//only LSB Receive address data pipe 5, MSB bytes is equal to RX_ADDR_P1[39:8]
{0x11,0x20},//Number of bytes in RX payload in data pipe0(32 byte)
{0x12,0x20},//Number of bytes in RX payload in data pipe1(32 byte)
{0x13,0x20},//Number of bytes in RX payload in data pipe2(32 byte)
{0x14,0x20},//Number of bytes in RX payload in data pipe3(32 byte)
{0x15,0x20},//Number of bytes in RX payload in data pipe4(32 byte)
{0x16,0x20},//Number of bytes in RX payload in data pipe5(32 byte)
{0x17,0x00},//fifo status
{0x1c,0x3F},//Enable dynamic payload length data pipe5\4\3\2\1\0
{0x1d,0x07}//Enables Dynamic Payload Length,Enables Payload with ACK,Enables the W_TX_PAYLOAD_NOACK command
};

static uint8_t PROGMEM dataBank1[][4]={
  {0x40, 0x4b, 0x01, 0xe2},
  {0xc0, 0x4b, 0x00, 0x00},
  {0xd0, 0xfc, 0x8c, 0x02},
  {0x99, 0x00, 0x39, 0x41},
  {0xd9, 0x9e, 0x86, 0x0b},
  {0x24, 0x06, 0x7f, 0xa6},
  {0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00},
  {0x00, 0x00, 0x00, 0x00},
  {0x00, 0x12, 0x73, 0x00},
  {0x36, 0xB4, 0x80, 0x00},
};


static uint8_t PROGMEM rampCurve[]={0x41,0x20,0x08,0x04,0x81,0x20,0xcf,0xf7,0xfe,0xff,0xff};

static uint8_t PROGMEM cmdSwitchBank[] = {0x50, 0x53};

static uint8_t PROGMEM cmdActivate[] = {0x50, 0x73};


static PROGMEM uint8_t bank0Reg4_tog1[]={0xd9 | 0x06, 0x9e, 0x86, 0x0b}; //assosciated with set1[4]!
static PROGMEM uint8_t bank0Reg4_tog2[]={0xd9 & ~0x06, 0x9e, 0x86, 0x0b};
uint8_t rInit()
{
	uint8_t i;

	SET(DDR, R_CSN);
	SET(DDR, R_CE);
	SET(PORT, R_CSN);


	_delay_ms(200);
	rSelectBank(0);


	for (i = 0; i < 23; i++)
	{
		rWriteRegister(pgm_read_byte(&dataBank0[i][0]), pgm_read_byte(&dataBank0[i][1]));
	}

	if (rReadRegister(0x1d) == 0)
	{
		rWriteCmd_P(cmdActivate, sizeof(cmdActivate));
	}

	rSelectBank(1);

	for(i=0;i<0x0e;++i) {
		rWriteRegister_P(i,dataBank1[i],4);
    }
	rWriteRegister_P(0x0e,rampCurve,sizeof(rampCurve));
	rWriteRegister_P(4,bank0Reg4_tog1,sizeof(bank0Reg4_tog1));
	rWriteRegister_P(4,bank0Reg4_tog2,sizeof(bank0Reg4_tog2));
	_delay_ms(50);
	//Check the ChipID
	if (rReadRegister(8) != 0x63)
	{
		return 0;
	}
	rSelectBank(0);
	return 1;
}

uint8_t rReadPacket(uint8_t *buf)
{
	uint8_t status = rReadRegister(STATUS);
	uint8_t len, fifo_status;
	if((STATUS_RX_DR&status) == 0x40)				// if receive data ready (RX_DR) interrupt
	{
		SET(PORT,LED_3);

		do
		{
			len = rReadRegister(R_RX_PL_WID_CMD);
			if(len<=MAX_PACKET_LEN)
			{
				rReadLongRegister(RD_RX_PLOAD, buf, len );
			}

			else
			{
				rWriteRegister(FLUSH_RX,0);
			}
			fifo_status = rReadRegister(FIFO_STATUS);

		}while((fifo_status&FIFO_STATUS_RX_EMPTY)==0); //while not empty

		rWriteRegister(0x07, status); //reset flags by writing 1

		_delay_ms(50);
		CLR(PORT,LED_3);
		return 1;
	}

	return 0;
}


void rSendPacket(uint8_t * payload, uint8_t len)
{
	TOG(PORT,LED_2);
	//rTXmode();
	uint8_t status;
	do
	{
		status = rReadRegister(FIFO_STATUS);
	} while (status & FIFO_STATUS_TX_FULL); //tx full


	SET(PORT,R_CSN);
	CLR(PORT, R_CSN);
	spiTransfer(WR_TX_PLOAD); //TX w/ ACK

	/*while (len--)
	{
		TOG(PORT,LED_3);
		spiTransfer(*(payload++));
	}*/
	for (uint8_t i = 0; i < len; ++i)
	{
		spiTransfer(payload[i]);
	}

	TOG(PORT,LED_3);

	SET(PORT,R_CSN);


}

uint8_t rReadRegister(uint8_t reg)
{
  uint8_t buf;
  SET(PORT, R_CSN);
  CLR(PORT, R_CSN);
  spiTransfer(reg);
  buf=spiTransfer(0);
  SET(PORT, R_CSN);
  return buf;
}

void rReadLongRegister(uint8_t reg, uint8_t* buffer, uint8_t len)
{
  SET(PORT, R_CSN);
  CLR(PORT, R_CSN);
  spiTransfer(reg);
  while(len--)
  {
	  *(buffer++)=spiTransfer(0);
  }
  SET(PORT, R_CSN);
}

void rSelectBank(uint8_t bank)
{
	uint8_t status = rReadRegister(NOP_NOP) & (1<<7);
	if (bank)
	{
		if (!status)
			rWriteCmd_P(cmdSwitchBank, sizeof(cmdSwitchBank));
	}
	else if (status)
			rWriteCmd_P(cmdSwitchBank, sizeof(cmdSwitchBank));
}


void rWriteCmd_P(uint8_t * cmd, uint8_t len)
{
	SET(PORT, R_CSN);
	CLR(PORT, R_CSN);
	while(len--) {
		spiTransfer(pgm_read_byte(cmd++));
	};
	SET(PORT, R_CSN);
}

void rWriteCmd(uint8_t cmd)
{
	SET(PORT, R_CSN);
	CLR(PORT, R_CSN);
	//spiSetDataOrder(SPI_ORDER_MSBF);
	spiTransfer(cmd);
	SET(PORT, R_CSN);
}

void rWriteRegister(uint8_t reg, uint8_t val)
{
	SET(PORT, R_CSN);
	CLR(PORT, R_CSN);
	spiTransfer(reg | WRITE_REG);
	//spiTransfer(WRITE_REG | reg);
	spiTransfer(val);
	SET(PORT, R_CSN);
}

void rWriteRegister_P(uint8_t reg, uint8_t *cmd, uint8_t len)
{
	SET(PORT, R_CSN);
	CLR(PORT, R_CSN);
	spiTransfer(reg | WRITE_REG);
	while(len--) {
		spiTransfer(pgm_read_byte(cmd++));
	};
	SET(PORT, R_CSN);
}

void rSetDirection(uint8_t dir)
{
	rSelectBank(0);

	rWriteRegister((dir)? FLUSH_RX: FLUSH_TX,0);//flush

	uint8_t reg = rReadRegister(STATUS);

	rWriteRegister(STATUS, reg);

	rSetCE(0);
	reg = rReadRegister(0);
	reg = (dir) ? reg| 1 : reg & ~1;
	rWriteRegister(0, reg);
	rSetCE(1);
}

void rSetPower(uint8_t mode)
{
	rSelectBank(0);
	uint8_t reg = rReadRegister(0);
	reg = (mode) ? reg| (1<<1) : reg & ~(1<<1);
	rWriteRegister(0, reg);
}

void rSetRXAddr(uint8_t pipe, uint8_t * address, uint8_t len)
{
//	if (pipe > 5)
//		return;
//
//	if (pipe > 1)
//		len = 1; //pipes 1..5 share all bytes but LSB
	rSelectBank(0);
	SET(PORT, R_CSN);
	CLR(PORT, R_CSN);
	spiTransfer((0x0a+pipe) | 0x20);
	while (len--)
	{
		spiTransfer(*(address++));
	}
	SET(PORT,R_CSN);
}

void rSetTXAddr(uint8_t * address, uint8_t len)
{
	rSelectBank(0);
	//SET(PORT, R_CSN);
	CLR(PORT, R_CSN);
	spiTransfer(0x10 | 0x20);
	//while (len--)
	while (len--)
	{
		spiTransfer(*(address++));
	}
	SET(PORT,R_CSN);
}

// Set address width. Only values between 3 and 5
void rSetAddrLen(uint8_t len)
{
	rSelectBank(0);
	rWriteRegister(3,len-2);
}

void rSetRXPipeMode(uint8_t pipe, uint8_t mode)
{
	rSelectBank(0);
	uint8_t reg = rReadRegister(2);
	reg = (mode) ? reg| (1<<pipe) : reg & ~(1<<pipe);
	rWriteRegister(2, reg);
}

inline void rSetCE(uint8_t mode)
{
	if (mode)
	{
		SET(PORT, R_CE);
	}
	else
		CLR(PORT, R_CE);
}
