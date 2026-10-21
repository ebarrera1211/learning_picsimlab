# Project 14: Temperature Sensor (LM35)

## Objective
Learn to read a temperature sensor (LM35) using ADC, convert ADC values to Celsius with proper scaling, implement temperature thresholds with LED indicators, and handle integer overflow in temperature calculations.

## Hardware Required (PicsimLab)
- Arduino Uno board
- LM35 temperature sensor (Spare Parts → Inputs → LM35)
- 3 LEDs (Spare Parts → Outputs → output_LEDs)
- Virtual terminal (accessible through PicsimLab terminal window)

## Key Concepts
- LM35 temperature sensor characteristics
- Temperature sensor voltage-to-temperature conversion
- ADC reading for analog sensors
- Temperature scaling formulas
- Integer overflow in multiplication
- Threshold-based control
- Multi-LED state management
- Signed integers (int16_t) for temperature
- Code reuse with common/avr_io.h

## How to Build
```bash
make
```

## How to Run in PicsimLab

1. Open PicsimLab main board window (Arduino Uno)
2. Open Spare Parts window
3. Add LM35 temperature sensor from Spare Parts → Inputs
4. Right-click LM35 → Properties
5. Configure LM35 to signal: PC0/A0
6. Add 3 LEDs from Spare Parts → Outputs
7. Right-click LED board → Properties
8. Configure LEDs:
   - LED D1 → PD6/6 (Red - hot)
   - LED D2 → PD5/5 (Blue - cold)
   - LED D3 → PD3/3 (Yellow - warning)
