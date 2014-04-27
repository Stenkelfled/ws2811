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
#include <util/delay.h>

#include <leds.h>
#include <ledbuffer.h>

void clock_init(void);

void clock_init(void){
    //Setup: calibrate 32MHz osc as 48MHz for USB
    //use 2MHz osc with PLL(mul = 10) as 20MHz CPU and peripheral clock
    //use for USART in SPI mode: BSEL=2 -> Baudrate=3.333333MHz ->1Bit = 300ns -> 4Bit per Color-bit

    OSC.CTRL |= OSC_RC32MEN_bm; //enable 32MHz oscillator
    while(!(OSC.STATUS&OSC_RC32MRDY_bm)){} //wait for 32MHz oscillator to be ready
    
    //set up the DFLL for 32MHz calibration
    uint16_t osc_rc32m_comp = 0xB71B;
    DFLLRC32M.COMP1 = (osc_rc32m_comp >> 0)&0xFF;
    DFLLRC32M.COMP2 = (osc_rc32m_comp >> 8)&0xFF;
    OSC.DFLLCTRL = OSC_RC32MCREF_RC32K_gc;
    OSC.CTRL |= OSC_RC32KEN_bm; //enable 32kHz oscillator as reference oscillator
    while(!(OSC.STATUS&OSC_RC32KRDY_bm)){} // wait for 32kHz oscillator to be ready
    DFLLRC32M.CTRL |= DFLL_ENABLE_bm; // enable DFLL

    CLK.USBCTRL = CLK_USBPSDIV_1_gc | CLK_USBSRC_RC32M_gc; //select 32MHz (48Mhz) oscillator as USB clock source

    //configure PLL: 
        //Input = 2MHz osc
        //Multiplier = 10 -> 20MHz
    OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | (10)<<OSC_PLLFAC_gp;
    OSC.CTRL |= OSC_PLLEN_bm; //enable PLL
    while(!(OSC.STATUS&OSC_PLLRDY_bm)){} // wait for PLL to be ready

    ccp_write_io((void*)&CLK.PSCTRL,CLK_PSADIV_1_gc | CLK_PSBCDIV_1_1_gc); //Prescalers: No division for peripherals
    ccp_write_io((void*)&CLK.CTRL,CLK_SCLKSEL_PLL_gc); //Select PLL output as System Clock source (should be 20MHz)
}

int main (void)
{
    clock_init();
	board_init();
	usart_init();
	dma_init(ledbuffer_get_data_pointer());
	
	/*while(1){
		_delay_ms(500);
		LED_RD_ON
		_delay_ms(500);
		LED_RD_OFF
	}*/

    /*define_group(0,0,2,1);
    define_group(1,3,7,2);
    append_to_group(0,4);

    rgbcolor_t clr={0x11,0x11,0x88};
    color_group(0,clr);
    clr.red=0xFF;clr.green=0x0F;clr.blue=0x00;
    color_group(1,clr);*/
	
	rgbcolor_t clr={0xFF,0xFF,0xFF};
	color_led(0,clr);

    fill_buffer();
	
	refresh_leds();

    while(1){
		asm volatile("nop");
	}


	// Insert application code here, after the board has been initialized.
}
