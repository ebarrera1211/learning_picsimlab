#include "../common/avr_io.h"
#include <util/twi.h>

void i2c_init(void) {
	TWSR = 0;	// No prescaler
	TWBR = 72;	// 100kHz @ 16MHz
}

uint8_t i2c_start(uint8_t address) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));

	if ((TWSR & 0xF8) != 0x08) return 1;	// Error
	
	TWDR = address;
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT)));

	return (TWSR & 0xF8) != 0x18;
}

void i2c_stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (TWCR & (1 << TWSTO));
}

void i2c_scan(void) {
	uart_print_string("Scanning I2C bus...\r\n");

	for (uint8_t addr = 1; addr < 128; addr <<= 1) {
		if (!i2c_start(addr)) {
			uart_print_string("Found device at: 0x");
			uart_print_hex(addr);
			uart_print_string("\r\n");
		}
		i2c_stop();
	}

	uart_print_string("Scan complete.\r\n");
}

int main(void) {
	uart_init();
	i2c_init();

	while(1) {
		i2c_scan();
		_delay_ms(5000);
	}

	return 0;
}