9. In main window: File → Load Hex → select main.hex
10. Tools → Arduino Bootloader
11. Click Run button
12. Open PicsimLab terminal/serial console
13. Set terminal baud rate to 9600
14. You will see "Temperature Monitor" message
15. Adjust LM35 temperature in PicsimLab and see ADC value, temperature, and LED states change

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
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}
```
- Reads ADC value (0-1023) from specified channel
- Channel 0 = A0 (LM35)
- Lower 4 bits (0x0F) select channel (corrected from 0xF0)
- Polling for conversion completion

### LED Pin Configuration
```c
DDRD |= (1 << 6) | (1 << 5) | (1 << 3);
```
- PD6 (bit 6): Red LED (hot indicator)
- PD5 (bit 5): Blue LED (cold indicator)
- PD3 (bit 3): Yellow LED (warning indicator)
- All configured as outputs

### ADC Initialization
```c
ADMUX = (1 << REFS0);  // AVcc as reference
ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
```
- 10-bit ADC with AVcc (5V) reference
- Prescaler 128 for 125 kHz ADC clock
- ADC enabled (ADEN bit)

### Main Loop - Temperature Reading
```c
while(1) {
    uint32_t adc_value = adc_read(0);
    uart_print_string("ADC: ");
    uart_print_number(adc_value);
    uart_print_string(" ");

    // LM35: 10mv per degree Celsius
    // ADC: 0-1023 = 0-5V, so 1 ADC count = 4.88mV
    // Temperature = (ADC * 4.88) / 10 = ADC * 0.488
    int16_t temp_c = (adc_value * 488) / 1000;

    uart_print_string("Temperature: ");
    uart_print_number(temp_c);
    uart_print_string("C\r\n");
```

**Reading ADC as uint32_t**:
```c
uint32_t adc_value = adc_read(0);
```
- Stores ADC value in uint32_t instead of uint16_t
- Prevents integer overflow in temperature calculation
- Type promotion happens automatically in subsequent calculations

**Why uint32_t is Needed**:
- Temperature formula: (adc_value * 488) / 1000
- At ADC = 306: 306 × 488 = 149,328
- This exceeds int16_t max (32,767) and int32_t would overflow in int16_t
- With uint32_t: calculation fits correctly
- Result: 149,328 / 1000 = 149°C (correct)

**Temperature Conversion Formula**:
```c
int16_t temp_c = (adc_value * 488) / 1000;
```
- LM35 outputs 10mV per degree Celsius
- ADC resolution: 5V / 1024 = 4.88mV per count
- Temperature per ADC count: 4.88mV / 10mV/°C = 0.488°C per count
- Fixed-point arithmetic: multiply by 488, divide by 1000
- Example: ADC = 204 → (204 × 488) / 1000 = 99°C

**Debug Output**:
```c
uart_print_string("ADC: ");
uart_print_number(adc_value);
uart_print_string(" Temperature: ");
uart_print_number(temp_c);
uart_print_string("C\r\n");
```
- Displays ADC value and temperature for debugging
- Helps verify conversion is working correctly
- Shows overflow is handled correctly

### Temperature Threshold Logic
```c
// Threshold indicators
if (temp_c >= 90) {
    PORTD |= (1 << 6);  // Red LED (hot)
    PORTD &= ~((1 << 5) | (1 << 3));
} else if (temp_c >= 75 & temp_c < 90) {
    PORTD |= (1 << 3);  // Yellow LED (warn)
    PORTD &= ~((1 << 5) | (1 << 6));
} else if (temp_c <= 20) {
    PORTD |= (1 << 5);  // Blue LED (cold)
    PORTD &= ~((1 << 6) | (1 << 3));
} else {
    PORTD &= ~((1 << 6) | (1 << 5) | (1 << 3));  // All off (normal)
}
```

**Temperature Ranges**:
- **≥ 90°C**: Red LED ON (hot)
  - Sets PD6 bit (red LED)
  - Clears PD5 and PD3 bits (blue and yellow off)
- **75-89°C**: Yellow LED ON (warning)
  - Sets PD3 bit (yellow LED)
  - Clears PD5 and PD6 bits (blue and red off)
- **≤ 20°C**: Blue LED ON (cold)
  - Sets PD5 bit (blue LED)
  - Clears PD6 and PD3 bits (red and yellow off)
- **21-74°C**: All LEDs OFF (normal range)
  - Clears all three LED bits

**Bit Manipulation**:
- `PORTD |= (1 << bit)`: Set bit (turn LED on)
- `PORTD &= ~((1 << bit1) | (1 << bit2))`: Clear multiple bits
- `&= ~`: AND with inverted mask clears bits

**Loop Delay**:
```c
_delay_ms(1000);
```
- 1 second delay between readings
- Temperature changes slowly, no need for faster updates
- Reduces serial output frequency

## Expected Behavior

### Cold Temperature (≤ 20°C)
- ADC value: Low (~41 or less)
- Temperature: ≤ 20°C
- Blue LED: ON
- Red LED: OFF
- Yellow LED: OFF
- Output: "ADC: 41 Temperature: 20C"

### Normal Temperature (21-74°C)
- ADC value: Medium (43-152)
- Temperature: 21-74°C
- Blue LED: OFF
- Red LED: OFF
- Yellow LED: OFF
- Output: "ADC: 100 Temperature: 49C"

### Warning Temperature (75-89°C)
- ADC value: High (154-182)
- Temperature: 75-89°C
- Blue LED: OFF
- Red LED: OFF
- Yellow LED: ON
- Output: "ADC: 164 Temperature: 80C"

### Hot Temperature (≥ 90°C)
- ADC value: Very high (184+)
- Temperature: ≥ 90°C
- Blue LED: OFF
- Red LED: ON
- Yellow LED: OFF
- Output: "ADC: 306 Temperature: 149C"

## Temperature to ADC Mapping

| Temperature | Voltage | ADC Value | LED State |
|-------------|---------|-----------|-----------|
| 0°C | 0V | 0 | Blue ON |
| 20°C | 0.2V | 41 | Blue ON |
| 50°C | 0.5V | 102 | All OFF |
| 75°C | 0.75V | 154 | Yellow ON |
| 90°C | 0.9V | 184 | Red ON |
| 150°C | 1.5V | 307 | Red ON |

## LM35 Sensor Characteristics

- **Output**: 10mV per degree Celsius
- **Range**: -55°C to +150°C
- **Accuracy**: ±0.5°C at room temperature
- **Output at 25°C**: 250mV
- **Output at 0°C**: 0V
- **Linear response**: Voltage directly proportional to temperature

## Key Issues Solved

### Integer Overflow in Temperature Calculation
**Problem**: `(adc_value * 488) / 1000` overflows at high ADC values
```
At ADC = 306: 306 × 488 = 149,328 (exceeds int16_t max of 32,767)
Result: Overflow wraps around, showing 18°C instead of 149°C
```

**Solution**: Use uint32_t for adc_value
```c
uint32_t adc_value = adc_read(0);
int16_t temp_c = (adc_value * 488) / 1000;
```
- Compiler promotes multiplication to 32-bit arithmetic
- No overflow, correct temperature calculation

### Missing Assignment Operator
**Problem**: `PORTD | (1 << 3)` doesn't assign result
```c
PORTD | (1 << 3);  // Wrong - evaluates but doesn't assign
```

**Solution**: Use compound assignment operator
```c
PORTD |= (1 << 3);  // Correct - OR and assign
```

### Wrong Channel Mask
**Problem**: `channel & 0xF0` uses upper 4 bits instead of lower
```c
ADMUX = (ADMUX & 0xF0) | (channel & 0xF0);  // Wrong
```

**Solution**: Use lower 4 bits for channel selection
```c
ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);  // Correct
```

## Key Concepts

- **LM35 temperature sensor**: Analog voltage proportional to temperature
- **Temperature conversion**: Voltage to Celsius using sensor characteristics
- **Sensor scaling**: 10mV per degree Celsius
- **ADC resolution**: 4.88mV per count at 5V reference
- **Integer overflow**: When multiplication exceeds variable size
- **Type promotion**: Compiler automatically upgrades types in mixed operations
- **uint32_t**: Larger integer type to prevent overflow
- **Threshold logic**: If-else chain for temperature ranges
- **Multi-LED control**: Managing multiple outputs based on conditions
- **Signed integers**: int16_t for temperature (can be negative)
- **Bit manipulation**: Setting and clearing multiple bits simultaneously
- **Debug output**: Displaying intermediate values for troubleshooting
- **Code reuse**: Using common/avr_io.h helper header

## Comparison with Previous Projects

| Aspect | Project 12 (Potentiometer) | Project 14 (Temperature) |
|--------|---------------------------|---------------------------|
| Sensor | Potentiometer | LM35 temperature sensor |
| Output | Serial voltage display | Serial temperature + LEDs |
| Conversion | ADC to voltage | ADC to temperature |
| Units | Volts | Celsius |
| Overflow handling | uint32_t for PWM calc | uint32_t for temp calc |
| Indicators | None | 3 LEDs (thresholds) |
| Integer type | uint16_t | int16_t (can be negative) |

## Learnings

- Temperature sensors output analog voltage proportional to temperature
- LM35 outputs 10mV per degree Celsius
- Sensor-specific scaling factors are needed for conversion
- Fixed-point arithmetic avoids floating-point: (value × scale) / divisor
- Integer multiplication can overflow if result exceeds variable size
- Using larger integer types (uint32_t) prevents overflow
- Type promotion happens automatically in mixed-type operations
- Threshold-based control uses if-else chains
- Multiple outputs can be managed with bit manipulation
- Setting one bit: `PORT |= (1 << bit)`
- Clearing multiple bits: `PORT &= ~((1 << bit1) \| (1 << bit2))`
- Signed integers (int16_t) needed for values that can be negative
- Debug output helps verify calculations
- Helper headers enable code reuse across projects
- This is foundational for reading all types of analog sensors

## Troubleshooting

### Temperature reads 18°C at high ADC values
- Integer overflow in temperature calculation
- Use uint32_t for adc_value instead of uint16_t
- Check multiplication: adc_value * 488 might exceed int16_t max

### Yellow or blue LEDs not turning on
- Check for missing |= operator (PORTD | vs PORTD |=)
- Verify LED pins are configured as outputs
- Check threshold logic conditions

### ADC always reads same value
- Verify channel mask is 0x0F (lower 4 bits), not 0xF0
- Check LM35 is connected to A0
- Ensure ADC is enabled (ADEN bit)

### Wrong temperature readings
- Verify LM35 scaling factor (10mV/°C)
- Check reference voltage is 5V (REFS0 bit)
- Verify calculation formula: (ADC × 488) / 1000

### All LEDs on simultaneously
- Check bit clearing logic
- Verify &= ~ operation is correct
- Check that conditions are mutually exclusive

### Build errors
- Ensure common/avr_io.h is accessible
- Check relative path "../common/avr_io.h"
- Verify Makefile includes common directory in include path

### No serial output
- Verify uart_init is called
- Check baud rate is 9600
- Ensure terminal baud rate matches
