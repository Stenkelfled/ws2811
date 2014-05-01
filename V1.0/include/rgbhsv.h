/*
 * rgbhsv.h
 *
 * Created: 01.05.2014 12:22:37
 *  Author: stenkelfled
 */ 


#ifndef RGBHSV_H_
#define RGBHSV_H_

#include <compiler.h>

#define RED		(0)
#define GREEN	(1)
#define BLUE	(2)

#define HUE		(4)
#define SAT		(5)
#define VAL		(6)


typedef struct{
	uint8_t green;
	uint8_t red;
	uint8_t blue;
}__attribute__((packed)) rgbcolor_t;

typedef struct{
	uint16_t hue; //0...359
	uint8_t sat; //0...255
	uint8_t val; //0...255
}hsvcolor_t;

rgbcolor_t hsv2rgb(hsvcolor_t hsv);


#endif /* RGBHSV_H_ */