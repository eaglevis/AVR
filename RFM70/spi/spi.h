/*
 * spi.h
 *
 *  Created on: 09-09-2011
 *      Author: eagle
 */

#ifndef SPI_H_
#define SPI_H_


#define SPI_ORDER_MSBF 0
#define SPI_ORDER_LSBF 1
uint8_t spiTransfer(uint8_t byte);
void spiSetDataOrder(uint8_t mode);
void spiMasterInit();

#endif /* SPI_H_ */
