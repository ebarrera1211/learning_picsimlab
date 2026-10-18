#ifndef AVR_IO_H
#define AVR_IO_H

#include <avr/io.h>
#include <util/delay.h>

// Bit manipulation macros
#define SET_BIT(port, bit)    (port |= (1 << bit))
#define CLEAR_BIT(port, bit)  (port &= ~(1 << bit))
#define TOGGLE_BIT(port, bit) (port ^= (1 << bit))
#define READ_BIT(port, bit)   ((port >> bit) & 1)

// Common pin definitions for Arduino Uno
#define LED_PIN      5  // PB5 (D13)
#define BUTTON_PIN   2  // PD2 (D2)


// Common BAUD Setup
#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

void uart_init(void) {
	UBRR0H = (UBRR_VALUE >> 8);
	UBRR0L = UBRR_VALUE;
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void uart_print_string(const char *str) {
	while (*str) {
		uart_transmit(*str);
		str++;
	}
}

void uart_print_number(uint16_t num) {
	char buffer[6];
	uint8_t i = 0;

	if (num == 0) {
		uart_transmit('0');
		return;
	}

	while (num > 0) {
		buffer[i++] = '0' + (num % 10);
		num /= 10;
	}

	while (i > 0) {
		uart_transmit(buffer[--i]);
	}
}

#endif // AVR_IO_H
