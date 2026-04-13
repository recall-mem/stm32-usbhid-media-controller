#ifndef USB_CORE_H
#define USB_CORE_H

#include "stm32f10x.h"                  // Device header
#include "usb_ll.h"
#include <stdint.h>

/*	***************REQUEST TYPES*************** */
#define		USB_REQ_GET_DESCRIPTOR		0x06
#define		USB_REQ_SET_ADDRESS				0x05
#define		USB_REQ_SET_CONFIG				0x09

// HID REQUESTS
#define		USB_HID_REQ_GET_REPORT		0x01
#define		USB_HID_REQ_GET_IDLE			0x02
#define		USB_HID_REQ_GET_PROTOCOL	0x03
#define		USB_HID_REQ_SET_REPORT		0x09
#define		USB_HID_REQ_SET_IDLE			0x0A
#define		USB_HID_REQ_SET_PROTOCOL	0x0B
/*	******************************************* */

/*	***************DESCRIPTOR TYPES*************** */
#define		USB_DESC_TYPE_DEVICE			0x01
#define		USB_DESC_TYPE_CONFIG			0x02
#define		USB_DESC_TYPE_STRING			0x03

// HID DESCRIPTOR TYPES
#define		USB_HID_DESC_TYPE_HID			0x21
#define		USB_HID_DESC_TYPE_REPORT	0x22
/*	********************************************** */

/*	************USB TYPEDEF************ */
typedef struct	{
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t bcdUSB;
	uint8_t  bDeviceClass;
	uint8_t  bDeviceSubClass;
	uint8_t  bDeviceProtocol;
	uint8_t  bMaxPacketSize0;
	uint16_t idVendor;
	uint16_t idProduct;
	uint16_t bcdDevice;
	uint8_t  iManufacturer;
	uint8_t  iProduct;
	uint8_t  iSerialNumber;
	uint8_t  bNumConfigurations;
}	__attribute__((packed)) USB_DeviceDescriptor;

typedef struct	{
	uint8_t  bLength;
	uint8_t  bDescriptorType;
	uint16_t wTotalLength;
	uint8_t  bNumInterfaces;
	uint8_t  bConfigurationValue;
	uint8_t  iConfiguration;
	uint8_t  bmAttributes;
	uint8_t  bMaxPower;
}	__attribute__((packed))	USB_ConfigDescriptor;

typedef struct	{
	uint8_t bLength;
	uint8_t bDescriptorType;
	uint8_t bInterfaceNumber;
	uint8_t bAlternateSetting;
	uint8_t bNumEndpoints;
	uint8_t bInterfaceClass;
	uint8_t bInterfaceSubClass;
	uint8_t bInterfaceProtocol;
	uint8_t iInterface;
}	__attribute__((packed))	USB_InterfaceDescriptor;

typedef struct	{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint8_t		bEndpointAddress;
	uint8_t		bmAttributes;
	uint16_t	wMaxPacketSize;
	uint8_t		bInterval;
}	__attribute__((packed))	USB_EndpointDescriptor;

typedef struct	{
	uint8_t  bmRequestType;
	uint8_t  bRequest;
	uint16_t wValue;
	uint16_t wIndex;
	uint16_t wLength;
}	USB_SetupPacket;

typedef struct	{
	const uint8_t *config_descriptor;
	uint16_t			config_descriptor_len;
	void (*class_request)(USB_SetupPacket *pkt);
	void (*data_in)(void);	// Will prepare data for host 
	void (*data_out)(void);	/*Can be NULL*/
}	USB_ClassDriver;
/*	*********************************** */

/*	***********GLOBAL FUNCTIONS*********** */
void usb_core_register(const USB_ClassDriver *driver);	// Set current driver type e.g CDC, HID

void usb_reset_handler(void);
void usb_ctr_handler(void);
void usb_read_setup(USB_SetupPacket *pkt);
/*	************************************** */

/*	*******************GLOBAL VARIABLES******************* */
extern const USB_ClassDriver* current_driver;
extern const USB_DeviceDescriptor			DeviceDescriptor;
//extern const USB_ConfigDescriptor			ConfigDescriptor;
//extern const USB_InterfaceDescriptor	InterfaceDescriptor;
//extern const USB_FullConfigDescriptor	FullConfigDescriptor;
/*	****************************************************** */


#endif