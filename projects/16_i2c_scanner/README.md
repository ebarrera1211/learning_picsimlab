# Project 16: I2C Scanner

## Objective
Learn to implement I2C (Inter-Integrated Circuit) communication using the AVR's TWI (Two-Wire Interface), scan the I2C bus for connected devices, and understand I2C protocol fundamentals including START/STOP conditions, ACK/NACK, and device addressing.

## Hardware Required (PicsimLab)
- Arduino Uno board
- I2C device (e.g., EEPROM, RTC, OLED, or any I2C-compatible device in PicsimLab)
- Virtual terminal (accessible through PicsimLab terminal window)

## Key Concepts
- I2C (Inter-Integrated Circuit) protocol
- TWI (Two-Wire Interface) - AVR's I2C implementation
- I2C clock speed configuration (TWBR)
- START and STOP conditions
- ACK/NACK acknowledgment
- 7-bit I2C addressing
- Device scanning and discovery
- Status codes (TWSR)
- Hexadecimal number printing
- Code reuse with common/avr_io.h

## How to Build
```bash
make
```

## How to Run in PicsimLab

1. Open PicsimLab main board window (Arduino Uno)
2. Open Spare Parts window
3. Add I2C device from Spare Parts → Others (e.g., EEPROM, RTC, OLED)
4. Configure I2C device to SDA/SCL pins (typically PC5/PC4 or analog pins A4/A5)
5. In main window: File → Load Hex → select main.hex
6. Tools → Arduino Bootloader
7. Click Run button
8. Open PicsimLab terminal/serial console
9. Set terminal baud rate to 9600
10. You will see "Scanning I2C bus..." message and detected device addresses

## Code Explanation

### I2C Header
```c
#include <util/twi.h>
```
- Provides macros and constants for TWI (Two-Wire Interface)
- TWI is AVR's implementation of I2C
- Includes register definitions for TWCR, TWSR, TWBR, TWDR

### Using Helper Header
```c
#include "../common/avr_io.h"
```
- Includes UART functions from common helper
- Provides uart_init, uart_print_string, uart_print_number, uart_print_hex
- Reuses code from previous UART projects

### I2C Initialization
```c
void i2c_init(void) {
    TWSR = 0;    // No prescaler
    TWBR = 72;    // 100kHz @ 16MHz
}
```

**TWSR (TWI Status Register)**:
- Contains prescaler bits for TWI clock
- Setting to 0 disables prescaler (prescaler = 1)

**TWBR (TWI Bit Rate Register)**:
- Sets the bit rate (clock speed) for I2C communication
- Formula: SCL frequency = CPU Clock / (16 + 2 × TWBR) × Prescaler
- TWBR = 72: 16MHz / (16 + 2×72) × 1 = 16MHz / 160 = 100kHz
- 100kHz is standard I2C speed

### I2C Start Condition
```c
uint8_t i2c_start(uint8_t address) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    if ((TWSR & 0xF8) != 0x08) return 1;  // Error
    
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));

    return (TWSR & 0xF8) != 0x18;
}
```

**TWCR (TWI Control Register)**:
- `TWINT`: TWI Interrupt Flag (clears by writing 1)
- `TWSTA`: Start condition bit
- `TWEN`: TWI Enable bit

**Send START Condition**:
```c
TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
```
- Sends START condition on I2C bus
- Enables TWI
- Clears interrupt flag to start operation

**Wait for Completion**:
```c
while (!(TWCR & (1 << TWINT)));
```
- Waits for TWINT flag to be set
- Indicates START condition completed

**Check START Status**:
```c
if ((TWSR & 0xF8) != 0x08) return 1;
```
- TWSR contains status code
- Lower 5 bits are status, upper 3 bits are reserved
- `& 0xF8` masks to get lower 5 bits
- 0x08 = START transmitted successfully
- If not 0x08, return error (no device on bus)

**Send Device Address**:
```c
TWDR = address;
TWCR = (1 << TWINT) | (1 << TWEN);
```
- Writes device address to TWDR (TWI Data Register)
- Sends address on I2C bus
- Address format: 7-bit address shifted left 1 bit + R/W bit
- In scanner: we send write bit (0), so address << 1

