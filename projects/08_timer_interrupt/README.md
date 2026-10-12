# Project 8: Timer Interrupts with Multi-Tasking

## Objective
Learn to use Timer1 for precise 1-second timing while simultaneously monitoring a button in the main loop, implementing multi-tasking with two LEDs where LED2 toggles 10 times then stays on, with button press resetting the timer.

## Hardware Required (PicsimLab)
- Arduino Uno board
- 2 LEDs (Spare Parts → Outputs → output_LEDs)
- 1 Push button (Spare Parts → Inputs → Push Buttons)

## Key Concepts
- Timer1 (16-bit timer) configuration
- CTC (Clear Timer on Compare) mode
- OCR for precise timing
- Multi-tasking (interrupts + polling)
- Timer calculations
- Button press resets timer state
- LED2 counting logic (10 ON cycles before staying ON)
- Active-low button with pull-up

## How to Build
```bash
make
```

## How to Run in PicsimLab

1. Open PicsimLab main board window (Arduino Uno)
2. Open Spare Parts window
3. Add 2 LEDs from Spare Parts → Outputs
4. Right-click LED board → Properties
5. Configure LEDs:
   - LED D1 → PB5/13 (LED1)
   - LED D2 → PB4/12 (LED2)
6. Add push button from Spare Parts → Inputs
7. Right-click button → Properties
8. Configure button to signal: PD2/2
9. Set button to "active-low" mode
10. In main window: File → Load Hex → select main.hex
11. Tools → Arduino Bootloader
12. Click Run button
13. LED2 toggles every second for 19 seconds (10 ON cycles), then stays ON
14. Button press toggles LED1 and resets LED2 timer (turns LED2 OFF, restarts toggle cycle)

## Code Explanation

### Shared Variables (Volatile)
```c
volatile uint8_t second_flag = 0;
volatile uint16_t seconds = 0;
```
- `volatile` critical because modified by interrupt
- `second_flag` set to 1 every second by interrupt
- `seconds` running counter of total elapsed time

### Timer1 Compare Match ISR
```c
ISR(TIMER1_COMPA_vect) {
    second_flag = 1;
    seconds++;
}
```
- `ISR(TIMER1_COMPA_vect)` handler for Timer1 Compare Match A
- Fires when Timer1 reaches OCR1A value
- Sets flag to notify main loop
- Increments global seconds counter
- ISR is very short (good practice!)

### Timer1 Configuration
```c
TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
OCR1A = 15624;
TIMSK1 = (1 << OCIE1A);
```

**TCCR1B - Control Register B**:
- `WGM12` bit = 1 → CTC mode (Clear Timer on Compare)
  - Timer counts up to OCR1A, then resets to 0
- `CS12` and `CS10` bits = 1 → prescaler 1024
  - Timer clock = 16MHz / 1024 = 15.625 kHz

**OCR1A - Output Compare Register A**:
- Sets the compare value for 1-second interrupts
- Value of 15624 gives 1Hz interrupt rate

**TIMSK1 - Timer Interrupt Mask Register**:
- `OCIE1A` bit = 1 enables Timer1 Compare Match A interrupt

### Timer Calculation
```
CPU Clock: 16 MHz
Prescaler: 1024
Timer Clock: 16,000,000 / 1024 = 15,625 Hz

Want: 1 Hz interrupt (1 second)
Need: Timer to reach OCR1A once per second

Calculation:
15,625 counts per second
OCR1A = 15,625 - 1 = 15,624

Verification:
Timer counts: 0 → 15,624 (15,625 counts)
Time: 15,625 / 15,625 Hz = 1 second ✓
```

### Main Loop Variables
```c
uint8_t led1_state = 0;
uint8_t led2_state = 0;
uint16_t timer_seconds = 0;  // Separate timer for LED2
```
- `led1_state` tracks LED1 (toggled by button)
- `led2_state` tracks LED2 (toggled by timer)
- `timer_seconds` counts seconds since last button press (independent of global `seconds`)

### Button Handling (Polling)
```c
if (((PIND >> BUTTON_PIN) & 1) == 0) {
    _delay_ms(50);  // Debounce
    if (((PIND >> BUTTON_PIN) & 1) == 0) {
        // Button pressed - reset LED2 timer and turn off LED2
        timer_seconds = 0;
        led2_state = 0;
        PORTB &= ~(1 << LED2_PIN);

        // Toggle LED1
        led1_state = !led1_state;
        PORTB ^= (1 << LED1_PIN);

        while (((PIND >> BUTTON_PIN) & 1) == 0);  // Wait for release
    }
}
```
- Checks button state in main loop (polling)
- Active-low (checks for 0)
- Debounce delay (50ms)
- When pressed:
  - Resets `timer_seconds` to 0
  - Turns off LED2 immediately
  - Toggles LED1
  - Waits for button release (blocking wait)

