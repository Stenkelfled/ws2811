/*
 * usb.c
 *
 * Created: 28.06.2014 14:01:00
 *  Author: stenkelfled
 */ 

#include <asf.h>

#include <usb.h>


void usb_start(void){
	udc_start();
#warning "later here should be a real event"
	UDC_VBUS_EVENT(true);
}

//////////////////////////////////////////////////////////////////////////
//usb-driver callbacks
void user_callback_vbus_action(bool b_vbus_high){ //voltage on usb-power-line changed
	if(b_vbus_high){
		udc_attach();
	} else {
		udc_detach();
	}
}

bool my_callback_cdc_enable(void){ //host opens device
	//TODO: nothing to do here at the moment (maybe later close the connection)
	//LED_GN_ON
	return true;
}

void my_callback_cdc_disable(void){ //host closes device
	//TODO: nothing to do here at the moment (maybe later close the connection)
	//LED_GN_OFF
}