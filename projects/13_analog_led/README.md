# Project 13: Analog LED Control

## Objective
Learn to combine ADC reading with PWM output to create an analog control system where a potentiometer controls LED brightness in real-time, including handling integer overflow and Fast PWM edge cases.

## Hardware Required (PicsimLab)
- Arduino Uno board
- Potentiometer (Spare Parts → Inputs → Pot)
- LED (Spare Parts → Outputs → output_LEDs)
- Virtual terminal (accessible through PicsimLab terminal window)

## Key Concepts
- ADC to PWM mapping
- Range conversion and scaling
- Integer overflow in multiplication
- Type promotion (uint16_t to uint32_t)
- Fast PWM edge case at OCR0A = 255
- Clamping values to avoid edge cases
- Real-time control loops
- Combined peripherals (ADC + PWM)
- Proportional control
- Code reuse with common/avr_io.h

## How to Build
```bash
make
```

## How to Run in PicsimLab

1. Open PicsimLab main board window (Arduino Uno)
2. Open Spare Parts window
3. Add potentiometer from Spare Parts → Inputs
4. Right-click potentiometer → Properties
5. Configure potentiometer to signal: PC0/A0
6. Add LED from Spare Parts → Outputs
7. Right-click LED → Properties
8. Configure LED to signal: PD6/6 (PWM-capable pin)
9. In main window: File → Load Hex → select main.hex
10. Tools → Arduino Bootloader
11. Click Run button
12. Open PicsimLab terminal/serial console
13. Set terminal baud rate to 9600
14. You will see "LED Control Via Pot" message
15. Adjust potentiometer and see ADC value, PWM value, and LED brightness change

## Code Explanation

### Using Helper Header
```c
#include "../common/avr_io.h"
```
- Includes UART functions from common helper
- Provides uart_init, uart_print_string, uart_print_number
- Reuses code from previous UART projects

### ADC Read Function
```c
uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | ( channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}
```
- Reads ADC value (0-1023) from specified channel
- Channel 0 = A0 (potentiometer)
- Polling for conversion completion
- Returns uint16_t (0-1023)

### ADC Initialization
```c
ADMUX = (1 << REFS0);  // AVcc as reference
ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
```
- 10-bit ADC with AVcc (5V) reference
- Prescaler 128 for 125 kHz ADC clock
- ADC enabled (ADEN bit)

### PWM Setup
```c
// PWM setup (Timer0, PD6/OC0A)
DDRD |= (1 << 6);
TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
TCCR0B = (1 << CS01) | (1 << CS00);
```

**DDRD Setup**:
- Configure PD6 as output
- PD6 is OC0A (Timer0 PWM output pin)

**TCCR0A (Timer0 Control Register A)**:
- `COM0A1`: Non-inverting PWM mode
- `WGM01` and `WGM00`: Fast PWM mode
- Timer counts 0 → 255 repeatedly

**TCCR0B (Timer0 Control Register B)**:
- `CS01` and `CS00`: Prescaler 64
- Timer clock = 16MHz / 64 = 250 kHz
- PWM frequency = 250kHz / 256 = 976 Hz

### Main Loop - ADC to PWM Mapping
```c
while(1) {
    uint32_t adc_value = adc_read(0);

    // Debug: print ADC value
    uart_print_string("ADC: ");
    uart_print_number(adc_value);
    uart_print_string(" PWM: ");

    // Map ADC (0-1023) to PWM (0-255)
    uint8_t pwm_value = (adc_value * 255) / 1023;
    if (pwm_value > 254) pwm_value = 254;  // Clamp at 254

    uart_print_number(pwm_value);
    uart_print_string("\r\n");

    OCR0A = pwm_value;
    _delay_ms(500);
}
```

**Reading ADC as uint32_t**:
```c
uint32_t adc_value = adc_read(0);
```
- Stores ADC value in uint32_t instead of uint16_t
- Prevents integer overflow in multiplication
- Type promotion happens automatically in subsequent calculations

**Why uint32_t is Needed**:
- If adc_value were uint16_t: `1023 * 255 = 260,865` (overflows uint16_t max of 65,535)
- With uint32_t: `1023 * 255 = 260,865` (fits in uint32_t max of 4,294,967,295)
- Compiler automatically promotes 255 to uint32_t when multiplying with uint32_t

**ADC to PWM Mapping**:
```c
uint8_t pwm_value = (adc_value * 255) / 1023;
```
- Converts ADC range (0-1023) to PWM range (0-255)
- Formula: PWM = (ADC / 1023) × 255
- Example: ADC=512 → (512/1023) × 255 = 127
- Example: ADC=0 → 0 (LED off)
- Example: ADC=1023 → 255 (LED full brightness)

**Clamping to 254**:
```c
if (pwm_value > 254) pwm_value = 254;
```
- Prevents OCR0A from being set to 255
- Fast PWM has an edge case at OCR0A = 255 where output may not toggle correctly
- At OCR0A = 254: duty cycle = 254/256 = 99.2% (indistinguishable from full brightness)
- Clamping avoids the LED turning off at maximum pot position

**Apply PWM**:
```c
OCR0A = pwm_value;
```
- Sets PWM duty cycle
- OCR0A = 0 → 0% duty cycle (LED off)
- OCR0A = 127 → 50% duty cycle (LED half brightness)
- OCR0A = 254 → 99.2% duty cycle (LED full brightness)

**Debug Output**:
```c
uart_print_string("ADC: ");
uart_print_number(adc_value);
uart_print_string(" PWM: ");
uart_print_number(pwm_value);
uart_print_string("\r\n");
```
- Displays ADC value and PWM value for debugging
- Helps verify mapping is working correctly
- Shows clamping effect (PWM never exceeds 254)

