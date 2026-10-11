# Project 1: Blink LED

## Objective
Learn to control GPIO pins by blinking an LED using direct register manipulation.

## Hardware Required (PicsimLab)
- Arduino Uno board
- LED (Spare Parts → Outputs → output_LEDs)

## Key Concepts
- DDR register (Data Direction Register) - controls input/output
- PORT register (Output) - controls output state
- PIN register (Input) - reads input state
- Bit manipulation (|, &, ~, <<, >>)
- Delay functions (_delay_ms)
- Infinite loop structure
- ATmega328P register mapping

## How to Build
```bash
make
```

This will compile main.c and generate main.hex.

## How to Run in PicsimLab

1. Open PicsimLab main board window (Arduino Uno)
2. Open Spare Parts window
3. Add LED from Spare Parts → Outputs
4. Right-click LED board → Properties
5. Configure one LED (e.g., D1) to signal: PB5/13
6. In main window: File → Load Hex → select main.hex
7. **Critical**: Tools → Arduino Bootloader (initializes the bootloader)
8. Click Run button
9. LED should blink at 1 Hz (500ms on, 500ms off)

## Code Explanation

### Setting the Pin as Output
```c
DDRB |= (1 << LED_PIN);
```
- `DDRB` is the Data Direction Register for Port B
- Setting a bit to 1 makes that pin an output
- LED_PIN = 5, so we set bit 5 of DDRB
- Equivalent to: DDRB = DDRB | 0b00100000

### Turning LED On
```c
PORTB |= (1 << LED_PIN);
```
- `PORTB` is the output register for Port B
- Setting a bit to 1 makes that pin HIGH (3.3V/5V)
- Turns the LED on

### Turning LED Off
```c
PORTB &= ~(1 << LED_PIN);
```
- `(1 << LED_PIN)` creates a mask with bit 5 set: 0b00100000
- `~` inverts it: 0b11011111
- `&=` clears only bit 5, preserves all other bits
- Turns the LED off

### Why PORTB &= ~(1 << bit)?
This is the safe way to clear a single bit without affecting others:
- WRONG: `PORTB = 0;` (clears ALL bits)
- WRONG: `PORTB &= ~(1 << bit);` without checking first might clear needed bits
- CORRECT: `PORTB &= ~(1 << bit);` clears only the target bit

## Learnings

- The ATmega328P has Ports A, B, C, D, each with 8 pins
- Each port has three registers: DDR (direction), PORT (output), PIN (input)
- PB5 (Port B, bit 5) corresponds to Arduino pin D13
- LEDs are active on the Arduino Uno built-in LED
- PicsimLab requires "Arduino Bootloader" step after loading hex for Arduino Uno
- Makefiles require TAB characters before recipe lines
- Bit manipulation is fundamental to embedded programming

## Troubleshooting

### LED not blinking
- Verify Arduino Bootloader was run (Tools → Arduino Bootloader)
- Check LED is configured to PB5/13 in Spare Parts
- Ensure simulation is running (Run button)
- Check Pin Viewer for PB5/13 state

### Makefile error: "missing separator"
- Makefile recipe lines must start with TAB characters, not spaces
- Use `cat -A Makefile` to check tabs appear as ^I

### Wrong timing
- Verify F_CPU = 16000000UL matches PicsimLab clock
- Check PicsimLab Microcontroller menu for clock frequency
- Verify both delays are 500ms (not using `!=` instead of `|=`)

