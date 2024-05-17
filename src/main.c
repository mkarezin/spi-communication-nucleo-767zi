#include <string.h>
#include <stdint.h>

#include "init.h"
#include "spi.h"
#include "dma.h"
#include "config.h"

volatile SPIFrame rxFrame, txFrame;
volatile uint8_t skipExtiInterruptDisable = 0;

int main(void)
{
	mcuInitialization();
	
	memset((void *)&rxFrame, 0, sizeof(SPIFrame));
	memset((void *)&txFrame, 0, sizeof(SPIFrame));
	
	MINI_PC_COMMUNICATION_INTERFACE->CR2 |= SPI_CR2_RXDMAEN;
	setupReceiver();
	resetDmaFlags(miniPcCommunicationReceiver);
	enableDma(MINI_PC_COMMUNICATION_RECEIVER_DMA_STREAM);
	
	while (1)
		;
}
