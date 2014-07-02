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
#include <usb.h>

void ev_switch_state(state_t *new_state);
void ev_switch_state_noentry(state_t *new_state);

//////////////////////////////////////////////////////////////////////////
//state functions
//////////////////////////////////////////////////////////////////////////
//prototypes:
void idle_statefunc(int8_t usb_data);
void group_entryfunc(void);
void group_statefunc(int8_t usb_data);
void row_statefunc1(int8_t usb_data);
void row_statefunc2(int8_t usb_data);
void row_statefunc3(int8_t usb_data);
void led_statefunc(int8_t usb_data);

//state data:
group_data_t group_data = {.current_group=-1, .first_led=-1, .last_led=-1, .step=-1};

//states:
state_t idle_state = {NULL, &idle_statefunc};
state_t group_state = {&group_entryfunc, &group_statefunc};
state_t row_state1 = {NULL, &row_statefunc1};
state_t row_state2 = {NULL, &row_statefunc2};
state_t row_state3 = {NULL, &row_statefunc3};
state_t led_state = {NULL, &led_statefunc};

//functions:
void idle_statefunc(int8_t usb_data){
	if(usb_data == EV_GROUP_START){
		ev_switch_state(&group_state);
	} else if(usb_data == EV_SEQ_START){
		ev_switch_state(NULL);
	} else {
		usb_rx_ready(); //cancel current operation and wait for the next transmission
	}
}

void group_entryfunc(void){
	if(group_data.current_group < (GROUP_COUNT-1)){
		group_data.current_group++;
	} else {
		usb_rx_ready();
	}
	group_data.first_led = -1;
	group_data.last_led = -1;
	group_data.step = -1;
}

void group_statefunc(int8_t usb_data){
	if(usb_data == EV_DEF_ROW){
		ev_switch_state(&row_state1);
	} else if(usb_data == EV_DEF_LED){
		ev_switch_state(&led_state);
	} else if(usb_data == EV_GROUT_END){
		ev_switch_state(&idle_state);
	} else {
		usb_rx_ready();
	}
}

void row_statefunc1(int8_t usb_data){
	group_data.first_led = usb_data;
	ev_switch_state(&row_state2);
}

void row_statefunc2(int8_t usb_data){
	group_data.last_led = usb_data;
	ev_switch_state(&row_state3);
}

void row_statefunc3(int8_t usb_data){
	group_data.step = usb_data;
	define_group(group_data.current_group, group_data.first_led, group_data.last_led, group_data.step);
	ev_switch_state_noentry(&group_state);
}

void led_statefunc(int8_t usb_data){
	append_led_to_group(group_data.current_group,(uint8_t )usb_data);
	ev_switch_state_noentry(&group_state);
}

//////////////////////////////////////////////////////////////////////////
// Evaluator kernel
//////////////////////////////////////////////////////////////////////////

state_t *current_state = &idle_state;

void evaluate(int8_t usb_data){
	current_state->state_func(usb_data);
}

void evaluator_new_evaluation(void){
	current_state = &idle_state;
	group_data.current_group=-1;
	
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

void ev_switch_state_noentry(state_t *new_state){
	if(new_state != NULL){
		current_state = new_state;
	}
}