#ifndef _DMA_H_
#define _DMA_H_

#include <stdbool.h>

#include <stm32f767xx.h>

typedef enum
{
	receiver,
	transmitter,
	txBufferFiller,
	miniPcCommunicationTransmitter,
	miniPcCommunicationReceiver
} DmaPeripheral;

bool dmaEnabled(DMA_Stream_TypeDef *DMA_Stream);
void disableDma(DMA_Stream_TypeDef *DMA_Stream);
void enableDma(DMA_Stream_TypeDef *DMA_Stream);
void resetDmaFlags(DmaPeripheral dmaPeripheral);
void setupReceiver(void);
void setupTransmitter(void);

#endif	//	_DMA_H_
