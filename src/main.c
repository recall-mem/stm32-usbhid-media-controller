#include "main.h"


extern uint32_t ticks;

/*	**************ISRHandler************** */
void SysTick_Handler(void)
{
	ticks++;
}

void USB_LP_CAN1_RX0_IRQHandler(void)
{
	if(USB->ISTR & USB_ISTR_RESET)	{
		USB->ISTR = (uint16_t)~USB_ISTR_RESET;
		usb_reset_handler();
	}
	if(USB->ISTR & USB_ISTR_CTR) {
		usb_ctr_handler();
	}
}
/*	************************************** */


int main(void)
{	
	systick_init();
	usb_core_register(&HIDClassDriver);
	usb_init();
	
	
	while(1){};
		
	return 0;
}
