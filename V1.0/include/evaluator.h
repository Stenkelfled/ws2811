/*
 * evaluator.h
 *
 * Created: 01.07.2014 17:29:40
 *  Author: stenkelfled
 */ 


#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include <stdint.h>

typedef struct{
	void (*entry_func)(void);
	void (*state_func)(uint8_t);
}state_t;

void evaluate(uint8_t usb_data);
void evaluator_new_evaluation(void);


//////////////////////////////////////////////////////////////////////////
// evaluator commands
//////////////////////////////////////////////////////////////////////////

#define EV_GROUP_START	(0xA0)
	#define EV_DEF_ROW		(0xA2)
	#define EV_DEF_LED		(0xA3)
#define EV_GROUP_END	(0xA1)

#define EV_SEQ_START	(0xB0)
	#define EV_SEQ_GROUP	(0xB4)
		#define EV_USE_LED		(0xEF)
	#define EV_LEDS_OFF		(0xE0)
	#define EV_SET_RGB		(0xE1)
	#define EV_SET_HSV		(0xE2)
	#define EV_RAINBOW		(0xE3)
	#define EV_SHIFT		(0xEE)
	
	#define EV_BRIGHTNESS	(0xF0)
	#define EV_DIMM			(0xF1)
	
	#define EV_SHIFT_GROUP_START	(0xB2)
	#define EB_SHIFT_GROUP_END		(0xB3)
#define EV_SEQ_END		(0xB1)

#define EV_START		(0xFF)


//////////////////////////////////////////////////////////////////////////
//State data structs
//////////////////////////////////////////////////////////////////////////

// typedef struct{
// 	uint8_t current_group;
// } idle_data_t;

typedef struct{
	uint8_t current_group;
	uint8_t first_led;
	uint8_t last_led;
	uint8_t step;
} group_data_t;

typedef struct{
	uint8_t group_id;
	uint8_t tmp[3];
} seq_data_t;

#endif /* EVALUATOR_H_ */