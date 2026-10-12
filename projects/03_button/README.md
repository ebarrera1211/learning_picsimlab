# Project 3: Read Push Button

## Objective
Learn to read digital input from a push button, implement software debouncing, and control an LED based on button state.

## Hardware Required (PicsimLab)
- Arduino Uno board
- 1 LED (Spare Parts → Outputs → output_LEDs)
- 1 Push button (Spare Parts → Inputs → Push Buttons)

## Key Concepts
- PIN register for reading input
- DDR register configuration for input
- Pull-up resistors (PORT bit set when DDR is input)
- Button press detection (active-low with pull-up)
- Software debouncing
- State management (last_button_state, led_state)
- Operator precedence in C (bitwise & vs comparison ==)
- Edge detection (falling edge)

## How to Build
```bash
make
```

## How to Run in PicsimLab

1. Open PicsimLab main board window (Arduino Uno)
2. Open Spare Parts window
3. Add LED from Spare Parts → Outputs
4. Right-click LED board → Properties
5. Configure LED (e.g., D1) to signal: PB5/13
6. Add push button from Spare Parts → Inputs
7. Right-click button → Properties
8. Configure button to signal: PD2/2
9. **Important**: Set button to "active-low" mode (not active-high)
10. In main window: File → Load Hex → select main.hex
11. Tools → Arduino Bootloader
12. Click Run button
13. Press button to toggle LED on/off

## Code Explanation

### Setting Button as Input with Pull-up
```c
DDRD &= ~(1 << BUTTON_PIN);  // Set PD2 as input
PORTD |= (1 << BUTTON_PIN);  // Enable pull-up resistor
```
- Clearing DDR bit makes pin input
- Setting PORT bit on input pin enables internal pull-up
- Pull-up keeps pin HIGH (1) when button not pressed
- Button connects pin to ground when pressed, making it LOW (0)

### Reading Button State
```c
uint8_t current_button_state = (PIND >> BUTTON_PIN) & 1;
```
- `PIND` is the input register for Port D
- Shift right by BUTTON_PIN (2) to isolate bit 2
- `& 1` masks to get just that bit
- Result is 0 or 1

### Button Press Detection (Edge Detection)
```c
if (current_button_state == 0 && last_button_state == 1) {
    // Button was just pressed (falling edge)
}
```
- Detects transition from HIGH to LOW
- Edge detection prevents triggering on held button
- Only triggers once per press

### Debouncing
```c
_delay_ms(DEBOUNCE_DELAY);  // Wait 50ms
if (((PIND >> BUTTON_PIN) & 1) == 0) {
    // Still pressed after debounce
}
```
- Mechanical buttons bounce (multiple on/off transitions)
- Short delay filters out bounce
- Checks button state again after delay
- In PicsimLab, this is less critical but important for real hardware

### Critical Operator Precedence
```c
// WRONG: if ((PIND >> BUTTON_PIN) & 1 == 0)
// == has higher precedence than &, so this becomes:
// if ((PIND >> BUTTON_PIN) & (1 == 0))
// Which is always false because 1 == 0 is 0

// CORRECT: if (((PIND >> BUTTON_PIN) & 1) == 0)
// Parentheses ensure & happens before ==
```

### LED State Toggle
```c
led_state = !led_state;  // Toggle between 0 and 1
```
- Toggles LED state on each button press
- `!` is logical NOT operator

### State Tracking
```c
last_button_state = current_button_state;
_delay_ms(10);  // Small delay to prevent rapid checking
```
- Tracks previous button state for edge detection
- 10ms delay prevents excessive CPU usage
- In PicsimLab, may not be necessary but good practice

## Learnings

- PIN register reads input states (different from PORT for output)
- Setting PORT bit on input pin enables internal pull-up resistor
- Active-low: button pressed = 0, not pressed = 1 (with pull-up)
- Pull-up resistors prevent floating pins (random noise)
- Edge detection (high→low transition) prevents repeated triggers
- Software debouncing handles mechanical button bounce
- C operator precedence: `==` has higher precedence than `&`
- Always use parentheses when mixing bitwise and comparison operators
- State management tracks previous values to detect changes
- PicsimLab buttons can be configured as active-low or active-high
- In real hardware, pull-up is essential; PicsimLab can simulate either
- Uses common/avr_io.h helper header for BUTTON_PIN definition

## Troubleshooting

### LED toggles without button press
- Check button is configured to PD2/2 (not PD2 with wrong number)
- Verify button is set to active-low mode in PicsimLab
- Check operator precedence in button reading (use extra parentheses)

### Button press not detected
- Verify button is added and configured in PicsimLab
- Check Pin Viewer for PD2/2 - should be 1 when idle, 0 when pressed
- Ensure pull-up is enabled (PORTD bit set)
- Make sure Tools → Arduino Bootloader was run

### LED toggles multiple times per press
- Increase DEBOUNCE_DELAY value
- In PicsimLab, this might not happen (no mechanical bounce)
- In real hardware, this is a real issue

### Wrong pin reading
- PD2 = Port D, bit 2 = Arduino D2
- PB2 = Port B, bit 2 = Arduino D10
- These are different pins!
