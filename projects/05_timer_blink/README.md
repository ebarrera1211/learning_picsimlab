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
9. LED blinks at approximately 1 Hz (timing depends on prescaler and overflow count)

## Code Explanation

### Interrupt Header
```c
#include <avr/interrupt.h>
```
- Required for interrupt-related functions and macros
- Provides `sei()`, `cli()`, and `ISR()` macro

### Volatile Global Variable
```c
volatile uint8_t timer_flag = 0;
```
- `volatile` keyword tells compiler variable can change unexpectedly
- Prevents compiler from caching or optimizing away reads
- Critical for variables shared between ISR and main loop
- Without volatile, compiler might remove the check thinking it never changes

### Interrupt Service Routine (ISR)
```c
ISR(TIMER0_OVF_vect) {
    timer_flag = 1;
}
```
- `ISR()` declares an Interrupt Service Routine
- `TIMER0_OVF_vect` is the vector name for Timer0 overflow interrupt
- Called automatically when Timer0 overflows (255 → 0)
- Sets flag to notify main loop
- Must be as short as possible (avoid long delays or complex logic)

### Timer Configuration
```c
TCCR0A = 0;  // Normal mode
TCCR0B = (1 << CS02) | (1 << CS00);  // Prescaler = 1024
TIMSK0 = (1 << TOIE0);  // Enable Timer0 overflow interrupt
```

**TCCR0A (Timer/Counter Control Register A)**:
- Set to 0 for "Normal mode"
- Timer counts 0 → 255, then overflows to 0
- Other modes: CTC (Clear Timer on Compare), PWM modes

**TCCR0B (Timer/Counter Control Register B)**:
- Controls prescaler via CS02, CS01, CS00 bits
- Prescaler divides CPU clock before reaching timer
- Prescaler = 1024: timer clock = 16MHz / 1024 = 15.625 kHz
- Slows down timer for practical timing

**TIMSK0 (Timer Interrupt Mask Register)**:
- `TOIE0` bit enables Timer0 overflow interrupt
- When timer overflows, ISR is called automatically

### Timer Frequency Calculation
```
CPU Clock: 16 MHz
Prescaler: 1024
Timer Clock: 16,000,000 / 1024 = 15,625 Hz
Timer Range: 0-255 (256 counts)
Overflow Frequency: 15,625 / 256 = 61 Hz
Overflow Period: 1 / 61 = 16.4 ms
```

To get 500ms (0.5s) for LED toggle:
- Need 500ms / 16.4ms ≈ 30 overflows
- Count overflows in ISR and set flag after 30

### Enable Global Interrupts
```c
sei();  // Set Enable Interrupts
```
- Must be called after configuring individual interrupts
- Without this, no interrupts will fire
- Enables global interrupt flag in status register

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

## Blocking vs Non-Blocking Comparison

**Project 1 (Blocking)**:
```c
PORTB |= (1 << LED_PIN);
_delay_ms(500);  // CPU blocked for 500ms, can't do anything else
PORTB &= ~(1 << LED_PIN);
_delay_ms(500);  // CPU blocked again
```

**Project 5 (Non-Blocking)**:
```c
while (1) {
    if (timer_flag) {
        // Toggle LED (microseconds)
    }
    // CPU free to check buttons, read sensors, update display, etc.
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
- Interrupts have priority and can interrupt main code at any time
- ISRs should be short and fast
- This is foundation for all timing-critical embedded code

## Troubleshooting

### LED not blinking
- Verify `sei()` was called (enables interrupts)
- Check TIMSK0 has TOIE0 bit set
- Ensure Tools → Arduino Bootloader was run
- Check that overflow counter reaches threshold

### LED blinks too fast
- Increase prescaler (use 1024 instead of 256)
- Increase overflow count threshold
- Check overflow counter logic in ISR

### LED blinks too slow
- Decrease prescaler
- Decrease overflow count threshold
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
