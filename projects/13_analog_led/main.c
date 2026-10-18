#include "../common/avr_io.h"

uint16_t adc_read(uint8_t channel) {
	ADMUX = (ADMUX & 0xF0) | ( channel & 0x0F);
	ADCSRA |= (1 << ADSC);
	while (ADCSRA & (1 << ADSC));
	return ADC;
}

int main(void) {
	// ADC setup
	ADMUX = (1 << REFS0);
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

	// PWM setup (Timer0, PD6/OC0A)
	DDRD |= (1 << 6);
	TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
	TCCR0B = (1 << CS01) | (1 << CS00);

	uart_init();
	uart_print_string("LED Control Via Pot\r\n");

	while(1) {
		uint32_t adc_value = adc_read(0);

		// Debug: print ADC value
		uart_print_string("ADC: ");
		uart_print_number(adc_value);
		uart_print_string(" PWM: ");

		// Map ADC (0-1023) to PWM (0-255)
		uint8_t pwm_value = (adc_value * 255) / 1023;
		if (pwm_value > 254) pwm_value = 254;	// Clamp at 254

		uart_print_number(pwm_value);
		uart_print_string("\r\n");

		OCR0A = pwm_value;
		_delay_ms(500);
	}

	return 0;
}

