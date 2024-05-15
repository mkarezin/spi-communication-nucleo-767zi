#include "init.h"

#include <stm32f767xx.h>

static void initializeClock(void);
static void initializeGPIO(void);

void mcuInitialization(void)
{
	initializeClock();
	initializeGPIO();
}

static void initializeClock(void)
{
	RCC->APB1ENR |= RCC_APB1ENR_PWREN;
	__NOP(); __NOP();
	
#ifdef DEBUG_MCU_SYSCLK
	RCC->CFGR &= ~RCC_CFGR_MCO2_Msk;
	RCC->CFGR &= ~RCC_CFGR_MCO2PRE_Msk;
	RCC->CFGR |= RCC_CFGR_MCO2PRE_2;
//	RCC->CFGR |= RCC_CFGR_MCO2PRE_1 | RCC_CFGR_MCO2PRE_2;
#endif	//	DEBUG_MCU_SYSCLK
	
	RCC->CR |= RCC_CR_HSION;
	while ((RCC->CR & RCC_CR_HSIRDY) != RCC_CR_HSIRDY)
		;
	RCC->PLLCFGR = ((2 << RCC_PLLCFGR_PLLR_Pos) | (2 << RCC_PLLCFGR_PLLQ_Pos) |
					(2 << RCC_PLLCFGR_PLLP_Pos) | (216 << RCC_PLLCFGR_PLLN_Pos) |
					(8 << RCC_PLLCFGR_PLLM_Pos) | RCC_PLLCFGR_PLLSRC_HSI);
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
}
