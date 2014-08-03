/*
 * usbprint.h
 *
 * Created: 09.07.2014 17:58:06
 *  Author: stenkelfled
 */ 


#ifndef USBPRINT_H_
#define USBPRINT_H_

#include <config.h>

#if (USB_DEBUG_ENABLE == YES)
#define USB_PRINT_BUFFER_LEN	(400)
void usb_print(char *buf);
void usb_printc(char chr);
void usb_printx(uint8_t x);
void usb_print_init(void);
#else
#define usb_print(x)
#define usb_printc(x)
#define usb_printx(x)
#define usb_print_init()
#endif



#endif /* USBPRINT_H_ */