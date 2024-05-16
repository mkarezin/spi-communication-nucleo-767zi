#include "init.h"

#include <stm32f767xx.h>

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
	RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
	__NOP(); __NOP();
	
#ifdef DEBUG_MCU_SYSCLK
	GPIOC->MODER &= ~GPIO_MODER_MODER9_Msk;
	GPIOC->MODER |= GPIO_MODER_MODER9_1;
#endif	//	DEBUG_MCU_SYSCLK
	
	/*
	*	SPI pins
	*		PA4 - NSS
	*		PA5 - SCK
	*		PA6 - MISO
	*		PA7 - MOSI
	*/
	for (uint8_t i = 0; i < 4; i++)
	{
		GPIOA->MODER &= ~(0b11 << (GPIO_MODER_MODER4_Pos + (i * 2)));
		GPIOA->MODER |= (0b10 << (GPIO_MODER_MODER4_Pos + (i * 2)));
		
		GPIOA->OTYPER &= ~(0b1 << (GPIO_OTYPER_OT4_Pos + i));
		
		GPIOA->OSPEEDR &= ~(0b11 << (GPIO_OSPEEDR_OSPEEDR4_Pos + (i * 2)));
		GPIOA->OSPEEDR |= (0b11 << (GPIO_OSPEEDR_OSPEEDR4_Pos + (i * 2)));
		
		GPIOA->PUPDR &= ~(0b11 << (GPIO_PUPDR_PUPDR4_Pos + (i * 2)));
		
		GPIOA->AFR[0] &= ~(15 << (GPIO_AFRL_AFRL4_Pos + (i * 4)));
		GPIOA->AFR[0] |= (5 << (GPIO_AFRL_AFRL4_Pos + (i * 4)));
	}
}

static void initializeDMA(void)
{
	return;
}

static void initializeSPI(void)
{
	return;
}
