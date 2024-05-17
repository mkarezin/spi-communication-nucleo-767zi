#include "init.h"

#include <stm32f767xx.h>

#include "dma.h"
#include "config.h"
#include "spi.h"
#include "functions.h"

static void initializeClock(void);
static void initializeGPIO(void);
static void initializeDMA(void);
static void initializeSPI(void);

void mcuInitialization(void)
{
	initializeClock();
	initializeGPIO();
	initializeDMA();
	initializeSPI();
}

static void initializeClock(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	__NOP(); __NOP();
	
#ifdef DEBUG_MCU_SYSCLK
	RCC->CFGR &= ~RCC_CFGR_MCO2_Msk;
	RCC->CFGR |= RCC_CFGR_MCO2PRE_Msk;
#endif	//	DEBUG_MCU_SYSCLK
	
	RCC->CR |= RCC_CR_HSEBYP;
	RCC->CR |= RCC_CR_HSEON;
	while ((RCC->CR & RCC_CR_HSERDY) != RCC_CR_HSERDY)
		;
	RCC->PLLCFGR = ((2 << RCC_PLLCFGR_PLLR_Pos) | (2 << RCC_PLLCFGR_PLLQ_Pos) |
					(0 << RCC_PLLCFGR_PLLP_Pos) | (216 << RCC_PLLCFGR_PLLN_Pos) |
					(4 << RCC_PLLCFGR_PLLM_Pos) | RCC_PLLCFGR_PLLSRC_HSE);
	PWR->CR1 |= PWR_CR1_VOS_Msk | PWR_CR1_ODEN;
	while ((PWR->CSR1 & PWR_CSR1_ODRDY) != PWR_CSR1_ODRDY)
		;
	PWR->CR1 |= PWR_CR1_ODSWEN;
	while ((PWR->CSR1 & PWR_CSR1_ODSWRDY) != PWR_CSR1_ODSWRDY)
		;
	RCC->CR |= RCC_CR_PLLON;
	while ((RCC->CR & RCC_CR_PLLRDY) != RCC_CR_PLLRDY)
		;
	FLASH->ACR |= (FLASH_ACR_LATENCY_7WS);
	
	RCC->CFGR &= ~(RCC_CFGR_PPRE2_Msk | RCC_CFGR_PPRE1_Msk | RCC_CFGR_HPRE_Msk);
	RCC->CFGR |= (RCC_CFGR_PPRE2_DIV2 | RCC_CFGR_PPRE1_DIV4 | RCC_CFGR_HPRE_DIV1);
	RCC->CFGR |= RCC_CFGR_SW_PLL;
	while ((RCC->CFGR & RCC_CFGR_SWS_PLL) != RCC_CFGR_SWS_PLL)
		;
	
	SystemCoreClockUpdate();
}

static void initializeGPIO(void)
{
	RCC->AHB1ENR |= (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN);
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	__NOP(); __NOP();
	
#ifdef DEBUG_MCU_SYSCLK
	GPIOC->MODER &= ~GPIO_MODER_MODER9_Msk;
	GPIOC->MODER |= GPIO_MODER_MODER9_1;
#endif	//	DEBUG_MCU_SYSCLK
	
	/*
	*	SPI3 pins
	*		PA15 - NSS (GPIO)	+
	*		PB3 - SCK			+
	*		PB4 - MISO			+
	*		PB5 - MOSI			+
	*/
	GPIOA->MODER &= ~(0b11 << GPIO_MODER_MODER15_Pos);
			
	GPIOA->OTYPER &= ~(0b1 << GPIO_OTYPER_OT15_Pos);
	
	GPIOA->OSPEEDR |= (0b11 << GPIO_OSPEEDR_OSPEEDR15_Pos);
	
	GPIOA->PUPDR &= ~(0b11 << GPIO_PUPDR_PUPDR15_Pos);
	GPIOA->PUPDR |= (0b01 << GPIO_PUPDR_PUPDR15_Pos);
	
	SYSCFG->EXTICR[3] |= SYSCFG_EXTICR4_EXTI15_PA;
	EXTI->RTSR |= EXTI_RTSR_TR15;
	enableExtiInterrupt();
	NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	for (uint8_t i = 0; i < 3; i++)
	{
		GPIOB->MODER &= ~(0b11 << (GPIO_MODER_MODER3_Pos + (i * 2)));
		GPIOB->MODER |= (0b10 << (GPIO_MODER_MODER3_Pos + (i * 2)));
		
		GPIOB->OTYPER &= ~(0b1 << (GPIO_OTYPER_OT3_Pos + i));
		
		GPIOB->OSPEEDR &= ~(0b11 << (GPIO_OSPEEDR_OSPEEDR3_Pos + (i * 2)));
		GPIOB->OSPEEDR |= (0b11 << (GPIO_OSPEEDR_OSPEEDR3_Pos + (i * 2)));
		
		GPIOB->PUPDR &= ~(0b11 << (GPIO_PUPDR_PUPDR3_Pos + (i * 2)));
		
		GPIOB->AFR[0] &= ~(15 << (GPIO_AFRL_AFRL3_Pos + (i * 4)));
		GPIOB->AFR[0] |= (6 << (GPIO_AFRL_AFRL3_Pos + (i * 4)));
	}
}

static void initializeDMA(void)
{
	/*
	*	DMA1.Stream2.Channel0 - SPI3_RX
	*	DMA1.Stream5.Channel0 - SPI3_TX
	*/
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
	
	disableDma(MINI_PC_COMMUNICATION_RECEIVER_DMA_STREAM);
	disableDma(MINI_PC_COMMUNICATION_TRANSMITTER_DMA_STREAM);
	
	NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

static void initializeSPI(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;
	
	disableSPI(MINI_PC_COMMUNICATION_INTERFACE);
	MINI_PC_COMMUNICATION_INTERFACE->CR1 = (SPI_CR1_SSM | (SPI_BAUDRATE_PRESCALER_2 << SPI_CR1_BR_Pos));
	MINI_PC_COMMUNICATION_INTERFACE->CR2 |= SPI_CR2_FRXTH;
	enableSPI(MINI_PC_COMMUNICATION_INTERFACE);
}
