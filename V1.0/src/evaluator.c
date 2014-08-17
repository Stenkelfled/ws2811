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
#include <protocoll.h>
#include <rgbhsv.h>
#include <usbprint.h>

void ev_switch_state(state_t *new_state);
void ev_switch_state_fromchild(state_t *new_state);
void ev_abort_all(void);
void ev_ready(void);

//////////////////////////////////////////////////////////////////////////
//state functions
//////////////////////////////////////////////////////////////////////////
//prototypes:
void idle_statefunc(uint8_t usb_data);
void run_red_statefunc(uint8_t usb_data);
void run_green_statefunc(uint8_t usb_data);
void run_blue_statefunc(uint8_t usb_data);


//state data:
rgbcolor_t run_color;

//states:
state_t idle_state = {NULL, &idle_statefunc};
state_t run_red_state = {NULL, &run_red_statefunc};
state_t run_green_state = {NULL, &run_green_statefunc};
state_t run_blue_state = {NULL, &run_blue_statefunc};


//functions:
void idle_statefunc(uint8_t usb_data){
	usb_print("idle\n");
	if(usb_data == PR_MSG_TYPE_TEST){
		ev_switch_state(&run_red_state);
	} else if(usb_data == PR_MSG_TYPE_RUN){
		//TODO
		//ev_switch_state(&eeprom_len_state1);
	} else {
		usb_print("idle: unknown cmd\n");
		ev_abort_all(); //cancel current operation and wait for the next transmission
 	}
}

void run_red_statefunc(uint8_t usb_data){
	usb_print("red_statefunc\n");
	run_color.red = usb_data;
	ev_switch_state(&run_green_state);
}

void run_green_statefunc(uint8_t usb_data){
	usb_print("green_statefunc\n");
	run_color.green = usb_data;
	ev_switch_state(&run_blue_state);
}

void run_blue_statefunc(uint8_t usb_data){
	usb_print("blue_statefunc\n");
	run_color.blue = usb_data;
	
	define_group(0, 0, GLOBAL_LED_COUNT-1, 1);
	color_group(0, run_color);
	fill_buffer();
	refresh_leds();
	
	ev_ready();
}

//////////////////////////////////////////////////////////////////////////
// Evaluator kernel
//////////////////////////////////////////////////////////////////////////

state_t *current_state = &idle_state;
void (*evaluator_ready_fcn)(void);

/**
 * evaluate the current usb data
 * @param: uint8_t usb_data: the received usv-data
 */
void evaluate(uint8_t usb_data){
	usb_print("ev:'");
	usb_printx(usb_data);
	usb_print("'\n");
	current_state->state_func(usb_data);
}

/**
 * start a new evaluation
 * @param: void (*rdy_fctn)(void): function-pointer to a function that will be called when the evaluator is ready
 */
void evaluator_new_evaluation(void (*rdy_fctn)(void)){
	evaluator_ready_fcn = rdy_fctn;
	current_state = &idle_state;
}

//private functions
/**
 * switch the evaluator state, including calling the entry-state(if present)
 * @param: state_t *new_state: the next state
 */
void ev_switch_state(state_t *new_state){
	if(new_state != NULL){
		current_state = new_state;
		if(current_state->entry_func != NULL){
			current_state->entry_func();
		}
	}
}

/**
 * reenter an evaluator state, but without calling the entry-state
 * @param: state_t *new_state: the state to enter
 */
void ev_switch_state_fromchild(state_t *new_state){
	if(new_state != NULL){
		current_state = new_state;
	}
}

/**
 * abort the evaluation
 */
void ev_abort_all(void){
	usb_print("ev abort\n");
	evaluator_ready_fcn();
}

/**
 * evaluation is finished
 */
void ev_ready(void){
	usb_print("ev ready\n");
	evaluator_ready_fcn();
}