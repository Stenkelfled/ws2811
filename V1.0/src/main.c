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

void usart_init(){
	USARTC1_BAUDCTRLA = 8; //BSEL = 8 --> 1,6667 MHz
	USARTC1_CTRLB |= 1<<USART_RXEN_bp;
	USARTC1_CTRLC |= USART_CMODE_MSPI_gc | (1<<1); //Master SPI Mode | lsb first
}

void dma_init(){
	DMA_CTRL = 0<<DMA_ENABLE_bp | 0<<DMA_RESET_bp | DMA_DBUFMODE_DISABLED_gc | DMA_PRIMODE_CH0123_gc;
	DMA_CH0_CTRLA |= DMA_CH_BURSTLEN_8BYTE_gc;
	DMA_CH0_ADDRCTRL |= DMA_CH_SRCRELOAD_TRANSACTION_gc | DMA_CH_SRCDIR_INC_gc | DMA_CH_DESTRELOAD_BURST_gc | DMA_CH_DESTDIR_FIXED_gc;
	DMA_CH0_TRIGSRC = DMA_CH_TRIGSRC_USARTC1_DRE_gc;
	DMA_CH0_TRFCNT = LED_COUNT*BYTES_PER_LED;
	
}

int main (void)
{
	sysclk_init();
	board_init();
	
	usart_init();
	dma_init();
	
	led_data_t led_data = {
		.red = 123,
		.green = 212,
		.blue = 050
	};

	// Insert application code here, after the board has been initialized.
	return 0;
}
