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

void eeprom_new_buffer(void){
	current_page = 0;
	current_page_addr = 0;
	while(NVM_STATUS&NVM_NVMBUSY_bm){}
	//erase page buffer
	NVM_CMD = NVM_CMD_ERASE_EEPROM_BUFFER_gc;
	ccp_write_io((void*)&NVM_CTRLA, NVM_CMDEX_bm);
}

void eeprom_buffer_byte(uint8_t data){
	while(NVM_STATUS&NVM_NVMBUSY_bm){}
	NVM_CMD = NVM_CMD_LOAD_EEPROM_BUFFER_gc;
	NVM_ADDR0 = current_page_addr++;
	NVM_DATA0 = data;
	
	if(current_page_addr >= EEPROM_PAGE_SIZE){
		eeprom_write_buffer();
	}
}

void eeprom_write_buffer(void){
	if(current_page_addr == 0){
		//no data in page buffer
		return;
	}
	uint16_t page_addr = current_page*EEPROM_PAGE_SIZE;
	while(NVM_STATUS&NVM_NVMBUSY_bm){}
	NVM_CMD = NVM_CMD_ERASE_WRITE_EEPROM_PAGE_gc;
	NVM_ADDR1 = (page_addr&0xFF00)>>8;
	NVM_ADDR0 = page_addr&0xFF;
	ccp_write_io((void*)&NVM_CTRLA, NVM_CMDEX_bm);
	current_page++;
	current_page_addr = 0;
}