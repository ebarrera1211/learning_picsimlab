# Project 12: Potentiometer Reading

## Objective
Learn to use the ADC (Analog-to-Digital Converter) peripheral to read analog voltage from a potentiometer, convert the digital value to voltage, and display the results over serial using fixed-point arithmetic.

## Hardware Required (PicsimLab)
- Arduino Uno board
- Potentiometer (Spare Parts → Inputs → Pot)
- Virtual terminal (accessible through PicsimLab terminal window)

## Key Concepts
- ADC (Analog-to-Digital Converter) peripheral
- ADC registers (ADMUX, ADCSRA, ADC)
- 10-bit ADC resolution (0-1023)
- Reference voltage selection (AVcc)
- ADC prescaler configuration
- Channel selection (A0-A5)
- Polling for conversion completion
- Voltage conversion formula
- Fixed-point arithmetic for display
- Floating-point to integer conversion
- Potentiometer as voltage divider

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
6. In main window: File → Load Hex → select main.hex
7. Tools → Arduino Bootloader
8. Click Run button
9. Open PicsimLab terminal/serial console
10. Set terminal baud rate to 9600
11. You will see "ADC Test" message
12. Adjust potentiometer and see ADC value and voltage change

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
    // Select ADC channel (0-5 for A0-A5)
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);

    // Start conversion
    ADCSRA |= (1 << ADSC);

    // Wait for conversion to complete
    while (ADCSRA & (1 << ADSC));

    // Return ADC value (0-1023)
    return ADC;
}
```

**ADMUX (ADC Multiplexer Selection Register)**:
- Selects which analog channel to read
- Lower 4 bits (MUX3:0) select channel (0-15)
- `ADMUX & 0xF0` preserves upper 4 bits (reference selection)
- `channel & 0x0F` ensures channel is 0-15
- Channel 0 = A0, Channel 1 = A1, etc.

**ADCSRA (ADC Control and Status Register A)**:
- `ADSC` (ADC Start Conversion) bit
- Setting to 1 starts conversion
- Bit stays 1 while conversion in progress
- Bit clears to 0 when conversion complete

**Polling for Completion**:
```c
while (ADCSRA & (1 << ADSC));
```
- Waits for ADSC bit to clear
- Indicates conversion finished
- Blocks until ready (simple but CPU-intensive)

**ADC Register**:
- Holds the 10-bit conversion result
- Value range: 0-1023 (2^10 = 1024 values)
- Returns as uint16_t

### ADC Initialization
```c
ADMUX = (1 << REFS0);  // AVcc as reference
ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Enable ADC, prescaler 128
```

**ADMUX - Reference Selection**:
- `REFS0` bit = 1: Use AVcc (5V) as reference voltage
- REFS0 = 0: Use internal 1.1V reference
- AVcc reference is most common for 0-5V signals

**ADCSRA - Enable and Prescaler**:
- `ADEN` (ADC Enable) bit = 1: Enable ADC peripheral
- `ADPS2`, `ADPS1`, `ADPS0`: Prescaler bits
- ADPS2=1, ADPS1=1, ADPS0=1 = prescaler 128
- ADC clock = 16MHz / 128 = 125 kHz
- ADC requires 50-200 kHz for accurate conversion

### Main Loop - Read and Convert
```c
while(1) {
    uint16_t adc_value = adc_read(0);  // Read A0
    float voltage = (adc_value * 5.0) / 1024.0;

    uart_print_string("ADC: ");
    uart_print_number(adc_value);
    uart_print_string(" Voltage: ");
    uart_print_number((uint16_t)(voltage * 100) / 100);
    uart_print_string(".");
    uart_print_number((uint16_t)(voltage * 100) % 100);
    uart_print_string("V\r\n");

    _delay_ms(500);
}
```

**Reading ADC Value**:
```c
uint16_t adc_value = adc_read(0);
```
- Reads channel 0 (A0)
- Returns 0-1023
- 0 = 0V, 1023 = 5V (with AVcc reference)

**Voltage Conversion**:
```c
float voltage = (adc_value * 5.0) / 1024.0;
```
- Converts ADC value to voltage
- Formula: Voltage = (ADC / 1024) × Reference
- Example: ADC=512 → (512/1024) × 5V = 2.5V
- Uses floating-point arithmetic

**Displaying Voltage (Fixed Point)**:
```c
uart_print_number((uint16_t)(voltage * 100) / 100);
uart_print_string(".");
uart_print_number((uint16_t)(voltage * 100) % 100);
```
- Multiplies by 100 to get integer (e.g., 2.5V → 250)
- First part: 250 / 100 = 2 (integer division)
- Second part: 250 % 100 = 50 (modulo)
- Displays as "2.50V"
- Avoids floating-point printf overhead

**Example**: voltage = 4.99V
- voltage * 100 = 499
- uart_print_number(499 / 100) → prints "4"
- uart_print_string(".") → prints "."
- uart_print_number(499 % 100) → prints "99"
- Result: "4.99V"

**Note**: The division by 100 in the first part is critical - without it, you'd see "499.99" instead of "4.99".

## ADC Architecture

### ATmega328P ADC Features
- **10-bit resolution**: 1024 values (0-1023)
- **6 channels**: A0-A5 on Arduino Uno
- **Reference voltage**: Can be AVcc (5V), internal 1.1V, or external
- **Prescaler**: Divides CPU clock to achieve 50-200 kHz ADC clock
- **Single-ended**: Measures voltage relative to ground
- **Successive approximation**: Type of ADC used

### ADC Clock Speed
```
CPU Clock: 16 MHz
Prescaler: 128
ADC Clock: 16,000,000 / 128 = 125 kHz
Conversion Time: 13 ADC clock cycles per conversion
Result: 125,000 / 13 ≈ 9,615 conversions per second
```

### Voltage Calculation
```
ADC Value: 0-1023
Reference: 5V (AVcc)
Formula: Voltage = (ADC / 1023) × Reference

