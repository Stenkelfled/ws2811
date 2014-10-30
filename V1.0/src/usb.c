/*
 * usb.c
 *
 * Created: 28.06.2014 14:01:00
 *  Author: stenkelfled
 */ 

#include <asf.h>

#include <config.h>
#include <global.h>
#include <protocoll.h>
#include <usb.h>
#include <usbprint.h>

#include <rgbhsv.h>
#include <ledbuffer.h>
#include <leds.h>
#include <eeprom.h>
#include <ee_interpreter.h>

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
	LED_GN_OFF
	preamble_pos = 0;
	usb_print("usb reset rx\n");
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
	static msg_type_t msg_type = msg_none;
	static test_color_t test_color_state = test_color_none;
	static rgbcolor_t test_color;
	static uint16_t eeprom_data_length;
	
	
	while(udi_cdc_is_rx_ready()){
		if(preamble_pos == USB_PREAMBLE_LEN){
			//preamble-check passed -> now evaluate the data
			uint8_t usb_data = (uint8_t)udi_cdc_getc();
			switch(msg_type){
				case msg_none:
					{
					if(usb_data == PR_MSG_TYPE_RUN){
						if(eeprom_get_write_access() != RET_OK){
							preamble_pos = 0;
							msg_type = msg_none;
							break;
						}
						msg_type = msg_run;
						eeprom_data_length = EEPROM_NEED_DATA_LENGTH_HIGH | EEPROM_NEED_DATA_LENGTH_LOW;
					} else if (usb_data == PR_MSG_TYPE_TEST){
						usb_print("test\n");
						msg_type = msg_test;
						test_color_state = test_color_red;
					} else {
						preamble_pos = 0;
						msg_type = msg_none;
					}
					break;
					}
				case msg_test:
					if(test_color_state == test_color_red){
						usb_print("red\n");
						test_color.red = usb_data;
						test_color_state = test_color_green;
					} else if(test_color_state == test_color_green){
						usb_print("green\n");
						test_color.green = usb_data;
						test_color_state = test_color_blue;
					} else if(test_color_state == test_color_blue){
						usb_print("blue\n");
						test_color.blue = usb_data;
						preamble_pos = 0;
						msg_type = msg_none;
						new_group(0);
						append_row_to_group(0, 0, LED_COUNT-1, 1);
						color_group(0, test_color);
						fill_buffer();
						refresh_leds();
					} else {
						preamble_pos = 0;
						msg_type = msg_none;
					}
					break;
				case msg_run:
					if(eeprom_data_length & EEPROM_NEED_DATA_LENGTH_LOW){
						eeprom_new_buffer();
						eeprom_data_length &= ~EEPROM_NEED_DATA_LENGTH_LOW;
						eeprom_data_length |= usb_data;
					} else if(eeprom_data_length & EEPROM_NEED_DATA_LENGTH_HIGH){
						eeprom_data_length &= ~EEPROM_NEED_DATA_LENGTH_HIGH;
						eeprom_data_length |= usb_data<<8;
					} else {
						//usb_print("eeprom_write...\n");
						eeprom_buffer_byte(usb_data);
						if( !(--eeprom_data_length) ){
							eeprom_write_buffer();
							eeprom_free_write_access();
							preamble_pos = 0;
							msg_type = msg_none;
							interprete_eeprom();
						}
					}
					break;
				default:
					preamble_pos = 0;
					msg_type = msg_none;
			}
			
		} else if(preamble_pos < USB_PREAMBLE_LEN){
			if( udi_cdc_getc() == preamble[preamble_pos]){
				preamble_pos++;
			} else {
				//preamble-check failed. try again.
				preamble_pos = 0;
				msg_type = msg_none;
			}
			if(preamble_pos == USB_PREAMBLE_LEN){
				LED_GN_ON
				//usb_print("preamble passed\n");
				msg_type = msg_none;
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