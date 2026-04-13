#ifndef USB_HID_H
#define USB_HID_H

#include "stm32f10x.h"                  // Device header
#include "usb_ll.h"
#include "usb_core.h"
#include <stdint.h>


/*	**************REPORT DESCRIPTOR INFO************** */
//bit0: Data(0) / Constant(1)
//bit1: Array(0) / Variable(1)
//bit2: Absolute(0) / Relative(1)

//1-byte item prefix
//     Bits:
//     7654 |    32 |    10
//     bTag | bType | bSize
//       
//     bSize: Length of data following the prefix
//     0b00= 0 bytes
//     0b01= 1 byte
//     0b10= 2 bytes
//     0b11= 4 bytes
//        
//     bType: Item Types
//     0b00= Main
//     0b01= Global
//     0b10= Local
//     0b11= Reserved
   
	 
//  bTag: Refer to 4-bit MSBs in below macros

//// Main Items
//0b1000 <- INPUT
//0b1001 <- OUTPUT
//0b1011 <- FEATURE
//0b1010 <- COLLECTION
//0b1100 <- END COLLECTION

//// Global Items
//0b0000 <- USAGE_PAGE
//0b0001 <- LOGICAL_MINIMUM
//0b0010 <- LOGICAL_MAXIMUM
//0b0111 <- REPORT_SIZE
//0b1001 <- REPORT_COUNT

//// Local Items
//0b0000 <- USAGE
//0b0001 <- USAGE_MINIMUM
//0b0010 <- USAGE_MAXIMUM
/*	************************************************** */

/*	************USB TYPEDEF************ */
typedef struct	{
	uint8_t		bLength;
	uint8_t		bDescriptorType;
	uint16_t	bcdHID;
	uint8_t		bCountryCode;
	uint8_t		bNumDescriptors;
	uint8_t		bClassDescriptorType;
	uint16_t	wDescriptorLength;
} __attribute__((packed))USB_HIDDescriptor;

typedef struct	{
	USB_ConfigDescriptor			configdesc;
	USB_InterfaceDescriptor 	interfacedesc;
	USB_HIDDescriptor					hiddesc;
	USB_EndpointDescriptor		endpointdesc;
}	__attribute__((packed))	USB_HIDFullConfig ;
/*	*********************************** */

/*	*******************GLOBAL VARIABLES******************* */
extern const USB_ClassDriver HIDClassDriver;
extern const uint8_t ReportDescriptor[29];
/*	****************************************************** */

/*	***********GLOBAL FUNCTIONS*********** */
void hid_class_request(USB_SetupPacket *pkt);
void hid_data_in(void);
/*	************************************** */

#endif