#ifndef PROTOCOLL_H
#define PROTOCOLL_H

////////////////////////////////////////////////////////////
// Protocoll for communication between PC and LED-Controller
////////////////////////////////////////////////////////////

// Commandset for configuring the leds

//3 Bytes: Preamble

//Message Type:
#define PR_MSG_TYPE_RUN		(0xEE) //store to EEPROM and run
																#define PR_MSG_TYPE_TEST	(0xED)	//set all LEDs to the specified color
																	//## 3 Bytes: RGB
																	//end of message
	//## 2 Bytes: message length (little endian)
	// --- all followed data will be stored in EEPROM ---
	//for every possible group:
		//## 2 Bytes: start-Adress of group (0x0000 if unused)
	
	//for every used group:
	
	//first: add as much leds as you like ;)
		#define PR_GRP_LED_SER (0xA2) //add a series of LEDs; add first-ID + n*step until ID>=last-ID
			//## 1 Byte: ID of first LED
			//## 1 Byte: ID of last LED
			//## 1 Byte: step size
		#define PR_GRP_LED_ADD (0xA3) //add a single LED
			//## 1 Byte: ID of LED
			
	#define PR_GRP_SEQ_START (0xA4) //second: define as much sequences as the EEPROM can hold :-P
		//for each sequence:
		//## 1 Byte: Repetitions of this group !!RESERVED: 0xFF for no repetitions!!
		//## 2 Byte: address of next sequence (if there is no other sequence: 0x0000; if this is the last sequence: address of first sequence
		//## 2 Byte: Duration (show this sequence Duration*10ms)
		
		//do color stuff
		#define PR_GRP_SEQ_SET_OFF (0xB0)
		#define PR_GRP_SEQ_SET_RGB (0xB1) //set a color for this group
			//## 3Bytes: RGB
/*		#define PR_GRP_SEQ_ADD_RGB (0xB2) //add this RGB-value to the current color (until saturation either 0x00 or 0xFF)
			//## 3Bytes(signed): RGB-difference
		#define PR_GRP_SEQ_ADD_HSV (0xB3) //add this HSV-value to the current color (until saturation either 0x00 or 0xFF)
			//## 3Bytes(signed): HSV-difference
*/
			
/*		#define PR_GRP_SEQ_SET_LED_RGB (0xC1) //set this RGB-value to a specified LED
			//## 1Byte: number of LED in group
			//## 3Bytes: RGB
		#define PR_GRP_SEQ_SET_LED_ADD_RGB (0xC2) //add this RGB-value to all following LEDs
			//## 3Bytes(signed): RGB-difference
		#define PR_GRP_SEQ_SET_LED_ADD_HSV (0xC3) //add this HSV-value to all following LEDs
			//## 3Bytes(signed): HSV-difference
			
		//action!
		#define PR_GRP_SEQ_ACT_RROT (0xD0) //rotate all LEDs by 1 to Right 
		#define PR_GRP_SEQ_ACT_RROT_NUM	(0xD1) //rotate all LEDs by a given number to right
			//## 1Byte: number
		#define PR_GRP_SEQ_ACT_RSHIFT (0xD9) //shift all LEDs 1 to right and fill from left with given value
			//## 3Byte: RGB
*/


#endif
