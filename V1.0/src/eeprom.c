/*
 * eeprom.c
 *
 * Created: 09.06.2014 17:28:11
 *  Author: stenkelfled
 */ 

#include <eeprom.h>
#include <asf.h>

uint8_t eeprom_locks = WRITE_LOCK_ENABLED | READ_LOCK_ENABLED;

void init_eeprom(void){
	//NVM_CTRLB = NVM_EEMAPEN_bm;
	NVM_ADDR2 = 0;
}

uint8_t eeprom_get_write_access(void){
	cli();
	if(eeprom_locks&READ_LOCK_ENABLED){
		eeprom_locks &= ~WRITE_LOCK_ENABLED;
		sei();
		return RET_OK;
	} else {
		sei();
		return RET_WRITE_LOCK;
	}
}

void eeprom_free_write_access(void){
	cli();
	eeprom_locks |= WRITE_LOCK_ENABLED;
	sei();
}

uint8_t eeprom_get_read_access(void){
	cli();
	if(eeprom_locks&WRITE_LOCK_ENABLED){
		eeprom_locks &= ~READ_LOCK_ENABLED;
		sei();
		return RET_OK;
	} else {
		sei();
		return RET_READ_LOCK;
	}
}

void eeprom_free_read_access(void){
	cli();
	eeprom_locks |= READ_LOCK_ENABLED;
	sei();
}

uint8_t eeprom_my_read_uint8(uint16_t addr){
	NVM_CMD = NVM_CMD_READ_EEPROM_gc;
	NVM_ADDR1 = (addr&0xFF00)>>8;
	NVM_ADDR0 = addr&0xFF;
	ccp_write_io((void*)&NVM_CTRLA, NVM_CMDEX_bm);
	while(NVM_STATUS&NVM_NVMBUSY_bm){}
	return NVM_DATA0;
}

uint16_t eeprom_my_read_uint16(uint16_t addr){
	uint16_t eeprom_data = eeprom_my_read_uint8(addr);
	eeprom_data |= (eeprom_my_read_uint8(addr+1) << 8);
	return eeprom_data;
}

/*void eeprom_write_page(uint8_t* data, uint8_t num, uint8_t page){
	uint8_t i;
	uint16_t page_addr = page*EEPROM_PAGE_SIZE;
	
	//TODO: support data for more than one page
	if(num > EEPROM_PAGE_SIZE){
		LED_RD_ON
		while(1){}
	}
	
	while(NVM_STATUS&NVM_NVMBUSY_bm){}
	
	//erase page buffer
	NVM_CMD = NVM_CMD_ERASE_EEPROM_BUFFER_gc;
	ccp_write_io((void*)&NVM_CTRLA, NVM_CMDEX_bm);
	while(NVM_STATUS&NVM_NVMBUSY_bm){}
		
	NVM_CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;
	for(i=0; i<num; i++){		
		NVM_ADDR0 = i;	
		NVM_DATA0 = (*data)++;
		while(NVM_STATUS&NVM_NVMBUSY_bm){}
	}
	
	NVM_CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
	NVM_ADDR1 = (page_addr&0xFF00)>>8;
	NVM_ADDR0 = page_addr&0xFF;
	ccp_write_io((void*)&NVM_CTRLA, NVM_CMDEX_bm);
	while(NVM_STATUS&NVM_NVMBUSY_bm){}
}*/

uint8_t current_page = 0;
uint8_t current_page_addr = 0;

uint8_t eeprom_new_buffer(void){
	if(eeprom_locks & WRITE_LOCK_ENABLED){
		return RET_WRITE_LOCK;
	}
	current_page = 0;
	current_page_addr = 0;
	while(NVM_STATUS&NVM_NVMBUSY_bm){}
	//erase page buffer
	NVM_CMD = NVM_CMD_ERASE_EEPROM_BUFFER_gc;
	ccp_write_io((void*)&NVM_CTRLA, NVM_CMDEX_bm);
	return RET_OK;
}

uint8_t eeprom_buffer_byte(uint8_t data){
	if(eeprom_locks & WRITE_LOCK_ENABLED){
		return RET_WRITE_LOCK;
	}
	while(NVM_STATUS&NVM_NVMBUSY_bm){}
	NVM_CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;
	NVM_ADDR0 = current_page_addr++;
	NVM_DATA0 = data;
	
	if(current_page_addr >= EEPROM_PAGE_SIZE){
		eeprom_write_buffer();
	}
	return RET_OK;
}

uint8_t eeprom_write_buffer(void){
	if(eeprom_locks & WRITE_LOCK_ENABLED){
		return RET_WRITE_LOCK;
	}
	if( !(NVM_STATUS&NVM_EELOAD_bm) ){
		//no data in page buffer
		return RET_NO_BUFFER;
	}
	uint16_t page_addr = current_page*EEPROM_PAGE_SIZE;
	while(NVM_STATUS&NVM_NVMBUSY_bm){}
	NVM_CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
	NVM_ADDR1 = (page_addr&0xFF00)>>8;
	NVM_ADDR0 = page_addr&0xFF;
	ccp_write_io((void*)&NVM_CTRLA, NVM_CMDEX_bm);
	current_page++;
	current_page_addr = 0;
	return RET_OK;
}