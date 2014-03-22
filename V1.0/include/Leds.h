/*
 * Leds.h
 *
 * Created: 22.03.2014 12:46:18
 *  Author: stenkelfled
 */ 


#ifndef LEDS_H_
#define LEDS_H_

#define SEND_BITS_PER_DATA_BIT	(4)
#define BYTES_PER_COLOR			(SEND_BITS_PER_DATA_BIT)//*8/8) //*8Bits/Color-Byte, /8send-Bits/data-Byte
#define BYTES_PER_LED			(BYTES_PER_COLOR*3)
#define LED_COUNT				(1)

#define LED_CODE_1				(1111100000) //11100
#define LED_CODE_0				(1100000000) //10000

//#define LED_GN_ON			()

typedef struct{
	unsigned char red[BYTES_PER_COLOR];
	unsigned char green[BYTES_PER_COLOR];
	unsigned char blue[BYTES_PER_COLOR];
}led_data_t;


#endif /* LEDS_H_ */