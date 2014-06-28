#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#include "main.h"
#include "usb_cdc.h"


//################################################################################################
// callibrierung adc
//################################################################################################
unsigned char ReadCalibrationByte( unsigned char index ) 
{ 
	unsigned char result; 

	/* Load the NVM Command register to read the calibration row. */ 
	NVM_CMD = NVM_CMD_READ_CALIB_ROW_gc; 
	result = pgm_read_byte(index); 

	/* Clean up NVM Command register. */ 
	NVM_CMD = NVM_CMD_NO_OPERATION_gc; 

	return( result ); 
}

//################################################################################################
// hardware, funktionen und variablen initialisieren
//################################################################################################
void init(void)
{
	CLK.PSCTRL = 0x00;  // no division on peripheral clock

	//################################################################################################
    // Power reduction: Stop unused peripherals
	//################################################################################################
	PR.PRGEN = PR_AES_bm|PR_EBI_bm;												// Stop: AES, EBI
	PR.PRPA  =           PR_AC_bm|PR_DAC_bm;									// Stop: DAC, ADC, AC
	PR.PRPB  =           PR_AC_bm|PR_DAC_bm;									// Stop: DAC, ADC, AC
	PR.PRPC  = PR_TWI_bm|PR_USART1_bm                                   ;		// Stop: TWI, USART0, USART1, SPI, HIRES
	PR.PRPD  = PR_TWI_bm|PR_USART1_bm|             PR_SPI_bm|PR_HIRES_bm;		// Stop: TWI, USART0, USART1, SPI, HIRES
	PR.PRPE  = PR_TWI_bm|PR_USART1_bm|PR_USART0_bm|PR_SPI_bm            ;		// Stop: TWI, USART0, USART1, SPI, HIRES

	PMIC.CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;

	Config32MHzClock();
	
	usb_init();

	sei();
}


int main(void)
{
	unsigned char res;
	//################################################################################################
	// initialisieren aller komponenten und variablen
	//################################################################################################
	init();

	// sign for getting new data
	cdc_rxb.flag=1;
	cdc_rxb.len=0;
	cdc_rxb.bytes=0;

	cdc_txb.flag=0;
	cdc_txb.len=0;
	cdc_txb.bytes=0;

	while(1)
	{

		// manage usb-task for the endpoints (in loop)
		EP_DEF_in(ep_out);
		EP_DEF_out(ep_in);
		EP_DEF_out(ep_note);

		// handle data if flag are cleared
		if(!cdc_rxb.flag)
		{
			if(cdc_rxb.bytes==cdc_rxb.len)
			{
				// all data prcessed; send the data in txd and wait for new data
				// only if out-data are processed
				if(!cdc_txb.flag)
				{
					cdc_txb.len=cdc_txb.bytes;
					cdc_txb.flag=1;
					cdc_rxb.flag=1;
					// blink led for receiving data
					// led_blink_flag|=LED_BLINK_RED;
				}
			}
			else
			{
				// process data
				res=cdc_rxb.data[cdc_rxb.bytes];
				cdc_txb.data[cdc_txb.bytes]=res;
				// process here the data (res)
				// !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
				cdc_rxb.bytes+=1;
				cdc_txb.bytes+=1;
			}
		}
	}
	return 0;
};


void Config32MHzClock(void)
{
	unsigned char tmp;

//	CCP = CCP_IOREG_gc; //Security Signature to modify clock 
	// initialize clock source to be 32MHz internal oscillator (no PLL)
//	OSC.CTRL = OSC_RC32MEN_bm; // enable internal 32MHz oscillator
//	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator ready
//	CCP = CCP_IOREG_gc; //Security Signature to modify clock 
//	CLK.CTRL = 0x01; //select sysclock 32MHz osc


	/////////////////////////////////////////////////////////////////
	// get USBRCOSC
	NVM.CMD  = NVM_CMD_READ_CALIB_ROW_gc;
	tmp = pgm_read_byte(offsetof(NVM_PROD_SIGNATURES_t, USBRCOSC));
	/* Clean up NVM Command register. */ 
	NVM_CMD = NVM_CMD_NO_OPERATION_gc; 
	//
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// enable DFLL for 32MHz osz and trim to 48MHz sync with USB start of frame
	OSC.DFLLCTRL = OSC_RC32MCREF_USBSOF_gc;
	DFLLRC32M.CALB = tmp;
	DFLLRC32M.COMP1 = 0x1B; //Xmega AU manual, 4.17.19
	DFLLRC32M.COMP2 = 0xB7;
	DFLLRC32M.CTRL = DFLL_ENABLE_bm;
	//
	/////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////
	// enable 32 MHZ osz (trimmed to 48MHZ for usb)
	CCP = CCP_IOREG_gc; //Security Signature to modify clock 
	OSC.CTRL = OSC_RC32MEN_bm| OSC_RC2MEN_bm; // enable internal 32MHz oscillator
    
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator ready
	//
	/////////////////////////////////////////////////////////////////
	
	OSC.PLLCTRL = OSC_PLLSRC_RC2M_gc | 16; // 2MHz * 16 = 32MHz
    
	CCP = CCP_IOREG_gc;
	OSC.CTRL = OSC_RC32MEN_bm | OSC_PLLEN_bm | OSC_RC2MEN_bm ; // Enable PLL
    
	while(!(OSC.STATUS & OSC_PLLRDY_bm)); // wait for PLL ready
    
	DFLLRC2M.CTRL = DFLL_ENABLE_bm;

	/////////////////////////////////////////////////////////////////
	// use PLL as systemclk
	CCP = CCP_IOREG_gc; /* allow changing CLK.CTRL */
	CLK.CTRL = CLK_SCLKSEL_PLL_gc; // use PLL output as system clock 
	//
	/////////////////////////////////////////////////////////////////

//	CLK.CTRL = CLK_SCLKSEL_XOSC_gc;                      //Route Osc on Clock-tree
};













