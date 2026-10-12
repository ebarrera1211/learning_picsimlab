# Project 5: Timer-Based Blink

## Objective
Learn to use hardware timers and interrupts for non-blocking LED blinking, replacing blocking delays with timer-driven timing.

## Hardware Required (PicsimLab)
- Arduino Uno board
- 1 LED (Spare Parts → Outputs → output_LEDs)

## Key Concepts
- AVR timer architecture (Timer0)
- Timer registers (TCCR0A, TCCR0B, TIMSK0)
- Timer prescaler configuration
- Interrupt Service Routines (ISR)
- TIMER0_OVF_vect interrupt vector
- Global interrupt enable (sei())
- Volatile variables for shared data
- Non-blocking programming
- Timer overflow frequency calculation
- Overflow counting for precise timing

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
6. In main window: File → Load Hex → select main.hex
7. Tools → Arduino Bootloader
8. Click Run button
9. LED blinks at 1 Hz (500ms on, 500ms off)

## Code Explanation

### Interrupt Header
```c
#include <avr/interrupt.h>
```
- Required for interrupt-related functions and macros
- Provides `sei()`, `cli()`, and `ISR()` macro

### Volatile Global Variables
```c
volatile uint8_t timer_flag = 0;
volatile uint8_t overflow_count = 0;
```
- `volatile` keyword tells compiler variables can change unexpectedly
- Prevents compiler from caching or optimizing away reads
- Critical for variables shared between ISR and main loop
- `timer_flag` is set when 30 overflows occur (~500ms)
- `overflow_count` counts individual overflows

### Interrupt Service Routine (ISR)
```c
ISR(TIMER0_OVF_vect) {
    overflow_count++;
    if (overflow_count >= 30) {  // 30 overflows = ~500ms
        overflow_count = 0;
        timer_flag = 1;
    }
}
```
- `ISR()` declares an Interrupt Service Routine
- `TIMER0_OVF_vect` is the vector name for Timer0 overflow interrupt
- Called automatically when Timer0 overflows (255 → 0)
- Counts overflows and sets flag after 30 overflows
- Must be as short as possible (avoids blocking)

### Timer Configuration
```c
TCCR0A = 0;  // Normal mode
TCCR0B = (1 << CS02) | (1 << CS00);  // Prescaler = 1024
TIMSK0 = (1 << TOIE0);  // Enable Timer0 overflow interrupt
```

**TCCR0A (Timer/Counter Control Register A)**:
- Set to 0 for "Normal mode"
- Timer counts 0 → 255, then overflows to 0

**TCCR0B (Timer/Counter Control Register B)**:
- `CS02` and `CS00` bits set = prescaler 1024
- Timer clock = 16MHz / 1024 = 15.625 kHz
- Overflow frequency = 15,625 / 256 = 61 Hz
- Overflow period = 1 / 61 = 16.4 ms

**TIMSK0 (Timer Interrupt Mask Register)**:
- `TOIE0` bit enables Timer0 overflow interrupt
- When timer overflows, ISR is called automatically

### Timer Calculation
```
CPU Clock: 16 MHz
Prescaler: 1024
Timer Clock: 16,000,000 / 1024 = 15,625 Hz
Overflow Frequency: 15,625 / 256 = 61 Hz
Overflow Period: 1 / 61 = 16.4 ms

For 500ms toggle:
30 overflows × 16.4 ms = 492 ms ≈ 500ms
```

### Enable Global Interrupts
```c
sei();  // Set Enable Interrupts
```
- Must be called after configuring individual interrupts
- Without this, no interrupts will fire

### Main Loop with Non-Blocking Check
```c
while (1) {
    if (timer_flag) {
        timer_flag = 0;
        led_state = !led_state;
        // Toggle LED
    }
    // Main loop can do other work here!
}
```
- Checks flag set by interrupt
- If set, clears flag and toggles LED
- Immediately continues (no blocking delay)
- Can add other tasks in the loop

### Helper Header
```c
#include "../common/avr_io.h"
```
- Uses helper header for LED_PIN definition
- Provides convenience macros (though not used in this implementation)

## Blocking vs Non-Blocking Comparison

**Project 1 (Blocking)**:
```c
PORTB |= (1 << LED_PIN);
_delay_ms(500);  // CPU blocked for 500ms
PORTB &= ~(1 << LED_PIN);
_delay_ms(500);  // CPU blocked again
```

**Project 5 (Non-Blocking)**:
```c
while (1) {
    if (timer_flag) {
        // Toggle LED (microseconds)
    }
    // CPU free to check buttons, read sensors, etc.
}
```

## Timer Prescaler Options

| CS02 | CS01 | CS00 | Prescaler | Timer Clock | Overflow Period |
|------|------|------|-----------|-------------|-----------------|
| 0 | 0 | 0 | Stopped | 0 Hz | - |
| 0 | 0 | 1 | 1 | 16 MHz | 16 µs |
| 0 | 1 | 0 | 8 | 2 MHz | 128 µs |
| 0 | 1 | 1 | 64 | 250 kHz | 1.02 ms |
| 1 | 0 | 0 | 256 | 62.5 kHz | 4.1 ms |
| 1 | 0 | 1 | 1024 | 15.625 kHz | 16.4 ms |

## Learnings

- Hardware timers provide precise, CPU-independent timing
- Interrupts allow background execution of code
- Timer0 is an 8-bit timer (0-255)
- Prescaler controls timer speed without changing CPU clock
- TCCR0A/TCCR0B configure timer mode and prescaler
- TIMSK0 enables specific timer interrupts
- ISR (Interrupt Service Routine) handles interrupt events
- `volatile` is critical for variables shared with interrupts
- Non-blocking programming allows multitasking
- `sei()` enables global interrupts (must be called)
- Timer overflow occurs automatically when counter reaches limit
- Overflow counting allows precise timing beyond single overflow period
- ISRs should be short and fast
- This is foundation for all timing-critical embedded code

## Troubleshooting

### LED not blinking
- Verify `sei()` was called (enables interrupts)
- Check TIMSK0 has TOIE0 bit set
- Ensure Tools → Arduino Bootloader was run
- Check that overflow_count reaches 30

### LED blinks too fast
- Increase overflow_count threshold (currently 30)
- Increase prescaler (use 1024 instead of 256)
- Verify CPU clock is 16MHz in PicsimLab

### LED blinks too slow
- Decrease overflow_count threshold
- Decrease prescaler
- Verify CPU clock is 16MHz in PicsimLab

### Nothing happens
- Check ISR vector name is correct (TIMER0_OVF_vect)
- Verify TCCR0B has prescaler bits set (not 0)
- Ensure `volatile` keyword on shared variables
- Check global interrupts enabled with `sei()`

### Compiler warnings about unused variables
- These are normal for the ISR flag pattern
- The compiler can't see that interrupt modifies the variable
- That's why `volatile` is necessary
