#include <avr/io.h>
#include <util/delay.h>
#include "../common/avr_io.h"

#define LED0 0 //PB0
#define LED1 1 //PB1
#define LED2 2 //PB2
#define LED3 3 //PB3

int main(void) {
	// Set all LED pins as output
	DDRB |= (1 << LED0) | (1 << LED1) | (1 << LED2) | (1 << LED3);

	while(1) {
		// Pattern 1: Running light (LED0 -> LED1 -> LED2 -> LED3)
		PORTB = (1 << LED0);
		_delay_ms(200);
		PORTB = (1 << LED1);
		_delay_ms(200);
		PORTB = (1 << LED2);
		_delay_ms(200);
		PORTB = (1 << LED3);
		_delay_ms(200);

		// Pattern 2: Binary counter (0-15)
		for (uint8_t i = 0; i < 16; i++) {
			PORTB = i;
			_delay_ms(500);
		}
	}

	return 0;
}