#ifdef aaa
#include <LUFA/Platform/XMEGA/ClockManagement.h>

// Start the PLL to multiply the 2MHz RC oscillator to 32MHz and switch the CPU core to run from it
XMEGACLK_StartPLL(CLOCK_SRC_INT_RC2MHZ, 2000000, F_CPU);
XMEGACLK_SetCPUClockSource(CLOCK_SRC_PLL);

// Start the 32MHz internal RC oscillator and start the DFLL 
// to increase it to 48MHz using the USB SOF as a reference
XMEGACLK_StartInternalOscillator(CLOCK_SRC_INT_RC32MHZ);
XMEGACLK_StartDFLL(CLOCK_SRC_INT_RC32MHZ, DFLL_REF_INT_USBSOF, F_USB);

// Enable all interrupt levels
PMIC_CTRL = PMIC_LOLVLEN_bm | PMIC_MEDLVLEN_bm | PMIC_HILVLEN_bm;















void Config32MHzClock(void)
{
	CCP = CCP_IOREG_gc; //Security Signature to modify clock 
	// initialize clock source to be 32MHz internal oscillator (no PLL)
	OSC.CTRL = OSC_RC32MEN_bm; // enable internal 32MHz oscillator
	while(!(OSC.STATUS & OSC_RC32MRDY_bm)); // wait for oscillator ready
	CCP = CCP_IOREG_gc; //Security Signature to modify clock 
	CLK.CTRL = 0x01; //select sysclock 32MHz osc
	
};
#endif
































































































#ifdef aaa



lcd_set_pixel(1,51); 
lcd_set_pixel(2,52); 
lcd_set_pixel(3,53); 
lcd_set_pixel(4,54); 
lcd_set_pixel(5,55); 
lcd_set_pixel(6,56); 
lcd_set_pixel(7,57); 
lcd_set_pixel(8,58); 
lcd_moveto_xy(0, 0);

lcd_line(20, 40, 20, 60); 

























unsigned char ee_mode EEMEM = MODE_MASTER;
unsigned char ee_cli_add_last EEMEM = 2;
unsigned char ee_cli_add EEMEM = 2;
unsigned char ee_cli_baud EEMEM = RS_BAUD_9600;
unsigned char ee_cli_mode EEMEM = RS_EN_RTS|RS_EN_CTS;

unsigned char ee_cli_txt[LCD_SENDBUFFER_LEN] EEMEM = "";

unsigned char cli_mode;
unsigned char cli_baud;
unsigned char cli_add;
unsigned char sel_client;

volatile unsigned char msg_time=0;
volatile unsigned char cli_time=0;
volatile unsigned char cli_time_na=0;
volatile unsigned char cli_res_time=0;
volatile unsigned char cli_pow_time=0;
volatile unsigned char lcd_err_time=0;
volatile unsigned char lcd_shift_time=0;
volatile unsigned char lcd_shift_cnt=0;
volatile unsigned char * lcd_shift_ptr=NULL;
volatile unsigned char lcd_shift_en=0;
volatile unsigned char lcd_timer=0;

char lcd_shift_buffer[LCD_SENDBUFFER_LEN];
char lcd_send_buffer[LCD_SENDBUFFER_LEN] ="";
unsigned char lcd_send_buffer_pos;
unsigned char lcd_send_buffer_cli;


//unsigned char mode=MODE_CLIENT;
unsigned char mode=MODE_MASTER;

/*
** constant definitions
*/

const PROGMEM char txt_Clip[] = "Prog Clientaddress";
const PROGMEM char txt_Clia[] = "Set Clientaddress";
const PROGMEM char txt_FCli[] = "Flush Client Reset";
const PROGMEM char txt_SRes[] = "Set Client Reset";
const PROGMEM char txt_RRes[] = "Reset Client Reset";
const PROGMEM char txt_SPow[] = "Set Client Power";
const PROGMEM char txt_RPow[] = "Reset Client Power";
const PROGMEM char txt_FPow[] = "Flush Client Power";

const PROGMEM char txt_DiWc[] = "Disable Client Write Control";
const PROGMEM char txt_EnWc[] = "Enable Client Write Control";
const PROGMEM char txt_DiC[] = "Disable Client Write/Read Control";
const PROGMEM char txt_EnC[] = "Enable Client Write/Read Control";
const PROGMEM char txt_DiRc[] = "Disable Client Read Control";
const PROGMEM char txt_EnRc[] = "Enable Client Read Control";
const PROGMEM char txt_FRB[] = "Flush Client Readbuffer";
const PROGMEM char txt_FWB[] = "Flush Client Writebuffer";
const PROGMEM char txt_B96[] = "Set Client 9600 Baud";
const PROGMEM char txt_B192[] = "Set Client 19200 Baud";
const PROGMEM char txt_B384[] = "Set Client 38400 Baud";
const PROGMEM char txt_B48[] = "Set Client 4800 Baud";
const PROGMEM char txt_B24[] = "Set Client 2400 Baud";
const PROGMEM char txt_B12[] = "Set Client 1200 Baud";
const PROGMEM char txt_B06[] = "Set Client 600 Baud";
const PROGMEM char txt_B03[] = "Set Client 300 Baud";
const PROGMEM char txt_Lcd[] = "Set Client LCD";

const PROGMEM char txt_version[] = "V 1.01";
const PROGMEM char txt_spc[] = "       ";

#define lcd_gotoxy(a,b)	lcd_moveto_xy(a,b);



//################################################################################################
// werte aus eeprom in variable initialisieren
//################################################################################################
void write_eeprom(unsigned char prog_only)
{
//	eeprom_write_byte(&ee_aprog,menu_aprog);
}

