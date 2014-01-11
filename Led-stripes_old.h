#ifndef LED-STRIPES_H_
#define LED-STRIPES_H_

#define BYTES_PER_COLOR		(30)
#define LED_COUNT			(200)

#define LED_CODE_1			(1111100000) //11100
#define LED_CODE_0			(1100000000) //10000

#define LED_GN_ON			()

typedef struct{
	unsigned char red[10];
	unsigned char green[10];
	unsigned char blue[10];
	}led_data_t;



#endif /* LED-STRIPES_H_ */