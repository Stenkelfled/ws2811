/*
 * ee_interpreter.c
 *
 * Created: 30.10.2014 17:11:54
 *  Author: stenkelfled
 */ 

#include <ee_interpreter.h>
#include <eeprom.h>
#include <ledbuffer.h>
#include <Leds.h>
//#include <rgbhsv.h>
#include <protocoll.h>

void interprete_eeprom(void){
	uint8_t group_count;
	uint16_t eeprom_address;
	uint8_t data;
	if( eeprom_get_read_access() != RET_OK){
		//could not ret read access to eeprom. abort.
		return;
	}
	eeprom_address = eeprom_my_read_uint16(0x0000); //read address of first group
	group_count = eeprom_address / 2;
	
	for(uint8_t i=0; i<group_count; i++){
		new_group(i);
		eeprom_address = eeprom_my_read_uint16(i*2);
		
		while(1){ //add all LEDs to the group
			data = eeprom_my_read_uint8(eeprom_address++);
			if(data == PR_GRP_LED_SER){
				uint8_t first_led = eeprom_my_read_uint8(eeprom_address++);
				uint8_t last_led = eeprom_my_read_uint8(eeprom_address++);
				uint8_t step = eeprom_my_read_uint8(eeprom_address++);
				append_row_to_group(i, first_led, last_led, step);
			} else if(data == PR_GRP_LED_ADD){
				append_led_to_group(i, eeprom_my_read_uint8(eeprom_address++));
			} else {
				break;
			}
		}
		
		//init the first sequence of the group
		if(data != PR_GRP_SEQ_START){
			continue; //something weird happened
		}
		uint16_t next_sequence = eeprom_my_read_uint16(eeprom_address);
		eeprom_address += 2;
		uint8_t repetitions = eeprom_my_read_uint8(eeprom_address++);
		uint16_t duration = eeprom_my_read_uint16(eeprom_address);
		eeprom_address += 2;
		group_set_properties(i, next_sequence, repetitions, duration);
		data = eeprom_my_read_uint8(eeprom_address++);
		while(data != PR_GRP_SEQ_START)
		{
			switch(data){
				case PR_GRP_SEQ_SET_OFF:{
					rgbcolor_t color = {.red = 0, .green = 0, .blue = 0};
					color_group(i, color);
					break;
					}
				case PR_GRP_SEQ_SET_RGB:{
					rgbcolor_t color;
					color.red = eeprom_my_read_uint8(eeprom_address++);
					color.green = eeprom_my_read_uint8(eeprom_address++);
					color.blue = eeprom_my_read_uint8(eeprom_address++);
					color_group(i, color);
					break;
					}
			}
			data = eeprom_my_read_uint8(eeprom_address++);
		}
		
		
	}
	
	eeprom_free_read_access();
	
	fill_buffer();
	refresh_leds();
}