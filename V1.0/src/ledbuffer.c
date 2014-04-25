/*
 * ledbuffer.c
 *
 * Created: 28.03.2014 10:49:18
 *  Author: stenkelfled
 */ 

#include <ledbuffer.h>
#include <leds.h>

led_color_t leds[LED_COUNT];
led_group_t groups[GROUP_COUNT];

uint8_t led_data[LED_COUNT*BYTES_PER_LED];

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

void define_group(uint8_t group_id, uint8_t start_led,uint8_t end_led, uint8_t step){    
    uint8_t current_led = start_led;
    uint8_t next_led = -1;
    groups[group_id].start_led = &leds[start_led];

    while(current_led != end_led){
        next_led = current_led + step;
        if(next_led > end_led) next_led = end_led;
        leds[current_led].next_led = &leds[next_led];
        current_led = next_led;
    }
    leds[end_led].next_led = NULL;
}

void append_to_group(uint8_t group_id, uint8_t led_id){
    led_color_t* ledptr = groups[group_id].start_led;
    while(ledptr->next_led != NULL){
        ledptr = ledptr->next_led;
    }
    ledptr->next_led = &leds[led_id];
}

void color_group(uint8_t group_id, rgbcolor_t color){
    led_color_t* ledptr = groups[group_id].start_led;
    while(ledptr != NULL){
        ledptr->color = color;
        ledptr = ledptr->next_led;
    }
}

void color_led(uint8_t led_id, rgbcolor_t color){
    leds[led_id].color = color;
}