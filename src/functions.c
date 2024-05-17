#include "functions.h"

#include <stdint.h>

#include <stm32f767xx.h>

extern volatile uint8_t skipExtiInterruptDisable;

void disableExtiInterrupt(void)
{
	EXTI->IMR &= ~EXTI_IMR_IM15;
}

void enableExtiInterrupt(void)
{
	EXTI->RTSR |= EXTI_RTSR_TR15;
	EXTI->PR |= EXTI_PR_PR15;
	skipExtiInterruptDisable = ((GPIOA->IDR & GPIO_IDR_ID15) != GPIO_IDR_ID15) ? 1 : 0;
	EXTI->IMR |= EXTI_IMR_IM15;
}
