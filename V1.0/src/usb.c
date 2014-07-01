/*
 * usb.c
 *
 * Created: 28.06.2014 14:01:00
 *  Author: stenkelfled
 */ 

#include <asf.h>

#include <config.h>
#include <usb.h>

uint8_t preamble_pos = 0;

void usb_start(void){
	udc_start();
	usb_power_monitor_init();
	if(USB_PORT.IN & (1<<USB_POWER_PIN)){
		UDC_VBUS_EVENT(true);
	}
}


void usb_power_monitor_init(void){
	USB_PORT.DIRCLR = (1<<USB_POWER_PIN);
	USB_PORT.INTFLAGS = PORT_INT1IF_bm | PORT_INT0IF_bm;
	*(&(USB_PORT.PIN0CTRL)+USB_POWER_PIN) = PORT_ISC_BOTHEDGES_gc;
	USB_PORT.INT0MASK = 1<<USB_POWER_PIN;
	USB_PORT.INTCTRL = PORT_INT0LVL_LO_gc;
}

ISR(PORTD_INT0_vect){
	UDC_VBUS_EVENT( USB_PORT.IN & (1<<USB_POWER_PIN) );
}

void usb_rx_ready(void){
	preamble_pos = 0;
}

//////////////////////////////////////////////////////////////////////////
//usb-driver callbacks

//voltage on usb-power-line changed
void user_callback_vbus_action(bool b_vbus_high){
	if(b_vbus_high){
		udc_attach();
		LED_RD_ON
	} else {
		udc_detach();
		LED_RD_OFF
	}
}

//there is data to receive
void my_callback_rx_notify(uint8_t port){ 
	int8_t preamble[USB_PREAMBLE_LEN] = USB_PREAMBLE;
	while(udi_cdc_is_rx_ready()){
		if(preamble_pos == USB_PREAMBLE_LEN){
			//preamble-check passed -> now evaluate the data
			usb_rx_ready();
		}
		if(preamble_pos < USB_PREAMBLE_LEN){
			if( udi_cdc_getc() == preamble[preamble_pos]){
				preamble_pos++;
			} else {
				//preamble-check failed. try again.
				preamble_pos = 0;
			}
		}
	}
}

/*bool my_callback_cdc_enable(void){ //host opens device
	//TODO: nothing to do here at the moment (maybe later open the connection)
	LED_GN_ON
	return true;
}

void my_callback_cdc_disable(void){ //host closes device
	//TODO: nothing to do here at the moment (maybe later close the connection)
	LED_GN_OFF
}*/