/**
 * \file
 *
 * \brief User board configuration template
 *
 */

#ifndef CONF_BOARD_H
#define CONF_BOARD_H

#define LED_GN				IOPORT_CREATE_PIN(PORTD,1)
#define LED_GN_ON			ioport_set_pin_level(LED_GN,true);
#define LED_GN_OFF			ioport_set_pin_level(LED_GN,false);
#define LED_RD				IOPORT_CREATE_PIN(PORTB,0)
#define LED_RD_ON			ioport_set_pin_level(LED_RD,true);
#define LED_RD_OFF			ioport_set_pin_level(LED_RD,false);

#define TASTER1				IOPORT_CREATE_PIN(PORTE,1)
#define TASTER2				IOPORT_CREATE_PIN(PORTE,2)

#define VBUS				IOPORT_CREATE_PIN(PORTD,5)

#define RGBLED_ENABLE_PIN	IOPORT_CREATE_PIN(PORTC,4)
#define RGBLED_ENABLE_TX	ioport_set_pin_level(RGBLED_ENABLE_PIN,true);
#define RGBLED_DISABLE_TX	ioport_set_pin_level(RGBLED_ENABLE_PIN,false);
#define RGBLED_DATA_PIN		IOPORT_CREATE_PIN(PORTC,7)

#endif // CONF_BOARD_H
