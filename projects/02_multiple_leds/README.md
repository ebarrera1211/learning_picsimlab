# Project 2: Multiple LEDs

## Objective
Learn to control multiple GPIO pins simultaneously, create LED patterns, and understand binary representation.

## Hardware Required (PicsimLab)
- Arduino Uno board
- 4 LEDs (Spare Parts → Outputs → output_LEDs)

## Key Concepts
- Setting multiple output pins at once
- Bitwise OR (|) for setting multiple bits
- Direct register assignment (PORTB = value)
- Binary representation and counting
- Pattern generation using loops
- Code size awareness

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
   - LED D1 → PB0/8
   - LED D2 → PB1/9
   - LED D3 → PB2/10
   - LED D4 → PB3/11
6. In main window: File → Load Hex → select main.hex
7. Tools → Arduino Bootloader
8. Click Run button
9. LEDs will display running lights pattern, then binary counter (0-15)

## Code Explanation

### Setting Multiple Output Pins
```c
DDRB |= (1 << LED1) | (1 << LED2) | (1 << LED3) | (1 << LED4);
```
- Sets bits 0, 1, 2, 3 of DDRB simultaneously
- Uses bitwise OR to combine multiple bit masks
- More efficient than setting each bit individually

### Pattern 1: Running Lights
```c
for (uint8_t i = 0; i < 4; i++) {
    PORTB = (1 << i);
    _delay_ms(200);
}
```
- Creates a wave pattern by shifting a single bit
- `(1 << 0)` = 0b00000001 (LED1)
- `(1 << 1)` = 0b00000010 (LED2)
- `(1 << 2)` = 0b00000100 (LED3)
- `(1 << 3)` = 0b00001000 (LED4)

### Pattern 2: Binary Counter
```c
for (uint8_t i = 0; i < 16; i++) {
    PORTB = i;
    _delay_ms(500);
}
```
- Counts from 0 to 15 in binary
- LEDs represent binary digits (0 = off, 1 = on)
- Example: value 5 = 0b0101 → LED1 on, LED2 off, LED3 on, LED4 off
- Demonstrates direct register assignment

### Direct Assignment vs Bitwise Operations
```c
PORTB |= (1 << 2);     // Set bit 2, preserve others
PORTB &= ~(1 << 2);    // Clear bit 2, preserve others
PORTB = 0b00000100;    // Set all bits to specific value
```
- `|=` is for setting individual bits
- `&= ~` is for clearing individual bits
- `=` is for setting the entire register

## Learnings

- Multiple pins can be configured in one operation using bitwise OR
- LED patterns can be created by shifting bits
- Binary numbers are directly represented by register values
- Direct register assignment (PORTB = value) sets all bits at once
- Each bit in a byte represents a distinct hardware state
- Running lights pattern teaches bit shifting
- Binary counter demonstrates the relationship between numbers and binary representation
- Arduino Uno Port B pins: PB0=D8, PB1=D9, PB2=D10, PB3=D11, PB5=D13

## Troubleshooting

### Wrong LEDs lighting up
- Verify LED signal assignments in Spare Parts (should be PB0/8, PB1/9, PB2/10, PB3/11)
- Check that DDRB has the correct bits set (0b00001111 for bits 0-3)

### LEDs not changing
- Ensure Tools → Arduino Bootloader was run
- Check simulation is running
- Verify code has both patterns uncommented

### Pattern too fast/slow
- Adjust _delay_ms() values
- 200ms for running lights, 500ms for binary counter

### Understanding the binary counter
- LED1 = least significant bit (bit 0)
- LED4 = most significant bit (bit 3)
- Value 0 = all LEDs off
- Value 15 = all LEDs on (0b1111 = 15)
