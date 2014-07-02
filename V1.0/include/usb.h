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

//////////////////////////////////////////////////////////////////////////
//usb-driver callbacks
void user_callback_vbus_action(bool b_vbus_high); //voltage on usb-power-line changed
bool my_callback_cdc_enable(void); //host opens device
void my_callback_cdc_disable(void); //host closes device

#endif /* USB_H_ */