/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to board_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
#include <asf.h>
#include <Leds.h>
#include <avr/io.h>

void usart_init(){
	USARTC1.BAUDCTRLA = 8; //BSEL = 8 --> 1,6667 MHz
	USARTC1.CTRLA |= USART_DREINTLVL_LO_gc;
    USARTC1.CTRLB |= USART_TXEN_bm;
	USARTC1.CTRLC |= USART_CMODE_MSPI_gc | (1<<1); //Master SPI Mode | lsb first
}

void dma_init(){
	DMA.CTRL = 0<<DMA_ENABLE_bp | 0<<DMA_RESET_bp | DMA_DBUFMODE_DISABLED_gc | DMA_PRIMODE_CH0123_gc;
	DMA.CH0.CTRLA |= DMA_CH_SINGLE_bm | DMA_CH_BURSTLEN_8BYTE_gc;
	DMA.CH0.CTRLA &= ~(DMA_CH_REPEAT_bm);
	DMA.CH0.ADDRCTRL |= DMA_CH_SRCRELOAD_TRANSACTION_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_BURST_gc | DMA_CH_DESTDIR_FIXED_gc;
	DMA.CH0.TRIGSRC = DMA_CH_TRIGSRC_USARTC1_DRE_gc;
	DMA.CH0.TRFCNT = LED_COUNT*BYTES_PER_LED;
	DMA.CH0.REPCNT = 1;
}

void fill_buffer(uint8_t src, uint8_t* dest){
    for (uint8_t i=0;i<8;i++){
        /*if(!(i%2)){
            dest[i/2] = 0;
        }*/
        if(src&(1<<i)){
            dest[i/2] |= LED_CODE_1<<(4*(i%2));
        } else {
            dest[i/2] |= LED_CODE_0<<(4*(i%2));
        }
    }
}

int main (void)
{
	sysclk_init();
	board_init();
	
    //!!CAUTION: Check, whether the registers of USART and DMA are really set - in the simulation they weren't.

	usart_init();
	dma_init();

    uint8_t red = 0xFF;
    uint8_t green = 0xF2;
    uint8_t blue = 0x0F;
    led_data_t led_data = {0,0,0};

    fill_buffer(red,led_data.red);
    fill_buffer(green,led_data.green);
    fill_buffer(blue,led_data.blue);

	DMA.CH0.SRCADDR0 = (((uint16_t)&led_data) >> 0 )&0xFF;
	DMA.CH0.SRCADDR1 = (((uint16_t)&led_data) >> 8 )&0xFF;
	DMA.CH0.SRCADDR2 = 0;

	DMA.CH0.DESTADDR0 = (((uint16_t)&USARTC1.DATA) >> 0 )&0xFF;
	DMA.CH0.DESTADDR1 = (((uint16_t)&USARTC1.DATA) >> 8 )&0xFF;
	DMA.CH0.DESTADDR2 = 0;

    DMA.CH0.CTRLA |= DMA_CH_ENABLE_bm;

	return 0;
}
