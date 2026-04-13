#ifndef MAIN_H
#define MAIN_H

#include "stm32f10x.h"                  // Device header
#include "SysTick.h"
#include "usb_hid.h"


#define PIN(pin)			\
				( (pin>7)			\
				? 4*(pin-8)		\
				: (4*pin) )


void pin_init(void);
void enable_clocks(void);

#endif