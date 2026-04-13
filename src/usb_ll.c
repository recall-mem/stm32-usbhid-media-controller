#include "usb_ll.h"



/*	**********************FUNCTION DEFINITIONS********************** */
void usb_init(void)
{	
	// Enable clocks
	RCC->APB1ENR |= RCC_APB1ENR_USBEN;
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN | RCC_APB2ENR_IOPAEN;
	
	// Init pins
	GPIOA->CRH &= ~(0xF << PIN(12));
	GPIOA->CRH |=  (0b1011 << PIN(12));
	GPIOA->BSRR = (1 << 12);
	
	GPIOA->CRL &= ~(0xF << PIN(11));
	GPIOA->CRL |=  (0x0 << PIN(11));
	
	///////////////////////////////
	GPIOA->CRL &= ~(0xF << PIN(0));	// Play/Pause
	GPIOA->CRL |=  (0x8 << PIN(0));
	
	GPIOA->CRL &= ~(0xF << PIN(1));	// Vol+
	GPIOA->CRL |=  (0x8 << PIN(1));
	
	GPIOA->CRL &= ~(0xF << PIN(2));	// Vol-
	GPIOA->CRL |=  (0x8 << PIN(2));
	///////////////////////////////
	
	// Set USB
	USB->CNTR = USB_CNTR_FRES;
	for(volatile int t=0; t<100; t++);
	USB->CNTR = 0;
	
	USB->ISTR = 0;
	USB->CNTR |= USB_CNTR_CTRM | USB_CNTR_RESETM;
	USB->BTABLE = 0x0000;
	
	NVIC->ISER[0] |= (1 << 20U);
}

void usb_write_pma(uint16_t offset, void *data, uint16_t len)
{
	uint16_t i;
	for(i = 0; i < len / 2; i++)	{
		PMA_WORD(offset + i * 2) = ((uint8_t*)data)[i*2] | (((uint8_t*)data)[i*2+1] << 8);
	}
	
	if(len & 1)	{
		PMA_WORD(offset + (len/2) * 2) = ((uint8_t*)data)[len-1];
	}
}
/*	**************************************************************** */