//################################################################################################
// werte aus eeprom in variable initialisieren
//################################################################################################
void init_eeprom(void)
{
	sel_client=(unsigned char)eeprom_read_byte(&ee_cli_add_last);
	cli_add=(unsigned char)eeprom_read_byte(&ee_cli_add);
	cli_baud=(unsigned char)eeprom_read_byte(&ee_cli_baud);
	cli_mode=(unsigned char)eeprom_read_byte(&ee_cli_mode);
	mode=(unsigned char)eeprom_read_byte(&ee_mode);
}

unsigned char lcd_za(unsigned int *za)
{
	unsigned char a;
	a=(*za % 10)+48;
	*za = *za/10;
	return a;
}

void lcd_int(uint16_t z)// Routine zum ausgeben von Zahlen
{ 
	char buffer[7];
	unsigned char i,j;

	buffer[6]= 0;
	buffer[5]= lcd_za(&z);
	buffer[4]= lcd_za(&z);
	buffer[3]= lcd_za(&z);
	buffer[2]= lcd_za(&z);
	buffer[1]= lcd_za(&z);
	buffer[0]= lcd_za(&z);
	i=2; j=0;
	while (buffer[i]!=0)
	{
		if(buffer[i]!='0') 
		{
			j=1;
		}

		if(j==1 || i==5) 
		{
			lcd_putc(buffer[i]);
		}
		else
		{
			lcd_putc(' ');
		}

		i++;
	}
}

// Print Number
void lcd_printN(int data,unsigned char len,unsigned char mark) 
{
	unsigned char d[6]={0x30,0x30,0x30,0x30,0x30,0};
	char *c;
	unsigned char i=0;

	if(data<0) 
	{
		data=-data;
		lcd_putc('-');
	}
	else
	{
//		lcd_putc(' ');
	}

	while (data>=10000)	{ d[0]++; data-=10000; }
	while (data>=1000)	{ d[1]++; data-=1000; }
	while (data>=100)	{ d[2]++; data-=100; }
	while (data>=10)	{ d[3]++; data-=10; }
	while (data>=1)		{ d[4]++; data-=1; }
	if(len>5) len=5;

	c=(char *)d+(5-len);

	while (*c)
	{
		if(i==mark)
		{
//			lcd_inv=1;
		}
		lcd_putc(*c++);
//		lcd_inv=0;
		i++;
	}
}

void lcd_shift(void)
{
	unsigned char *pt;

	if(lcd_shift_en)
	{
		if(lcd_shift_ptr)
		{
			unsigned char i,num=0;

			lcd_gotoxy(0,0);
			pt=(unsigned char *)(lcd_shift_ptr+lcd_shift_cnt);
			if(!(*(unsigned char *)pt))
			{
				pt=(unsigned char *)lcd_shift_ptr;
				lcd_shift_cnt=0;
			}
			else
			{
				lcd_shift_cnt++;
			}

			while(*pt)
			{
				if(num>7)
				{
					break;
				}
				lcd_putc(*pt);
				num++;
				pt++;
			}

			for(i=num;i<8;i++)
			{
				lcd_putc(' ');
			}
		}
	}
}

void lcd_disp_baud(unsigned char baud)
{
	lcd_gotoxy(0,0);
	switch(baud)
	{
		case RS_BAUD_300:
			lcd_putc(' ');
			lcd_printN(300,3,0);
			lcd_putc(' ');
			break;

		case RS_BAUD_600:
			lcd_putc(' ');
			lcd_printN(600,3,0);
			lcd_putc(' ');
			break;

		case RS_BAUD_1200:
			lcd_printN(1200,4,0);
			lcd_putc(' ');
			break;

		case RS_BAUD_2400:
			lcd_printN(2400,4,0);
			lcd_putc(' ');
			break;

		case RS_BAUD_4800:
			lcd_printN(4800,4,0);
			lcd_putc(' ');
			break;

		case RS_BAUD_9600:
			lcd_printN(9600,4,0);
			lcd_putc(' ');
			break;

		case RS_BAUD_19200:
			lcd_printN(19200,5,0);
			break;

		case RS_BAUD_38400:
			lcd_printN(38400,5,0);
			break;

		case RS_BAUD_57600:
			lcd_printN(57600,5,0);
			break;
	}
}


void lcd_shift_worker(void)
{
	if(mode==MODE_MASTER)
	{
		if(!lcd_shift_time)
		{
			lcd_shift_time=LCD_SH_TIME;
			lcd_shift();
		}

		if(!lcd_err_time)
		{
			lcd_gotoxy(6,1);
			lcd_putc(' ');
			lcd_putc(' ');
		}
	}
	else if(mode==MODE_CLIENT)
	{
		if(!lcd_shift_time)
		{
			if(GET(PIN_RES))
			{
				lcd_shift_en=0;
				lcd_timer=LCD_STATE_TIME_SW;
			}

			if(GET(PIN_POW))
			{
				lcd_shift_en=0;
				lcd_timer=LCD_STATE_TIME_SW;
			}

			if(!lcd_shift_en)
			{
				lcd_disp_baud(cli_baud);

				if(GET(PIN_RES))
				{
					MAIN_PUTS_P("Res");
				}
				else if(GET(PIN_POW))
				{
					MAIN_PUTS_P("Pow");
				}
				else
				{
					MAIN_PUTS_P("   ");
				}
			}
			else
			{
				lcd_shift();
			}

			lcd_gotoxy(0,1);
			MAIN_PUTS_P("C=" );
			lcd_printN(SnapNodeAddress,2,0);

			lcd_gotoxy(5,1);

			if(GET(rs232_RTS_PIN))
			{
				lcd_putc('.');
			}
			else if(cli_mode&RS_EN_RTS)
			{
				lcd_putc('R');
			}
			else
			{
				lcd_putc('r');
			}

			if(GET(rs232_CTS_PIN))
			{
				lcd_putc('.');
			}
			else if(cli_mode&RS_EN_CTS)
			{
				lcd_putc('C');
			}
			else
			{
				lcd_putc('c');
			}

			if(cli_time)
			{
				lcd_putc('A');
			}
			else
			{
				lcd_putc('a');
			}

			lcd_shift_time=LCD_SH_TIME;
		}
	}
}

