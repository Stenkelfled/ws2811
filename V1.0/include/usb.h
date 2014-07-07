/*
 * usb.h
 *
 * Created: 28.06.2014 14:02:19
 *  Author: stenkelfled
 */ 


#ifndef USB_H_
#define USB_H_

#include <asf.h>

void usb_start(void);
void usb_power_monitor_init(void);

void usb_rx_ready(void);

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

//////////////////////////////////////////////////////////////////////////
//usb-driver callbacks
void user_callback_vbus_action(bool b_vbus_high); //voltage on usb-power-line changed
bool my_callback_cdc_enable(void); //host opens device
void my_callback_cdc_disable(void); //host closes device

#endif /* USB_H_ */