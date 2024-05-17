#include "interrupts.h"

#include <stdint.h>
#include <string.h>

#include <stm32f767xx.h>

#include "config.h"
#include "functions.h"
#include "dma.h"
#include "spi.h"

extern volatile uint8_t skipExtiInterruptDisable;
extern volatile SPIFrame rxFrame, txFrame;

void EXTI15_10_IRQHandler(void)
{
	if (EXTI->PR & EXTI_PR_PR15)
	{
		EXTI->PR |= EXTI_PR_PR15;
		
		if (skipExtiInterruptDisable)
		{
			skipExtiInterruptDisable = 0;
			
			return;
		}
		
		disableDma(MINI_PC_COMMUNICATION_RECEIVER_DMA_STREAM);
		disableExtiInterrupt();
		while (MINI_PC_COMMUNICATION_INTERFACE->SR & SPI_SR_BSY)
			;
		MINI_PC_COMMUNICATION_INTERFACE->CR2 &= ~SPI_CR2_RXDMAEN;
		rxFrame.buffer.length = (sizeof(rxFrame.buffer.value)) - MINI_PC_COMMUNICATION_RECEIVER_DMA_STREAM->NDTR;
		disableDma(MINI_PC_COMMUNICATION_TRANSMITTER_DMA_STREAM);
		memcpy((void *)txFrame.buffer.value, (const void *)rxFrame.buffer.value, rxFrame.buffer.length);
		txFrame.buffer.length = rxFrame.buffer.length;
		setupTransmitter();
		resetDmaFlags(miniPcCommunicationTransmitter);
		enableDma(MINI_PC_COMMUNICATION_TRANSMITTER_DMA_STREAM);
		MINI_PC_COMMUNICATION_INTERFACE->CR2 |= SPI_CR2_TXDMAEN;
	}
}

void DMA1_Stream5_IRQHandler(void)
{
	if (MINI_PC_COMMUNICATION_TRANSMITTER_DMA_CONTROLLER->HISR & DMA_HISR_TCIF5)
	{
		disableDma(MINI_PC_COMMUNICATION_TRANSMITTER_DMA_STREAM);
		while (MINI_PC_COMMUNICATION_INTERFACE->SR & SPI_SR_BSY)
			;
		MINI_PC_COMMUNICATION_INTERFACE->CR2 &= ~SPI_CR2_TXDMAEN;
		disableDma(MINI_PC_COMMUNICATION_RECEIVER_DMA_STREAM);
		setupReceiver();
		resetDmaFlags(miniPcCommunicationReceiver);
		enableDma(MINI_PC_COMMUNICATION_RECEIVER_DMA_STREAM);
		MINI_PC_COMMUNICATION_INTERFACE->CR2 |= SPI_CR2_RXDMAEN;
		enableExtiInterrupt();
	}
	
	resetDmaFlags(miniPcCommunicationTransmitter);
}
