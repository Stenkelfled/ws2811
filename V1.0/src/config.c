/*
 * config.c
 *
 * Created: 07.05.2014 17:39:30
 *  Author: stenkelfled
 */ 
#include <asf.h>
#include <config.h>

void init_clock(void){
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

void init_ports(void){
	BUTTON_PORT.DIRCLR = (1<<BUTTON1_PIN) | (1<<BUTTON2_PIN);
	BUTTON_PORT.INTCTRL = PORT_INT0LVL_LO_gc | PORT_INT1LVL_LO_gc;
	BUTTON_PORT.INT0MASK = 1<<BUTTON1_PIN;
	BUTTON_PORT.INT1MASK = 1<<BUTTON2_PIN;
	*(&(BUTTON_PORT.PIN0CTRL)+BUTTON1_PIN) = PORT_ISC_FALLING_gc;
	*(&(BUTTON_PORT.PIN0CTRL)+BUTTON2_PIN) = PORT_ISC_FALLING_gc;
}

void init_interrupts(void){
	PMIC.CTRL = PMIC_LOLVLEN_bm;
}