**Check ACK/NACK**:
```c
return (TWSR & 0xF8) != 0x18;
```
- 0x18 = SLA+W transmitted, ACK received
- Device acknowledged our address
- If ACK received, return 0 (success)
- If NACK or error, return 1 (no device)

### I2C Stop Condition
```c
void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}
```

**Send STOP Condition**:
```c
TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
```
- `TWSTO`: Stop condition bit
- Sends STOP condition on I2C bus
- Releases the bus

**Wait for Stop Completion**:
```c
while (TWCR & (1 << TWSTO));
```
- Waits for TWSTO bit to clear
- Indicates STOP condition completed

### I2C Scan Function
```c
void i2c_scan(void) {
    uart_print_string("Scanning I2C bus...\r\n");

    for (uint8_t addr = 1; addr < 128; addr <<= 1) {
        if (!i2c_start(addr)) {
            uart_print_string("Found device at: 0x");
            uart_print_hex(addr);
            uart_print_string("\r\n");
        }
        i2c_stop();
    }

    uart_print_string("Scan complete.\r\n");
}
```

**Address Scanning**:
```c
for (uint8_t addr = 1; addr < 128; addr <<= 1)
```
- I2C uses 7-bit addresses (0-127)
- Scan addresses: 1, 2, 4, 8, 16, 32, 64
- `addr <<= 1` left-shifts by 1 (multiplies by 2)
- We scan even addresses (write addresses)
- Odd addresses are for read operations

**Why Start at 1?**
- Address 0 is reserved (general call address)
- Valid device addresses: 1-127

**Why Left Shift?**
- I2C protocol: 7-bit address + R/W bit
- Write address: (7-bit address << 1) | 0
- Read address: (7-bit address << 1) | 1
- Scanner uses write addresses to detect devices

**Detection Logic**:
```c
if (!i2c_start(addr)) {
    uart_print_string("Found device at: 0x");
    uart_print_hex(addr);
    uart_print_string("\r\n");
}
```
- If i2c_start returns 0 (success), device exists
- Prints address in hex format
- Example: "Found device at: 0x50"

**Stop After Each Attempt**:
```c
i2c_stop();
```
- Sends STOP condition after each address
- Releases bus for next scan
- Clears bus state

### Hex Printing Function (in common/avr_io.h)
```c
void uart_print_hex(uint8_t num) {
    char hex[] = "0123456789ABCDEF";
    uart_transmit(hex[num >> 4]);
    uart_transmit(hex[num & 0x0F]);
}
```

**Hex Lookup Table**:
- Array maps nibble values to hex characters
- Example: 10 → 'A', 15 → 'F'

**High Nibble**:
```c
uart_transmit(hex[num >> 4]);
```
- Shifts right by 4 to get upper 4 bits
- Example: 0xA5 >> 4 = 0xA (10) → 'A'

**Low Nibble**:
```c
uart_transmit(hex[num & 0x0F]);
```
- Masks with 0x0F to get lower 4 bits
- Example: 0xA5 & 0x0F = 0x5 (5) → '5'

**Example**: num = 0xA5
- High nibble: 0xA → 'A'
- Low nibble: 0x5 → '5'
- Output: "A5"

### Main Function
```c
int main(void) {
    uart_init();
    i2c_init();

    while(1) {
        i2c_scan();
        _delay_ms(5000);
    }

    return 0;
}
```

**Initialization**:
- UART for serial output
- I2C for device scanning

**Continuous Scanning**:
```c
while(1) {
    i2c_scan();
    _delay_ms(5000);
}
```
- Scans I2C bus every 5 seconds
- Allows for hot-plug detection
- Useful for debugging dynamic connections

## I2C Protocol Overview

**Communication Sequence**:
1. START condition
2. Send device address (7-bit + R/W bit)
3. Wait for ACK/NACK
4. Send data (if ACK)
5. Wait for ACK/NACK
6. STOP condition

**Address Format**:
- 7-bit device address (0-127)
- Plus R/W bit (0=write, 1=read)
- Combined as 8-bit byte

**Scanner Behavior**:
- Sends START + address + STOP for each address
- If ACK received → device exists at that address
- If NACK received → no device at that address

## Expected Output

**No Devices Connected**:
```
Scanning I2C bus...
Scan complete.
```

