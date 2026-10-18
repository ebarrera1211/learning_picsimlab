#include <avr/io.h>
#include <util/delay.h>
#include "../common/avr_io.h"	// Added serial functions and params

uint16_t adc_read(uint8_t channel) {
	// Select ADC channel (0-5 for A0-A5)
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

	// Start conversion
	ADCSRA |= (1 << ADSC);

	// Wait for conversion to complete
	while (ADCSRA & (1 << ADSC));

	// Return ADC value (0-1023)
	return ADC;
}

int main(void) {
	// Initialize ADC
	ADMUX = (1 << REFS0);	// AVcc as reference
	ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);	// Enable ADC, prescaler 128
	
	uart_init();
	_delay_ms(100);		// Wait for uart to init to avoid garbage output
	uart_print_string("ADC Test\r\n");

	while(1) {
		uint16_t adc_value = adc_read(0);	// Read A0
		float voltage = (adc_value * 5.0) / 1024.0;

		uart_print_string("ADC: ");
		uart_print_number(adc_value);
		uart_print_string(" Voltage: ");
		uart_print_number((uint16_t)(voltage * 100) / 100);
		uart_print_string(".");
		uart_print_number((uint16_t)(voltage * 100) % 100);
		uart_print_string("V\r\n");

		_delay_ms(500);
	}

	return 0;
}

