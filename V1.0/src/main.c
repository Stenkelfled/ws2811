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
	sei();
	//LED_RD_ON
	
	eeprom_write_page("abcdef", 6, 2);
	
	while(1){
		asm volatile ("nop");
	}

    /*define_group(0,0,2,1);
    define_group(1,3,7,2);
    append_to_group(0,4);

    rgbcolor_t clr={0x11,0x11,0x88};
    color_group(0,clr);
    clr.red=0xFF;clr.green=0x0F;clr.blue=0x00;
    color_group(1,clr);*/
	
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
		}
		sei();
		color_led(0,clr);
		fill_buffer();
		refresh_leds();
		_delay_ms(20);
	}
	
	color_led(0,clr);

    fill_buffer();
	
	refresh_leds();

    while(1){
		asm volatile("nop");
	}


	// Insert application code here, after the board has been initialized.
}

ISR(PORTE_INT0_vect){ //Button 1 pressed
	if(++mode > 3){
		mode = 0;
	}
}

ISR(PORTE_INT1_vect){ //Button 2 pressed
	if(mode-- == 0){
		mode = 3;
	}
}
