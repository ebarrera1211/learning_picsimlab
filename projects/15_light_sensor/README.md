# Project 15: Light Sensor (LDR)

## Objective
Learn to use an LDR (Light Dependent Resistor) for automatic light control, implement hysteresis to prevent rapid LED switching, and understand the inverse relationship between LDR resistance and light level.

## Hardware Required (PicsimLab)
- Arduino Uno board
- LDR (Light Dependent Resistor) (Spare Parts → Inputs → LDR)
- LED (Spare Parts → Outputs → output_LEDs)
- Virtual terminal (accessible through PicsimLab terminal window)

## Key Concepts
- LDR (Light Dependent Resistor) characteristics
- Inverse relationship between resistance and light
- Voltage divider behavior with LDR
- ADC reading for light level
- Hysteresis for stability
- Dead zone between thresholds
- State memory for hysteresis logic
- Automatic environmental control
- Threshold-based switching
- Code reuse with common/avr_io.h

## How to Build
```bash
make
```

## How to Run in PicsimLab

1. Open PicsimLab main board window (Arduino Uno)
2. Open Spare Parts window
3. Add LDR from Spare Parts → Inputs
4. Right-click LDR → Properties
5. Configure LDR to signal: PC0/A0
6. Add LED from Spare Parts → Outputs
7. Right-click LED → Properties
8. Configure LED to signal: PB5/13
9. In main window: File → Load Hex → select main.hex
10. Tools → Arduino Bootloader
11. Click Run button
12. Open PicsimLab terminal/serial console
13. Set terminal baud rate to 9600
14. You will see "Light Sensor Control" message
15. Adjust LDR slider in PicsimLab and see light level changes and LED state changes

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
- Channel 0 = A0 (LDR)
- Lower 4 bits (0x0F) select channel
- Polling for conversion completion

### LED Configuration
```c
DDRB |= (1 << 5);  // LED on PB5
```
- Configures PB5 (Arduino D13) as output
- LED will be controlled by this pin

### ADC Initialization
```c
ADMUX = (1 << REFS0);  // AVcc as reference
ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
```
- 10-bit ADC with AVcc (5V) reference
- Prescaler 128 for 125 kHz ADC clock
- ADC enabled (ADEN bit)

### Hysteresis Thresholds
```c
const uint16_t DARK_THRESHOLD = 400;
const uint16_t LIGHT_THRESHOLD = 600;
uint8_t led_state = 0;
```

**DARK_THRESHOLD = 400**:
- When light level drops below 400, it's considered "dark"
- LED will turn ON
- Low ADC value = dark environment

**LIGHT_THRESHOLD = 600**:
- When light level rises above 600, it's considered "bright"
- LED will turn OFF
- High ADC value = bright environment

**led_state = 0**:
- Tracks current LED state (0 = off, 1 = on)
- Used for hysteresis logic
- Prevents rapid switching

### Main Loop with Hysteresis
```c
while(1) {
    uint16_t light_level = adc_read(0);

    uart_print_string("Light: ");
    uart_print_number(light_level);
    uart_print_string("\r\n");

    // Hysteresis: prevent rapid switching at threshold
    if (led_state == 0 && light_level < DARK_THRESHOLD) {
        led_state = 1;
        PORTB |= (1 << 5);  // Turn on LED
        uart_print_string("Dark detected - LED ON\r\n");
    } else if (led_state == 1 && light_level > LIGHT_THRESHOLD) {
        led_state = 0;
        PORTB &= ~(1 << 5);  // Turn off LED
        uart_print_string("Light detected - LED OFF\r\n");
    }
    _delay_ms(500);
}
```

**Reading Light Level**:
```c
uint16_t light_level = adc_read(0);
```
- Reads LDR on A0
- Returns 0-1023
- **Low value** = dark (high LDR resistance)
- **High value** = bright (low LDR resistance)

