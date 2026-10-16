# Project 11: Serial Data Logging

## Objective
Learn to create a data logging system that records button press/release events with timestamps over serial in CSV format, combining timer interrupts for timing and UART for data export.

## Hardware Required (PicsimLab)
- Arduino Uno board
- 1 Push button (Spare Parts → Inputs → Push Buttons)
- Virtual terminal (accessible through PicsimLab terminal window)

## Key Concepts
- Data logging with timestamps
- CSV (Comma-Separated Values) format
- Timer1 CTC mode for 1-second interrupts
- Custom integer-to-string conversion (avoiding printf overhead)
- Event-driven logging
- Edge detection for button press/release
- Blocking wait for button release
- Active-low button with pull-up resistor
- Combining timer interrupts with polling
- Volatile variables for interrupt safety

## How to Build
```bash
make
```

## How to Run in PicsimLab

1. Open PicsimLab main board window (Arduino Uno)
2. Open Spare Parts window
3. Add push button from Spare Parts → Inputs
4. Right-click button → Properties
5. Configure button to signal: PD2/2
6. **Important**: Set button to "active-low" mode (pull-up enabled in code)
7. In main window: File → Load Hex → select main.hex
8. Tools → Arduino Bootloader
9. Click Run button
10. Open PicsimLab terminal/serial console
11. Set terminal baud rate to 9600
12. You will see "Logging started..." and format message
13. Press/release button to see events logged with timestamps

## Code Explanation

### Global Timer Variable
```c
volatile uint16_t seconds = 0;
```
- Tracks elapsed time in seconds
- Incremented by Timer1 interrupt every second
- `volatile` because modified by interrupt
- Used as timestamp for all logged events

### Timer1 Interrupt for Timestamp
```c
ISR(TIMER1_COMPA_vect) {
    seconds++;
}
```
- Timer1 compare match interrupt fires every second
- Increments global `seconds` counter
- Provides time reference for all logged events
- CTC mode with OCR1A = 15624 (1 Hz interrupt at 16MHz with prescaler 1024)

### Timer1 Configuration
```c
TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
OCR1A = 15624;
TIMSK1 = (1 << OCIE1A);
```
- `WGM12`: CTC mode (Clear Timer on Compare)
- `CS12` and `CS10`: Prescaler 1024
- `OCR1A = 15624`: Compare value for 1-second intervals
- `TIMSK1`: Enable Timer1 Compare Match A interrupt

### Custom Number to String Conversion
```c
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
```

**Why Custom Function?**
- Standard `printf` with `%d` adds significant code size
- This custom function is lightweight (~50 bytes vs ~2KB for printf)
- Converts integer to ASCII characters manually

**How It Works**:
1. Extract last digit using modulo (num % 10)
2. Convert to ASCII by adding '0' (ASCII 48)
3. Divide by 10 to remove last digit
4. Repeat until num = 0
5. Digits stored in buffer in reverse order
6. Send in reverse order (buffer[--i]) to get correct order

**Example**: num = 123
- 123 % 10 = 3 → buffer[0] = '3'
- 123 / 10 = 12
- 12 % 10 = 2 → buffer[1] = '2'
- 12 / 10 = 1
- 1 % 10 = 1 → buffer[2] = '1'
- 1 / 10 = 0
- Send in reverse: '1', '2', '3'

### Log Event Function
```c
void log_event(const char *event) {
    uart_print_string("EVENT,");
    uart_print_number(seconds);
    uart_print_string(",");
    uart_print_string(event);
    uart_print_string("\r\n");
}
```

**CSV Format**:
```
EVENT,TIME,TYPE
EVENT,5,BUTTON PRESSED
EVENT,5,BUTTON RELEASED
```

**Fields**:
- `EVENT`: Fixed string identifying data type
- `TIME`: Timestamp in seconds (from `seconds` variable)
- `TYPE`: Event description (string literal)

**Event Descriptions**:
- "BUTTON PRESSED" - Button press event
- "BUTTON RELEASED" - Button release event

### Button Setup with Pull-Up
```c
DDRD &= ~(1 << BUTTON_PIN);
PORTD |= (1 << BUTTON_PIN);
```
- Configure PD2 as input
- Enable internal pull-up resistor
- Idle state: HIGH (1)
- Pressed state: LOW (0)
- Button must be configured as active-low in PicsimLab

### Main Loop with Edge Detection
```c
uint8_t last_button = 1;

while(1) {
    uint8_t current_button = (PIND >> BUTTON_PIN) & 1;

    if (current_button == 0 && last_button == 1) {
        _delay_ms(50);
        if (((PIND >> BUTTON_PIN) & 1) == 0) {
            log_event("BUTTON PRESSED");
            while (((PIND >> BUTTON_PIN) & 1) == 0);
            log_event("BUTTON RELEASED");
        }
    }
    last_button = current_button;
}
```

**Edge Detection**:
- `current_button == 0 && last_button == 1` = falling edge (button press)
- Detects transition from HIGH to LOW
- Only triggers once per press (not while held)

**Debounce**:
- 50ms delay after detecting edge
- Checks button state again
- Ensures it's a real press, not bounce

**Button Press Logging**:
```c
log_event("BUTTON PRESSED");
```

