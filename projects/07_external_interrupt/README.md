# Project 7: External Interrupts

## Objective
Learn to use external interrupts for event-driven programming, implement button-triggered LED toggle, and handle button debouncing in interrupt context.

## Hardware Required (PicsimLab)
- Arduino Uno board
- 1 LED (Spare Parts → Outputs → output_LEDs)
- 1 Push button (Spare Parts → Inputs → Push Buttons)

## Key Concepts
- External interrupt vectors (INT0, INT1)
- Interrupt Service Routines (ISR)
- EICRA (External Interrupt Control Register A)
- EIMSK (External Interrupt Mask Register)
- Falling edge vs rising edge triggering
- Interrupt priority and nesting
- Volatile variables for interrupt safety
- Debouncing in interrupt context with disable/enable pattern
- Event-driven vs polling programming

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
8. Configure button to signal: PD2/2 (INT0 pin)
9. Set button to "active-low" mode
10. In main window: File → Load Hex → select main.hex
11. Tools → Arduino Bootloader
12. Click Run button
13. Press button to toggle LED on/off (immediate response)

## Code Explanation

### Interrupt-Driven Variable
```c
volatile uint8_t led_state = 0;
volatile uint8_t debounce_counter = 0;
```
- `volatile` is critical for variables modified by interrupts
- Prevents compiler from caching or optimizing away reads
- `led_state` tracks LED on/off
- `debounce_counter` prevents repeat interrupts during bounce

### Interrupt Service Routine (ISR)
```c
ISR(INT0_vect) {
    // Disable INT0 immediately to prevent repeat triggers
    EIMSK &= ~(1 << INT0);
    
    led_state = !led_state;
    
    if (led_state) {
        PORTB |= (1 << LED_PIN);
    } else {
        PORTB &= ~(1 << LED_PIN);
    }
    // Set debounce counter (50 x 10ms = 0.5s)
    debounce_counter = 50;
}
```
- `ISR(INT0_vect)` declares handler for INT0 interrupt
- INT0 is triggered by PD2 pin changes
- Immediately disables INT0 to prevent bounce from queuing more interrupts
- Toggles LED state
- Sets debounce counter for main loop to re-enable interrupt later
- Uses 500ms debounce period (50 × 10ms)

### External Interrupt Configuration
```c
EICRA |= (1 << ISC01);  // Falling edge triggers interrupt
EIMSK |= (1 << INT0);   // Enable INT0
```

**EICRA (External Interrupt Control Register A)**:
- Controls trigger condition for INT0 and INT1
- ISC01=1, ISC00=0 = falling edge trigger (HIGH → LOW)
- ISC01=0, ISC00=1 = rising edge trigger (LOW → HIGH)
- ISC01=1, ISC00=1 = any logical change
- ISC01=0, ISC00=0 = low level trigger

**EIMSK (External Interrupt Mask Register)**:
- INT0 bit enables INT0 interrupt
- INT1 bit enables INT1 interrupt
- Must be set for interrupt to fire

### Main Loop Debounce Handling
```c
while (1) {
    // Decrement debounce counter
    if (debounce_counter > 0) {
        debounce_counter--;
        
        // Re-enable interrupt after debounce period
        if (debounce_counter == 0) {
            EIMSK |= (1 << INT0);
        }
    }
    
    _delay_ms(10);
}
```
- Main loop handles debounce timing (non-blocking in ISR)
- Decrements counter every 10ms
- After 500ms (50 × 10ms), re-enables INT0
- By then, button bounce has settled
- Main loop can do other work (currently empty)

### Why Disable Interrupt in ISR?

**Problem without disabling**:
1. Button press → falling edge → ISR fires
2. During ISR, button bounces → multiple falling edges
3. These edges set interrupt flag
4. When ISR returns, all pending interrupts fire immediately
5. Result: multiple toggles from one button press

**Solution**:
- Disable interrupt immediately when ISR fires
- Prevents bounce from queuing more interrupts
- Re-enable after debounce period when button is stable

## Event-Driven vs Polling

### Polling (Project 3)
```c
while (1) {
    if (((PIND >> BUTTON_PIN) & 1) == 0) {
        // Handle button press
    }
    _delay_ms(10);
}
```
- Main loop must continuously check button
- If loop is busy, might miss button presses
- CPU wasted checking when nothing happens

### Interrupt-Driven (Project 7)
```c
ISR(INT0_vect) {
    // Handle button press automatically
}

while (1) {
    // Can do anything here, button still works
}
```
- Interrupt fires instantly when button pressed
- Main loop can do other work
- CPU efficient (only reacts when button pressed)
- Immediate response regardless of main loop state

## Interrupt Priority and Safety

- Interrupts can interrupt each other
- When an ISR runs, global interrupts are disabled by default
- ISRs should be short and fast
- Avoid long delays or complex logic in ISRs
- Use volatile for shared data
- Be careful with shared data access

## Learnings

- External interrupts provide event-driven programming
- INT0/INT1 correspond to PD2/PD3 pins on Arduino Uno
- EICRA configures trigger condition (falling/rising edge)
- EIMSK enables/disables specific interrupts
- Interrupts fire immediately when trigger condition met
- `volatile` is critical for shared variables
- Button bounce causes multiple edge transitions
- Debouncing in interrupts requires disabling interrupt temporarily
- Main loop can handle debounce timing (non-blocking ISR)
- Event-driven is more efficient than polling
- ISRs should be short (delegation pattern)
- Interrupts provide immediate response regardless of main loop
- This pattern is used for real-time event handling in embedded systems
- Debounce period of 500ms (50 × 10ms) provides good stability

## Troubleshooting

### LED toggles multiple times per press
- Button bounce causing multiple interrupts
- Ensure ISR disables INT0 immediately
- Verify debounce_counter logic in main loop
- Check that interrupt is re-enabled after debounce period

### LED doesn't toggle at all
- Verify EIMSK has INT0 bit set
- Check EICRA trigger condition (falling edge for active-low)
- Ensure sei() was called (global interrupts enabled)
- Verify button is on PD2/2 (INT0 pin)
- Check button is configured to active-low

### LED state unpredictable
- Debounce not working correctly
- Increase debounce_counter value
- Verify button is active-low in PicsimLab
- Check that INT0 is disabled in ISR

### Main loop not re-enabling interrupt
- Verify debounce_counter decrements correctly
- Check condition when counter reaches 0
- Ensure EIMSK |= (1 << INT0) executes
- Use Pin Viewer to check INT0 state

### Interrupt not firing
- Verify sei() was called after setup
- Check EIMSK has INT0 enabled
- Ensure button is on correct pin (PD2 for INT0)
- Verify trigger condition matches button behavior
- Check that button creates falling edge (HIGH → LOW)

### Wrong pin used
- INT0 = PD2 (Arduino D2)
- INT1 = PD3 (Arduino D3)
- These are the only pins with external interrupts on ATmega328P