**Loop Delay**:
```c
_delay_ms(500);
```
- 500ms delay between readings
- 2 updates per second
- Slower than typical (50ms) but sufficient for debugging
- Can be reduced to 50ms for smoother response

## Expected Behavior

### Potentiometer at Minimum (0V)
- ADC reads: 0
- PWM value: 0
- LED: OFF
- Output: "ADC: 0 PWM: 0"

### Potentiometer at 50% (2.5V)
- ADC reads: ~512
- PWM value: 127
- LED: Half brightness
- Output: "ADC: 512 PWM: 127"

### Potentiometer at Maximum (5V)
- ADC reads: 1023
- PWM value: 254 (clamped from 255)
- LED: Full brightness
- Output: "ADC: 1023 PWM: 254"

### User Experience
- Turn pot counterclockwise: LED gradually dims
- Turn pot clockwise: LED gradually brightens
- At maximum pot position: LED stays on (doesn't turn off due to clamping)
- Smooth transition: No sudden jumps
- Debug output shows real-time values

## Mapping Table

| Pot Position | Voltage | ADC Value | PWM Value (Clamped) | LED Brightness |
|--------------|---------|-----------|---------------------|----------------|
| 0% (min) | 0V | 0 | 0 | 0% (off) |
| 25% | 1.25V | 256 | 63 | 25% |
| 50% | 2.5V | 512 | 127 | 50% |
| 75% | 3.75V | 768 | 191 | 75% |
| 100% (max) | 5V | 1023 | 254 | 99.2% (full) |

## Key Issues Solved

### Integer Overflow
**Problem**: `uint16_t * 255` overflows at high ADC values
```
1023 * 255 = 260,865 (exceeds uint16_t max of 65,535)
Result: Overflow wraps around, causing incorrect PWM values
```

**Solution**: Use uint32_t for adc_value
```
uint32_t adc_value = adc_read(0);
pwm_value = (adc_value * 255) / 1023;
```
- Compiler promotes multiplication to 32-bit arithmetic
- No overflow, correct mapping

### Fast PWM Edge Case at 255
**Problem**: OCR0A = 255 in Fast PWM causes LED to turn off
```
In Fast PWM mode, timer counts 0 → 255
At OCR0A = 255, the compare condition at exact wrap causes issues
Result: LED off instead of full brightness
```

**Solution**: Clamp PWM value to 254
```c
if (pwm_value > 254) pwm_value = 254;
```
- OCR0A = 254 gives 99.2% duty cycle
- Indistinguishable from full brightness to human eye
- LED stays on at maximum pot position

## Key Concepts

- **ADC to PWM mapping**: Converting one range to another
- **Scaling**: (input / input_max) × output_max
- **Integer overflow**: When multiplication exceeds variable size
- **Type promotion**: Compiler automatically upgrades types in mixed operations
- **uint32_t**: Larger integer type to prevent overflow
- **Fast PWM edge case**: OCR0A = 255 can cause incorrect output
- **Clamping**: Limiting values to safe range
- **Real-time control loop**: Continuously reading and updating
- **Proportional control**: Output directly proportional to input
- **Combined peripherals**: ADC + PWM working together
- **Debug output**: Displaying intermediate values for troubleshooting
- **Code reuse**: Using common/avr_io.h helper header

## Comparison with Previous Projects

| Aspect | Project 6 (PWM) | Project 12 (ADC) | Project 13 (Combined) |
|--------|-----------------|-------------------|------------------------|
| Input | Software (breathing) | Potentiometer | Potentiometer |
| Output | LED brightness | Serial display | LED brightness |
| Conversion | None | ADC to voltage | ADC to PWM |
| Control | Automated | Manual (reading) | Manual (control) |
| Real-time | Yes | Yes (500ms delay) | Yes (500ms delay) |
| Overflow handling | Not needed | Not needed | Critical (uint32_t) |
| Edge cases | None | None | OCR0A = 255 clamp |

## Learnings

- ADC values can be mapped to PWM duty cycles
- Range conversion: (input / input_max) × output_max
- Integer multiplication can overflow if result exceeds variable size
- Using larger integer types (uint32_t) prevents overflow
- Type promotion happens automatically in mixed-type operations
- Fast PWM has edge case at OCR0A = 255
- Clamping values avoids edge cases
- Real-time analog control requires fast loops
- Potentiometers provide smooth analog input
- PWM provides smooth analog output
- Combined: analog input → digital processing → analog output
- Debug output helps verify calculations
- This is foundation for motor speed control, light dimmers, volume controls
- Helper headers enable code reuse across projects

## Troubleshooting

### LED blinks when adjusting pot
- Integer overflow in PWM calculation
- Use uint32_t for adc_value instead of uint16_t
- Check that multiplication uses 32-bit arithmetic

### LED turns off at maximum pot position
- Fast PWM edge case at OCR0A = 255
- Clamp PWM value to 254
- Verify clamping logic is present

### PWM value never reaches 255
- This is intentional due to clamping
- 254 provides 99.2% duty cycle (full brightness)
- If you need exact 255, switch to Phase Correct PWM

### No response to pot changes
- Verify ADC is reading correctly (check debug output)
- Check potentiometer is connected to A0
- Ensure PWM pin is PD6 (PWM-capable)
- Verify OCR0A is being updated

### Wrong brightness range
- Check mapping formula: (adc_value * 255) / 1023
- Verify ADC range is 0-1023
- Check PWM range is 0-254 (clamped)

### Build errors
- Ensure common/avr_io.h is accessible
- Check relative path "../common/avr_io.h"
- Verify Makefile includes common directory in include path

### Debug output shows wrong values
- Check uart_print_number function
- Verify adc_value is uint32_t
- Check division is using correct divisor (1023)
