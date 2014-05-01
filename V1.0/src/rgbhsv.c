/*
 * rgbhsv.c
 *
 * Created: 01.05.2014 12:22:20
 *  Author: stenkelfled
 */ 

#include <rgbhsv.h>

#include <asf.h>

rgbcolor_t hsv2rgb(hsvcolor_t hsv){
	rgbcolor_t rgb;
	uint8_t hi, p, t;
	
	if(hsv.val >=360){
		LED_GN_ON
		while(1){
			asm volatile("nop");
		}
	}
	
	hi = hsv.hue/60;
	p = hsv.val*(255-hsv.sat)/255;
	t = (hsv.val-p)*(hsv.hue-60*hi)/59;
	
	switch(hi){
		case 0:	rgb.red = hsv.val;
				rgb.green = t+p;
				rgb.blue = p;
				break;
		case 1:	rgb.red = hsv.val-t;
				rgb.green = hsv.val;
				rgb.blue = p;
				break;
		case 2: rgb.red = p;
				rgb.green = hsv.val;
				rgb.blue = t+p;
				break;
		case 3:	rgb.red = p;
				rgb.green = hsv.val-t;
				rgb.blue = hsv.val;
				break;
		case 4:	rgb.red = t+p;
				rgb.green = p;
				rgb.blue = hsv.val;
				break;
		case 5:	rgb.red = hsv.val;
				rgb.green = p;
				rgb.blue = hsv.val-t;
				break;
	}
	return rgb;
}

/*hsvcolor_t rgb2hsv(rgbcolor_t){
	
}*/