#include "dma.h"

#include <stdbool.h>

#include <stm32f767xx.h>

#include "config.h"
#include "spi.h"

extern volatile SPIFrame rxFrame, txFrame;

bool dmaEnabled(DMA_Stream_TypeDef *DMA_Stream)
{
	return (DMA_Stream->CR & DMA_SxCR_EN);
}

void disableDma(DMA_Stream_TypeDef *DMA_Stream)
{
	DMA_Stream->CR &= ~DMA_SxCR_EN;
	while (DMA_Stream->CR & DMA_SxCR_EN)
		;
}

void enableDma(DMA_Stream_TypeDef *DMA_Stream)
{
	DMA_Stream->CR |= DMA_SxCR_EN;
	while ((DMA_Stream->CR & DMA_SxCR_EN) != DMA_SxCR_EN)
		;
}

void resetDmaFlags(DmaPeripheral dmaPeripheral)
{
	if (dmaPeripheral == miniPcCommunicationTransmitter)
		MINI_PC_COMMUNICATION_TRANSMITTER_DMA_CONTROLLER->HIFCR = 0x00000F40;
	else if (dmaPeripheral == miniPcCommunicationReceiver)
		MINI_PC_COMMUNICATION_RECEIVER_DMA_CONTROLLER->LIFCR = 0x003D0000;
}

void setupReceiver(void)
{
	MINI_PC_COMMUNICATION_RECEIVER_DMA_STREAM->CR = MINI_PC_COMMUNICATION_RECEIVER_DMA_CONFIGURATION;
	MINI_PC_COMMUNICATION_RECEIVER_DMA_STREAM->PAR = (uint32_t)(&MINI_PC_COMMUNICATION_INTERFACE->DR);
	MINI_PC_COMMUNICATION_RECEIVER_DMA_STREAM->M0AR = (uint32_t)(rxFrame.buffer.value);
	MINI_PC_COMMUNICATION_RECEIVER_DMA_STREAM->NDTR = sizeof(rxFrame.buffer.value);
}

void setupTransmitter(void)
{
	MINI_PC_COMMUNICATION_TRANSMITTER_DMA_STREAM->CR = MINI_PC_COMMUNICATION_TRANSMITTER_DMA_CONFIGURATION;
	MINI_PC_COMMUNICATION_TRANSMITTER_DMA_STREAM->PAR = (uint32_t)(&MINI_PC_COMMUNICATION_INTERFACE->DR);
	MINI_PC_COMMUNICATION_TRANSMITTER_DMA_STREAM->M0AR = (uint32_t)(txFrame.buffer.value);
	MINI_PC_COMMUNICATION_TRANSMITTER_DMA_STREAM->NDTR = txFrame.buffer.length;
}