### LED2 Logic (Timer Flag Handler)
```c
if (second_flag) {
    second_flag = 0;
    
    // LED2 logic
    if (timer_seconds < 19) {
        // First 19 seconds: toggle LED2 every second so on 10th ON cycle it stays ON
        led2_state = !led2_state;
        if (led2_state) {
            PORTB |= (1 << LED2_PIN);
        } else {
            PORTB &= ~(1 << LED2_PIN);
        }
        timer_seconds++;
    } else {
        // After 10 seconds: keep LED2 on
        PORTB |= (1 << LED2_PIN);
    }
}
```

**LED2 Behavior**:
- First 19 seconds: toggles every second
- LED2 starts OFF (led2_state = 0)
- Second 1: OFF → ON (count = 1 ON)
- Second 2: ON → OFF (count = 1 ON)
- Second 3: OFF → ON (count = 2 ON)
- ...
- Second 19: OFF → ON (count = 10 ON)
- After 19 seconds: LED2 stays ON permanently
- Button press resets to 0 and starts cycle over

**Why 19 seconds?**
- LED2 toggles every second
- To get 10 ON cycles, need 19 total toggles
- Pattern: ON, OFF, ON, OFF, ..., ON (10th ON) = 19 transitions
- After 19th toggle, LED2 is ON and stays ON

## Multi-Tasking Pattern

**Separation of Concerns**:
- Timer interrupt handles precise 1-second timing
- Main loop handles button polling
- Both run concurrently
- Interrupt doesn't block main loop
- Main loop doesn't block interrupt

**Two Independent Timers**:
- `seconds` - global counter (never resets)
- `timer_seconds` - LED2 cycle counter (resets on button press)
- This allows tracking total time while managing LED2 cycle independently

## LED2 State Timeline

**Normal Cycle (no button press)**:
```
Time: 0s   LED2: OFF
Time: 1s   LED2: ON  (count = 1)
Time: 2s   LED2: OFF (count = 1)
Time: 3s   LED2: ON  (count = 2)
...
Time: 18s  LED2: OFF (count = 9)
Time: 19s  LED2: ON  (count = 10)
Time: 20s+ LED2: ON  (stays ON)
```

**With Button Press**:
```
Anytime: Button press → LED2 OFF, timer_seconds = 0
Time: 0s   LED2: OFF
Time: 1s   LED2: ON  (count = 1)
... (cycle restarts)
```

## Comparison with Previous Projects

| Project | Timing Method | Button Method | LED Count | Concurrency |
|---------|--------------|---------------|-----------|-------------|
| 5 | Timer0 overflow interrupt | None | 1 | Timer only |
| 7 | None | External interrupt | 1 | Button only |
| 8 | Timer1 CTC interrupt | Polling | 2 | Both! |

## Learnings

- Timer1 is a 16-bit timer (more range than Timer0)
- CTC mode allows flexible compare values via OCR1A
- OCR1A sets the "top" value in CTC mode
- Timer calculation: (CPU Clock / Prescaler) / OCR1A = Frequency
- Multi-tasking: interrupts + polling together
- ISR should be short (set flag, exit)
- Main loop handles the actual work
- Volatile variables for interrupt-shared data
- Two independent timers can track different things
- Button can reset timer state
- LED2 counting: 19 toggles = 10 ON cycles
- This pattern is used for real-time systems with multiple concurrent tasks
- Interrupts provide background timing, main loop handles foreground tasks
- Button polling works alongside timer interrupts

## Troubleshooting

### LED2 not toggling
- Verify Timer1 is configured correctly (WGM12, CS12, CS10)
- Check OCR1A value (15624 for 1Hz)
- Ensure TIMSK1 has OCIE1A bit set
- Verify sei() was called

### LED2 never stays ON
- Check timer_seconds comparison (should be < 19)
- Verify timer_seconds increments correctly
- Check that else branch executes after 19 seconds

### Button doesn't reset LED2
- Verify button is configured to active-low
- Check button polling logic
- Ensure timer_seconds is set to 0 on button press
- Verify LED2 is turned off immediately on button press

### LED1 not toggling
- Check XOR operation: PORTB ^= (1 << LED1_PIN)
- Verify button press detection works
- Check while loop waiting for release isn't stuck

### Timing too fast/slow
- Verify CPU clock is 16MHz in PicsimLab
- Check prescaler bits (CS12, CS10 for 1024)
- Verify OCR1A value (15624)

### Wrong pin assignments
- LED1 = PB5/13
- LED2 = PB4/12
- Button = PD2/2
