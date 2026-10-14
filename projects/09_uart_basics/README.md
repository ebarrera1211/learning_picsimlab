# Project 9: UART Basics

## Objective
Learn to configure and use UART (Universal Asynchronous Receiver-Transmitter) for serial communication, implement string formatting with sprintf, and create an interactive greeting program that reads user input and generates custom responses.

## Hardware Required (PicsimLab)
- Arduino Uno board
- Virtual terminal (accessible through PicsimLab terminal window)

## Key Concepts
- UART hardware and registers (UBRR, UCSR, UDR)
- Baud rate calculation and configuration
- UART initialization sequence
- Transmit/receive operations
- Polling-based UART (UDRE0, RXC0 flags)
- String handling in embedded C
- sprintf for string formatting
- Character accumulation and buffering
- Null-terminated strings
- Line endings (\r\n)
- Spurious data clearing on initialization

## How to Build
```bash
make
```

## How to Run in PicsimLab

1. Open PicsimLab main board window (Arduino Uno)
2. In main window: File → Load Hex → select main.hex
3. Tools → Arduino Bootloader
4. Click Run button
5. Open PicsimLab terminal/serial console
6. Set terminal baud rate to 9600
7. Type a name and press Enter
8. See "Hello [name]!" response
9. Repeat for multiple names

## Code Explanation

### Baud Rate Configuration
```c
#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
```
- Sets communication speed to 9600 bits per second
- UBRR_VALUE = (16,000,000 / 16 / 9600) - 1 = 103
- Both ends must use same baud rate for successful communication

### UART Initialization
```c
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
    
    // Clear any spurious data
    UDR0 = 0;  // Write null byte
    
    // Wait for the transmission to complete
    while (!(UCSR0A & (1 << TXC0)));
    UCSR0A |= (1 << TXC0);  // Clear the flag
}
```

**UBRR0H/UBRR0L**: Set baud rate divisor (103 for 9600 baud at 16MHz)
**UCSR0C**: Configures 8N1 frame format (8 data, no parity, 1 stop)
**UCSR0B**: Enables transmitter (TXEN0) and receiver (RXEN0)
**UDR0 = 0**: Clears spurious data in transmit buffer after transmitter enabled
**TXC0 wait**: Ensures the null byte finishes transmitting before sending real data

### UART Transmit Function
```c
void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}
```
- Waits for UDRE0 flag (transmit buffer empty)
- Writes character to UDR0
- UART hardware automatically sends it

### UART Receive Function
```c
char uart_receive(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}
```
- Waits for RXC0 flag (data received)
- Reads and returns character from UDR0
- Blocks until data is available (polling)

### String Print Function
```c
void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}
```
- Sends each character individually
- Uses pointer arithmetic to iterate through string
- Stops at null terminator ('\0')

### Main Function with Input Accumulation
```c
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
    }
}
```

**Input Accumulation**:
- Characters are accumulated in `name` buffer
- Each character is echoed back as typed
- Enter key ('\r' or '\n') triggers greeting generation
- `sprintf` formats "Hello [name]!" string
- Index resets for next input

**Buffer Management**:
- 16-byte buffer for name (15 chars + null terminator)
- Index tracks current position
- Ignores characters if buffer full (index >= 15)

**sprintf Usage**:
- Requires `#include <stdio.h>`
- Formats string into buffer using placeholders
- `%s` for string, `%c` for character, `%d` for number
- Adds code size but simplifies string building

## UART Registers Explained

### UBRR0H/UBRR0L (Baud Rate Registers)
- 12-bit register split into high (4 bits) and low (8 bits)
- Sets divisor for CPU clock to achieve desired baud rate
- UBRR_VALUE = (F_CPU / 16 / BAUD) - 1

### UCSR0A (Control and Status Register A)
- UDRE0: Data Register Empty flag (transmit)
- RXC0: Receive Complete flag
- TXC0: Transmit Complete flag

### UCSR0B (Control and Status Register B)
- TXEN0: Transmitter Enable
- RXEN0: Receiver Enable

### UCSR0C (Control and Status Register C)
- UCSZ01/UCSZ00: Character size bits (8 bits when both set)

### UDR0 (Data Register)
- Write to: puts data in transmit buffer
- Read from: gets received data from receive buffer

## Timing Example

At 9600 baud:
- Each bit = 104 microseconds
- 1 byte (10 bits with start/stop) = 1.04 ms
- Sending "Hello World" (12 chars) = ~12.5 ms

## Line Endings

The code uses `\r\n`:
- `\r` (carriage return) = ASCII 13
- `\n` (line feed) = ASCII 10
- Together = CRLF (Windows-style line ending)
- PicsimLab terminal expects this for proper display

## String Accumulation Pattern

**Example: Typing "Alice"**

1. Type 'A' → Echo 'A' → name[0] = 'A', index = 1
2. Type 'l' → Echo 'l' → name[1] = 'l', index = 2
3. Type 'i' → Echo 'i' → name[2] = 'i', index = 3
4. Type 'c' → Echo 'c' → name[3] = 'c', index = 4
5. Type 'e' → Echo 'e' → name[4] = 'e', index = 5
6. Press Enter → name[5] = '\0' → Output: "Hello Alice!" → Reset

## Learnings

- UART is a hardware peripheral for serial communication
- Baud rate must match on both ends (transmitter and receiver)
- UBRR register controls UART speed via clock divisor
- UCSR registers configure UART settings (enable, frame format)
- UDR0 is the data register (write for TX, read for RX)
- UART uses polling flags (UDRE0, RXC0) to check status
- Polling blocks CPU until operation completes
- sprintf formats strings using placeholders (%s, %c, %d)
- Strings in C are null-terminated (must end with '\0')
- Character accumulation requires buffer management
- Input echoes provide user feedback
- Enter key detection triggers processing
- Spurious data in transmit buffer must be cleared on init
- TXC0 flag ensures transmission completes before proceeding
- Serial communication is asynchronous (no clock line)
- UART frame format: start bit + 8 data bits + stop bit
- This is foundational for embedded system communication

## Troubleshooting

### Garbled characters
- Check baud rate matches on both ends (9600)
- Verify F_CPU is 16MHz in Makefile
- Check PicsimLab clock is 16MHz

### Leading dot character on first transmission
- Caused by spurious data in UDR0 on power-up
- The initialization code clears this by writing null byte and waiting for TXC0
- Ensure uart_init is called before any transmit

### Nothing transmitted
- Verify TXEN0 bit is set in UCSR0B
- Check that UBRR values are correct
- Ensure uart_init is called

### Nothing received
- Verify RXEN0 bit is set in UCSR0B
- Check terminal is connected and baud rate matches
- Ensure uart_receive is being called

### sprintf not working
- Ensure `#include <stdio.h>` is present
- Check buffer is large enough (32 bytes in this code)
- Verify format string syntax (%s for string, %c for char)

### Buffer overflow vulnerability
- Current code limits to 15 characters (index < 15)
- If user types more, characters are ignored
- This prevents buffer overflow but silently truncates input

### Build errors with sprintf
- Ensure stdio library is linked (avr-gcc usually includes it)
- Check for undefined reference errors

### Terminal not showing output
- Verify PicsimLab terminal is open
- Check terminal baud rate is 9600
- Ensure Arduino Bootloader was run
