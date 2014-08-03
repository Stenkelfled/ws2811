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
void eeprom_len_statefunc1(uint8_t usb_data);
void eeprom_len_statefunc2(uint8_t usb_data);
void eeprom_len_statefunc3(uint8_t usb_data);
void eeprom_len_statefunc4(uint8_t usb_data);
void eeprom_store_statefunc(uint8_t usb_data);
void grp_entryfunc(void);
void grp_statefunc(uint8_t usb_data);
void grp_row_statefunc1(uint8_t usb_data);
void grp_row_statefunc2(uint8_t usb_data);
void grp_row_statefunc3(uint8_t usb_data);
void grp_led_statefunc(uint8_t usb_data);
void seq_entryfunc(void);
void seq_statefunc(uint8_t usb_data);
void seq_group_statefunc(uint8_t usb_data);
void seq_off_statefunc(uint8_t usb_data);
void seq_rgb_statefunc1(uint8_t usb_data);
void seq_rgb_statefunc2(uint8_t usb_data);
void seq_rgb_statefunc3(uint8_t usb_data);

//state data:
eeprom_data_t eeprom_data;
group_data_t group_data = {.current_group=INVALID_1, .first_led=INVALID_1, .last_led=INVALID_1, .step=INVALID_1};
seq_data_t seq_data = {.group_id = INVALID_1};

//states:
state_t idle_state = {NULL, &idle_statefunc};
state_t eeprom_len_state1 = {NULL, &eeprom_len_statefunc1};
state_t eeprom_len_state2 = {NULL, &eeprom_len_statefunc2};
state_t eeprom_len_state3 = {NULL, &eeprom_len_statefunc3};
state_t eeprom_len_state4 = {NULL, &eeprom_len_statefunc4};
state_t eeprom_store_state = {NULL, &eeprom_store_statefunc};
state_t grp_state = {&grp_entryfunc, &grp_statefunc};
state_t grp_row_state1 = {NULL, &grp_row_statefunc1};
state_t grp_row_state2 = {NULL, &grp_row_statefunc2};
state_t grp_row_state3 = {NULL, &grp_row_statefunc3};
state_t grp_led_state = {NULL, &grp_led_statefunc};
state_t seq_state = {&seq_entryfunc, &seq_statefunc};
state_t seq_group_state = {NULL, &seq_group_statefunc};
state_t seq_off_state = {NULL, &seq_off_statefunc};
state_t seq_rgb_state1 = {NULL, &seq_rgb_statefunc1};
state_t seq_rgb_state2 = {NULL, &seq_rgb_statefunc2};
state_t seq_rgb_state3 = {NULL, &seq_rgb_statefunc3};

//functions:
void idle_statefunc(uint8_t usb_data){
	usb_print("idle\n");
	if(usb_data == EV_ON_THE_GO){
		//wait for next command
	} else if(usb_data = EV_TO_EEPROM){
		ev_switch_state(&eeprom_len_state1);
	} else if(usb_data == EV_GROUP_START){
		ev_switch_state(&grp_state);
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

void eeprom_len_statefunc1(uint8_t usb_data){
	eeprom_data.len = (uint16_t)usb_data;
	ev_switch_state(&eeprom_len_state2);
}

void eeprom_len_statefunc2(uint8_t usb_data){
	eeprom_data.len = ((uint16_t)usb_data) << 8;
	ev_switch_state(&eeprom_len_state3);
}

void eeprom_len_statefunc3(uint8_t usb_data){
	eeprom_data.num_groups = usb_data;
	ev_switch_state(&eeprom_len_state4);
}

void eeprom_len_statefunc4(uint8_t usb_data){
	eeprom_data.num_seqs = usb_data;
	ev_switch_state(&eeprom_store_state);
}

void eeprom_store_statefunc(uint8_t usb_data){
	usb_print("eeprom store not implemented yet!");
	ev_abort_all();
}

void grp_entryfunc(void){
	usb_print("group entry\n");
	if( (group_data.current_group < (GROUP_COUNT-1)) || (group_data.current_group == INVALID_1) ){
		group_data.current_group++;
	} else {
		ev_abort_all();
	}
	group_data.first_led = INVALID_1;
	group_data.last_led = INVALID_1;
	group_data.step = INVALID_1;
}

void grp_statefunc(uint8_t usb_data){
	usb_print("group\n");
	if(usb_data == EV_DEF_ROW){
		ev_switch_state(&grp_row_state1);
	} else if(usb_data == EV_DEF_LED){
		ev_switch_state(&grp_led_state);
	} else if(usb_data == EV_GROUP_END){
		ev_switch_state(&idle_state);
	} else {
		ev_abort_all();
	}
}

void grp_row_statefunc1(uint8_t usb_data){
	group_data.first_led = usb_data;
	ev_switch_state(&grp_row_state2);
}

void grp_row_statefunc2(uint8_t usb_data){
	group_data.last_led = usb_data;
	ev_switch_state(&grp_row_state3);
}

void grp_row_statefunc3(uint8_t usb_data){
	group_data.step = usb_data;
	if(!define_group(group_data.current_group, group_data.first_led, group_data.last_led, group_data.step)){
		ev_abort_all();
	}
	ev_switch_state_fromchild(&grp_state);
}

void grp_led_statefunc(uint8_t usb_data){
	append_led_to_group(group_data.current_group,(uint8_t )usb_data);
	ev_switch_state_fromchild(&grp_state);
}

void seq_entryfunc(void){
	seq_data.group_id = INVALID_1;
}

void seq_statefunc(uint8_t usb_data){
	usb_print("state\n");
	if(seq_data.group_id == INVALID_1){ //invalid group id
		if(usb_data == EV_SEQ_GROUP){
			ev_switch_state(&seq_group_state);
			return;
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
	if(usb_data == INVALID_1){ //invalid group id
		ev_abort_all();
		return;
	}
	seq_data.group_id = usb_data;
	ev_switch_state_fromchild(&seq_state);
}

void seq_off_statefunc(uint8_t usb_data){
	rgbcolor_t color = {0,0,0};
	color_group(seq_data.group_id, color);
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
void (*evaluator_ready_fcn)(void);

void evaluate(uint8_t usb_data){
	usb_print("ev:'");
	usb_printx(usb_data);
	usb_print("'\n");
	current_state->state_func(usb_data);
}

void evaluator_new_evaluation(void (*rdy_fctn)(void)){
	evaluator_ready_fcn = rdy_fctn;
	current_state = &idle_state;
	group_data.current_group = INVALID_1;
	seq_data.group_id = INVALID_1;
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
	evaluator_ready_fcn();
}

void ev_ready(void){
	usb_print("ev ready\n");
	evaluator_ready_fcn();
}