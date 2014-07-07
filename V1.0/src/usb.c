/*
 * usb.c
 *
 * Created: 28.06.2014 14:01:00
 *  Author: stenkelfled
 */ 

#include <asf.h>

#include <config.h>
#include <evaluator.h>
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
	LED_GN_OFF
	preamble_pos = 0;
	usb_print("usb reset rx\n");
}

#if(USB_DEBUG_ENABLE == YES)
//////////////////////////////////////////////////////////////////////////
//usb debug print
char print_buf[USB_PRINT_BUFFER_LEN];
char *print_buf_write, *print_buf_read, print_buf_isfull;

void usb_print_overflow(void);

void usb_print_overflow(void){
	if(print_buf_write == print_buf){
		print_buf[USB_PRINT_BUFFER_LEN-1] = '@';
		print_buf[USB_PRINT_BUFFER_LEN-2] = '@';
		print_buf_write = print_buf+USB_PRINT_BUFFER_LEN-1;
	} else {
		*print_buf_write-- = '@';
		*(print_buf_write-1) = '@';
	}
}

void usb_print_init(void){
	print_buf_read = print_buf;
	print_buf_write = print_buf;
}

void usb_print(char *buf){
	while(*buf != '\0'){
		*print_buf_write++ = *buf++;
		if(print_buf_write == (print_buf+USB_PRINT_BUFFER_LEN)){
			print_buf_write = print_buf;
		}
		if(print_buf_write == print_buf_read){ //Buffer Overflow
			usb_print_overflow();
			return;
		}
	}
	if(udi_cdc_is_tx_ready()){
		//write first byte
		udi_cdc_putc(*print_buf_read++);
 	}
}

void usb_printc(char chr){
	char buf[2] = {'\0','\0'};
	buf[0] = chr;
	usb_print(buf);
}

void usb_printx(uint8_t input){
	char buf[3] = {0,0,'\0'};
	uint8_t tmp[2];
	tmp[0] = (input>>4) & 0x0F;
	tmp[1] = input & 0x0F;
	for(uint8_t i=0; i<2; i++){
		if( tmp[i] < 0x0A){
			buf[i] = '0' + tmp[i];
		} else {
			buf[i] = 'A' + tmp[i] - 0x0A;
		}
	}
	usb_print(buf);
}

void my_callback_tx_empty_notify(uint8_t port){
	asm volatile ("nop"); //for debugging
	while( (udi_cdc_is_tx_ready()) && (print_buf_read != print_buf_write) ){
		udi_cdc_putc(*print_buf_read++);
		if(print_buf_read == (print_buf+USB_PRINT_BUFFER_LEN)){
			print_buf_read = print_buf;
		}
 	}
}

#endif
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
			evaluate((uint8_t)udi_cdc_getc());
		} else if(preamble_pos < USB_PREAMBLE_LEN){
			if( udi_cdc_getc() == preamble[preamble_pos]){
				preamble_pos++;
			} else {
				//preamble-check failed. try again.
				preamble_pos = 0;
			}
			if(preamble_pos == USB_PREAMBLE_LEN){
				//preamble-check passed -> now evaluate the data
				LED_GN_ON
				evaluator_new_evaluation();
				usb_print("preamble passed\n");
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