**Hysteresis Logic**:
```c
if (led_state == 0 && light_level < DARK_THRESHOLD) {
    // LED is OFF, light level dropped below 400
    // Turn LED ON
}
else if (led_state == 1 && light_level > LIGHT_THRESHOLD) {
    // LED is ON, light level rose above 600
    // Turn LED OFF
}
```

**Why Two Different Thresholds?**
- If you used a single threshold (e.g., 500):
  - At light level 500, LED would rapidly toggle ON/OFF
  - Sensor noise around 500 would cause flickering
- With hysteresis (400 and 600):
  - LED turns ON only when light drops below 400
  - LED turns OFF only when light rises above 600
  - Gap between 400-600 prevents rapid switching
  - State only changes when crossing the appropriate threshold

**Loop Delay**:
```c
_delay_ms(500);
```
- 500ms delay between readings
- Light changes slowly, no need for faster updates
- Reduces serial output frequency

## LDR Behavior in PicsimLab

The LDR slider in PicsimLab simulates light level with resistance:

| Slider Position | LDR Resistance | Light Level | Voltage Divider Output | ADC Value |
|-----------------|----------------|-------------|----------------------|-----------|
| Slider right (low) | 0.1Ω | Bright | High (near Vcc) | High (near 1023) |
| Slider left (high) | 100kΩ | Dark | Low (near GND) | Low (near 0) |

**Important**: LDR resistance is **inversely proportional** to light:
- **Low resistance (0.1Ω)** = Bright light → High ADC value
- **High resistance (100kΩ)** = Dark → Low ADC value

This is the expected behavior for LDRs: resistance decreases as light increases.

## What is Hysteresis?

Hysteresis is a **dead zone** where the system doesn't change state:

```
LED OFF → stays OFF until light < 400
LED ON  → stays ON  until light > 600

Dead zone: 400-600 (no state change)
```

**Without Hysteresis**:
```
LED OFF → light < 500 → LED ON
LED ON  → light > 500 → LED OFF
Problem: At exactly 500, rapid toggling due to noise
```

**With Hysteresis**:
```
LED OFF → light < 400 → LED ON → stays ON until light > 600
LED ON  → light > 600 → LED OFF → stays OFF until light < 400
Benefit: No rapid toggling, stable operation
```

## Expected Simulation Behavior

### Scenario 1: Starting in Bright Light

**Initial state**: Slider at high position (e.g., 800), LED OFF

**Adjust slider left (decreasing light)**:
- Light level: 800 → 700 → 650 → 610 → 600 → 590 → 550 → 500 → 450 → 400 → 350
- At 600: Still above LIGHT_THRESHOLD, LED stays OFF
- At 400: Crosses DARK_THRESHOLD, LED turns ON
- Output: "Dark detected - LED ON"
- LED: ON

**Continue slider left**:
- Light level: 350 → 300 → 200 → 100 → 50
- Below 400: LED stays ON (state is 1)
- No change until slider moves right

### Scenario 2: Starting in Darkness

**Initial state**: Slider at low position (e.g., 200), LED ON

**Adjust slider right (increasing light)**:
- Light level: 200 → 300 → 350 → 400 → 450 → 500 → 550 → 600 → 610 → 650
- At 400: Still below LIGHT_THRESHOLD, LED stays ON
- At 600: Crosses LIGHT_THRESHOLD, LED turns OFF
- Output: "Light detected - LED OFF"
- LED: OFF

**Continue slider right**:
- Light level: 650 → 700 → 800 → 900 → 1000
- Above 600: LED stays OFF (state is 0)
- No change until slider moves left

### Scenario 3: Rapid Slider Movement Around Thresholds

**Slider moves back and forth around 500**:
- Light level: 450 → 550 → 480 → 520 → 490 → 510
- All values are in the dead zone (400-600)
- LED state does NOT change
- No rapid toggling
- Stable behavior

## State Diagram

```
                    [light < 400]
   LED OFF ───────────────────────→ LED ON
   (state=0)                         (state=1)
        ↑                                ↓
        │                    [light > 600]
        └────────────────────────────────┘
              [state changes only at thresholds]
```

