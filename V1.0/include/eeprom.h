/*
 * eeprom.h
 *
 * Created: 09.06.2014 17:28:21
 *  Author: stenkelfled
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>

void init_eeprom(void);
void eeprom_write_page(uint8_t* data, uint8_t num, uint8_t page);




#endif /* EEPROM_H_ */