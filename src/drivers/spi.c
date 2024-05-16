#include "spi.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>

#include <stm32f767xx.h>

void sendDataSPI(SPI_TypeDef *SPIx, SPIFrame *frame)
{
	uint8_t byte = 0;
	
    for (size_t i = 0; i < frame->buffer.length; i++)
    {
        while ((SPIx->SR & SPI_SR_TXE) != SPI_SR_TXE)
            ;
        *((volatile uint8_t *)(&SPIx->DR)) = frame->buffer.value[i];
		
		while ((SPIx->SR & SPI_SR_RXNE) != SPI_SR_RXNE)
			;
		
		byte = (uint8_t)(SPIx->DR);
		(void)byte;
    }
	
	while ((SPIx->SR & SPI_SR_TXE) != SPI_SR_TXE)
		;
}

void receiveDataSPI(SPI_TypeDef *SPIx, SPIFrame *frame)
{
	for (size_t i = 0; i < frame->buffer.length; i++)
		frame->buffer.value[i] = receiveByteSPI(SPIx);
}

uint8_t receiveByteSPI(SPI_TypeDef *SPIx)
{
	uint8_t byte = 0;
	
	while ((SPIx->SR & SPI_SR_RXNE) != SPI_SR_RXNE)
		;
	byte = (uint8_t)(SPIx->DR);
	
	while ((SPIx->SR & SPI_SR_TXE) != SPI_SR_TXE)
		;
	*((volatile uint8_t *)(&SPIx->DR)) = (uint8_t)0xFF;
	
	return byte;
}

void disableSPI(SPI_TypeDef *SPIx)
{
	while ((SPIx->SR & SPI_SR_TXE) != SPI_SR_TXE)
		;
	while ((SPIx->SR & SPI_SR_BSY) == SPI_SR_BSY)
		;
	SPIx->CR1 &= ~SPI_CR1_SPE;
}

void enableSPI(SPI_TypeDef *SPIx)
{
	SPIx->CR1 |= SPI_CR1_SPE;
}
