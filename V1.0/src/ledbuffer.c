/*
 * ledbuffer.c
 *
 * Created: 28.03.2014 10:49:18
 *  Author: stenkelfled
 */ 

#include <asf.h>

#include <config.h>
#include <ledbuffer.h>
#include <leds.h>

led_color_t leds[LED_COUNT];
led_group_t groups[GROUP_COUNT];

uint8_t led_data[LED_COUNT*BYTES_PER_LED];

uint8_t* ledbuffer_get_data_pointer(void){
	return led_data;
}

void color2data(uint8_t* *data, uint8_t color){
    uint8_t raw_data;

    for(uint8_t _byte_=0;_byte_<4;_byte_++){
        for(uint8_t _nibble_=0;_nibble_<2;_nibble_++){
            if(color&(0x80>>(2*_byte_+_nibble_))){
                raw_data = LED_CODE_1;
            } else {
                raw_data = LED_CODE_0;
            }
            **data = ( **data&(0x0F<<(4*_nibble_)) ) | ( raw_data<<(4*(1-_nibble_)) );
        }
        (*data)++;
    }
}

void fill_buffer(void){
    
    uint8_t *led_buf = led_data;
    uint8_t *srcptr = (uint8_t*)leds;
    for(uint8_t led=0;led<LED_COUNT;led++){ //leds
        for(uint8_t clr=0;clr<3;clr++){ //color
            color2data(&led_buf, *srcptr++);
        }
        srcptr += 2; //skip the address of the next led
    }

}

/**
 * (re)define the given group with a row of leds
 * @param: uint8_t group_id: the id of the group
 * @param: uint8_t start_led: the first led of the row
 * @param: uint8_t end_led: the last led of the row
 * @param: uint8_t step: the step in the row
 */
/*void define_group(uint8_t group_id, uint8_t start_led){    
    groups[group_id].start_led = &leds[start_led];
}*/

void clear_group(uint8_t group_id){
	groups[group_id].start_led = NULL;
}

uint8_t append_row_to_group(uint8_t group_id, uint8_t start_led,uint8_t end_led, uint8_t step){
	
	if(end_led >= LED_COUNT){
		//LED_RD_ON
		return FALSE;
	}
	if(group_id >= GROUP_COUNT){
		//LED_RD_ON
		return FALSE;
	}
	if(step == 0){
		return FALSE;
	}
	
	if(groups[group_id].start_led == NULL){
		groups[group_id].start_led = &leds[start_led];
	}
	
	led_color_t *current_led = groups[group_id].start_led;
	uint8_t next_led = start_led;
	
	while(current_led->next_led != NULL){
		current_led = current_led->next_led;
	}

	if(current_led == &(leds[start_led])){
		next_led += step;
	}

	while(next_led != end_led){
		current_led->next_led = &leds[next_led];
		current_led = current_led->next_led;
		next_led += step;
	}
	current_led->next_led = &leds[end_led];
	leds[end_led].next_led = NULL;
	return TRUE;
}

void append_led_to_group(uint8_t group_id, uint8_t led_id){
	if(led_id >= LED_COUNT){
		while(1){asm volatile ("nop");}
	}
	if(group_id >= GROUP_COUNT){
		while(1){asm volatile ("nop");}
	}
	
    led_color_t* ledptr = groups[group_id].start_led;
    while(ledptr->next_led != NULL){
        ledptr = ledptr->next_led;
    }
    ledptr->next_led = &leds[led_id];
	ledptr->next_led->next_led = NULL;
}

void color_group(uint8_t group_id, rgbcolor_t color){
    if(group_id >= GROUP_COUNT){
	    while(1){asm volatile ("nop");}
    }
	
	led_color_t* ledptr = groups[group_id].start_led;
    while(ledptr != NULL){
        ledptr->color = color;
        ledptr = ledptr->next_led;
    }
}

void color_led(uint8_t led_id, rgbcolor_t color){
    leds[led_id].color = color;
}

void group_set_properties(uint8_t group_id, uint16_t next_sequence, uint8_t remaining_repititions, uint16_t remaining_time){
	led_group_t *grp = &groups[group_id];
	grp->remaining_time = remaining_time;
	grp->remaining_repititions = remaining_repititions;
	grp->next_sequence = next_sequence;
}


















