#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#define LED1_PIN 5	// PB5/13
#define LED2_PIN 4	// PB4/12
#define BUTTON_PIN 2 	// PD2/2

volatile uint8_t second_flag = 0;
volatile uint16_t seconds = 0;

// Timer1 compare match interrupt
ISR(TIMER1_COMPA_vect) {
	second_flag = 1;
	seconds++;
}


int main(void) {
	// Set LED pin as output
	DDRB |= (1 << LED1_PIN) | (1 << LED2_PIN);

	// Set button pin as input with pull-up
	DDRD &= ~(1 << BUTTON_PIN);
	PORTD |= (1 << BUTTON_PIN);

	// Configure Timer1 for 1-second interrupts
	// CTC mode, prescaler 1024
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	OCR1A = 15624;		// 16MHz/1024/15625 = 1Hz
	TIMSK1 = (1 << OCIE1A);	// Enable compare match interrupt
	
	sei();

	uint8_t led1_state = 0;
	uint8_t led2_state = 0;
	uint16_t timer_seconds = 0; // Separate timer for LED2

	while(1) {
		// Handle button in main loop (polling)
		if (((PIND >> BUTTON_PIN) & 1) == 0) {
			_delay_ms(50);	// Debounce
			if (((PIND >> BUTTON_PIN) & 1) == 0) {
				// Button pressed - reset LED2 timer and turn off LED2
				timer_seconds = 0;
				led2_state = 0;
				PORTB &= ~(1 << LED2_PIN);

				// Toggle LED1
				led1_state = !led1_state;
				PORTB ^= (1 << LED1_PIN);

				while (((PIND >> BUTTON_PIN) & 1) == 0);	// Wait for release
			}
		}
		// Handle timer flag
		if (second_flag) {
			second_flag = 0;
			
			// LED2 logic
			if (timer_seconds < 19) {
				// First 19 seconds: toggle LED2 every second so on 10th ON cycle it stays ON
				led2_state = !led2_state;
				if (led2_state) {
					PORTB |= (1 << LED2_PIN);
				} else {
					PORTB &= ~(1 << LED2_PIN);
				}
				timer_seconds++;
			} else {
				// After 10 ON cycles: keep LED2 ON until button reset
				PORTB |= (1 << LED2_PIN);
			}
		}
	}

	return 0;
}