**Wait for Release**:
```c
while (((PIND >> BUTTON_PIN) & 1) == 0);
```
- Blocks until button is released
- Prevents logging multiple events for one press
- Ensures single press → single release pair

**Button Release Logging**:
```c
log_event("BUTTON RELEASED");
```

## Data Flow Example

**Example Session**:

```
Time 0:    "Logging started..."
Time 0:    "Format: EVENT,TIME,TYPE"
Time 0-5:  (user does nothing)
Time 5:    (user presses button)
Time 5:    "EVENT,5,BUTTON PRESSED"
Time 5:    (user holds button)
Time 5:    (user releases button)
Time 5:    "EVENT,5,BUTTON RELEASED"
Time 12:   (user presses button)
Time 12:   "EVENT,12,BUTTON PRESSED"
Time 12:   (user releases button)
Time 12:   "EVENT,12,BUTTON RELEASED"
```

## CSV Data Format

The output is in CSV (Comma-Separated Values) format:

```
EVENT,TIME,TYPE
EVENT,5,BUTTON PRESSED
EVENT,5,BUTTON RELEASED
EVENT,12,BUTTON PRESSED
EVENT,12,BUTTON RELEASED
```

This format can be:
- Imported into spreadsheet software (Excel, Google Sheets)
- Parsed by scripts on host computer (Python, etc.)
- Analyzed for timing patterns
- Used for debugging and monitoring
- Stored in log files for later analysis

## Timer Calculation

```
CPU Clock: 16 MHz
Prescaler: 1024
Timer Clock: 16,000,000 / 1024 = 15,625 Hz
OCR1A: 15,624 (count 0-15624 = 15,625 counts)
Interrupt Frequency: 15,625 / 15,625 = 1 Hz
Result: 1 interrupt per second
```

## Key Concepts

- **Data logging**: Recording events with timestamps for analysis
- **CSV format**: Comma-separated values for easy parsing and interchange
- **Custom number conversion**: Avoiding printf overhead to save code space
- **Event-driven logging**: Recording events as they happen
- **Timestamping**: Using timer interrupts for accurate time reference
- **Edge detection**: Detecting transitions (press vs release)
- **Blocking wait**: Holding until button release for clean event pairs
- **Active-low input**: Pull-up resistor, button pressed = 0
- **Combined peripherals**: Using timer interrupts + UART + GPIO together
- **Interrupt safety**: Volatile variables for interrupt-shared data

## Comparison with Previous Projects

| Aspect | Project 8 | Project 11 |
|--------|-----------|-------------|
| Timer1 | CTC mode for LED timing | CTC mode for timestamps |
| Interrupt usage | LED toggle | Second counter |
| UART | Not used | Data export |
| Data format | None | CSV |
| Button | Resets timer | Logs events |
| Timing | Software timer | Hardware timestamp |

## Use Cases

This pattern is used in:
- Data loggers (temperature, humidity, sensors)
- Event recorders (security systems, access control)
- Debugging systems (error logging)
- Telemetry systems (remote monitoring)
- Industrial monitoring (machine events)
- Diagnostic systems (fault recording)

## Learnings

- Timer interrupts provide accurate timestamps for events
- CSV format is standard for data interchange and analysis
- Custom integer-to-string conversion saves significant code space
- Edge detection differentiates press from release events
- Data logging requires both time and event information
- Blocking waits can synchronize event completion
- Event-driven systems can log data in real-time
- Serial data can be parsed and analyzed by host systems
- Interrupts + polling can work together effectively
- Volatile variables critical for interrupt-shared data
- Active-low input with pull-up is standard for buttons
- This is foundational for embedded data acquisition systems

## Troubleshooting

### No events logged when button pressed
- Verify button is configured as active-low in PicsimLab
- Check pull-up is enabled (PORTD bit set)
- Use Pin Viewer to check PD2/2 state (should be 1 when idle, 0 when pressed)
- Verify edge detection logic (current==0 && last==1)

### Timestamps not incrementing
- Check Timer1 configuration (WGM12, CS12, CS10 bits)
- Verify OCR1A = 15624
- Ensure TIMSK1 has OCIE1A bit set
- Check sei() was called (global interrupts enabled)
- Verify Timer1 is actually running

### uart_print_number outputs nothing
- Test with known value: uart_print_number(123)
- Check buffer array size (6 bytes enough for 0-65535)
- Verify modulo and division operations working
- Check that while loop executes

### CSV format incorrect
- Verify commas are in correct positions
- Check \r\n line endings
- Ensure uart_print_string works correctly
- Test with fixed strings first

### Button events log multiple times
- Edge detection not working correctly
- Check last_button state updates
- Verify debounce delay is present
- Ensure blocking wait for release works

### Build errors
- Ensure all headers included (avr/io.h, util/delay.h, avr/interrupt.h)
- Check Makefile has correct MCU flag (-mmcu=atmega328p)
- Verify F_CPU is defined (16000000UL)

### Terminal shows garbage
- Verify baud rate is 9600 on both ends
- Check F_CPU is 16MHz in Makefile
- Ensure uart_init delay is present (100ms)
- Confirm Arduino Bootloader was run
