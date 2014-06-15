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

#define CLR_RED		255,0  ,0
#define CLR_GREEN	0  ,255,0
#define CLR_BLUE	0  ,0  ,255
#define CLR_YELLOW	255,255,0
#define CLR_CYAN	0  ,255,255
#define CLR_MAGENTA	255,0  ,255
#define CLR_WHITE	255,255,255


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
void set_predefined_color(rgbcolor_t* clr, uint8_t red, uint8_t green, uint8_t blue);


#endif /* RGBHSV_H_ */