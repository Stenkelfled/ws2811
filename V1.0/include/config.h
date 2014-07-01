/*
 * config.h
 *
 * Created: 07.05.2014 17:39:17
 *  Author: stenkelfled
 */ 


#ifndef CONFIG_H_
#define CONFIG_H_
#include <avr/io.h>

#define BUTTON_PORT		(PORTE)
#define BUTTON1_PIN		(1)
#define BUTTON2_PIN		(2)

#define USB_PORT		(PORTD)
#define USB_POWER_PIN	(5)

void init_interrupts(void);
void init_clock(void);
void init_ports(void);

#define USB_PREAMBLE		"Led"
#define USB_PREAMBLE_LEN	(3)


#endif /* CONFIG_H_ */