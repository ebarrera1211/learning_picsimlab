#include <avr/io.h>
#include <util/delay.h>
#include "../common/avr_io.h"

#define LED_PIN 5 // PB5
#define BUTTON_PIN 2 // PD2

// Simple debounce delay (not production-quality, but good for learning)
#define DEBOUNCE_DELAY 50


int main(void) {
	// Set LED pin as output
	DDRB |= (1 << LED_PIN);
	
	// Set button pin as input with pull-up resistor
	DDRD &= ~(1 << BUTTON_PIN);
	PORTD |= (1 << BUTTON_PIN);	// Enable pull-up
	
	uint8_t last_button_state = 1;	// Pull-up means idle = 1
	uint8_t led_state = 0;

	while(1) {
		uint8_t current_button_state = (PIND >> BUTTON_PIN) & 1;
		
		// Check for button press (low = pressed with pull-up)
		if (current_button_state == 0 && last_button_state == 1) {
			_delay_ms(DEBOUNCE_DELAY);	// Debounce delay
		
			// Check again after debounce
			if (((PIND >> BUTTON_PIN) & 1) == 0) {
				led_state = !led_state;	// Toggle LED state
		
				if (led_state) {
					PORTB |= (1 << LED_PIN);	// LED on
				} else {
					PORTB &= ~(1 << LED_PIN);	// LED off
				}
			}
		}
	
		last_button_state = current_button_state;
		_delay_ms(10);	// Small delay to prevent rapid checking
	}

	return 0;
}

