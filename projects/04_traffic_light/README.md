# Project 4: Traffic Light Simulation

## Objective
Implement a traffic light controller with pedestrian button using state machines, non-blocking timing, and event-driven programming.

## Hardware Required (PicsimLab)
- Arduino Uno board
- 4 LEDs (Spare Parts → Outputs → output_LEDs)
- 1 Push button (Spare Parts → Inputs → Push Buttons)

## Key Concepts
- State machine design and implementation
- Enum for state management
- Non-blocking timing (timer-based transitions)
- Event-driven programming (button requests)
- Separation of output setting from state transition logic
- Fixed-interval loop design (100ms ticks)
- Timer expiration handling
- Use of helper macros from common/avr_io.h

## How to Build
```bash
make
```

## How to Run in PicsimLab

1. Open PicsimLab main board window (Arduino Uno)
2. Open Spare Parts window
3. Add 4 LEDs from Spare Parts → Outputs
4. Right-click LED board → Properties
5. Configure LEDs:
   - LED D1 → PB0/8 (Red)
   - LED D2 → PB1/9 (Yellow)
   - LED D3 → PB2/10 (Green)
   - LED D4 → PB3/11 (Walk)
6. Add push button from Spare Parts → Inputs
7. Right-click button → Properties
8. Configure button to signal: PD2/2
9. Set button to "active-low" mode
10. In main window: File → Load Hex → select main.hex
11. Tools → Arduino Bootloader
12. Click Run button
13. Traffic light cycles: Green (5s) → Yellow (2s) → Red (3s) → Green
14. Press button during Red phase to activate Walk signal (5s)

## Code Explanation

### State Enum Definition
```c
typedef enum {
    STATE_GREEN,
    STATE_YELLOW,
    STATE_RED,
    STATE_WALK
} TrafficState;
```
- Enum defines named states for the traffic light
- Improves code readability compared to magic numbers
- Compiler assigns values (0, 1, 2, 3) automatically

### Non-Blocking Timer
```c
uint16_t state_timer = 0;  // Counts down in 100ms units
```
- Timer value represents remaining time in current state
- Each unit = 100ms (due to _delay_ms(100) in loop)
- When timer reaches 0, state transitions

### Output Setting (First Switch)
```c
switch (state) {
    case STATE_GREEN:
        PORTB = (1 << GREEN_PIN);
        break;
    // ... other states
}
```
- Sets LEDs based on current state
- Runs every loop iteration (every 100ms)
- Ensures LEDs always reflect current state
- Separated from state transition logic for clarity

### State Transition Logic (Second Switch)
```c
if (state_timer == 0) {
    switch (state) {
        case STATE_GREEN:
            state = STATE_YELLOW;
            state_timer = 20;  // 20 x 100ms = 2000ms
            break;
        // ... other transitions
    }
}
```
- Only runs when timer expires
- Determines next state
- Sets timer for new state
- Includes pedestrian request handling

### Button Checking Every Loop
```c
if (((PIND >> BUTTON_PIN) & 1) == 0) {
    _delay_ms(50);
    if (((PIND >> BUTTON_PIN) & 1) == 0) {
        pedestrian_request = 1;
    }
}
```
- Button checked every 100ms (not blocked by long delays)
- Can be pressed at any time during any state
- Debounce delay is short (50ms) relative to loop

### Timer Decrement
```c
if (state_timer > 0) {
    state_timer--;
}
```
- Decrements timer each loop iteration
- Prevents underflow (stops at 0)
- Provides predictable timing

### Fixed Loop Interval
```c
_delay_ms(100);
```
- Ensures consistent loop timing
- 100ms interval = 10 Hz loop rate
- Makes timer values predictable (e.g., 50 = 5 seconds)

### Helper Macros
```c
CLEAR_BIT(DDRD, BUTTON_PIN);
SET_BIT(PORTD, BUTTON_PIN);
```
- Uses macros from common/avr_io.h
- CLEAR_BIT and SET_BIT are convenience macros for bit manipulation
- BUTTON_PIN is defined in common/avr_io.h as PD2

## State Flow

**Normal cycle (no button press)**:
```
GREEN (5s) → YELLOW (2s) → RED (3s) → GREEN (5s) → ...
```

**With pedestrian button press**:
```
GREEN (5s) → YELLOW (2s) → RED (3s) → WALK (5s) → GREEN (5s) → ...
```

Button press during GREEN or YELLOW is remembered and acted upon during RED phase.

## Learnings

- State machines model real-world systems with discrete states
- Enum improves code readability and maintainability
- Non-blocking timing allows multiple concurrent tasks
- Fixed-interval loops provide predictable timing
- Separating output from logic improves code organization
- Event-driven programming handles asynchronous inputs (button)
- Timer-based transitions are more flexible than blocking delays
- Button can be pressed at any time (no blocking delays)
- State transitions can be conditional (based on inputs)
- Pedestrian request flag persists across state changes
- Direct register assignment (PORTB = value) sets multiple LEDs efficiently
- 100ms loop interval provides responsive button checking while keeping timing simple
- Helper macros (CLEAR_BIT, SET_BIT) from common/avr_io.h simplify bit manipulation

## Troubleshooting

### Walk LED always turns on
- Button pin reading 0 constantly (button stuck or not configured)
- Check button is set to active-low in PicsimLab
- Verify button is configured to PD2/2 (not PB2)
- Use Pin Viewer to check PD2/2 state

### Timing too fast/slow
- Adjust state_timer values (each unit = 100ms)
- Verify _delay_ms(100) is actually 100ms
- Check PicsimLab clock frequency is 16MHz

### Wrong LEDs in wrong states
- Verify LED pin assignments match code (RED_PIN=0, YELLOW_PIN=1, etc.)
- Check LEDs are configured to correct signals in PicsimLab
- Ensure both switches are using same pin definitions

### Button not working
- Must be pressed during RED phase to trigger WALK
- Button press during GREEN/YELLOW is queued for next RED
- Verify button is checking in loop (not commented out)
- Check operator precedence (use extra parentheses around bitwise &)

### LEDs not updating
- Ensure output switch runs every loop
- Check that state_timer decrements correctly
- Verify PORTB assignment is in output switch
