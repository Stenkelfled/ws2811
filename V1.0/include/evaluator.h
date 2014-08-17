/*
 * evaluator.h
 *
 * Created: 01.07.2014 17:29:40
 *  Author: stenkelfled
 */ 


#ifndef EVALUATOR_H_
#define EVALUATOR_H_

#include <stdint.h>

#define INVALID_1		((uint8_t)-1)

typedef struct{
	void (*entry_func)(void);
	void (*state_func)(uint8_t);
}state_t;

void evaluate(uint8_t usb_data);
void evaluator_new_evaluation(void (*rdy_fctn)(void));


//////////////////////////////////////////////////////////////////////////
//State data structs
//////////////////////////////////////////////////////////////////////////

// typedef struct{
// 	uint8_t current_group;
// } idle_data_t;



#endif /* EVALUATOR_H_ */