void timer_running(void)
{
	if(lcd_shift_time)
	{
		lcd_shift_time--;
	}

	if(lcd_err_time)
	{
		lcd_err_time--;
	}

	if(cli_time)
	{
		cli_time--;
	}

	if(cli_time_na)
	{
		cli_time_na--;
	}

	if(msg_time)
	{
		msg_time--;
	}

	if(cli_res_time)
	{
		cli_res_time--;
		if(!cli_res_time)
		{
			CLR(PIN_RES);
		}
	}

	if(cli_pow_time)
	{
		cli_pow_time--;
		if(!cli_pow_time)
		{
			CLR(PIN_POW);
		}
	}

	if(TimerRun) TimerRun();
}

void timer_running_1s(void)
{
	unsigned char i;
	unsigned char *eeptr;

	if(lcd_timer==LCD_STATE_TIME_SW)
	{
		// switch to no scrolling
		lcd_shift_en=0;
	}
	else if(lcd_timer==LCD_RAM_TIME_SW)
	{
		// switch to scrolling from ram
		if(*lcd_send_buffer)
		{
			// set to ram
			lcd_shift_en=0;
			for(i=0;i<LCD_SENDBUFFER_LEN;i++)
			{
				lcd_shift_buffer[i]=lcd_send_buffer[i];
			}
			lcd_shift_cnt=0;
			lcd_shift_en=1;
		}
	}

	if(lcd_timer)
	{
		lcd_timer--;
	}
	else
	{
		eeptr=ee_cli_txt;
		// switch to scrolling from eeprom
		if(eeprom_read_byte(eeptr))
		{
			// set to eeprom
			lcd_shift_en=0;
			for(i=0;i<LCD_SENDBUFFER_LEN;i++)
			{
				lcd_shift_buffer[i]=eeprom_read_byte(eeptr++);
			}
			lcd_shift_cnt=0;
			lcd_shift_en=1;
		}

		lcd_timer=LCD_CYCLE_TIME;
	}

	if(TimerRun_1s) TimerRun_1s();
}

void set_lcd_scroll(const char *ptr)
{
	register unsigned char c;
	unsigned char *p;

	lcd_shift_en=0;
	lcd_shift_cnt=0;
	p=(unsigned char *)lcd_shift_buffer;
	lcd_shift_ptr=(unsigned char *)lcd_shift_buffer;
	while ( (c = pgm_read_byte(ptr++)) ) 
	{
		*p++=c;
	}
	*p=0;
	lcd_shift_en=1;
	msg_time=LCD_MSG_TIME;
}

void set_lcd_err(unsigned char err)
{
//return;
	lcd_gotoxy(6,1);
	lcd_err_time=LCD_ERR_TIME;

	switch(err)
	{
		case ERR_TO:
			lcd_puts("TO");
			break;
		case ERR_NACK:
			lcd_puts("NA");
			break;
	}
}


void mode_set_baud(unsigned char baud)
{
	if(baud&RS_BAUD_STORE)
	{
		baud&=~(RS_BAUD_STORE);
		eeprom_write_byte(&ee_cli_baud,baud);
		cli_baud=baud;
	}

	baud&=~(RS_BAUD_STORE);
	switch(baud)
	{
		case RS_BAUD_300:
			cli_baud=RS_BAUD_300;
			rs232_setspeed(300);
			lcd_disp_baud(cli_baud);
			break;

		case RS_BAUD_600:
			cli_baud=RS_BAUD_600;
			rs232_setspeed(600);
			lcd_disp_baud(cli_baud);
			break;

		case RS_BAUD_1200:
			cli_baud=RS_BAUD_1200;
			rs232_setspeed(1200);
			lcd_disp_baud(cli_baud);
			break;

		case RS_BAUD_2400:
			cli_baud=RS_BAUD_2400;
			rs232_setspeed(2400);
			lcd_disp_baud(cli_baud);
			break;

		case RS_BAUD_4800:
			cli_baud=RS_BAUD_4800;
			rs232_setspeed(4800);
			lcd_disp_baud(cli_baud);
			break;

		case RS_BAUD_9600:
			cli_baud=RS_BAUD_9600;
			rs232_setspeed(9600);
			lcd_disp_baud(cli_baud);
			break;

		case RS_BAUD_19200:
			cli_baud=RS_BAUD_19200;
			rs232_setspeed(19200);
			lcd_disp_baud(cli_baud);
			break;

		case RS_BAUD_38400:
			cli_baud=RS_BAUD_38400;
			rs232_setspeed(38400);
			lcd_disp_baud(cli_baud);
			break;

		case RS_BAUD_57600:
			cli_baud=RS_BAUD_57600;
			rs232_setspeed(57600);
			lcd_disp_baud(cli_baud);
			break;
	}
	lcd_shift_en=0;
	lcd_timer=LCD_STATE_TIME;
}

