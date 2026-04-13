#ifndef USB_LL_H
#define USB_LL_H

#include "stm32f10x.h"                  // Device header
#include <stdint.h>

#define NULL ((void*)0)

#define PIN(pin)			\
				( (pin>7)			\
				? 4*(pin-8)		\
				: (4*pin) )

#define		EP_BUFF_MAX_SIZE			0x8800


/*	************USB BASE ADDRESSES************ */
#define		USB_BASE							0x40005C00UL
#define 	PMA_BASE							0x40006000UL
#define 	PMA_WORD(offset)  		(*((volatile uint16_t*)(PMA_BASE + (offset) * 2)))
/*	****************************************** */

/*	************USB BTABLE ADDRESSES************ */
#define		EP0_TX_BUFF_ADDR			0x0040
#define		EP0_RX_BUFF_ADDR			0x0080
#define		EP1_TX_BUFF_ADDR			0x00C0
#define		EP1_RX_BUFF_ADDR			0x0100
/*	******************************************** */

/*	************USB EP OFFSETS************ */
#define		EP0_TX_BUFF_OFFSET		0x00
#define		EP0_TX_COUNT_OFFSET		0x02
#define		EP0_RX_BUFF_OFFSET		0x04
#define		EP0_RX_COUNT_OFFSET		0x06
#define		EP1_TX_BUFF_OFFSET		0x08
#define		EP1_TX_COUNT_OFFSET		0x0A
#define		EP1_RX_BUFF_OFFSET		0x0C
#define		EP1_RX_COUNT_OFFSET		0x0E
/*	************************************** */

/*	************USB ENDPOINT MACROS************ */
// EP_TYPE
#define		EPnR_TYPE_BULK						0x0000
#define		EPnR_TYPE_CONTROL					0x0200
#define		EPnR_TYPE_ISO							0x0400
#define		EPnR_TYPE_INTERRUPT				0x0600
// STAT_RX
#define		EPnR_STAT_RX_DISABLED			0x0000
#define		EPnR_STAT_RX_STALL				0x1000
#define		EPnR_STAT_RX_NAK					0x2000
#define		EPnR_STAT_RX_VALID				0x3000
// STAT_TX
#define		EPnR_STAT_TX_DISABLED			0x0000
#define		EPnR_STAT_TX_STALL				0x0010
#define		EPnR_STAT_TX_NAK					0x0020
#define		EPnR_STAT_TX_VALID				0x0030
/*	******************************************* */

/*	*******************TOGGLE MACROS******************* */
#define SET_TX_STATUS(ep_reg, status)										\
do	{																										\
	uint16_t _reg = ep_reg; 		/*STAT_TX[5:4]*/					\
	uint16_t _mask = (_reg ^ status) & 0x0030;						\
	ep_reg = (_reg & 0x878F) | _mask;											\
}	while(0)

#define SET_RX_STATUS(ep_reg, status)										\
do	{																										\
	uint16_t _reg = ep_reg; 		/*STAT_RX[13:12]*/				\
	uint16_t _mask = (_reg ^ status) & 0x3000;						\
	ep_reg = (_reg & 0x878F) | _mask;											\
}	while(0)

#define CLEAR_CTR_TX(ep_reg)														\
do	{																										\
	ep_reg = (ep_reg & 0x870F);	/*CTR_TX Bit 7*/					\
}	while(0)

#define CLEAR_CTR_RX(ep_reg)														\
do	{																										\
	ep_reg = (ep_reg & 0x078F);	/*CTR_RX Bit 15*/					\
}	while(0)
/*	*************************************************** */

/*	************USB TYPEDEF************ */
typedef struct	{
	__IO uint16_t EP0R;
	__IO uint16_t RESERVED0;
	__IO uint16_t EP1R;
	__IO uint16_t RESERVED1;
	__IO uint16_t EP2R;
	__IO uint16_t RESERVED2;
	__IO uint16_t EP3R;
	__IO uint16_t RESERVED3;
	__IO uint16_t EP4R;
	__IO uint16_t RESERVED4;
	__IO uint16_t EP5R;
	__IO uint16_t RESERVED5;
	__IO uint16_t EP6R;
	__IO uint16_t RESERVED6;
	__IO uint16_t EP7R;
	__IO uint16_t RESERVED7;
	__IO uint32_t RESERVED8[8];
	__IO uint16_t CNTR;
	__IO uint16_t RESERVED9;
	__IO uint16_t ISTR;
	__IO uint16_t RESERVED10;
	__IO uint16_t FNR;
	__IO uint16_t RESERVED11;
	__IO uint16_t DADDR;
	__IO uint16_t RESERVED12;
	__IO uint16_t BTABLE;
	__IO uint16_t RESERVED13;
}	USB_TypeDef;

#define			USB			((USB_TypeDef*)USB_BASE)
/*	*********************************** */

/*	***********GLOBAL FUNCTIONS*********** */
void usb_init(void);
void usb_write_pma(uint16_t offset, void *data, uint16_t len);
/*	************************************** */

/*	*******************GLOBAL VARIABLES******************* */

/*	****************************************************** */

#endif