#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
#define LED_PIN 5
#define BUFFER_SIZE 32

char rx_buffer[BUFFER_SIZE];
uint8_t rx_index = 0;

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

char uart_receive(void) {
	while (!(UCSR0A & (1 << RXC0)));
	return UDR0;
}

void uart_print_string(const char *str) {
	while (*str) {
		uart_transmit(*str);
		str++;
	}
}

void execute_command(char *cmd) {
	// Convert to uppercase for case-insensitive comparison
	for (char *p = cmd; *p; p++) {
		if (*p >= 'a' && *p <= 'z') {
			*p -= 32;
		}
	}

	if (strcmp(cmd, "ON") == 0) {
		PORTB |= (1 << LED_PIN);
		uart_print_string("LED ON\r\n");
	} else if (strcmp(cmd, "OFF") == 0) {
		PORTB &= ~(1 << LED_PIN);
		uart_print_string("LED OFF\r\n");
	} else if (strcmp(cmd, "BLINK") == 0) {		
		uart_print_string("BLINKING\r\n");
		for (int i = 0; i < 10; i++) {
			PORTB ^= (1 << LED_PIN);
			_delay_ms(500);
		}
		uart_print_string("BLINK DONE\r\n");
	} else if (strcmp(cmd, "STATUS") == 0) {
		if (PORTB & (1 << LED_PIN)) {
			uart_print_string("LED IS ON\r\n");
		} else {
			uart_print_string("LED IS OFF\r\n");
		}
	} else {
		uart_print_string("UNKNOWN COMMAND\r\n");
	}
}

int main(void) {
	DDRB |= (1 << LED_PIN);
	uart_init();
	_delay_ms(100);		// wait for uart to initialize to avoid garbage output
	uart_print_string("Enter a command: ON, OFF, BLINK, STATUS\r\n");

	while(1) {
		char c = uart_receive();

		if (c == '\r' || c == '\n') {
			rx_buffer[rx_index] = '\0';
			if (rx_index > 0) {
				execute_command(rx_buffer);
				rx_index = 0;
			}
		} else if (rx_index < BUFFER_SIZE - 1) {
			rx_buffer[rx_index++] = c;
		}
	}

	return 0;
}