## Output Examples

```
Light: 800
Light: 750
Light: 700
Light: 650
Light: 600
Light: 550
Light: 500
Light: 450
Light: 400
Dark detected - LED ON    ← LED turns ON here
Light: 350
Light: 300
Light: 350
Light: 400
Light: 450
Light: 500
Light: 550
Light: 600
Light detected - LED OFF   ← LED turns OFF here
Light: 650
Light: 700
```

## Key Concepts

- **LDR (Light Dependent Resistor)**: Resistance changes with light
- **Inverse relationship**: Low resistance = bright light, high resistance = dark
- **Voltage divider**: Converts resistance to voltage
- **ADC reading**: Measures voltage as digital value
- **Hysteresis**: Different thresholds for turning ON vs OFF
- **Dead zone**: Range where state doesn't change (400-600)
- **State memory**: System remembers current state
- **Stability**: Prevents rapid switching at threshold
- **Automatic control**: No manual intervention needed
- **Environmental sensing**: Responds to ambient light conditions
- **Threshold gap**: 200 ADC counts between thresholds (600-400)
- **Inverse proportionality**: LDR slider left = high resistance = low ADC (dark)

## Comparison with Previous Projects

| Aspect | Project 14 (Temperature) | Project 15 (Light) |
|--------|------------------------|-------------------|
| Sensor | LM35 temperature sensor | LDR light sensor |
| Sensor response | Linear (10mV/°C) | Inverse (resistance down with light) |
| Output | Serial + LEDs | Serial + LED |
| Conversion | ADC to temperature | ADC to light level |
| Units | Celsius | ADC counts (arbitrary) |
| Thresholds | Single fixed value | Two values with hysteresis |
| State management | No state memory | State memory for hysteresis |
| Stability issue | Not applicable | Hysteresis prevents flickering |

## Learnings

- LDR resistance decreases with increasing light (inverse relationship)
- LDR range: ~0.1Ω (bright) to 100kΩ+ (dark)
- Voltage divider converts resistance to voltage
- ADC reads voltage as digital value
- High ADC = bright light, low ADC = dark
- Hysteresis prevents oscillation/flickering
- Different thresholds for ON vs OFF create dead zone
- State memory is essential for hysteresis
- Automatic systems need stability considerations
- Real-world sensors have noise; hysteresis handles it
- This pattern is used in thermostats, light switches, etc.
- Dead zone width determines system sensitivity
- Too narrow dead zone = unstable behavior
- Too wide dead zone = sluggish response
- PicsimLab LDR slider: left = high resistance (dark), right = low resistance (bright)
- Helper headers enable code reuse across projects

## Troubleshooting

### LED rapidly toggles around threshold
- Hysteresis not implemented correctly
- Check that led_state is checked before switching
- Verify both thresholds are different (400 vs 600)
- Ensure state is updated when switching

### LED never turns ON
- Check DARK_THRESHOLD value (400)
- Verify ADC reading goes below 400
- Check LED pin configuration (PB5)
- Verify led_state starts at 0

### LED never turns OFF
- Check LIGHT_THRESHOLD value (600)
- Verify ADC reading goes above 600
- Check state logic (should turn off when led_state=1 and light>600)
- Verify state is updated when switching

### ADC value doesn't change with slider
- Verify LDR is connected to A0
- Check ADC is enabled (ADEN bit)
- Verify channel mask is 0x0F (lower 4 bits)
- Check LDR slider in PicsimLab is moving

### Wrong ADC behavior (high = dark instead of bright)
- This is actually correct for LDR (inverse relationship)
- Low resistance (bright) = high voltage = high ADC
- High resistance (dark) = low voltage = low ADC
- If you want opposite behavior, invert the threshold logic

### Build errors
- Ensure common/avr_io.h is accessible
- Check relative path "../common/avr_io.h"
- Verify Makefile includes common directory in include path

### No serial output
- Verify uart_init is called
- Check baud rate is 9600
- Ensure terminal baud rate matches
