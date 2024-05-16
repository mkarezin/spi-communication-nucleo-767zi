#ifndef _SPI_H_
#define _SPI_H_

#include <stddef.h>
#include <stdint.h>

#include <stm32f767xx.h>

typedef struct
{
	struct
	{
		size_t length;
		uint8_t value[1024];
	} buffer;
} CommunicationFrame;

#define SPIFrame	CommunicationFrame

#define SPI_BAUDRATE_PRESCALER_2		(0)
#define SPI_BAUDRATE_PRESCALER_4		(1)
#define SPI_BAUDRATE_PRESCALER_8		(2)
#define SPI_BAUDRATE_PRESCALER_16		(3)
#define SPI_BAUDRATE_PRESCALER_32		(4)
#define SPI_BAUDRATE_PRESCALER_64		(5)
#define SPI_BAUDRATE_PRESCALER_128		(6)
#define SPI_BAUDRATE_PRESCALER_256		(7)

void sendDataSPI(SPI_TypeDef *SPIx, SPIFrame *frame);
void receiveDataSPI(SPI_TypeDef *SPIx, SPIFrame *frame);
uint8_t receiveByteSPI(SPI_TypeDef *SPIx);
void disableSPI(SPI_TypeDef *SPIx);
void enableSPI(SPI_TypeDef *SPIx);

#endif  //  _SPI_H_
