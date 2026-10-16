#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
#define BUTTON_PIN 2

volatile uint16_t seconds = 0;

// Timer1 interrupt for timestamp
ISR(TIMER1_COMPA_vect) {
	seconds++;
}

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

void log_event(const char *event) {
	uart_print_string("EVENT,");
	uart_print_number(seconds);
	uart_print_string(",");
	uart_print_string(event);
	uart_print_string("\r\n");
}


int main(void) {
	DDRD &= ~(1 << BUTTON_PIN);
	PORTD |= (1 << BUTTON_PIN);

	// Timer1 for 1-second timestamps
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	OCR1A = 15624;
	TIMSK1 = (1 << OCIE1A);

	uart_init();
	_delay_ms(100);		// wait for uart to initialize to avoid garbage output
	uart_print_string("Logging started...\r\n");
	uart_print_string("Format: EVENT,TIME,TYPE\r\n");

	sei();

	uint8_t last_button = 1;

	while(1) {
		uint8_t current_button = (PIND >> BUTTON_PIN) & 1;

		if (current_button == 0 && last_button == 1) {
			_delay_ms(50);
			if (((PIND >> BUTTON_PIN) & 1) == 0) {
				log_event("BUTTON PRESSED");	// Button press
				while (((PIND >> BUTTON_PIN) & 1) == 0);
				log_event("BUTTON RELEASED");	// Button release
			}
		}
		last_button = current_button;
	}

	return 0;
}

