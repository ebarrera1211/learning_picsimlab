#include <avr/io.h>
#include <util/delay.h>


#define LED_PIN 6	// PD6 (OC0A - Timer0 PWM output)


int main(void) {
	// Set LED pin as output;
	DDRD |= (1 << LED_PIN);

	// Configure Timer0 for Fast PWM mode, non-inverting
	TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);	// Fast PWM, OC0A non-inverting
	TCCR0B = (1 << CS01) | (1 << CS00);	// Prescaler 64 (Clock = 16Mhz/64 = 250kHz)
	
	uint8_t brightness = 0;
	int8_t direction = 1;

	while(1) {
		// Set PWM duty cycle (0-255)
		OCR0A = brightness;

		// Breathing effect
		brightness += direction;

		if (brightness >= 255) {
			direction = -1;
		} else if (brightness <= 0) {
			direction = 1;
		}
		_delay_ms(10);
	}

	return 0;
}