void mode_set_cli(unsigned char m)
{
	if(m&RS_DI_STORE)
	{
		m&=~(RS_DI_STORE);
		eeprom_write_byte(&ee_cli_mode,m);
		cli_mode=m;
	}

	m&=~(RS_DI_STORE);

	if(m&RS_DI_RTS)
	{
		cli();
		//Interrupt Level an PORT festlegen
		PORT_N(rs232_RTS_PIN).INTCTRL &= ~(PORT_INT0LVL_MED_gc);
		rs232_UART.CTRLA |= rs232_IDRE_LV; // USART_DREINTLVL_MED_gc 
		sei();
	}

	if(m&RS_EN_RTS)
	{
		cli();
		//Interrupt Level an PORT festlegen
		PORT_N(rs232_RTS_PIN).INTCTRL |= PORT_INT0LVL_MED_gc;
		sei();
	}

	if(m&RS_DI_CTS)
	{
		cli();
		rs232_cts_on();
		rs232_Rxr.Under_Lowmark=NULL;
		rs232_Rxr.Over_Highmark=NULL;
		sei();
	}

	if(m&RS_EN_CTS)
	{
		cli();
		rs232_Rxr.Under_Lowmark=rs232_cts_on;
		rs232_Rxr.Over_Highmark=rs232_cts_off;
		if(ring_heighmark(&rs232_Rxr))
		{
			rs232_cts_off();
		}
		else
		{
			rs232_cts_on();
		}
		sei();
	}
	lcd_shift_en=0;
	lcd_timer=LCD_STATE_TIME;

}

//################################################################################################
// hardware, funktionen und variablen initialisieren
//################################################################################################
void init(void)
{
	CLK.PSCTRL = 0x00;  // no division on peripheral clock

	//################################################################################################
    // Power reduction: Stop unused peripherals
	//################################################################################################
	PR.PRGEN = PR_AES_bm|PR_EBI_bm;												// Stop: AES, EBI
	PR.PRPA  = PR_ADC_bm|PR_AC_bm|PR_DAC_bm;									// Stop: DAC, ADC, AC
	PR.PRPB  = PR_ADC_bm|PR_AC_bm|PR_DAC_bm;									// Stop: DAC, ADC, AC
	PR.PRPC  = PR_TWI_bm|                          PR_SPI_bm|PR_HIRES_bm;		// Stop: TWI, USART0, USART1, SPI, HIRES
	PR.PRPD  = PR_TWI_bm|             PR_USART0_bm|PR_SPI_bm|PR_HIRES_bm;		// Stop: TWI, USART0, USART1, SPI, HIRES
	PR.PRPE  = PR_TWI_bm|PR_USART1_bm|PR_USART0_bm|PR_SPI_bm|PR_HIRES_bm;		// Stop: TWI, USART0, USART1, SPI, HIRES

	PMIC.CTRL = PMIC_MEDLVLEN_bm|PMIC_HILVLEN_bm; // enable high/med level interrupts

	//################################################################################################
	// read low ADCA calibration byte from NVM signature row into register
	//################################################################################################
	ADCA.CALL = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL0) );

	//################################################################################################
	// read high ADCA calibration byte from NVM signature row into register
	//################################################################################################
	ADCA.CALH = ReadCalibrationByte( offsetof(NVM_PROD_SIGNATURES_t, ADCACAL1) );

//	ADCA.PRESCALER = ADC_PRESCALER_DIV512_gc;									// Prescaler auf 512 (32Mhz/512)
//	ADCA.CTRLB = ADC_FREERUN_bm;												// unsigned mode und 12Bit
//	ADCA.REFCTRL = ADC_REFSEL_INT1V_gc;											// internal 1.0V Reference
//	ADCA.EVCTRL = ADC_SWEEP_01_gc;											// only channel 0 for freerun
//	ADCA.CTRLA = ADC_DMASEL_OFF_gc|ADC_ENABLE_bm;								// aktivieren


	Config32MHzClock();

#ifdef SNAP_TXE_PIN
	// disable TX on rs485
	CLR(SNAP_TXE_PIN);
	// disable tx on rs485
	OUTPUT(SNAP_TXE_PIN);
#endif

#ifdef SNAP_RXE_PIN
	// enable rx on rs485
	CLR(SNAP_RXE_PIN);
	// enable rx on rs485
	OUTPUT(SNAP_RXE_PIN);
#endif

	INPUT(PIN_BR1);
	CTL(PIN_BR1)=PORT_OPC_PULLUP_gc;

	CLR(PIN_POW);
	OUTPUT(PIN_POW);
	CLR(PIN_RES);
	OUTPUT(PIN_RES);

	//################################################################################################
	// initialize memory with eeprom stored values
	//################################################################################################
	EEpromInit();
	init_eeprom();

	//################################################################################################
	// initialize timer
	//################################################################################################
	timer_init();

	TxRing.buf=TxBuf;
	ring_init(&TxRing,TX_BLEN);

	rs232_init();

	TimerRun=TimerFunct_100ms;
	TimerFunct_100ms=timer_running;

	TimerRun_1s=TimerFunct_1s;
	TimerFunct_1s=timer_running_1s;

	//################################################################################################
	// initialize lcd
	//################################################################################################
	lcd_init();

	//################################################################################################
	// enable interrupt
	//################################################################################################
	sei();
}

