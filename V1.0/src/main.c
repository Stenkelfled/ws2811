/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <util/delay.h>

#include <config.h>
#include <eeprom.h>
#include <leds.h>
#include <ledbuffer.h>
#include <rgbhsv.h>
#include <usb.h>
#include <usbprint.h>

uint8_t mode = 0;

int main (void)
{
	init_interrupts();
	init_clock();
	init_board();
	init_ports();
	init_usart();
	init_dma(ledbuffer_get_data_pointer());
	init_eeprom();
	start_leds();
	usb_print_init();
	sei();
	//LED_RD_ON
	
	usb_start();

 	//_delay_ms(1000);

// 	int8_t uart_char;
// 	while(1){
// 		uart_char = udi_cdc_getc();
// 		TCC2.LCNT = uart_char;
// 		asm volatile ("nop");
// 		if(udi_cdc_is_tx_ready()){
// 			udi_cdc_putc('k');
// 		}
// 	}
	
	while(1){
		asm volatile ("nop");
	}


	uint8_t gid = 0;
    define_group(gid,0,LED_COUNT-1,1);
	
	rgbcolor_t clr={0x00,0x00,0x00};

	hsvcolor_t hsvcol;
	hsvcol.sat = 255;
	hsvcol.val = 255;
	hsvcol.hue = 0;

	while(1){
		cli();
		switch(mode){
			case 0:	
				hsvcol.hue +=1;
				if(hsvcol.hue == 360) hsvcol.hue = 0;
				clr = hsv2rgb(hsvcol);
				break;
			case 1:
				set_predefined_color(&clr,CLR_RED);
				break;
			case 2:
				set_predefined_color(&clr,CLR_GREEN);
				break;
			case 3:
				set_predefined_color(&clr,CLR_BLUE);
				break;
			case 4:
				set_predefined_color(&clr,CLR_WHITE);
				break;
		}
		sei();
		color_group(gid,clr);
		fill_buffer();
		refresh_leds();
		_delay_ms(20);
	}
	// Insert application code here, after the board has been initialized.
}

ISR(PORTE_INT0_vect){ //Button 1 pressed
	if(++mode > 4){
		mode = 0;
	}
	usb_print("Button 1\n");
}

ISR(PORTE_INT1_vect){ //Button 2 pressed
	if(mode-- == 0){
		mode = 4;
	}
	usb_print("Button 2\n");
}
