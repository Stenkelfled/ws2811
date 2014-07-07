/*
 * evaluator.c
 *
 * Created: 01.07.2014 17:28:54
 *  Author: stenkelfled
 *
 * evaluates the data from EEPROM/USB and generates nice led-blinking
 */ 

#include <stddef.h>

#include <config.h>
#include <evaluator.h>
#include <ledbuffer.h>
#include <Leds.h>
#include <usb.h>

void ev_switch_state(state_t *new_state);
void ev_switch_state_fromchild(state_t *new_state);
void ev_abort_all(void);
void ev_ready(void);

//////////////////////////////////////////////////////////////////////////
//state functions
//////////////////////////////////////////////////////////////////////////
//prototypes:
void idle_statefunc(uint8_t usb_data);
void group_entryfunc(void);
void group_statefunc(uint8_t usb_data);
void row_statefunc1(uint8_t usb_data);
void row_statefunc2(uint8_t usb_data);
void row_statefunc3(uint8_t usb_data);
void led_statefunc(uint8_t usb_data);
void seq_entryfunc(void);
void seq_statefunc(uint8_t usb_data);
void seq_group_statefunc(uint8_t usb_data);
void seq_rgb_statefunc1(uint8_t usb_data);
void seq_rgb_statefunc2(uint8_t usb_data);
void seq_rgb_statefunc3(uint8_t usb_data);

//state data:
group_data_t group_data = {.current_group=-1, .first_led=-1, .last_led=-1, .step=-1};
seq_data_t seq_data = {.group_id = -1};

//states:
state_t idle_state = {NULL, &idle_statefunc};
state_t group_state = {&group_entryfunc, &group_statefunc};
state_t row_state1 = {NULL, &row_statefunc1};
state_t row_state2 = {NULL, &row_statefunc2};
state_t row_state3 = {NULL, &row_statefunc3};
state_t led_state = {NULL, &led_statefunc};
state_t seq_state = {&seq_entryfunc, &seq_statefunc};
state_t seq_group_state = {NULL, &seq_group_statefunc};
state_t seq_rgb_state1 = {NULL, &seq_rgb_statefunc1};
state_t seq_rgb_state2 = {NULL, &seq_rgb_statefunc2};
state_t seq_rgb_state3 = {NULL, &seq_rgb_statefunc3};

//functions:
void idle_statefunc(uint8_t usb_data){
	usb_print("idle\n");
// 	switch(usb_data){
// 		case EV_GROUP_START:
// 			ev_switch_state(&group_state);
// 			break;
// 		case EV_SEQ_START:
// 			ev_switch_state(&seq_state);
// 			break;
// 		case EV_START:
// 			fill_buffer();
// 	 		refresh_leds();
//  			ev_ready();
// 			break;
// 		default:
// 			usb_print("idle: unknown cmd\n");
// 			ev_abort_all(); //cancel current operation and wait for the next transmission
// 	}
	if(usb_data == EV_GROUP_START){
		ev_switch_state(&group_state);
	} else if(usb_data == EV_SEQ_START){
		ev_switch_state(&seq_state);
	} else if(usb_data == EV_START){
		fill_buffer();
		refresh_leds();
		ev_ready();
	} else {
		usb_print("idle: unknown cmd\n");
		ev_abort_all(); //cancel current operation and wait for the next transmission
 	}
}

void group_entryfunc(void){
	usb_print("group entry\n");
	if(group_data.current_group < (GROUP_COUNT-1)){
		group_data.current_group++;
	} else {
		ev_abort_all();
	}
	group_data.first_led = -1;
	group_data.last_led = -1;
	group_data.step = -1;
}

void group_statefunc(uint8_t usb_data){
	usb_print("group\n");
	if(usb_data == EV_DEF_ROW){
		ev_switch_state(&row_state1);
	} else if(usb_data == EV_DEF_LED){
		ev_switch_state(&led_state);
	} else if(usb_data == EV_GROUP_END){
		ev_switch_state(&idle_state);
	} else {
		ev_abort_all();
	}
}

void row_statefunc1(uint8_t usb_data){
	group_data.first_led = usb_data;
	ev_switch_state(&row_state2);
}

void row_statefunc2(uint8_t usb_data){
	group_data.last_led = usb_data;
	ev_switch_state(&row_state3);
}

void row_statefunc3(uint8_t usb_data){
	group_data.step = usb_data;
	if(!define_group(group_data.current_group, group_data.first_led, group_data.last_led, group_data.step)){
		ev_abort_all();
	}
	ev_switch_state_fromchild(&group_state);
}

void led_statefunc(uint8_t usb_data){
	append_led_to_group(group_data.current_group,(uint8_t )usb_data);
	ev_switch_state_fromchild(&group_state);
}

void seq_entryfunc(void){
	seq_data.group_id = -1;
}

void seq_statefunc(uint8_t usb_data){
	usb_print("state\n");
	if(seq_data.group_id == -1){ //invalid group id
		if(usb_data == EV_SEQ_GROUP){
			ev_switch_state(&seq_group_state);
		} else {
			ev_abort_all();
		}
	}
	
	if(usb_data == EV_SEQ_END){
		ev_switch_state_fromchild(&idle_state);
	} else if(usb_data == EV_SET_RGB){
		ev_switch_state(&seq_rgb_state1);
	} else {
		ev_abort_all();
	}
}

void seq_group_statefunc(uint8_t usb_data){
	if(usb_data == -1){ //invalid group id
		ev_abort_all();
		return;
	}
	seq_data.group_id = usb_data;
	ev_switch_state_fromchild(&seq_state);
}

void seq_rgb_statefunc1(uint8_t usb_data){
	seq_data.tmp[0] = usb_data;
	ev_switch_state(&seq_rgb_state2);
}

void seq_rgb_statefunc2(uint8_t usb_data){
	seq_data.tmp[1] = usb_data;
	ev_switch_state(&seq_rgb_state3);
}

void seq_rgb_statefunc3(uint8_t usb_data){
	seq_data.tmp[2] = usb_data;
	rgbcolor_t color = {.red=seq_data.tmp[0], .green=seq_data.tmp[1], .blue=seq_data.tmp[2]};
	color_group(seq_data.group_id, color);
	ev_switch_state_fromchild(&seq_state);
}

//////////////////////////////////////////////////////////////////////////
// Evaluator kernel
//////////////////////////////////////////////////////////////////////////

state_t *current_state = &idle_state;

void evaluate(uint8_t usb_data){
	usb_print("ev:'");
	usb_printx(usb_data);
	usb_print("'\n");
	current_state->state_func(usb_data);
}

void evaluator_new_evaluation(void){
	current_state = &idle_state;
	group_data.current_group = -1;
	seq_data.group_id = -1;
}

//private functions
void ev_switch_state(state_t *new_state){
	if(new_state != NULL){
		current_state = new_state;
		if(current_state->entry_func != NULL){
			current_state->entry_func();
		}
	}
}

void ev_switch_state_fromchild(state_t *new_state){
	if(new_state != NULL){
		current_state = new_state;
	}
}

void ev_abort_all(void){
	usb_print("ev abort\n");
	usb_rx_ready();
}

void ev_ready(void){
	usb_print("ev ready\n");
	usb_rx_ready();
}