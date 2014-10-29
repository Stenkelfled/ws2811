/*
 * eeprom.h
 *
 * Created: 09.06.2014 17:28:21
 *  Author: stenkelfled
 */ 


#ifndef EEPROM_H_
#define EEPROM_H_

#define WRITE_LOCK_ENABLED	(1<<0)
//#define WRITE_LOCK_DISABLED	(0<<0)
#define READ_LOCK_ENABLED	(1<<1)
//#define READ_LOCK_DISABLED	(0<<1)

#define RET_OK			(0)
#define RET_WRITE_LOCK	(0x11)
#define RET_READ_LOCK	(0x12)
#define RET_NO_BUFFER	(0x22)

#include <stdint.h>

void init_eeprom(void);
uint8_t eeprom_get_write_access(void);
void eeprom_free_write_access(void);
uint8_t eeprom_get_read_access(void);
void eeprom_free_read_access(void);
//void eeprom_write_page(uint8_t* data, uint8_t num, uint8_t page);
uint8_t eeprom_new_buffer(void);
uint8_t eeprom_buffer_byte(uint8_t data);
uint8_t eeprom_write_buffer(void);




#endif /* EEPROM_H_ */