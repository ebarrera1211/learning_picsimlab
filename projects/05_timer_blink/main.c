#include <avr/io.h>
#include <avr/interrupt.h>
#include "../common/avr_io.h"

#define LED_PIN 5 //PB5

volatile uint8_t timer_flag =0;
volatile uint8_t overflow_count = 0;

// Timer0 overflow interrupt service routine
ISR(TIMER0_OVF_vect) {
	overflow_count++;
	if (overflow_count >= 30) {	// 30 overflows = ~500ms
		overflow_count = 0;
		timer_flag = 1;
	}
}

int main(void) {
	// Set LED pin as output
	DDRB |= (1 << LED_PIN);

	// Configure Timer0 for overflow interrupt
	// Normal mode, no prescalar (timer increments every clock cycle)
	TCCR0A = 0;	// Normal mode
	TCCR0B = (1 << CS02) | (1 << CS00);	// Set to 61Hz overflow
	TIMSK0 = (1 << TOIE0);	// Enable Timer0 overflow interrupt
	
	// Enable global interrupts
	sei();

	uint8_t led_state = 0;

	while(1) {
		if (timer_flag) {
			timer_flag = 0;
			led_state = !led_state;

			if (led_state) {
				PORTB |= (1 << LED_PIN);
			} else {
				PORTB &= ~(1 << LED_PIN);
			}
		}
		// Main loop can do other work here
		// Unlike _delay_ms(), this doesn't block
	}

	return 0;
}

