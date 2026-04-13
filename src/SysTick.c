#include "stm32f10x.h"                  // Device header
#include "SysTick.h"

uint32_t ticks;

void systick_init(void)
{
	SysTick->CTRL |= 0b111;
	SysTick->LOAD = (SystemCoreClock / 1000) - 1; // 1 MHz resolution | 1ms precision
	SysTick->VAL = 0;
}


void delay_ms(uint16_t ms)
{
	
}

uint32_t get_tick(void)
{
	return ticks;
}