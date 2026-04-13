#include "usb_core.h"

/*	*******************GLOBAL VARIABLES******************* */
static uint8_t _usbPendingAddr;
const USB_ClassDriver *current_driver = NULL;

const USB_DeviceDescriptor DeviceDescriptor = {
	.bLength							= 18,
	.bDescriptorType			= 0x01,
	.bcdUSB								= 0x0200,
	.bDeviceClass					= 0x00,
	.bDeviceSubClass			= 0x00,
	.bDeviceProtocol			= 0x00,
	.bMaxPacketSize0			= 0x40,
	.idVendor							= 0x0483,	// STM32F10X_MD spesific
	.idProduct						= 0x5750,
	.bcdDevice						= 0x0100,
	.iManufacturer				= 0x00,
	.iProduct							= 0x00,
	.iSerialNumber				= 0x00,
	.bNumConfigurations		= 0x01,
};
/*	****************************************************** */

/*	**********************FUNCTION DEFINITIONS********************** */
void usb_core_register(const USB_ClassDriver *driver)
{
	current_driver = driver;	// "I drive..."
}

void usb_reset_handler(void)
{
	USB->BTABLE = 0;
	
	// EP0 configuration
	PMA_WORD(EP0_TX_BUFF_OFFSET) = EP0_TX_BUFF_ADDR;
	PMA_WORD(EP0_TX_COUNT_OFFSET) = 0x0000;	// No data to send
	PMA_WORD(EP0_RX_BUFF_OFFSET) = EP0_RX_BUFF_ADDR;
	PMA_WORD(EP0_RX_COUNT_OFFSET) = EP_BUFF_MAX_SIZE;
	
	USB->EP0R |= EPnR_TYPE_CONTROL;

	SET_TX_STATUS(USB->EP0R, EPnR_STAT_TX_NAK);
	SET_RX_STATUS(USB->EP0R, EPnR_STAT_RX_VALID);
	
	// EP1 configuration
	PMA_WORD(EP1_TX_BUFF_OFFSET) = EP1_TX_BUFF_ADDR;
	PMA_WORD(EP1_TX_COUNT_OFFSET) = 0x0000;	// No data to send
	
	USB->EP1R = EPnR_TYPE_INTERRUPT | 0x01;
	
	SET_TX_STATUS(USB->EP1R, EPnR_STAT_TX_NAK);
	
	USB->DADDR = USB_DADDR_EF;
}


void usb_read_setup(USB_SetupPacket *pkt)
{
	uint16_t data[4];
	data[0] = PMA_WORD(EP0_RX_BUFF_ADDR);
	data[1] = PMA_WORD(EP0_RX_BUFF_ADDR + 0x02);
	data[2] = PMA_WORD(EP0_RX_BUFF_ADDR + 0x04);
	data[3] = PMA_WORD(EP0_RX_BUFF_ADDR + 0x06);
	
	pkt->bmRequestType = (uint8_t)(data[0] & 0x00FF);
	pkt->bRequest = (uint8_t)((data[0] & 0xFF00) >> 8);
	pkt->wValue = data[1];
	pkt->wIndex = data[2];
	pkt->wLength = data[3];
}

static int log_count;
USB_SetupPacket pkt_log[20];
void usb_ctr_handler(void)
{
	volatile uint16_t istr = USB->ISTR;
	uint8_t ep_number = istr & 0x0F;	// Endpoint number
	uint8_t dir = (istr >> 4) & 1;	// IN or OUT
	
	switch(ep_number)
	{
		case 0: // ENDPOINT 0
			
			if(dir == 1)	{	
				CLEAR_CTR_RX(USB->EP0R);
				if((USB->EP0R & USB_EP0R_SETUP))	{	// SETUP Packet
					USB_SetupPacket pkt;
					usb_read_setup(&pkt);
					
					if(log_count < 20)	{
						pkt_log[log_count++] = pkt;
					}
					
					switch(pkt.bRequest)
					{
						case USB_REQ_GET_DESCRIPTOR:
								switch(pkt.wValue >> 8)
								{
									case USB_DESC_TYPE_DEVICE:
										usb_write_pma(EP0_TX_BUFF_ADDR, (void*)&DeviceDescriptor, 18U);
										PMA_WORD(EP0_TX_COUNT_OFFSET) = 18;
										SET_TX_STATUS(USB->EP0R, EPnR_STAT_TX_VALID);
									break;
									case USB_DESC_TYPE_CONFIG:
										uint16_t len = (pkt.wLength < current_driver->config_descriptor_len)
										? pkt.wLength 
										: current_driver->config_descriptor_len;
										usb_write_pma(EP0_TX_BUFF_ADDR, (void*)current_driver->config_descriptor, current_driver->config_descriptor_len);
										PMA_WORD(EP0_TX_COUNT_OFFSET) = len;
										SET_TX_STATUS(USB->EP0R, EPnR_STAT_TX_VALID);
									break;
									case USB_DESC_TYPE_STRING:
										PMA_WORD(EP0_TX_COUNT_OFFSET) = 0;
										SET_TX_STATUS(USB->EP0R, EPnR_STAT_TX_STALL);
									break;
									case 0x06:	// Device Qualifier
									break;
										PMA_WORD(EP0_TX_COUNT_OFFSET) = 0;
										SET_TX_STATUS(USB->EP0R, EPnR_STAT_TX_STALL);
									default:	// Hanle class spesific request
										current_driver->class_request(&pkt);
									break;
								}	// Switch End
						break;	// USB_REQ_GET_DESCRIPTOR
								
						case USB_REQ_SET_ADDRESS:
							_usbPendingAddr = (uint8_t)(pkt.wValue & 0x00FF);	// LSB is address here
							PMA_WORD(EP0_TX_COUNT_OFFSET) = 0;
							SET_TX_STATUS(USB->EP0R, EPnR_STAT_TX_VALID);
						break;	// USB_REQ_SET_ADDRESS
						
						case USB_REQ_SET_CONFIG:
							PMA_WORD(EP0_TX_COUNT_OFFSET) = 0;
							SET_TX_STATUS(USB->EP0R, EPnR_STAT_TX_VALID);
							PMA_WORD(EP1_TX_COUNT_OFFSET) = 0;
							SET_TX_STATUS(USB->EP1R, EPnR_STAT_TX_VALID);
							current_driver->data_in();
						break;	// USB_REQ_SET_CONFIG
						
						default:
							(current_driver->class_request)(&pkt);
						break;
					}
				}	else	{	// OUT Packet
					if((current_driver->data_out) != NULL)	{
						(current_driver->data_out)();
					}
				}
			}	// if(dir == 1)
			else if(dir == 0)	{	// IN Packet
				CLEAR_CTR_TX(USB->EP0R);
				if(_usbPendingAddr != 0)	{	// Set address
					USB->DADDR = USB_DADDR_EF | _usbPendingAddr;
					_usbPendingAddr = 0;
				}
				else	{
					if((current_driver->data_in) != NULL)	{
						(current_driver->data_in)();
					}
				}
				
				SET_RX_STATUS(USB->EP0R, EPnR_STAT_RX_VALID);
			}
			
		break;	// CASE 0 END
		
		case 1:	// ENDPOINT 1
			CLEAR_CTR_TX(USB->EP1R);
			if(current_driver->data_in != NULL)
        current_driver->data_in();
		break;	// CASE 1 END
			
		default:
		break;
	}// Switch End
}
/*	**************************************************************** */
