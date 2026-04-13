# stm32-usbhid-media-controller

Bare-metal USB HID Consumer Control device on STM32F103CB (Blue Pill).
Register-level implementation.

## Features
- Full USB enumeration from scratch (EP0, control transfer)
- Layered architecture: `usb_ll` → `usb_core` → `usb_hid`
- Callback-based class driver system (extendable to CDC, Vendor)
- HID Consumer Control: Play/Pause, Volume+, Volume-
- Basic Debounce + accelerated repeat logic

## Hardware
- STM32F103CB Blue Pill
- 3 buttons on breadboard:
  - PA0 (white) → Play/Pause
  - PA1 (blue)  → Volume+
  - PA2 (red)   → Volume-
- Required: 1.5kΩ pull-up resistor on PA12 for D+ (Onboard)

## Toolchain
- Keil uVision5
- ARMCLANG V6

## Files
usb_ll.c/h > Usb implementation, macros...
usb_core.c/h > EP0, control transfer, enumeration, class driver interface
usb_hid.c/h > HID & report descriptors, class callbacks

## Data
- bit[0] = Play/Pause
- bit[1] = Volume+
- bit[2] = Volume-
- bit[3:7] = Padding