Examples:
ADC = 0    → 0V
ADC = 256  → 1.25V
ADC = 512  → 2.50V
ADC = 768  → 3.75V
ADC = 1023 → 5.00V
```

## Potentiometer Behavior

A potentiometer is a variable resistor that acts as a voltage divider:

```
5V ───[Pot]─── A0 ───[Pot]─── GND
          └───── Wiper ──────┘
```

- Turning pot changes wiper position
- Wiper outputs 0V to 5V based on position
- ADC reads this voltage
- A0 on Arduino Uno corresponds to ADC channel 0

## Output Example

```
ADC Test
ADC: 0 Voltage: 0.00V      (Pot at minimum)
ADC: 256 Voltage: 1.25V    (Pot at 25%)
ADC: 512 Voltage: 2.50V    (Pot at 50%)
ADC: 768 Voltage: 3.75V    (Pot at 75%)
ADC: 1023 Voltage: 5.00V   (Pot at maximum)
```

## Key Concepts

- **ADC (Analog-to-Digital Converter)**: Converts analog voltage to digital value
- **10-bit resolution**: 1024 discrete values (0-1023)
- **Reference voltage**: Maximum voltage ADC can measure
- **Channel selection**: Choosing which pin to read (A0-A5)
- **Prescaler**: Slowing CPU clock for ADC timing requirements
- **Polling**: Waiting for conversion to complete
- **Voltage conversion**: Formula to convert ADC value to voltage
- **Fixed-point arithmetic**: Avoiding floating-point for display
- **Potentiometer**: Variable voltage source for testing
- **Successive approximation**: ADC conversion algorithm
- **Code reuse**: Using common/avr_io.h helper header

## ADC Registers Summary

| Register | Purpose | Key Bits |
|----------|---------|----------|
| ADMUX | Channel & reference selection | REFS0, MUX3:0 |
| ADCSRA | Control & status | ADEN, ADSC, ADPS2:0 |
| ADC | Result register | 10-bit value |

## Learnings

- ADC converts analog voltage to digital values
- 10-bit ADC provides 1024 values (0-1023)
- Reference voltage determines maximum measurable voltage
- ADC clock must be 50-200 kHz for accuracy
- Prescaler divides CPU clock to achieve ADC clock
- Channel selection determines which pin to read
- Conversion takes ~13 ADC clock cycles
- Polling ADSC bit waits for conversion completion
- Voltage = (ADC / 1023) × Reference
- Fixed-point arithmetic avoids floating-point overhead
- Potentiometers provide variable voltage for testing
- Integer division (/) and modulo (%) split fixed-point numbers
- Helper headers (common/avr_io.h) enable code reuse
- This is foundational for reading sensors (temperature, light, etc.)

## Comparison with Digital Input

| Aspect | Digital Input (Project 3) | Analog Input (Project 12) |
|--------|--------------------------|---------------------------|
| Value type | 0 or 1 | 0-1023 |
| Register | PIN | ADC |
| Peripherals | GPIO | ADC |
| Resolution | 1 bit | 10 bits |
| Use case | Buttons, switches | Sensors, potentiometers |
| Conversion | None | Required (ADC) |

## Troubleshooting

### ADC value always 0
- Verify ADC is enabled (ADEN bit set)
- Check prescaler bits (ADPS2:0) are set
- Ensure channel selection is correct (channel 0 for A0)
- Check potentiometer is connected to A0
- Verify reference voltage (REFS0 bit)

### ADC value always 1023
- Check potentiometer connection to 5V and GND
- Verify wiper is connected to A0
- Check if pot is stuck at maximum
- Verify AVcc reference is selected (REFS0=1)

### Wrong voltage displayed
- Check voltage calculation formula
- Verify division by 100 in display code
- Ensure reference voltage is 5V
- Check ADC value range (should be 0-1023)

### No serial output
- Verify uart_init is called
- Check 100ms delay after uart_init
- Ensure BAUD is 9600
- Verify terminal baud rate matches

### Build errors
- Ensure common/avr_io.h is accessible
- Check relative path "../common/avr_io.h"
- Verify Makefile includes common directory in include path

### Voltage shows 499.99 instead of 4.99
- Missing division by 100 in first part of display
- Should be: (voltage * 100) / 100 not just (voltage * 100)
- The modulo (%) is correct for decimal part
