/*
 * eeprom.c
 *
 * Created: 09.06.2014 17:28:11
 *  Author: stenkelfled
 */ 

#include <eeprom.h>
#include <asf.h>

void init_eeprom(void){
	//NVM_CTRLB = NVM_EEMAPEN_bm;
	NVM_ADDR2 = 0;
}

void eeprom_write_page(uint8_t* data, uint8_t num, uint8_t page){
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
}