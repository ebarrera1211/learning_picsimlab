# Project 10: Serial Commands

## Objective
Learn to parse serial commands using a command interpreter pattern, implement case-insensitive command parsing, control an LED via serial commands, and use XOR toggle pattern for LED blinking that returns to original state.

## Hardware Required (PicsimLab)
- Arduino Uno board
- 1 LED (Spare Parts → Outputs → output_LEDs)
- Virtual terminal (accessible through PicsimLab terminal window)

## Key Concepts
- String parsing and comparison
- Command interpreter pattern
- Case-insensitive command parsing
- String handling in embedded C
- strcmp() function from string.h
- XOR toggle operation (port ^= (1 << bit))
- Buffer management and overflow protection
- Command acknowledgment messages
- Status checking
- Null-terminated strings
- Line ending detection

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
7. Type commands and press Enter:
   - "ON" - turns LED on
   - "OFF" - turns LED off
   - "BLINK" - blinks LED 10 times and returns to original state
   - "STATUS" - reports current LED state
   - Any other text - "UNKNOWN COMMAND"

## Code Explanation

### String Handling Header
```c
#include <string.h>
```
- Provides string manipulation functions
- Specifically `strcmp()` for string comparison
- Standard C library (included in avr-libc)

### Buffer and Index
```c
#define BUFFER_SIZE 32

char rx_buffer[BUFFER_SIZE];
uint8_t rx_index = 0;
```
- `rx_buffer`: Array to store received characters
- `rx_index`: Tracks current position in buffer
- 32-byte buffer enough for commands like "ON", "OFF", "BLINK", "STATUS"

### UART Initialization
```c
void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}
```
- Sets baud rate to 9600
- Enables transmitter and receiver
- Configures 8N1 frame format
- Note: No spurious data clearing (simpler than Project 9)

### Command Execution Function
```c
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
```

**Case-Insensitive Conversion**:
- Converts lowercase to uppercase by subtracting 32
- ASCII: 'a' = 97, 'A' = 65 (difference of 32)
- Makes "on", "ON", "On" all work the same
- Modifies the string in-place

**strcmp Function**:
- Compares two strings
- Returns 0 if strings are equal
- Returns non-zero if different

**Command Handlers**:
- **ON**: Sets LED bit high (turns on)
- **OFF**: Clears LED bit (turns off)
- **BLINK**: Toggles LED 10 times using XOR, returns to original state
- **STATUS**: Reports current LED state
- **UNKNOWN**: For unrecognized commands

**XOR Toggle Pattern**:
```c
PORTB ^= (1 << LED_PIN);
```
- XOR flips the bit (0→1 or 1→0)
- 10 toggles = even number = returns to original state
- No need to save/restore original state
- Equivalent to TOGGLE_BIT macro from helper header

### Main Loop with Command Parsing
```c
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
```

**Character-by-Character Accumulation**:
- Receives one character at a time
- Adds to buffer until Enter pressed
- Echoes character back to terminal (implied by receive/transmit cycle)

**Line End Detection**:
- Detects Enter key ('\r' or '\n')
- Null-terminates the string
- Only executes if buffer has content
- Resets index for next command

**Buffer Overflow Protection**:
- Stops adding characters if buffer nearly full
- BUFFER_SIZE - 1 leaves room for null terminator
- Extra characters are silently ignored

## Command Processing Flow

**Example: Typing "BLINK" + Enter**

1. Receive 'B' → Add to buffer → Echo
2. Receive 'L' → Add to buffer → Echo
3. Receive 'I' → Add to buffer → Echo
4. Receive 'N' → Add to buffer → Echo
5. Receive 'K' → Add to buffer → Echo
6. Receive '\r' → Null-terminate → Execute BLINK command → Send "BLINKING" → Toggle 10 times → Send "BLINK DONE" → Reset

## XOR Toggle vs State Saving

**XOR Toggle Approach (Used Here)**:
```c
for (int i = 0; i < 10; i++) {
    PORTB ^= (1 << LED_PIN);
    _delay_ms(500);
}
```
- Toggles bit using XOR
- 10 toggles = even number = returns to original state
- No variable needed to save state
- Simpler code

**State Saving Approach**:
```c
uint8_t original_state = PORTB & (1 << LED_PIN);
// ... blink ...
if (original_state) {
    PORTB |= (1 << LED_PIN);
} else {
    PORTB &= ~(1 << LED_PIN);
}
```
- Requires variable to save state
- More code
- Needed if blink count is odd

## Available Commands

| Command | Action | Result |
|---------|--------|--------|
| ON | Set LED bit high | LED turns on |
| OFF | Clear LED bit | LED turns off |
| BLINK | Toggle 10 times | LED blinks, returns to original state |
| STATUS | Check LED bit | Reports "LED IS ON" or "LED IS OFF" |
| Anything else | N/A | "UNKNOWN COMMAND" |

## Command Interpreter Pattern

This is a classic command interpreter pattern:

```
Input → Buffer → [Accumulate] → [Enter] → Parse → Execute → Response
```

**Benefits**:
- User can edit command before sending (backspace, etc.)
- Natural for interactive terminals
- Easy to extend with new commands
- Standard in command-line interfaces

## Extending the System

To add a new command, add another `else if`:

```c
else if (strcmp(cmd, "TOGGLE") == 0) {
    PORTB ^= (1 << LED_PIN);
    uart_print_string("LED TOGGLED\r\n");
}
```

## Key Learnings

- `strcmp()` for string comparison from string.h
- String handling functions in embedded C
- Case-insensitive command parsing
- Command interpreter pattern
- Buffer management and overflow protection
- Null-terminator importance in C strings
- Line ending detection (\r and \n)
- Separation of parsing and execution
- Extensible command structure
- XOR toggle operation (port ^= (1 << bit))
- Even/odd toggle count determines final state
- Interactive command-line interfaces
- Acknowledgment messages for user feedback
- LED state checking with bitwise AND

## Troubleshooting

### Commands not recognized
- Check case sensitivity (code converts to uppercase, so should work)
- Verify null-termination of buffer
- Check for extra spaces in command

### BLINK doesn't return to original state
- Verify XOR operation is used (not ON→OFF pattern)
- Check loop count is even (10 in this code)
- Ensure no state saving/restoring that might interfere

### STATUS always reports wrong state
- Verify bit masking: PORTB & (1 << LED_PIN)
- Check LED_PIN definition (should be 5 for PB5)

### Buffer overflow
- Current code ignores extra characters (BUFFER_SIZE - 1)
- If command longer than 31 chars, gets truncated
- Increase BUFFER_SIZE if needed

### No echo of typed characters
- The receive/transmit cycle naturally echoes in PicsimLab
- If you need explicit echo, add uart_transmit(c) after uart_receive()

### Build errors with strcmp
- Ensure `#include <string.h>` is present
- Check that string.h is available in avr-libc

### Nothing transmitted
- Verify TXEN0 bit is set in UCSR0B
- Check UBRR values are correct
- Ensure uart_init is called

### Nothing received
- Verify RXEN0 bit is set in UCSR0B
- Check terminal is connected and baud rate matches
