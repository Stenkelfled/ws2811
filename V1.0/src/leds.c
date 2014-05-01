/*
 * leds.c
 *
 * Created: 25.04.2014 12:28:50
 *  Author: stenkelfled
 */
#include <asf.h>
#include <util/delay.h>
#include <leds.h>

uint8_t zero = 0;

void usart_init(void){
	USARTC1.BAUDCTRLA = 2; //BSEL = 2 --> 3,333 MHz @f_per = 20MHz; this are 0.3us/spi-bit and 1.2us per data bit
	//other idea, BSEL=3 --> 2,5MHz and only 3spi-bits per data-bit
	USARTC1.CTRLA = USART_DREINTLVL_LO_gc;
	//USARTC1.CTRLB = USART_TXEN_bm;
	USARTC1.CTRLC = USART_CMODE_MSPI_gc;
	USARTC1.CTRLA = USART_DREINTLVL_MED_gc;
}

void dma_init(uint8_t* ledbuffer){
	DMA.CTRL = DMA_ENABLE_bm;
	DMA.CTRL |= DMA_DBUFMODE_DISABLED_gc | DMA_PRIMODE_CH0123_gc;
	DMA.CH0.CTRLA = DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_1BYTE_gc;
	DMA.CH0.ADDRCTRL = DMA_CH_SRCRELOAD_BLOCK_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_NONE_gc | DMA_CH_DESTDIR_FIXED_gc;
	DMA.CH0.TRIGSRC = DMA_CH_TRIGSRC_USARTC1_DRE_gc;
	DMA.CH0.TRFCNT = LED_COUNT*BYTES_PER_LED;
	
	DMA.CH0.SRCADDR0 = (((uint16_t)ledbuffer) >> 0 )&0xFF;
	DMA.CH0.SRCADDR1 = (((uint16_t)ledbuffer) >> 8 )&0xFF;
	DMA.CH0.SRCADDR2 = 0;

	DMA.CH0.DESTADDR0 = (((uint16_t)&USARTC1.DATA) >> 0 )&0xFF;
	DMA.CH0.DESTADDR1 = (((uint16_t)&USARTC1.DATA) >> 8 )&0xFF;
	DMA.CH0.DESTADDR2 = 0;
	
	
	DMA.CH1.REPCNT = 0;
	DMA.CH1.CTRLA = DMA_CH_REPEAT_bm | DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_1BYTE_gc;
	DMA.CH1.ADDRCTRL = DMA_CH_SRCRELOAD_NONE_gc | DMA_CH_SRCDIR_FIXED_gc | DMA_CH_DESTRELOAD_NONE_gc | DMA_CH_DESTDIR_FIXED_gc;
	DMA.CH1.TRIGSRC = DMA_CH_TRIGSRC_USARTC1_DRE_gc;
	DMA.CH1.TRFCNT = 1;
	
	DMA.CH1.SRCADDR0 = (((uint16_t)&zero) >> 0 )&0xFF;
	DMA.CH1.SRCADDR1 = (((uint16_t)&zero) >> 8 )&0xFF;
	DMA.CH1.SRCADDR2 = 0;

	DMA.CH1.DESTADDR0 = (((uint16_t)&USARTC1.DATA) >> 0 )&0xFF;
	DMA.CH1.DESTADDR1 = (((uint16_t)&USARTC1.DATA) >> 8 )&0xFF;
	DMA.CH1.DESTADDR2 = 0;		
}

void start_leds(void){
	//switch on levelshifter
	RGBLED_ENABLE_TX
	_delay_us(10);
	USARTC1.CTRLB = USART_TXEN_bm;
	DMA.CH1.CTRLA |= DMA_CH_ENABLE_bm;
}

void refresh_leds(void){
	
	LED_GN_ON
	DMA.CH0.CTRLA |= DMA_CH_ENABLE_bm;
	/*asm volatile ("nop");
	asm volatile ("nop");
	USARTC1.CTRLB = USART_TXEN_bm;*/
	//while(!(USARTC1.STATUS & USART_TXCIF_bm)){
	while( (DMA.CH0.CTRLB&DMA_CH_CHBUSY_bm) ){
		//asm volatile("nop");
	}
	//USARTC1.CTRLB &= ~USART_TXEN_bm; //disable USART transmitter
	//reset_leds();
	LED_GN_OFF
	/*ioport_set_pin_dir(RGBLED_DATA_PIN,IOPORT_DIR_OUTPUT);
	ioport_set_pin_level(RGBLED_DATA_PIN,false);*/
	
	USARTC1.STATUS |= USART_TXCIF_bm; //clear TXCIF flag
	
	_delay_us(60);
}