#ifndef DRIVER_H_
#define DRIVER_H_

#include <avr/io.h>

/* Hardware pin definitions for LEDs (PORTB) */
#define LED_GREEN  (1 << PB0)
#define LED_YELLOW (1 << PB1)
#define LED_RED    (1 << PB2)

/* Hardware pin definitions for Buttons (PORTD) */
#define BTN_GREEN  (1 << PD2)
#define BTN_YELLOW (1 << PD3)
#define BTN_RED    (1 << PD4)

/* Function prototypes */
void driver_init(void);
void led_on(uint8_t led_mask);
void led_off(uint8_t led_mask);
void led_toggle(uint8_t led_mask);
uint8_t button_read(uint8_t btn_mask);

#endif