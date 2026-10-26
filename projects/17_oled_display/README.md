# Project 17: OLED Display (SSD1306)

## Overview

This project demonstrates how to interface with an SSD1306 OLED display over I2C using the ATmega328P. The program initializes the display, clears it, fills it with white pixels, and writes "Hello World" text to the screen.

## Hardware

- **Display**: SSD1306 128×64 OLED
- **Interface**: I2C (TWI)
- **Pins**: SDA (PC4/A4), SCL (PC5/A5)
- **Address**: 0x78 (write address, 0x3C shifted left by 1)

## Implementation Details

### I2C Configuration

- Uses shared `common/i2c.h` helper for I2C/TWI operations
- Configured for 100 kHz at 16 MHz (TWSR = 0, TWBR = 72)
- Address format: 0x78 (write address of 0x3C << 1)

### OLED Initialization

The OLED is initialized with the following sequence:
- Display OFF → Configure → Display ON
- Horizontal addressing mode (0x20, 0x00)
- Charge pump enabled (0x8D, 0x14) - critical for powering the OLED panel
- Segment remap (0xA1) and COM scan direction (0xC8) for correct orientation
- Multiplex ratio set to 0x3F for 64 rows
- Contrast, pre-charge, and VCOMH settings for optimal display

### Display Modes

The SSD1306 supports three addressing modes:
- **Horizontal (0x00)**: Data fills columns, then wraps to next page
- **Vertical (0x01)**: Data fills pages, then wraps to next column
- **Page (0x02)**: Manual page switching required

This project uses horizontal addressing mode for simplicity.

### Screen Operations

#### Clear Display
- Sets column range (0-127) and page range (0-7)
- Writes 1024 bytes (128 columns × 8 pages) of 0x00 (black)
- **Important**: Data must be sent in chunks of 128 bytes with delays between chunks to prevent I2C buffer overflow in PicsimLab

#### Fill Display
- Same addressing as clear
- Writes 1024 bytes of 0xFF (white)
- Uses chunking (128 bytes per chunk) with 5ms delays

### Text Rendering

#### Font Format
- 8×8 pixel font for characters
- Each character is 8 bytes (one byte per column)
- Bit 0 is the top pixel, bit 7 is the bottom pixel
- Font bitmaps are stored as arrays of hex values

#### Character Drawing
- Sets window to character position using column/page addressing
- Sends 8 bytes of font data in data mode
- Characters are 8 pixels wide, spaced 8 pixels apart
- Position calculated from x (column) and y (row) coordinates

#### Hello World Display
- Text: "Hello World"
- Position: x=10, y=24 (centered vertically)
- Characters: H, e, l, o, W, r, d, space
- Custom font bitmaps for each character

## Key Lessons Learned

1. **I2C Address Format**: The SSD1306 uses a 7-bit address (0x3C) but the write address is 0x3C << 1 = 0x78. When scanning with Project 16, the device appears at 0x78.

2. **Charge Pump is Critical**: Without the charge pump command (0x8D, 0x14), the OLED panel won't power on and the screen will remain blank.

3. **Data Chunking Required**: Sending all 1024 bytes in one continuous I2C transaction fails in PicsimLab. Data must be broken into chunks (128 bytes per chunk) with delays between chunks to prevent I2C buffer overflow.

4. **Addressing Mode Selection**: Horizontal addressing mode (0x00) is the simplest for full-screen operations. It automatically handles page advancement as data is sent.

5. **Font Orientation**: Font bitmaps must match the OLED's bit orientation. Bit 0 is the top pixel and bit 7 is the bottom pixel in each column. Segment remap (0xA1) and COM scan direction (0xC8) settings affect character orientation.

6. **Command vs Data Mode**: I2C writes must be preceded by 0x00 for commands and 0x40 for display data. Mixing these causes incorrect operation.

## Compiling and Running

```bash
cd /home/erbarr/CascadeProjects/picsimlab/learning/17_oled_display
make
```

Load the resulting `main.hex` file into PicsimLab with:
- SSD1306 OLED configured for I2C mode
- SDA connected to PC4 (A4)
- SCL connected to PC5 (A5)
- Virtual terminal for UART debug output

## Expected Behavior

1. Display initializes and turns on
2. Screen clears to black (5 seconds)
3. Screen fills with white (5 seconds)
4. Screen clears to black (5 seconds)
5. "Hello World" text appears at position (10, 24)
6. Text remains on screen indefinitely

## UART Output

The program outputs the following over UART:
```
Initializing OLED...
Clearing display...
Making whole screen light up...
Clearing display...
Writing Hello World...
Done!
```

## Files

- `main.c`: Main program with OLED initialization and text rendering
- `Makefile`: Build configuration
- `main.hex`: Compiled firmware for PicsimLab
- `main.elf`: ELF build artifact
- `../common/i2c.h`: I2C/TWI helper functions
- `../common/avr_io.h`: UART helper functions
