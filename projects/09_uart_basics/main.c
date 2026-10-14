#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

void uart_init(void) {
	// Set baud rate
	UBRR0H = (UBRR_VALUE >> 8);
	UBRR0L = UBRR_VALUE;
	
	// Set frame format: 8 data bits, 1 stop bit, no parity
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);

	// Enable receiver and transmitter
	UCSR0B = (1 << TXEN0) | (1 << RXEN0);

	// Wait for transmit buffer to be empty
	while (!(UCSR0A & (1 << UDRE0)));

	// Clear any spuriuous data
	UDR0 = 0;	// Write null byte

	
	// Wait for the transmission to complete
	while (!(UCSR0A & (1 << TXC0)));	// Wait for Transmit Complete Flag
	UCSR0A |= (1 << TXC0);	// Clear the flag

}

void uart_transmit(char data) {
	// Wait for empty transmit buffer
	while (!(UCSR0A & (1 << UDRE0)));

	// Put data into buffer, sends the data
	UDR0 = data;
}

char uart_receive(void) {
	// Wait for data to be received
	while (!(UCSR0A & (1 << RXC0)));

	// Get and return received data from buffer
	return UDR0;
}

void uart_print_string(const char *str) {
	while (*str) {
		uart_transmit(*str);
		str++;
	}
}

int main(void) {
	uart_init();
    
	_delay_ms(100);
	uart_print_string("Please enter whom to greet:\r\n");
    
	char name[16];  // Buffer to hold the name
	uint8_t index = 0;
    
	while(1) {
		char received = uart_receive();
        
		// Echo the character so user sees what they type
		uart_transmit(received);
        
		// Check for Enter key (carriage return or line feed)
		if (received == '\r' || received == '\n') {
			// Null-terminate the string
			name[index] = '\0';
            
			// Generate and send greeting
			char buffer[32];
			sprintf(buffer, "Hello %s!", name);
			uart_print_string(buffer);
			uart_print_string("\r\n");
            
			// Reset for next input
			index = 0;
			uart_print_string("Please enter whom to greet:\r\n");
		} else if (index < 15) {
			// Add character to name buffer
			name[index++] = received;
		}
		// If index >= 15, ignore characters (buffer full)
	}
    
	return 0;
}
