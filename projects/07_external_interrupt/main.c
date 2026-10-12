#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED_PIN 5	// PB5

volatile uint8_t led_state = 0;
volatile uint8_t debounce_counter = 0;

// External Interrupt 0 Service Routine
ISR(INT0_vect) {
	// Disable INT0 immediately to prevent repeat triggers
	EIMSK &= ~(1 << INT0);

	led_state = !led_state;

	if (led_state) {
		PORTB |= (1 << LED_PIN);
	} else {
		PORTB &= ~(1 << LED_PIN);
	}
	// Set debounce counter (50 x 10ms = 0.5s)
	debounce_counter = 50;
}

int main(void) {
	// Set LED pin as output
	DDRB |= (1 << LED_PIN);

	// Set button pin as input with pull-up
	DDRD &= ~(1 << PD2);
	PORTD |= (1 << PD2);

	// Configure INT0 for falling edge (button press)
	EICRA |= (1 << ISC01);	// Falling edge triggers interrupt
	EIMSK |= (1 << INT0);	// Enable INT0
	
	// Enable global interrupts
	sei();

	while(1) {
		// Decrement debounce counter
		if (debounce_counter > 0) {
			debounce_counter--;

			// Re-enable interrupt after debounce period
			if (debounce_counter == 0) {
				EIMSK |= (1 << INT0);
			}
		}
		_delay_ms(10);
	}

	return 0;
}

