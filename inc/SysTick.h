#ifndef SYSTICK_H
#define SYSTICK_H


#include "stm32f10x.h"                  // Device header
#include <stdint.h>


void systick_init(void);
void delay_ms(uint16_t ms);		// Delay miliseconds
uint32_t get_tick(void);

#endif
