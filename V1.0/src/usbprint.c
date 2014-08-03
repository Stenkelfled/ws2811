/*
 * usbprint.c
 *
 * Created: 09.07.2014 17:57:30
 *  Author: stenkelfled
 */ 


#if(USB_DEBUG_ENABLE == YES)

#include <asf.h>
#include <usbprint.h>

char print_buf[USB_PRINT_BUFFER_LEN];
char *print_buf_write, *print_buf_read, print_buf_isfull;

void usb_print_overflow(void);

void usb_print_overflow(void){
	if(print_buf_write == print_buf){
		print_buf[USB_PRINT_BUFFER_LEN-1] = '@';
		print_buf[USB_PRINT_BUFFER_LEN-2] = '@';
		print_buf_write = print_buf+USB_PRINT_BUFFER_LEN-1;
		} else if(print_buf_write == (print_buf+1)){
		print_buf[0] = '@';
		print_buf[USB_PRINT_BUFFER_LEN-1] = '@';
		print_buf_write = print_buf;
		} else {
		*--print_buf_write = '@';
		*(print_buf_write-1) = '@';
	}
}

void usb_print_init(void){
	print_buf_read = print_buf;
	print_buf_write = print_buf;
}

void usb_print(char *buf){
	uint8_t isempty = FALSE;
	if(print_buf_read == print_buf_write){
		isempty = TRUE;
	}
	while(*buf != '\0'){
		*print_buf_write++ = *buf++;
		if(print_buf_write == (print_buf+USB_PRINT_BUFFER_LEN)){
			print_buf_write = print_buf;
		}
		if(print_buf_write == print_buf_read){ //Buffer Overflow
			usb_print_overflow();
			break;
		}
	}
	if(isempty && udi_cdc_is_tx_ready()){
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