**Device at 0x50**:
```
Scanning I2C bus...
Found device at: 0x50
Scan complete.
```

**Multiple Devices**:
```
Scanning I2C bus...
Found device at: 0x50
Found device at: 0x68
Scan complete.
```

## I2C Device Address Reference

Common I2C device addresses:

| Address | Device Type | Example |
|---------|-------------|---------|
| 0x50 | EEPROM | 24LCxx series |
| 0x57 | EEPROM | 24AA02 |
| 0x68 | RTC | DS1307, DS3231 |
| 0x3C | OLED | SSD1306 |
| 0x48 | Temp Sensor | TMP102 |
| 0x68 | IMU | MPU6050 |
| 0x5C | Pressure Sensor | BMP280 |
| 0x40 | Temperature Sensor | HDC1080 |

Note: The scanner only detects addresses, not device types. Device identification requires either documentation lookup or reading device-specific registers.

## Key Concepts

- **I2C (Inter-Integrated Circuit)**: Serial communication protocol
- **TWI (Two-Wire Interface)**: AVR's I2C implementation
- **I2C clock speed**: 100kHz standard, set by TWBR
- **START condition**: Begins I2C transaction
- **STOP condition**: Ends I2C transaction
- **ACK/NACK**: Acknowledge from device
- **7-bit addressing**: I2C uses 7-bit device addresses
- **Address scanning**: Try each address to find devices
- **Status codes**: TWSR indicates operation success/failure
- **Bit rate register**: Controls TWI clock frequency
- **Hex printing**: Display addresses in hexadecimal
- **Two-wire bus**: SDA (data) and SCL (clock) lines
- **Multi-drop**: Multiple devices share same bus
- **Code reuse**: common/avr_io.h for UART functions

## Learnings

- I2C is a multi-drop serial protocol (multiple devices on same bus)
- Two wires: SDA (data) and SCL (clock)
- Each device has unique 7-bit address
- START/STOP conditions frame transactions
- ACK indicates device responded, NACK means no device
- Status codes indicate operation state
- Scanner is useful for device discovery
- TWBR controls I2C clock speed
- 100kHz is standard I2C speed
- Addresses 0 reserved for general call
- Scanner uses write addresses (even numbers)
- I2C doesn't automatically identify device types
- Device identification requires documentation or register reading
- This is foundational for I2C device communication (EEPROMs, sensors, displays)
- TWI is AVR's hardware implementation of I2C
- TWI registers: TWCR (control), TWSR (status), TWBR (bit rate), TWDR (data)

## Comparison with Previous Projects

| Aspect | Project 15 (LDR) | Project 16 (I2C) |
|--------|----------------|----------------|
| Sensor type | Analog (LDR) | Digital (I2C) |
| Communication | ADC reading | Serial protocol |
| Output | Single LED | Serial terminal |
| Protocol | None | I2C/TWI |
| Addressing | Single pin | 7-bit address |
| Discovery | Not applicable | Scan for devices |
| Status checking | None | ACK/NACK checking |
| Registers | ADC, PORT | TWCR, TWSR, TWBR, TWDR |

## Troubleshooting

### No devices detected
- Verify I2C device is connected to correct pins (SDA/SCL)
- Check I2C device is powered
- Verify I2C device address is in range 1-127
- Check device is configured correctly in PicsimLab

### Wrong clock speed
- Verify TWBR = 72 for 100kHz at 16MHz
- Check CPU clock is 16MHz
- Formula: SCL = 16MHz / (16 + 2×TWBR)

### Always returns error
- Check TWSR status codes
- Verify START condition is successful (0x08)
- Check ACK received (0x18)
- Verify I2C is enabled (TWEN bit)

### Empty lines in output
- May be caused by I2C bus state not resetting
- Try resetting TWI between scans
- Check for address collisions

### Build errors
- Ensure common/avr_io.h is accessible
- Check relative path "../common/avr_io.h"
- Verify Makefile includes common directory in include path
- Ensure util/twi.h is available

### Hex printing issues
- Verify uart_print_hex function in common/avr_io.h
- Check hex lookup table is correct
- Verify bit shifting and masking

### No serial output
- Verify uart_init is called
- Check baud rate is 9600
- Ensure terminal baud rate matches
