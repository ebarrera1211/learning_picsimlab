#include "../common/avr_io.h"

uint16_t adc_read(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

int main(void) {
	// LED pins
	DDRD |= (1 << 6) | (1 << 5) | (1 << 3);

	//ADC setup
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	uart_init();
	uart_print_string("Temperature Monitor\r\n");

	while(1) {
		uint32_t adc_value = adc_read(0);
		uart_print_string("ADC: ");
		uart_print_number(adc_value);
		uart_print_string(" ");

		// LM35: 10mv per degree Celsius
		// ADC: 0-1023 = 0-5V, so 1 ADC count = 4.88mV
		// Temperature = (ADC * 4.88) / 10 = ADC * 0.488
		int16_t temp_c = (adc_value * 488) / 1000;

		uart_print_string("Temperature: ");
		uart_print_number(temp_c);
		uart_print_string("C\r\n");

		// Threshold indicators
		if (temp_c >= 90) {
			PORTD |= (1 << 6);	// Red LED (hot)
			PORTD &= ~((1 << 5) | (1 << 3));
		} else if (temp_c >= 75 & temp_c < 90) {
			PORTD |= (1 << 3);	// Yellow LED (warn)
			PORTD &= ~((1 << 5) | (1 << 6));
		} else if (temp_c <= 20) {
			PORTD |= (1 << 5);	// Blue LED (cold)
			PORTD &= ~((1 << 6) | (1 << 3));
		} else {
			PORTD &= ~((1 << 6) | (1 << 5) | (1 << 3));	// All off (normal)
		}
		_delay_ms(1000);
	}

	return 0;
}

