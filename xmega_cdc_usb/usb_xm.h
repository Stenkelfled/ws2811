#ifndef _USB_XM_H
#define _USB_XM_H
#include "usb_defaults.h"
#include "Descriptors.h"
#include "usb_ep.h"

extern volatile uint8_t USB_DeviceState;
extern volatile uint8_t USB_Device_ConfigurationNumber;

void USB_ep0_send_progmem(const uint8_t* addr, uint16_t size);
void USB_ResetInterface(void);
void USB_Evt_Task(void);
void USB_Task(void);
void USB_xm_Init(void);

#endif
