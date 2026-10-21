#include "../common/avr_io.h"

uint16_t adc_read(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}


int main(void) {
	DDRB |= (1 << 5);	// LED on PB5
	
	// ADC setup
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	uart_init();
	uart_print_string("Light Sensor Control\r\n");

	// Thresholds with hysteresis
	const uint16_t DARK_THRESHOLD = 400;
	const uint16_t LIGHT_THRESHOLD = 600;
	uint8_t led_state = 0;

	while(1) {
		uint16_t light_level = adc_read(0);

		uart_print_string("Light: ");
		uart_print_number(light_level);
		uart_print_string("\r\n");

		// Hysteresis: prevent rapid switching at threshold
		if (led_state == 0 && light_level < DARK_THRESHOLD) {
			led_state = 1;
			PORTB |= (1 << 5);	// Turn on LED
			uart_print_string("Dark detected - LED ON\r\n");
		} else if (led_state == 1 && light_level > LIGHT_THRESHOLD) {
			led_state = 0;
			PORTB &= ~(1 << 5);	// Turn off LED
			uart_print_string("Light detected - LED OFF\r\n");
		}
		_delay_ms(500);
	}

	return 0;
}

