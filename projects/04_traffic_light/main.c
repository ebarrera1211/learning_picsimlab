#include <avr/io.h>
#include <util/delay.h>
#include "../common/avr_io.h"

#define RED_PIN		0	// PB0
#define YELLOW_PIN	1	// PB1
#define GREEN_PIN	2	// PB2
#define WALK_PIN	3	// PB3

typedef enum {
	STATE_GREEN,
	STATE_YELLOW,
	STATE_RED,
	STATE_WALK
} TrafficState;

uint16_t state_timer = 0;

int main(void) {
	// Set LED pins as output
	DDRB |= (1 << RED_PIN) | (1 << YELLOW_PIN) | (1 << GREEN_PIN) | (1 << WALK_PIN);

	// Set button pin as input with pull-up
	CLEAR_BIT(DDRD, BUTTON_PIN);		// I know it's unclear, just wanted to see inheritance from .h
	SET_BIT(PORTD, BUTTON_PIN);

	TrafficState state = STATE_GREEN;
	uint8_t pedestrian_request = 0;

	// Initial state setup
	PORTB = (1 << GREEN_PIN);
	state_timer = 50;			// 50 x 100ms == 5000ms = 5s

	while(1) {
		// Set LEDs based on current state
		switch (state) {
			case STATE_GREEN:
				PORTB = (1 << GREEN_PIN);
				break;
			case STATE_YELLOW:
				PORTB = (1 << YELLOW_PIN);
				break;
			case STATE_RED:
				PORTB = (1 << RED_PIN);
				break;
			case STATE_WALK:
				PORTB = (1 << WALK_PIN);
				break;
		}

		// Check for pedestrian button press
		if (((PIND >> BUTTON_PIN) & 1) == 0) {
			_delay_ms(50);	// Debounce
			if (((PIND >> BUTTON_PIN) & 1) == 0) {
				pedestrian_request = 1;
			}
		}

		// State machine - check timer
		if (state_timer == 0) {
			switch (state) {
				case STATE_GREEN:
					state = STATE_YELLOW;
					state_timer = 20;	// 20 x 100ms = 2s
					break;

				case STATE_YELLOW:
					state = STATE_RED;
					state_timer = 30;	// 30 x 100ms = 3s
					break;
				case STATE_RED:
					state_timer = 50;	// 50 x 100ms = 5s
					if (pedestrian_request) {
						state = STATE_WALK;
						pedestrian_request = 0;
					} else {
						state = STATE_GREEN;
					}
					break;
				case STATE_WALK:
					state = STATE_GREEN;
					state_timer = 50;	// 50 x 100ms = 5s
					break;
			}
		}
		// Decrement timer
		if (state_timer > 0) {
			state_timer--;
		}

		//
		_delay_ms(100);
	}

	return 0;
}

