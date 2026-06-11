#include "driver.h"

/* * Configures data direction registers (DDRx) and initial pin states (PORTx).
 * LEDs are set as outputs and initially turned off.
 * Buttons are set as inputs with internal pull-up resistors enabled.
 */
void driver_init(void) {
    DDRB |= (LED_GREEN | LED_YELLOW | LED_RED);      
    PORTB &= ~(LED_GREEN | LED_YELLOW | LED_RED);  
    
    DDRD &= ~(BTN_GREEN | BTN_YELLOW | BTN_RED);   
    PORTD |= (BTN_GREEN | BTN_YELLOW | BTN_RED);     
}

/* Sets the specified LED pin HIGH to turn it on. */
void led_on(uint8_t led_mask) {
    PORTB |= led_mask;
}

/* Sets the specified LED pin LOW to turn it off. */
void led_off(uint8_t led_mask) {
    PORTB &= ~led_mask;
}

/* Toggles the state of the specified LED pin. */
void led_toggle(uint8_t led_mask) {
    PORTB ^= led_mask;
}

/* * Reads the button state. 
 * Since pull-up resistors are used, a pressed button reads as 0 (LOW).
 * Returns 1 if pressed, 0 otherwise.
 */
uint8_t button_read(uint8_t btn_mask) {
    return !(PIND & btn_mask);
}