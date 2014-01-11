/**
 * \file
 *
 * \brief User board initialization template
 *
 */

#include <asf.h>
#include <board.h>
#include <conf_board.h>

void board_init(void)
{
	/* This function is meant to contain board-specific initialization code
	 * for, e.g., the I/O pins. The initialization can rely on application-
	 * specific board configuration, found in conf_board.h.
	 */
	ioport_init();
	ioport_set_pin_level(LED_GN,false);
	ioport_set_pin_level(LED_RD,false);
	ioport_set_pin_dir(LED_GN,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(LED_RD,IOPORT_DIR_OUTPUT);
	
	ioport_set_pin_dir(TASTER1,IOPORT_DIR_INPUT);
	ioport_set_pin_dir(TASTER2,IOPORT_DIR_INPUT);
	
	ioport_set_pin_dir(VBUS,IOPORT_DIR_INPUT);
	
	ioport_set_pin_level(RGBLED_ENABLE,false);
	ioport_set_pin_dir(RGBLED_ENABLE,IOPORT_DIR_OUTPUT);
	ioport_set_pin_dir(RGBLED_DATA_PIN,IOPORT_DIR_OUTPUT);
}