void mode_master(void)
{
	unsigned char resend=0;
	unsigned char resend_len=0;
	unsigned char result;
	unsigned char cmd;
	unsigned int rcvc;
	unsigned char rcvcb;
	unsigned char cli_addr=0;
	unsigned char cmpb[CMP_BUF_SIZE];
	unsigned char cmpb_size=0;
	unsigned int ii;
	unsigned char j,k,is_send_lcd;
	SnapReq req;
	unsigned char reqdata[34];
	SnapHeaderDef defh;

	SnapDefHeader(&defh);
	req.defheader=&defh;

	// initialize snap
	SnapInit(MASTER_ADDR);
	req.data=reqdata;

	result=SNAP_RESULT_OK;
	lcd_send_buffer_pos=0;
	is_send_lcd=0;

	while(1)
	{
		_delay_ms(4);

		cli();
		ii=ring_pending(&rs232_Rxr);
		sei();

		while(ii)
		{
			ii--;

			if(ring_heighmark(&TxRing))
			{
				// to many data in buffer for snap
				break;
			}

			rcvc=rs232_getc();
			if(rcvc<0x100)
			{
				// data ok, no error
				rcvcb=rcvc&0xFF;

				if(is_send_lcd)
				{
					// we catch the char and stor to lcd_send_buffer
					if(rcvcb=='\n' || rcvcb=='\r')
					{
						lcd_send_buffer[lcd_send_buffer_pos]=0;

						defh.pfb=SNAP_PFB_8;
						defh.cmd=SNAP_CMD_ON;
						defh.ack=SNAP_ACK_NONE;
						if(is_send_lcd==1)
						{
							SnapSend(&req,lcd_send_buffer_cli,COM_LCD, (unsigned char *)&lcd_send_buffer, LCD_SENDBUFFER_LEN);
						}
						else
						{
							SnapSend(&req,lcd_send_buffer_cli,COM_LCD_P, (unsigned char *)&lcd_send_buffer, LCD_SENDBUFFER_LEN);
						}

						lcd_send_buffer_pos=0;
						is_send_lcd=0;
						set_lcd_scroll(txt_Lcd);

						// get the next char
						continue;
					}

					if(lcd_send_buffer_pos<(LCD_SENDBUFFER_LEN-1))
					{
						lcd_send_buffer[lcd_send_buffer_pos]=rcvcb;
						lcd_send_buffer_pos++;
					}

					// get the next char
					continue;
				}

				ring_put(&TxRing,rcvcb);

				// sind daten bereits im compare-buffer oder ist das zeichen ein '>' ?
				if((cmpb_size==0 && rcvcb=='>') || cmpb_size)
				{
					// in den compare-buffer speichern
					cmpb[cmpb_size++]=rcvcb;

					// sind alle daten im buffer?
					if(cmpb_size==CMP_BUF_SIZE)
					{
						// damit es gueltig ist, muss im compare buffer folgendes stehen : > 0 1 c <
						// 01 ist die adresse, c ist der steuercode 
						if(cmpb[0]=='>' && cmpb[4]=='<' && isDigit(cmpb[1]) && isDigit(cmpb[2]))
						{
							cli_addr=(10*(cmpb[1]-'0')) + (cmpb[2]-'0');

							cmd=0;

							switch(cmpb[3])
							{
								case 'o':
									set_lcd_scroll(txt_Clip);
									defh.pfb=SNAP_PFB_8;
									defh.cmd=SNAP_CMD_ON;
									defh.ack=SNAP_ACK_NONE;
									SnapSend(&req,0,COM_PROGC, &cli_addr, 1);
									break;
								case 'a':
									set_lcd_scroll(txt_Clia);
									sel_client=cli_addr;
									eeprom_write_byte(&ee_cli_add_last,cli_addr);
									break;
								case 'l':
									// send next chars as lcd-text
									is_send_lcd=1;
									lcd_send_buffer_pos=0;
									lcd_send_buffer_cli=cli_addr;
									break;
								case 'L':
									// send next chars as lcd-text to store into eeprom
									is_send_lcd=2;
									lcd_send_buffer_pos=0;
									lcd_send_buffer_cli=cli_addr;
									break;
								case 'f':
									set_lcd_scroll(txt_FCli);
									cmd=COM_RES_PULSE;
									break;
								case 'r':
									set_lcd_scroll(txt_SRes);
									cmd=COM_RES_SET;
									break;
								case 'R':
									set_lcd_scroll(txt_RRes);
									cmd=COM_RES_CLR;
									break;
								case 'p':
									set_lcd_scroll(txt_SPow);
									cmd=COM_POW_SET;
									break;
								case 'P':
									set_lcd_scroll(txt_RPow);
									cmd=COM_POW_CLR;
									break;
								case 'Q':
									set_lcd_scroll(txt_FPow);
									cmd=COM_POW_PULSE;
									break;
								case 'w':
									set_lcd_scroll(txt_DiWc);
									cmd=COM_RTS_DI;
									break;
								case 'W':
									set_lcd_scroll(txt_EnWc);
									cmd=COM_RTS_EN;
									break;
								case 'h':
									set_lcd_scroll(txt_DiC);
									cmd=COM_RTS_CTS_DI;
									break;
								case 'H':
									set_lcd_scroll(txt_EnC);
									cmd=COM_RTS_CTS_EN;
									break;
								case 'e':
									set_lcd_scroll(txt_DiRc);
									cmd=COM_CTS_DI;
									break;
								case 'E':
									set_lcd_scroll(txt_EnRc);
									cmd=COM_CTS_EN;
									break;
								case 'c':
									set_lcd_scroll(txt_FRB);
									cmd=COM_RB_CLR;
									break;
								case 'C':
									set_lcd_scroll(txt_FWB);
									cmd=COM_WB_CLR;
									break;
								case 'u':
									set_lcd_scroll(txt_B384);
									cmd=COM_SET_BAUD_38400;
									break;
								case 'U':
									set_lcd_scroll(txt_B03);
									cmd=COM_SET_BAUD_300;
									break;
								case 'x':
									set_lcd_scroll(txt_B96);
									cmd=COM_SET_BAUD_9600;
									break;
								case 'X':
									set_lcd_scroll(txt_B192);
									cmd=COM_SET_BAUD_19200;
									break;
								case 'y':
									set_lcd_scroll(txt_B48);
									cmd=COM_SET_BAUD_4800;
									break;
								case 'Y':
									set_lcd_scroll(txt_B24);
									cmd=COM_SET_BAUD_2400;
									break;
								case 'z':
									set_lcd_scroll(txt_B12);
									cmd=COM_SET_BAUD_1200;
									break;
								case 'Z':
									set_lcd_scroll(txt_B06);
									cmd=COM_SET_BAUD_600;
									break;
							}

							if(cmd)
							{
								defh.pfb=SNAP_PFB_8;
								defh.cmd=SNAP_CMD_ON;
								defh.ack=SNAP_ACK_NONE;
								SnapSend(&req,cli_addr,cmd, reqdata, 0);
							}
						}
						cmpb_size=0;
					}
				}
			}
		}

		lcd_gotoxy(0,1);
		MAIN_PUTS_P("M\006\005" );
		lcd_printN(sel_client,2,0);


		defh.pfb=SNAP_PFB_NONE;
		defh.cmd=SNAP_CMD_NONE;
		if(ring_heighmark(&rs232_Txr))
		{
			// don't request more data
			defh.ack=SNAP_ACK_NONE;
		}
		else
		{
			// we can receive data
			defh.ack=SNAP_ACK_REQ;
		}

		// data to send?
		if(resend && resend_len)
		{
			result=SnapSend(&req,sel_client,0, reqdata, resend_len);
		}
		else if((k=ring_pending(&TxRing)))
		{
			if(k>32)
			{
				k=32;
			}
			else if(k>16)
			{
				k=16;
			}
			else
			{
				k=8;
			}

			for(j=0;j<k;j++)
			{
				rcvc=ring_get(&TxRing);
				if(rcvc>=0x100)
				{
					break;
				}
				rcvcb=rcvc&0xff;
				reqdata[j]=rcvcb;
			}

			result=SnapSend(&req,sel_client,0, reqdata, j);
			resend_len=j;
		}
		else
		{
			defh.pfb=SNAP_PFB_8;
			defh.cmd=SNAP_CMD_ON;
			result=SnapSend(&req,sel_client,COM_GET_DATA, reqdata, 0);
			resend_len=0;
		}

		if(result==SNAP_RESULT_TO)
		{
			set_lcd_err(ERR_TO);
		}

		if(SnapPacketReceived())
		{
			// we have received a packet

			// if a broadcast from other master
			if(((SnapRcvReq.header.hdb1&SNAP_CMD_MASK)==SNAP_CMD_ON) && (SnapRcvReq.pfb) && (SnapRcvReq.dest==0) && (SnapPacketData[0]))
			{
				cli_add=SnapPacketData[0];
				eeprom_write_byte(&ee_cli_add,cli_add);
				SnapNodeAddress=cli_add;
				if(cli_add>1)
				{
					mode=MODE_CLIENT;
					eeprom_write_byte(&ee_mode,mode);
					return;
				}
			}

			for(j=0;j<SnapRcvReq.rndb;j++)
			{
				rs232_putc(SnapPacketData[j]);
			}

			if((SnapRcvReq.header.hdb2&SNAP_ACK_MASK)==SNAP_ACK_NACK)
			{
				set_lcd_err(ERR_NACK);
				resend=1;
			}
			else
			{
				resend=0;
			}

			// Reset the Snap receiver. Once a full packet has been received
			// the Snap library won't begin receiving the next packet
			// until this is called.
			SnapReset();
		}
		lcd_shift_worker();
	}
}

