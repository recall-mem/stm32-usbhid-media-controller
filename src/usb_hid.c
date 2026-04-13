#include "usb_hid.h"

extern uint32_t get_tick(void); // in SysTick.c

/*	*******************GLOBAL VARIABLES******************* */
const USB_InterfaceDescriptor InterfaceDescriptor = {
	.bLength							= 9,
	.bDescriptorType			= 0x04,
	.bInterfaceNumber			= 0,
	.bAlternateSetting		= 0,
	.bNumEndpoints				= 1,	// (EP1 IN)
	.bInterfaceClass			= 0x03,	// (HID)
	.bInterfaceSubClass		= 0x00,
	.bInterfaceProtocol		= 0x00,
	.iInterface						= 0
};

const USB_ConfigDescriptor ConfigDescriptor = {
	.bLength							= 9,
	.bDescriptorType			= 0x02,	// CONFIG
	.wTotalLength					= 34,	// 9+9+9+7 (config+interface+hid+endpoint)
	.bNumInterfaces				= 1,	// EP0
	.bConfigurationValue	= 1,
	.iConfiguration				= 0,
	.bmAttributes					= 0x80 /* | (1 << 6) for self-powered*/,	// (bus powered)
	.bMaxPower						= 50	// (2*50mA)
};

const USB_EndpointDescriptor EndpointDescriptor = {
	.bLength 					= 7,
	.bDescriptorType	= 0x05,
	.bEndpointAddress = 0x81,	// EP1 IN
	.bmAttributes			= 0x03,	// Interrupt
	.wMaxPacketSize		= 0x01, // Report descriptor is only 1 byte
	.bInterval				= 8 /* in milliseconds */
};

const USB_HIDDescriptor HIDDescriptor = {
	.bLength								= 9,
	.bDescriptorType				= 0x21,
	.bcdHID									= 0x0111,	// USB 11.1
	.bCountryCode						= 0x00,	// For devices like keyboards use country table 
	.bNumDescriptors				= 0x01,	// Just REPORT descriptor 
	.bClassDescriptorType		=	0x22,
	.wDescriptorLength			= 29	// REPORT descriptor length
};

const uint8_t ReportDescriptor[29] = {
	0x05, 0x0C,	// USAGE_PAGE (Consumer)
	0x09, 0x01,	// USAGE (Consumer Control)
	0xA1, 0x01,	// COLLECTION (Application)
	0x09, 0xCD,	// USAGE (Play/Pause)
	0x09, 0xE9,	// USAGE (VOL+)
	0x09, 0xEA,	// USAGE (VOL-)
	0x15, 0x00,	// LOGICAL_MINIMUM (0)
	0x25, 0x01,	// LOGICAL_MAXIMUM (1)
	0x75, 0x01,	// REPORT_SIZE (1)
	0x95, 0x03,	// REPORT_COUNT (3)
	0x81, 0x02,	// INPUT (Data,Var,Abs)
	0x75, 0x01,	// REPORT_SIZE (1)
	0x95, 0x05,	// REPORT_COUNT (5)
	0x81, 0x03,	// INPUT (Cnst,Var,Abs)
	0xC0	// END COLLECTION
};

const USB_HIDFullConfig HIDFullConfig = {
	.configdesc			= ConfigDescriptor,
	.interfacedesc	= InterfaceDescriptor,
	.hiddesc				= HIDDescriptor,
	.endpointdesc		= EndpointDescriptor
};

const USB_ClassDriver HIDClassDriver = {
	.config_descriptor			= (uint8_t*)&HIDFullConfig,
	.config_descriptor_len	= sizeof(USB_HIDFullConfig),
	.class_request					= hid_class_request,
	.data_in								= hid_data_in,
	.data_out								= NULL
};
/*	****************************************************** */

/*	**********************FUNCTION DEFINITIONS********************** */
void hid_class_request(USB_SetupPacket *pkt)
{
	switch (pkt->bRequest)
	{
		case USB_REQ_GET_DESCRIPTOR:
			switch (pkt->wValue >> 8)
			{
				case USB_HID_DESC_TYPE_REPORT:
					usb_write_pma(EP0_TX_BUFF_ADDR, (void*)ReportDescriptor, sizeof(ReportDescriptor));
					PMA_WORD(EP0_TX_COUNT_OFFSET) = sizeof(ReportDescriptor);
					SET_TX_STATUS(USB->EP0R, EPnR_STAT_TX_VALID);
				break;
				
				case USB_HID_DESC_TYPE_HID:
					// ...
				break;
			}
		break;
			
		case USB_HID_REQ_SET_IDLE:
		// send ZLP ...
		PMA_WORD(EP0_TX_COUNT_OFFSET) = 0;
		SET_TX_STATUS(USB->EP0R, EPnR_STAT_TX_VALID);
		break;
	}
}


uint8_t data, buttonHold;
uint32_t last_tick, repeat_tick;
void hid_data_in(void)
{
	// BIT0 PLAY/PAUSE, BIT1 VOL+, BIT2VOL -
	uint8_t buttons = (uint8_t)(GPIOA->IDR & 0x07);
	
	if(buttons != 0 && data != 0)	{
		buttonHold = 1;
	}
	
	if(buttonHold)	{
		if(buttons != 0)	{
			data = 0;
		}	else {
			buttonHold = 0;
			data = 0;
		}
		if(((get_tick() - repeat_tick) > 500) && ((get_tick() - last_tick) > 55))	{
			last_tick = get_tick();
			data = buttons;
		}
	}	else if(!buttonHold && ((get_tick() - last_tick) > 10))	{
		last_tick = get_tick();
		repeat_tick = get_tick();
		data = buttons;
	}
	
	usb_write_pma(EP1_TX_BUFF_ADDR, &data, 1);
	PMA_WORD(EP1_TX_COUNT_OFFSET) = 1;
	SET_TX_STATUS(USB->EP1R, EPnR_STAT_TX_VALID);
}
/*	**************************************************************** */