void mode_client(void)
{
	unsigned char last_cli_time=0;
	unsigned char rcvcb;
	unsigned char j;
	unsigned int rcvc;
	unsigned int ii;
	SnapReq req;
	SnapHeaderDef defh;
	unsigned char reqdata[34];
	unsigned char *eeptr;

	SnapDefHeader(&defh);
	req.defheader=&defh;

	// initialize snap
	SnapInit(cli_add);
	req.data=reqdata;

	lcd_clrscr();

	mode_set_baud(cli_baud);
	mode_set_cli(cli_mode);
	lcd_send_buffer[0]=0;
	lcd_shift_en=0;
	lcd_shift_ptr=(unsigned char *)lcd_shift_buffer;

	while(1)
	{
		if(SnapPacketReceived())
		{
			// we have received a packet

			cli_time=COM_TIMEOUT;

			if((SnapRcvReq.header.hdb1&SNAP_CMD_MASK)==SNAP_CMD_ON)
			{
				// this is a command
				switch(SnapRcvReq.pfb)
				{
					case COM_NONE:
					case COM_GET_DATA:
						break;

					case COM_PROGC:
						if((SnapRcvReq.dest==0) && SnapPacketData[0]) // broadcast?
						{
							cli_add=SnapPacketData[0];
							eeprom_write_byte(&ee_cli_add,cli_add);
							SnapNodeAddress=cli_add;
							if(cli_add==1)
							{
								mode=MODE_MASTER;
								eeprom_write_byte(&ee_mode,mode);
								return;
							}
						}
						break;

					case COM_POW_PULSE:
						SET(PIN_POW);
						cli_pow_time=POW_TIME_FLUSH;
						lcd_shift_en=0;
						lcd_timer=LCD_STATE_TIME;
						break;

					case COM_RES_PULSE:
						SET(PIN_RES);
						cli_res_time=RES_TIME_FLUSH;
						lcd_shift_en=0;
						lcd_timer=LCD_STATE_TIME;
						break;

					case COM_RES_SET:
						SET(PIN_RES);
						lcd_shift_en=0;
						lcd_timer=LCD_STATE_TIME;
						break;

					case COM_RES_CLR:
						CLR(PIN_RES);
						lcd_shift_en=0;
						lcd_timer=LCD_STATE_TIME;
						break;

					case COM_POW_SET:
						SET(PIN_POW);
						lcd_shift_en=0;
						lcd_timer=LCD_STATE_TIME;
						break;

					case COM_POW_CLR:
						CLR(PIN_POW);
						lcd_shift_en=0;
						lcd_timer=LCD_STATE_TIME;
						break;

					case COM_RTS_DI:
						mode_set_cli(RS_DI_RTS|RS_DI_STORE);
						break;

					case COM_RTS_EN:
						mode_set_cli(RS_EN_RTS|RS_DI_STORE);
						break;

					case COM_RTS_CTS_DI:
						mode_set_cli(RS_DI_CTS|RS_DI_RTS|RS_DI_STORE);
						break;

					case COM_RTS_CTS_EN:
						mode_set_cli(RS_EN_CTS|RS_EN_RTS|RS_DI_STORE);
						break;

					case COM_CTS_DI:
						mode_set_cli(RS_DI_CTS|RS_DI_STORE);
						break;

					case COM_CTS_EN:
						mode_set_cli(RS_EN_CTS|RS_DI_STORE);
						break;

					case COM_RB_CLR:
						cli();
						ring_flush(&rs232_Rxr);
						sei();
						lcd_shift_en=0;
						lcd_timer=LCD_STATE_TIME;
						break;

					case COM_WB_CLR:
						cli();
						ring_flush(&rs232_Txr);
						sei();
						lcd_shift_en=0;
						lcd_timer=LCD_STATE_TIME;
						break;

					case COM_SET_BAUD_38400:
						mode_set_baud(RS_BAUD_38400|RS_BAUD_STORE);
						break;

					case COM_SET_BAUD_300:
						mode_set_baud(RS_BAUD_300|RS_BAUD_STORE);
						break;

					case COM_SET_BAUD_9600:
						mode_set_baud(RS_BAUD_9600|RS_BAUD_STORE);
						break;

					case COM_SET_BAUD_19200:
						mode_set_baud(RS_BAUD_19200|RS_BAUD_STORE);
						break;

					case COM_SET_BAUD_4800:
						mode_set_baud(RS_BAUD_4800|RS_BAUD_STORE);
						break;

					case COM_SET_BAUD_2400:
						mode_set_baud(RS_BAUD_2400|RS_BAUD_STORE);
						break;

					case COM_SET_BAUD_1200:
						mode_set_baud(RS_BAUD_1200|RS_BAUD_STORE);
						break;

					case COM_SET_BAUD_600:
						mode_set_baud(RS_BAUD_600|RS_BAUD_STORE);
						break;

					case COM_LCD:
						for(j=0;j<LCD_SENDBUFFER_LEN;j++)
						{
							lcd_send_buffer[j]=SnapPacketData[j];
						}
						lcd_shift_cnt=0;
						lcd_shift_en=1;
						lcd_timer=LCD_RAM_TIME_SW;
						break;

					case COM_LCD_P:
						eeptr=ee_cli_txt;
						for(j=0;j<LCD_SENDBUFFER_LEN;j++)
						{
//							lcd_send_buffer[j]=SnapPacketData[j];
							eeprom_write_byte(eeptr++,SnapPacketData[j]);
						}
						lcd_shift_cnt=0;
						lcd_shift_en=1;
						lcd_timer=LCD_CYCLE_TIME;
						break;
				}
			}

			if((SnapRcvReq.header.hdb2&SNAP_ACK_MASK)==SNAP_ACK_REQ)
			{
				defh.pfb=SNAP_PFB_NONE;
				defh.cmd=SNAP_CMD_NONE;
				defh.ack=SNAP_ACK_ACK;

				// data as ack requested
				cli_time_na=COM_TIMEOUT_NA;

				cli();
				ii=ring_heighmark(&rs232_Txr);
				sei();

				if(ii)
				{
					defh.ack=SNAP_ACK_NACK;
				}
				else
				{
					// get the data i have received and send it to rs232
					for(j=0;j<SnapRcvReq.rndb;j++)
					{
						rs232_putc(SnapPacketData[j]);
					}
				}

				// Ack/nack this transfer with new data
				j=0;

				cli();
				ii=ring_pending(&rs232_Rxr);
				sei();

				if(ii)
				{
					if(ii>32)
					{
						ii=32;
					}
					else if(ii>16)
					{
						ii=16;
					}
					else
					{
						ii=8;
					}

					for(j=0;j<ii;j++)
					{
						rcvc=rs232_getc();
						if(rcvc&0x100)
						{
							break;
						}
						rcvcb=rcvc&0xff;
						reqdata[j]=rcvcb;
					}
				}

				rcvcb=SnapSend(&req,SnapRcvReq.src,0, reqdata, j);
			}

			// Reset the Snap receiver. Once a full packet has been received
			// the Snap library won't begin receiving the next packet
			// until this is called.
			SnapReset();
		}

		rcvcb=1;
		if(!cli_time)
		{
			rcvcb=0;
		}

		if(!cli_time_na)
		{
			rcvcb=0;
		}

		if(last_cli_time!=rcvcb)
		{
			if(!rcvcb)
			{
				// this client is not active
				// switch to CTS disable
				mode_set_cli(RS_DI_CTS|RS_DI_RTS);
			}
			else
			{
				// this client is active
				// switch to default mode stored in eeprom
				mode_set_cli(cli_mode);
			}
			last_cli_time=rcvcb;
		}

		lcd_shift_worker();
	}
}

void mode_repeater(void)
{
	unsigned int a;
	unsigned char b=100;
	unsigned char traf=0;

//	lcd_gotoxy(0,1);
//	MAIN_PUTS_P( "do nothing\n" );

	RepInit();

	while(1)
	{
		a=rep_getc();
		if(!(a&0xff00))
		{
			rep1_putc(a);
			traf|=1;
		}

		a=rep1_getc();
		if(!(a&0xff00))
		{
			rep_putc(a);
			traf|=2;
		}

		b--;
		if(!b)
		{
			lcd_gotoxy(0,1);
			if(traf&0x01)
			{
				lcd_putc(0x06);
			}
			else
			{
				lcd_putc(' ');
			}
			traf>>=1;
			if(traf&0x01)
			{
				lcd_putc(0x05);
			}
			else
			{
				lcd_putc(' ');
			}
			b=250;
		}
	}
}

void delay_3sec(void)
{
	unsigned char i;
	//################################################################################################
	// starten etwas verzögern
	//################################################################################################
	for(i=0;i<30;i++)
	{
		_delay_ms(100);
	}
}

#endif






























