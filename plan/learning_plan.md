---
agent: devin-local
session: nostalgic-basketball
created: 2026-10-06T18:46:41Z
---
# PicsimLab Embedded C Learning Plan

A comprehensive step-by-step curriculum for learning embedded C programming using PicsimLab with Arduino Uno, starting from basics and progressing to advanced concepts, including implementation roadmap for setting up and executing the learning journey.

# PicsimLab Embedded C Learning Plan

A comprehensive step-by-step curriculum for learning embedded C programming using PicsimLab with Arduino Uno, starting from basics and progressing to advanced concepts.

## Overview
A structured curriculum for learning embedded C programming from scratch using PicsimLab's Arduino Uno simulator. The plan progresses from fundamental concepts to advanced peripherals, building practical skills through hands-on exercises.

## Prerequisites Check
- ✅ PicsimLab installed (in progress)
- ✅ avr-gcc installed (via deps script)
- ✅ Make and gdb available
- ✅ Nano/gedit editors available
- ⏳ Arduino IDE (optional, can install if desired)

## Learning Philosophy
**Step-by-step basics approach**: Each concept builds on the previous one. We'll start with GPIO fundamentals and gradually add complexity, ensuring mastery before moving forward.

**Direct C approach**: Using avr-gcc directly (not Arduino IDE) to learn true embedded C fundamentals, register manipulation, and hardware-specific programming.

---

## Implementation Roadmap

This section details the concrete steps to set up and execute the learning curriculum.

### Step 1: Complete PicsimLab Installation
**Status**: In progress (user running dependency installation)

**Remaining Steps**:
1. Wait for `./bscripts/install_deps.sh` to complete
2. Run `./bscripts/build_all_and_install.sh` to build and install PicsimLab
3. Verify installation: `picsimlab --version` or run `picsimlab` directly
4. Test GUI launches successfully
5. Close PicsimLab after verification

**Expected Outcome**: Fully functional PicsimLab installation with all dependencies

---

### Step 2: Create Learning Directory Structure
**Command**: `mkdir -p ~/CascadeProjects/picsimlab-learning/{01_blink,02_multiple_leds,03_button,04_traffic_light,05_timer_blink,06_pwm,07_external_interrupt,08_timer_interrupt,09_uart_basics,10_serial_commands,11_serial_logging,12_potentiometer,13_analog_led,14_temp_sensor,15_light_sensor,16_i2c_scanner,17_oled_display,18_rtc,19_dht_sensor,20_7segment,21_sdcard,22_accelerometer,common,docs}`

**Structure to Create**:
```
~/CascadeProjects/picsimlab-learning/
├── 01_blink/
├── 02_multiple_leds/
├── 03_button/
├── 04_traffic_light/
├── 05_timer_blink/
├── 06_pwm/
├── 07_external_interrupt/
├── 08_timer_interrupt/
├── 09_uart_basics/
├── 10_serial_commands/
├── 11_serial_logging/
├── 12_potentiometer/
├── 13_analog_led/
├── 14_temp_sensor/
├── 15_light_sensor/
├── 16_i2c_scanner/
├── 17_oled_display/
├── 18_rtc/
├── 19_dht_sensor/
├── 20_7segment/
├── 21_sdcard/
├── 22_accelerometer/
├── common/
└── docs/
```

---

### Step 3: Create Common Files

#### 3.1 Create Makefile Template
**File**: `~/CascadeProjects/picsimlab-learning/common/Makefile.template`

**Content**:
```makefile
MCU = atmega328p
F_CPU = 16000000UL
BAUD = 9600
PORT = /dev/ttyUSB0

CC = avr-gcc
OBJCOPY = avr-objcopy
OBJDUMP = avr-objdump
SIZE = avr-size

CFLAGS = -mmcu=$(MCU) -DF_CPU=$(F_CPU) -Os -Wall -Wextra

TARGET = main

SRC = main.c
OBJ = $(SRC:.c=.o)

all: $(TARGET).hex

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET).elf: $(OBJ)
	$(CC) $(CFLAGS) $(OBJ) -o $@

$(TARGET).hex: $(TARGET).elf
	$(OBJCOPY) -O ihex -R .eeprom $< $@
	$(SIZE) --format=avr --mcu=$(MCU) $<

clean:
	rm -f $(OBJ) $(TARGET).elf $(TARGET).hex

.PHONY: all clean
```

#### 3.2 Create AVR I/O Helper Header
**File**: `~/CascadeProjects/picsimlab-learning/common/avr_io.h`

**Content**:
```c
#ifndef AVR_IO_H
#define AVR_IO_H

#include <avr/io.h>
#include <util/delay.h>

// Bit manipulation macros
#define SET_BIT(port, bit)   (port |= (1 << bit))
#define CLEAR_BIT(port, bit) (port &= ~(1 << bit))
#define TOGGLE_BIT(port, bit) (port ^= (1 << bit))
#define READ_BIT(port, bit)  ((port >> bit) & 1)

// Common pin definitions for Arduino Uno
#define LED_PIN      5  // PB5 (D13)
#define BUTTON_PIN   2  // PD2 (D2)

#endif // AVR_IO_H
```

#### 3.3 Create Learning Journal Template
**File**: `~/CascadeProjects/picsimlab-learning/docs/learning_journal.md`

**Content**:
```markdown
# PicsimLab Learning Journal

## Session Log

### Session 1 - [Date]
**Goal**: [What you planned to accomplish]
**Completed**: [What you actually did]
**Challenges**: [Problems encountered]
**Solutions**: [How you solved them]
**Learnings**: [Key takeaways]
**Next Steps**: [What to do next]

---

## Project Notes

### Project 1: Blink LED
- Started: [Date]
- Completed: [Date]
- Key concepts learned:
- Difficulties faced:
- Code improvements made:

### Project 2: Multiple LEDs
- Started: [Date]
- Completed: [Date]
- Key concepts learned:
- Difficulties faced:
- Code improvements made:

---

## Reference Notes

### Register Quick Reference
- DDRx: Data Direction Register (1=output, 0=input)
- PORTx: Output register (1=high, 0=low) / Pull-up enable
- PINx: Input register (read pin state)

### Common Patterns
- Button debouncing
- Timer initialization
- Interrupt setup

---

## Resources
- ATmega328P Datasheet: [Link]
- PicsimLab Docs: [Link]
- AVR Libc Manual: [Link]
```

---

### Step 4: Initialize First Project (Blink LED)

#### 4.1 Create Project Directory
**Already done in Step 2**: `~/CascadeProjects/picsimlab-learning/01_blink/`

#### 4.2 Create Makefile
**File**: `~/CascadeProjects/picsimlab-learning/01_blink/Makefile`

**Content**: Copy from `common/Makefile.template`

#### 4.3 Create Main C File
**File**: `~/CascadeProjects/picsimlab-learning/01_blink/main.c`

**Content**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN 5  // PB5 (Arduino D13)

int main(void) {
    // Set LED pin as output
    DDRB |= (1 << LED_PIN);
    
    while (1) {
        // Turn LED on
        PORTB |= (1 << LED_PIN);
        _delay_ms(500);
        
        // Turn LED off
        PORTB &= ~(1 << LED_PIN);
        _delay_ms(500);
    }
    
    return 0;
}
```

#### 4.4 Create README
**File**: `~/CascadeProjects/picsimlab-learning/01_blink/README.md`

**Content**:
```markdown
# Project 1: Blink LED

## Objective
Learn to control GPIO pins by blinking an LED.

## Hardware Required (PicsimLab)
- Arduino Uno board
- LED (output_LEDs part)

## Key Concepts
- DDR register (Data Direction Register)
- PORT register (Output)
- Delay functions
- Infinite loop structure

## How to Build
To build the project, run:
```bash
make
```

## How to Run in PicsimLab
1. Open PicsimLab (two windows will open: main board and Spare Parts)
2. In Spare Parts window, go to Outputs → LEDs
3. Right-click an LED (e.g., D1) → Properties → Select PB5/13
4. In main window, File → Load Hex → select main.hex
5. **CRITICAL**: Tools → Arduino Bootloader (required for firmware to execute)
6. Click Run button (green triangle)
7. LED should blink at 1 Hz

## Code Explanation
- `DDRB |= (1 << LED_PIN)` - Sets PB5 as output
- `PORTB |= (1 << LED_PIN)` - Sets PB5 high (LED on)
- `PORTB &= ~(1 << LED_PIN)` - Sets PB5 low (LED off)
- `_delay_ms(500)` - 500ms delay


### Step 5: Test Compilation
**Command**: `cd ~/CascadeProjects/picsimlab-learning/01_blink && make`

**Expected Output**:
```
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -Wall -Wextra -c main.c -o main.o
avr-gcc -mmcu=atmega328p -DF_CPU=16000000UL -Os -Wall -Wextra main.o -o main.elf
avr-objcopy -O ihex -R .eeprom main.elf main.hex
avr-size --format=avr --mcu=atmega328p main.elf
AVR Memory Usage
----------------
Device: atmega328p
Program:     [bytes] bytes ([percent]% Full)
(.text + .data + .bootloader)
Data:        [bytes] bytes ([percent]% Full)
```

**Verification**: Check that `main.hex` file was created

---

### Step 6: PicsimLab First Run Workflow

#### 6.1 Launch PicsimLab
**Command**: `picsimlab`

**What you'll see**:
- Two windows: Main board window (Arduino Uno - atmega328p) and Spare Parts window
- Main window menus: File, Board, Microcontroller, Modules, Tools, Code, Help
- Spare Parts window menus: File, Edit, Inputs, Outputs, Others, Virtual, Help

#### 6.2 Configure LED in Spare Parts
1. In the **Spare Parts window**, click **Outputs**
2. Look for **LEDs** (or "output_LEDs")
3. Right-click on one of the LEDs (e.g., D1)
4. Select **Properties**
5. From the dropdown, select **PB5/13** as the signal
6. Click **OK**

**Note**: PB5/13 corresponds to Arduino pin D13 (the built-in LED pin)

#### 6.3 Load Firmware
1. In the **main window**, click **File** → **Load Hex**
2. Navigate to `~/CascadeProjects/picsimlab-learning/01_blink/main.hex`
3. Select and open

#### 6.4 Initialize Arduino Bootloader (CRITICAL STEP)
1. In the **main window**, click **Tools** → **Arduino Bootloader**
2. This initializes the Arduino bootloader, allowing the firmware to execute
3. **Without this step, the firmware loads but doesn't run**

#### 6.5 Run Simulation
1. Click the **Run** button (green triangle/play icon)
2. Observe the LED blinking in the Spare Parts window
3. The LED should blink at 1 Hz (500ms on, 500ms off)

#### 6.6 Save Workspace
1. Click **File** → **Save Workspace**
2. Save as `~/CascadeProjects/picsimlab-learning/01_blink/blink.pzw`

---

### Step 7: Establish Workflow Script

Create a helper script to streamline project setup:
**File**: `~/CascadeProjects/picsimlab-learning/common/new_project.sh`

**Usage**: Takes project number and name as separate arguments, combines them with underscore (e.g., `02 blink` → `02_blink`)

**Content**:
```bash
#!/bin/bash
# Usage: ./new_project.sh project_number project_name
# Example: ./new_project.sh 02 blink  (creates directory 02_blink)

if [ $# -ne 2 ]; then
    echo "Usage: $0 project_number project_name"
    echo "Example: $0 02 blink"
    exit 1
fi

PROJECT_NUM="$1"
PROJECT_NAME="$2"
# Convert project name to lowercase for directory name
PROJECT_NAME_LOWER=$(echo "$PROJECT_NAME" | tr '[:upper:]' '[:lower:]')
PROJECT_DIR="${PROJECT_NUM}_${PROJECT_NAME_LOWER}"

# Check if project number is already taken
if ls -d ${PROJECT_NUM}_* 1> /dev/null 2>&1; then
    echo "Error: Project number '$PROJECT_NUM' is already taken"
    echo "Existing project: $(ls -d ${PROJECT_NUM}_*)"
    echo "Please use a different project number or remove the existing project first."
    exit 1
fi

# Check if project name is already taken (by any project number)
if ls -d *_${PROJECT_NAME_LOWER} 1> /dev/null 2>&1; then
    echo "Error: Project name '$PROJECT_NAME' is already taken"
    echo "Existing project: $(ls -d *_${PROJECT_NAME_LOWER})"
    echo "Please use a different project name or remove the existing project first."
    exit 1
fi

# Create directory if it doesn't exist
mkdir -p "$PROJECT_DIR"

# Copy Makefile template
cp common/Makefile.template "$PROJECT_DIR/Makefile"

# Create main.c
cat > "$PROJECT_DIR/main.c" << EOF
#include <avr/io.h>
#include <util/delay.h>
#include "../common/avr_io.h"

int main(void) {
    // TODO: Initialize hardware
    
    while (1) {
        // TODO: Main loop
    }
    
    return 0;
}
EOF

# Create README
cat > "$PROJECT_DIR/README.md" << EOF
# Project: $PROJECT_NAME
# Note: Directory name is lowercase: $PROJECT_DIR

## Objective
[Describe the project goal]

## Hardware Required (PicsimLab)
- [List required parts]

## Key Concepts
- [List key concepts to learn]

## How to Build
To build the project, run:
make

## How to Run in PicsimLab
1. [Step 1]
2. [Step 2]
3. [Step 3]

## Code Explanation
[Explain key code sections]

## Learnings
[Document what you learned]
EOF

echo "Project '$PROJECT_NAME' created in directory '$PROJECT_DIR'"
echo "Next steps:"
echo "  1. Edit $PROJECT_DIR/main.c"
echo "  2. Run 'cd $PROJECT_DIR && make'"
echo "  3. Load hex file in PicsimLab"
```

**Make executable**: `chmod +x ~/CascadeProjects/picsimlab-learning/common/new_project.sh`

---

### Step 8: Create Progress Tracker

**File**: `~/CascadeProjects/picsimlab-learning/docs/progress.md`

**Content**:
```markdown
# Learning Progress Tracker

## Phase 1: Environment Setup & Basics
- [x] Clone PicsimLab repository
- [ ] Complete dependency installation
- [ ] Build and install PicsimLab
- [ ] Test PicsimLab launch
- [ ] Create learning directory structure
- [ ] Create common files (Makefile, helpers)
- [ ] Initialize first project (blink)
- [ ] Test compilation
- [ ] Load and run in PicsimLab
- [ ] Save workspace

## Phase 2: GPIO Fundamentals
- [ ] Project 1: Blink LED
- [ ] Project 2: Multiple LEDs
- [ ] Project 3: Read Push Button
- [ ] Project 4: Traffic Light Simulation

## Phase 3: Timers & Interrupts
- [ ] Project 5: Timer-Based Blink
- [ ] Project 6: PWM
- [ ] Project 7: External Interrupts
- [ ] Project 8: Timer Interrupts

## Phase 4: Serial Communication
- [ ] Project 9: UART Basics
- [ ] Project 10: Serial Commands
- [ ] Project 11: Serial Data Logging

## Phase 5: Analog Input
- [ ] Project 12: Potentiometer Reading
- [ ] Project 13: Analog LED Control
- [ ] Project 14: Temperature Sensor (LM35)
- [ ] Project 15: Light Sensor (LDR)

## Phase 6: I2C Communication
- [ ] Project 16: I2C Scanner
- [ ] Project 17: OLED Display (SSD1306)
- [ ] Project 18: Real-Time Clock (DS1307)
- [ ] Project 19: Temperature/Humidity (DHT11/DHT22)

## Phase 7: SPI Communication
- [ ] Project 20: 7-Segment Display (MAX7219)
- [ ] Project 21: SD Card Module
- [ ] Project 22: Accelerometer (ADXL345)

## Phase 8: Advanced Projects
- [ ] Select advanced project(s)
- [ ] Complete advanced project 1
- [ ] Complete advanced project 2
- [ ] Complete advanced project 3 (optional)

## Phase 9: Real Hardware Transition (Optional)
- [ ] Acquire Arduino Uno board
- [ ] Set up physical breadboard
- [ ] Port projects to real hardware
- [ ] Debug with real hardware

---

## Statistics
- Total Projects: 22
- Completed: 0
- In Progress: 0
- Remaining: 22
- Completion: 0%
```

---

### Step 9: Create Quick Reference Guide

**File**: `~/CascadeProjects/picsimlab-learning/docs/quick_reference.md`

**Content**:
```markdown
# PicsimLab Quick Reference

## Common Commands

### Build project
To build a project:
```bash
cd ~/CascadeProjects/picsimlab-learning/[project_name]
make
```

### Clean build artifacts
To clean build files:
```bash
make clean
```

### Launch PicsimLab
To start PicsimLab:
```bash
picsimlab
```

### Create new project
To create a new project:
```bash
cd ~/CascadeProjects/picsimlab-learning
./common/new_project.sh [project_number] [project_name]
# Example: ./common/new_project.sh 02 blink (creates directory 02_blink)
```

## PicsimLab Workflow

1. **Create/Edit Code**: Edit `main.c` in project directory
2. **Build**: Run `make` to compile
3. **Load in PicsimLab**: File → Load Hex → select `main.hex`
4. **Initialize Bootloader**: Tools → Arduino Bootloader (CRITICAL for Arduino Uno)
5. **Run**: Click Run button
6. **Debug**: Use oscilloscope, virtual terminal, or pause simulation
7. **Save Workspace**: File → Save Workspace → save as `.pzw` file

## Common Pin Mappings (Arduino Uno)

| Port | Pin | Arduino Pin | Function |
|------|-----|-------------|----------|
| PB5  | 5   | D13         | LED (built-in) |
| PD2  | 2   | D2          | External Interrupt 0 |
| PD3  | 3   | D3          | External Interrupt 1 |
| PC0  | 0   | A0          | ADC0 |
| PC1  | 1   | A1          | ADC1 |
| PD0  | 0   | D0          | RX |
| PD1  | 1   | D1          | TX |

## Register Quick Reference

### GPIO
- `DDRx`: Data Direction (1=output, 0=input)
- `PORTx`: Output (1=high, 0=low) / Pull-up (if input)
- `PINx`: Input register

### Timer0 (8-bit)
- `TCCR0B`: Timer control register
- `TCNT0`: Timer counter
- `OCR0A`: Output compare A

### Timer1 (16-bit)
- `TCCR1B`: Timer control register
- `TCNT1`: Timer counter (high/low)
- `OCR1A`: Output compare A

### ADC
- `ADMUX`: ADC multiplexer
- `ADCSRA`: ADC control and status
- `ADCH/ADCL`: ADC data registers

### UART
- `UBRR0H/L`: Baud rate
- `UCSR0B`: Control register
- `UDR0`: Data register

## Bit Manipulation Macros

```c
#define SET_BIT(port, bit)   (port |= (1 << bit))
#define CLEAR_BIT(port, bit) (port &= ~(1 << bit))
#define TOGGLE_BIT(port, bit) (port ^= (1 << bit))
#define READ_BIT(port, bit)  ((port >> bit) & 1)
```

## Troubleshooting

### Compilation errors
- Check syntax in main.c
- Verify avr-gcc is installed: `avr-gcc --version`
- Check Makefile is correctly configured

### PicsimLab won't load hex
- Verify .hex file exists
- Check file path is correct
- Ensure compilation succeeded

### Simulation not working
- Check parts are connected correctly
- Verify pin assignments
- Check if simulation is running (not paused)
- Review code logic

### LED not blinking
- Verify DDR register set correctly
- Check PORT register writes
- Ensure delay functions work
- Verify pin assignment in PicsimLab
- **CRITICAL**: Ensure Arduino Bootloader is initialized (Tools → Arduino Bootloader)
- Without bootloader, firmware loads but doesn't execute

---

### Step 10: Initial Learning Session Planning

**First Session Goals** (after PicsimLab installation completes):
1. Launch PicsimLab and explore GUI (15 min)
2. Load Arduino Uno board (5 min)
3. Add LED part and configure (10 min)
4. Load pre-compiled blink.hex (5 min)
5. Run simulation and observe (5 min)
6. Review the blink code (15 min)
7. Modify delay time and recompile (10 min)
8. Test modified version (5 min)
9. Document learnings in journal (10 min)

**Total time**: ~80 minutes

---

## Execution Checklist

Use this checklist to track implementation of the roadmap:

### Environment Setup
- [ ] PicsimLab dependencies installed
- [ ] PicsimLab built and installed
- [ ] PicsimLab launches successfully
- [ ] Learning directory structure created
- [ ] Common files created (Makefile template, avr_io.h)
- [ ] Learning journal template created
- [ ] Progress tracker created
- [ ] Quick reference guide created
- [ ] new_project.sh script created and made executable

### First Project Setup
- [ ] 01_blink directory created
- [ ] Makefile copied to 01_blink
- [ ] main.c created with blink code
- [ ] README.md created for 01_blink
- [ ] Compilation tested successfully
- [ ] main.hex generated

### PicsimLab Testing
- [ ] PicsimLab launched
- [ ] Arduino Uno board loaded
- [ ] LED part added and configured
- [ ] blink.hex loaded successfully
- [ ] Simulation runs and LED blinks
- [ ] Workspace saved as blink.pzw

### Documentation
- [ ] First session logged in learning journal
- [ ] Progress tracker updated
- [ ] Any issues documented

---

## Weekly Workflow Template

After initial setup, follow this weekly routine:

### Daily (1-2 hours)
1. **Review previous day's notes** (5 min)
2. **Set daily goal** (5 min)
3. **Work on current project** (45-90 min)
4. **Test in PicsimLab** (10 min)
5. **Document learnings** (10 min)
6. **Update progress tracker** (5 min)

### Weekly (Sunday)
1. **Review week's progress** (15 min)
2. **Plan next week's goals** (15 min)
3. **Update learning journal summary** (10 min)
4. **Backup project files** (5 min)
5. **Review challenging concepts** (15 min)

---

## Success Metrics

Track these metrics to ensure progress:

### Quantitative
- Number of projects completed
- Lines of code written
- Hours spent learning
- Compilation success rate
- Bugs found and fixed

### Qualitative
- Understanding of register-level programming
- Ability to debug without external help
- Confidence in adding new peripherals
- Ability to read datasheets independently
- Capability to design simple projects

---

## Next Immediate Actions (After Plan Approval)

1. **Wait for PicsimLab installation to complete** (user action)
2. **Run `./bscripts/build_all_and_install.sh`** (user action with sudo)
3. **Verify PicsimLab installation** (command: `picsimlab --version`)
4. **Create learning directory structure** (run mkdir command)
5. **Create common files** (Makefile template, avr_io.h, etc.)
6. **Initialize first project** (01_blink)
7. **Test compilation**
8. **Launch PicsimLab and test blink**
9. **Document first session**

---

## Curriculum Phases (Detailed)

The following sections detail the actual learning curriculum content.

---

## Phase 1: Environment Setup & Basics (Week 1)

### Objective
Set up development environment and understand PicsimLab basics.

### Tasks
1. **Complete PicsimLab Installation**
   - Finish dependency installation
   - Run `./bscripts/build_all_and_install.sh`
   - Verify PicsimLab launches successfully
   - Explore the GUI interface

2. **Understand the Toolchain**
   - Learn about avr-gcc compiler
   - Understand makefiles for AVR projects
   - Set up project directory structure
   - Create a template Makefile for Arduino Uno projects

3. **PicsimLab Basics**
   - Load the Arduino Uno board in PicsimLab
   - Explore the parts library (LEDs, buttons, etc.)
   - Understand how to load firmware (.hex files)
   - Learn to use the oscilloscope and virtual terminal

### Resources
- PicsimLab documentation: https://lcgamboa.github.io/picsimlab_docs/stable/
- AVR-GCC documentation
- ATmega328P datasheet (for reference)

---

## Phase 2: GPIO Fundamentals (Week 2)

### Objective
Master basic input/output operations.

### Learning Projects

#### Project 1: Blink LED
- Learn DDR registers (Data Direction Register)
- Understand PORT registers (output)
- Write first embedded C program
- Compile with avr-gcc
- Load and test in PicsimLab
- Experiment with different timing

#### Project 2: Multiple LEDs
- Use multiple GPIO pins
- Create LED patterns (running lights, binary counter)
- Learn bit manipulation operations (AND, OR, XOR, shift)
- Optimize code size

**Detailed Implementation**:

**Objective**: Control multiple LEDs to create patterns and understand bit manipulation.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- 4 LEDs (from Spare Parts → Outputs → LEDs)
- Configure LEDs to: PB0/8, PB1/9, PB2/10, PB3/11

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define LED0 0  // PB0
#define LED1 1  // PB1
#define LED2 2  // PB2
#define LED3 3  // PB3

int main(void) {
    // Set all LED pins as output
    DDRB |= (1 << LED0) | (1 << LED1) | (1 << LED2) | (1 << LED3);
    
    while (1) {
        // Pattern 1: Running light (LED0 → LED1 → LED2 → LED3)
        PORTB = (1 << LED0);
        _delay_ms(200);
        PORTB = (1 << LED1);
        _delay_ms(200);
        PORTB = (1 << LED2);
        _delay_ms(200);
        PORTB = (1 << LED3);
        _delay_ms(200);
        
        // Pattern 2: Binary counter (0-15)
        for (uint8_t i = 0; i < 16; i++) {
            PORTB = i;
            _delay_ms(500);
        }
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add 4 LEDs from Spare Parts → Outputs
3. Configure LEDs:
   - LED D1 → PB0/8
   - LED D2 → PB1/9
   - LED D3 → PB2/10
   - LED D4 → PB3/11
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation

**Key Learnings**:
- Setting multiple bits at once using OR
- Direct register assignment (PORTB = value)
- Understanding binary representation
- Pattern generation using loops

#### Project 3: Read Push Button
- Learn PIN registers (input)
- Understand pull-up/pull-down resistors
- Implement button debouncing in software
- Create LED controlled by button

**Detailed Implementation**:

**Objective**: Read button input and control an LED, implementing debouncing.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- 1 LED (PB5/13)
- 1 Push button (Spare Parts → Inputs → Push Buttons or Switches)
- Configure button to: PD2/2

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN  5  // PB5
#define BUTTON_PIN 2  // PD2

// Simple debounce delay (not production-quality, but good for learning)
#define DEBOUNCE_DELAY 50

int main(void) {
    // Set LED pin as output
    DDRB |= (1 << LED_PIN);
    
    // Set button pin as input with pull-up resistor
    DDRD &= ~(1 << BUTTON_PIN);
    PORTD |= (1 << BUTTON_PIN);  // Enable pull-up
    
    uint8_t last_button_state = 1;  // Pull-up means idle = 1
    uint8_t led_state = 0;
    
    while (1) {
        uint8_t current_button_state = (PIND >> BUTTON_PIN) & 1;
        
        // Check for button press (low = pressed with pull-up)
        if (current_button_state == 0 && last_button_state == 1) {
            _delay_ms(DEBOUNCE_DELAY);  // Debounce delay
            
            // Check again after debounce
            if (((PIND >> BUTTON_PIN) & 1) == 0) {
                led_state = !led_state;  // Toggle LED state
                
                if (led_state) {
                    PORTB |= (1 << LED_PIN);  // LED on
                } else {
                    PORTB &= ~(1 << LED_PIN);  // LED off
                }
            }
        }
        
        last_button_state = current_button_state;
        _delay_ms(10);  // Small delay to prevent rapid checking
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add LED from Spare Parts → Outputs
3. Configure LED → PB5/13
4. Add push button from Spare Parts → Inputs
5. Configure button → PD2/2
6. Load hex file
7. Tools → Arduino Bootloader
8. Run simulation

**Key Learnings**:
- PIN register for reading input
- Pull-up resistors (PORT bit set when DDR is input)
- Button press detection (active-low with pull-up)
- Software debouncing
- State management

#### Project 4: Traffic Light Simulation
- Combine multiple LEDs and timing
- Implement state machine logic
- Use delays and counters
- Add pedestrian button interaction

**Detailed Implementation**:

**Objective**: Implement a traffic light controller with pedestrian button using state machines.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- 3 LEDs for traffic lights (Red, Yellow, Green)
- 1 LED for pedestrian walk signal
- 1 Push button for pedestrian request
- Configure LEDs: PB0/8 (Red), PB1/9 (Yellow), PB2/10 (Green), PB3/11 (Walk)
- Configure button: PD2/2

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define RED_PIN     0  // PB0
#define YELLOW_PIN  1  // PB1
#define GREEN_PIN   2  // PB2
#define WALK_PIN    3  // PB3
#define BUTTON_PIN  2  // PD2

typedef enum {
    STATE_GREEN,
    STATE_YELLOW,
    STATE_RED,
    STATE_WALK
} TrafficState;

int main(void) {
    // Set LED pins as output
    DDRB |= (1 << RED_PIN) | (1 << YELLOW_PIN) | (1 << GREEN_PIN) | (1 << WALK_PIN);
    
    // Set button pin as input with pull-up
    DDRD &= ~(1 << BUTTON_PIN);
    PORTD |= (1 << BUTTON_PIN);
    
    TrafficState state = STATE_GREEN;
    uint8_t pedestrian_request = 0;
    
    while (1) {
        // Check for pedestrian button press
        if (((PIND >> BUTTON_PIN) & 1) == 0) {
            _delay_ms(50);  // Debounce
            if (((PIND >> BUTTON_PIN) & 1) == 0) {
                pedestrian_request = 1;
            }
        }
        
        // State machine
        switch (state) {
            case STATE_GREEN:
                PORTB = (1 << GREEN_PIN);  // Green on, others off
                _delay_ms(5000);
                state = STATE_YELLOW;
                break;
                
            case STATE_YELLOW:
                PORTB = (1 << YELLOW_PIN);  // Yellow on
                _delay_ms(2000);
                state = STATE_RED;
                break;
                
            case STATE_RED:
                PORTB = (1 << RED_PIN);  // Red on
                _delay_ms(3000);
                
                if (pedestrian_request) {
                    state = STATE_WALK;
                    pedestrian_request = 0;
                } else {
                    state = STATE_GREEN;
                }
                break;
                
            case STATE_WALK:
                PORTB = (1 << WALK_PIN);  // Walk signal on
                _delay_ms(5000);
                state = STATE_GREEN;
                break;
        }
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add 4 LEDs from Spare Parts → Outputs
3. Configure LEDs:
   - LED D1 → PB0/8 (Red)
   - LED D2 → PB1/9 (Yellow)
   - LED D3 → PB2/10 (Green)
   - LED D4 → PB3/11 (Walk)
4. Add push button from Spare Parts → Inputs
5. Configure button → PD2/2
6. Load hex file
7. Tools → Arduino Bootloader
8. Run simulation

**Key Learnings**:
- State machine design
- Enum for state management
- Event-driven programming (button requests)
- Timing coordination between states

### Key Concepts
- GPIO pin configuration
- Register-level programming
- Bit manipulation
- Timing and delays
- Simple state machines

---

## Phase 3: Timers & Interrupts (Week 3)

### Objective
Move from blocking delays to efficient timer-based programming.

### Learning Projects

#### Project 5: Timer-Based Blink
- Understand AVR timer architecture
- Learn timer registers (TCCR, TCNT, OCR)
- Implement non-blocking LED blink using timer overflow
- Compare with delay-based approach

**Detailed Implementation**:

**Objective**: Use Timer0 for non-blocking LED blinking instead of _delay_ms().

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- 1 LED (PB5/13)

**Code Template**:
```c
#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_PIN 5  // PB5

volatile uint8_t timer_flag = 0;

// Timer0 overflow interrupt service routine
ISR(TIMER0_OVF_vect) {
    timer_flag = 1;
}

int main(void) {
    // Set LED pin as output
    DDRB |= (1 << LED_PIN);
    
    // Configure Timer0 for overflow interrupt
    // Normal mode, no prescaler (timer increments every clock cycle)
    TCCR0A = 0;  // Normal mode
    TCCR0B = (1 << CS00);  // No prescaler, clock = 16MHz
    TIMSK0 = (1 << TOIE0);  // Enable Timer0 overflow interrupt
    
    // Enable global interrupts
    sei();
    
    uint8_t led_state = 0;
    
    while (1) {
        if (timer_flag) {
            timer_flag = 0;
            
            led_state = !led_state;
            
            if (led_state) {
                PORTB |= (1 << LED_PIN);
            } else {
                PORTB &= ~(1 << LED_PIN);
            }
        }
        
        // Main loop can do other work here
        // Unlike _delay_ms(), this doesn't block
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add LED from Spare Parts → Outputs
3. Configure LED → PB5/13
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation

**Key Learnings**:
- Timer0 configuration
- Interrupt Service Routines (ISR)
- TIMER0_OVF_vect interrupt vector
- Global interrupt enable (sei())
- Volatile variables for shared data
- Non-blocking programming

#### Project 6: PWM (Pulse Width Modulation)
- Learn timer PWM modes
- Control LED brightness
- Create breathing LED effect
- Understand duty cycle and frequency

**Detailed Implementation**:

**Objective**: Use Timer0 in PWM mode to control LED brightness.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- 1 LED (PB5/13 - actually need to use PWM-capable pin like PD6/6)

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define LED_PIN 6  // PD6 (OC0A - Timer0 PWM output)

int main(void) {
    // Set LED pin as output
    DDRD |= (1 << LED_PIN);
    
    // Configure Timer0 for Fast PWM mode, non-inverting
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);  // Fast PWM, OC0A non-inverting
    TCCR0B = (1 << CS01) | (1 << CS00);  // Prescaler 64 (clock = 16MHz/64 = 250kHz)
    
    uint8_t brightness = 0;
    int8_t direction = 1;
    
    while (1) {
        // Set PWM duty cycle (0-255)
        OCR0A = brightness;
        
        // Breathing effect
        brightness += direction;
        
        if (brightness >= 255) {
            direction = -1;
        } else if (brightness <= 0) {
            direction = 1;
        }
        
        _delay_ms(10);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add LED from Spare Parts → Outputs
3. Configure LED → PD6/6 (PWM-capable pin)
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation

**Key Learnings**:
- PWM (Pulse Width Modulation) concept
- Timer PWM modes (Fast PWM)
- OCR (Output Compare Register)
- Duty cycle control
- PWM frequency calculation

#### Project 7: External Interrupts
- Learn interrupt vectors and ISR
- Implement button-triggered interrupt
- Compare polling vs interrupt-based approach
- Understand interrupt safety and context

**Detailed Implementation**:

**Objective**: Use external interrupt INT0 (PD2) to toggle LED on button press.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- 1 LED (PB5/13)
- 1 Push button (PD2/2 - INT0)

**Code Template**:
```c
#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_PIN 5  // PB5

volatile uint8_t led_state = 0;

// External Interrupt 0 Service Routine
ISR(INT0_vect) {
    led_state = !led_state;
    
    if (led_state) {
        PORTB |= (1 << LED_PIN);
    } else {
        PORTB &= ~(1 << LED_PIN);
    }
}

int main(void) {
    // Set LED pin as output
    DDRB |= (1 << LED_PIN);
    
    // Set button pin as input with pull-up
    DDRD &= ~(1 << PD2);
    PORTD |= (1 << PD2);
    
    // Configure INT0 for falling edge (button press)
    EICRA |= (1 << ISC01);  // Falling edge triggers interrupt
    EIMSK |= (1 << INT0);   // Enable INT0
    
    // Enable global interrupts
    sei();
    
    while (1) {
        // Main loop does nothing - interrupt handles everything
        // Can add other tasks here
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add LED → PB5/13
3. Add button → PD2/2
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation

**Key Learnings**:
- External interrupt configuration (EICRA, EIMSK)
- INT0 interrupt vector
- Falling edge triggering
- Event-driven programming

#### Project 8: Timer Interrupts
- Create precise timing using timer interrupts
- Implement multi-tasking (LED blink + button monitoring)
- Learn about interrupt priorities

**Detailed Implementation**:

**Objective**: Use Timer1 for precise 1-second timing while monitoring button in main loop.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- 1 LED (PB5/13)
- 1 Button (PD2/2)

**Code Template**:
```c
#include <avr/io.h>
#include <avr/interrupt.h>

#define LED_PIN 5   // PB5
#define BUTTON_PIN 2  // PD2

volatile uint8_t second_flag = 0;
volatile uint16_t seconds = 0;

// Timer1 compare match interrupt
ISR(TIMER1_COMPA_vect) {
    second_flag = 1;
    seconds++;
}

int main(void) {
    // Set LED pin as output
    DDRB |= (1 << LED_PIN);
    
    // Set button pin as input with pull-up
    DDRD &= ~(1 << BUTTON_PIN);
    PORTD |= (1 << BUTTON_PIN);
    
    // Configure Timer1 for 1-second interrupts
    // CTC mode, prescaler 1024
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 15624;  // 16MHz / 1024 / 15625 = 1Hz
    TIMSK1 = (1 << OCIE1A);  // Enable compare match interrupt
    
    sei();
    
    uint8_t led_state = 0;
    
    while (1) {
        // Handle button in main loop (polling)
        if (((PIND >> BUTTON_PIN) & 1) == 0) {
            _delay_ms(50);  // Debounce
            if (((PIND >> BUTTON_PIN) & 1) == 0) {
                // Button pressed - do something
                led_state = !led_state;
                PORTB ^= (1 << LED_PIN);
                while (((PIND >> BUTTON_PIN) & 1) == 0);  // Wait for release
            }
        }
        
        // Handle timer flag
        if (second_flag) {
            second_flag = 0;
            // Do something every second
        }
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add LED → PB5/13
3. Add button → PD2/2
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation

**Key Learnings**:
- Timer1 (16-bit timer) configuration
- CTC (Clear Timer on Compare) mode
- OCR for precise timing
- Multi-tasking (interrupts + polling)
- Timer calculations

### Key Concepts
- Timer/counters
- PWM generation
- Interrupt service routines
- Non-blocking programming
- Real-time considerations

---

## Phase 4: Serial Communication (Week 4)

### Objective
Enable communication between microcontroller and host.

### Learning Projects

#### Project 9: UART Basics
- Learn UART hardware and registers
- Configure baud rate
- Send "Hello World" to virtual terminal
- Receive characters from virtual terminal
- Implement echo program

**Detailed Implementation**:

**Objective**: Configure UART for 9600 baud and implement echo functionality.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- Virtual terminal (built-in in PicsimLab)

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

void uart_init(void) {
    // Set baud rate
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    
    // Enable transmitter and receiver
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    
    // Set frame format: 8 data bits, 1 stop bit, no parity
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    // Wait for empty transmit buffer
    while (!(UCSR0A & (1 << UDRE0)));
    
    // Put data into buffer, sends the data
    UDR0 = data;
}

char uart_receive(void) {
    // Wait for data to be received
    while (!(UCSR0A & (1 << RXC0)));
    
    // Get and return received data from buffer
    return UDR0;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

int main(void) {
    uart_init();
    
    uart_print_string("Hello World!\r\n");
    
    while (1) {
        char received = uart_receive();
        uart_transmit(received);  // Echo back
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Load hex file
3. Tools → Arduino Bootloader
4. Run simulation
5. Open virtual terminal (Tools → Virtual Terminal or similar)
6. Set baud rate to 9600
7. Type characters and see them echoed

**Key Learnings**:
- UART register configuration (UBRR, UCSR)
- Baud rate calculation
- Transmit/receive operations
- Polling-based UART
- String handling

#### Project 10: Serial Commands
- Parse simple commands from serial
- Implement command interpreter
- Control LEDs via serial commands
- Learn string handling in embedded C

**Detailed Implementation**:

**Objective**: Parse commands like "ON", "OFF", "BLINK" from serial to control LED.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- 1 LED (PB5/13)
- Virtual terminal

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
#define LED_PIN 5
#define BUFFER_SIZE 32

char rx_buffer[BUFFER_SIZE];
uint8_t rx_index = 0;

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

char uart_receive(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void execute_command(char *cmd) {
    // Convert to uppercase for case-insensitive comparison
    for (char *p = cmd; *p; p++) {
        if (*p >= 'a' && *p <= 'z') {
            *p -= 32;
        }
    }
    
    if (strcmp(cmd, "ON") == 0) {
        PORTB |= (1 << LED_PIN);
        uart_print_string("LED ON\r\n");
    } else if (strcmp(cmd, "OFF") == 0) {
        PORTB &= ~(1 << LED_PIN);
        uart_print_string("LED OFF\r\n");
    } else if (strcmp(cmd, "BLINK") == 0) {
        for (int i = 0; i < 5; i++) {
            PORTB |= (1 << LED_PIN);
            _delay_ms(500);
            PORTB &= ~(1 << LED_PIN);
            _delay_ms(500);
        }
        uart_print_string("BLINK DONE\r\n");
    } else {
        uart_print_string("UNKNOWN COMMAND\r\n");
    }
}

int main(void) {
    DDRB |= (1 << LED_PIN);
    uart_init();
    uart_print_string("Commands: ON, OFF, BLINK\r\n");
    
    while (1) {
        char c = uart_receive();
        
        if (c == '\r' || c == '\n') {
            rx_buffer[rx_index] = '\0';
            if (rx_index > 0) {
                execute_command(rx_buffer);
                rx_index = 0;
            }
        } else if (rx_index < BUFFER_SIZE - 1) {
            rx_buffer[rx_index++] = c;
        }
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add LED → PB5/13
3. Load hex file
4. Tools → Arduino Bootloader
5. Run simulation
6. Open virtual terminal
7. Type commands: ON, OFF, BLINK

**Key Learnings**:
- String parsing
- Command interpreter pattern
- Case-insensitive comparison
- Buffer management

#### Project 11: Serial Data Logging
- Read button states and send over serial
- Format data with timestamps
- Create simple protocol
- Parse data on host side

**Detailed Implementation**:

**Objective**: Log button presses with timestamps over serial in CSV format.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- 1 Button (PD2/2)
- Virtual terminal

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
#define BUTTON_PIN 2

volatile uint16_t seconds = 0;

// Timer1 interrupt for timestamp
ISR(TIMER1_COMPA_vect) {
    seconds++;
}

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void uart_print_number(uint16_t num) {
    char buffer[6];
    uint8_t i = 0;
    
    if (num == 0) {
        uart_transmit('0');
        return;
    }
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        uart_transmit(buffer[--i]);
    }
}

void log_event(uint8_t event) {
    uart_print_string("EVENT,");
    uart_print_number(seconds);
    uart_print_string(",");
    uart_print_number(event);
    uart_print_string("\r\n");
}

int main(void) {
    DDRD &= ~(1 << BUTTON_PIN);
    PORTD |= (1 << BUTTON_PIN);
    
    // Timer1 for 1-second timestamps
    TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
    OCR1A = 15624;
    TIMSK1 = (1 << OCIE1A);
    
    uart_init();
    uart_print_string("Logging started...\r\n");
    uart_print_string("Format: EVENT,TIME,TYPE\r\n");
    
    sei();
    
    uint8_t last_button = 1;
    
    while (1) {
        uint8_t current_button = (PIND >> BUTTON_PIN) & 1;
        
        if (current_button == 0 && last_button == 1) {
            _delay_ms(50);
            if (((PIND >> BUTTON_PIN) & 1) == 0) {
                log_event(1);  // Button press
                while (((PIND >> BUTTON_PIN) & 1) == 0);
                log_event(0);  // Button release
            }
        }
        
        last_button = current_button;
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add button → PD2/2
3. Load hex file
4. Tools → Arduino Bootloader
5. Run simulation
6. Open virtual terminal
7. Press button to see log entries

**Key Learnings**:
- Data logging
- CSV protocol design
- Timestamp generation
- Number to string conversion

### Key Concepts
- UART configuration
- Baud rate calculation
- Serial data framing
- Interrupt-driven serial
- Data protocols

---

## Phase 5: Analog Input (Week 5)

### Objective
Read and process analog signals.

### Learning Projects

#### Project 12: Potentiometer Reading
- Learn ADC registers
- Read analog values from potentiometer
- Convert to voltage
- Display on serial terminal

**Detailed Implementation**:

**Objective**: Read analog input from potentiometer and display voltage over serial.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- Potentiometer (Spare Parts → Inputs → Pot)
- Virtual terminal

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void uart_print_number(uint16_t num) {
    char buffer[6];
    uint8_t i = 0;
    
    if (num == 0) {
        uart_transmit('0');
        return;
    }
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        uart_transmit(buffer[--i]);
    }
}

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

int main(void) {
    // Initialize ADC
    ADMUX = (1 << REFS0);  // AVcc as reference
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);  // Enable ADC, prescaler 128
    
    uart_init();
    uart_print_string("ADC Test\r\n");
    
    while (1) {
        uint16_t adc_value = adc_read(0);  // Read A0
        float voltage = (adc_value * 5.0) / 1024.0;
        
        uart_print_string("ADC: ");
        uart_print_number(adc_value);
        uart_print_string(" Voltage: ");
        uart_print_number((uint16_t)(voltage * 100));
        uart_print_string(".");
        uart_print_number((uint16_t)(voltage * 100) % 100);
        uart_print_string("V\r\n");
        
        _delay_ms(500);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add potentiometer from Spare Parts → Inputs
3. Configure pot → PC0/A0
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation
7. Open virtual terminal
8. Adjust pot and see values change

**Key Learnings**:
- ADC register configuration (ADMUX, ADCSRA)
- ADC conversion process
- Analog to digital conversion
- Voltage calculation
- Reference voltage

#### Project 13: Analog LED Control
- Use pot to control LED brightness (PWM)
- Map ADC values to PWM range
- Smooth the output

**Detailed Implementation**:

**Objective**: Map potentiometer position to LED brightness using PWM.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- Potentiometer (PC0/A0)
- LED (PD6/6 - PWM pin)

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

int main(void) {
    // ADC setup
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    // PWM setup (Timer0, PD6/OC0A)
    DDRD |= (1 << 6);
    TCCR0A = (1 << COM0A1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);
    
    uart_init();
    uart_print_string("LED Control via Pot\r\n");
    
    while (1) {
        uint16_t adc_value = adc_read(0);
        
        // Map ADC (0-1023) to PWM (0-255)
        uint8_t pwm_value = (adc_value * 255) / 1023;
        OCR0A = pwm_value;
        
        _delay_ms(50);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add pot → PC0/A0
3. Add LED → PD6/6
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation
7. Adjust pot to change LED brightness

**Key Learnings**:
- ADC to PWM mapping
- Value scaling (range conversion)
- Real-time control loops

#### Project 14: Temperature Sensor (LM35)
- Read temperature sensor
- Convert ADC to Celsius
- Display on virtual terminal
- Add temperature thresholds with LED indication

**Detailed Implementation**:

**Objective**: Read LM35 temperature sensor and display temperature with threshold LEDs.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- LM35 temperature sensor (Spare Parts → Inputs → LM35)
- 2 LEDs (PD6/6 and PD5/5)
- Virtual terminal

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void uart_print_number(int16_t num) {
    if (num < 0) {
        uart_transmit('-');
        num = -num;
    }
    
    char buffer[6];
    uint8_t i = 0;
    
    if (num == 0) {
        uart_transmit('0');
        return;
    }
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        uart_transmit(buffer[--i]);
    }
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSRA & (1 << ADSC));
    return ADC;
}

int main(void) {
    // LED pins
    DDRD |= (1 << 6) | (1 << 5);
    
    // ADC setup
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    uart_init();
    uart_print_string("Temperature Monitor\r\n");
    
    while (1) {
        uint16_t adc_value = adc_read(0);
        
        // LM35: 10mV per degree Celsius
        // ADC: 0-1023 = 0-5V, so 1 ADC count = 4.88mV
        // Temperature = (ADC * 4.88) / 10 = ADC * 0.488
        int16_t temp_c = (adc_value * 488) / 1000;
        
        uart_print_string("Temperature: ");
        uart_print_number(temp_c);
        uart_print_string("C\r\n");
        
        // Threshold indicators
        if (temp_c >= 30) {
            PORTD |= (1 << 6);  // Red LED (hot)
            PORTD &= ~(1 << 5);
        } else if (temp_c <= 20) {
            PORTD |= (1 << 5);  // Green LED (cold)
            PORTD &= ~(1 << 6);
        } else {
            PORTD &= ~((1 << 6) | (1 << 5));  // Both off (normal)
        }
        
        _delay_ms(1000);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add LM35 → PC0/A0
3. Add 2 LEDs → PD6/6 (red) and PD5/5 (green)
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation
7. Open virtual terminal

**Key Learnings**:
- Sensor datasheet interpretation
- Temperature conversion formulas
- Threshold-based control
- Conditional logic

#### Project 15: Light Sensor (LDR)
- Read light-dependent resistor
- Create automatic light control
- Implement hysteresis

**Detailed Implementation**:

**Objective**: Use LDR to automatically control LED based on light level with hysteresis.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- LDR (Spare Parts → Inputs → LDR)
- LED (PB5/13)

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void uart_print_number(uint16_t num) {
    char buffer[6];
    uint8_t i = 0;
    
    if (num == 0) {
        uart_transmit('0');
        return;
    }
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        uart_transmit(buffer[--i]);
    }
}

uint16_t adc_read(uint8_t channel) {
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    ADCSRA |= (1 << ADSC);
    while (ADCSA & (1 << ADSC));
    return ADC;
}

int main(void) {
    DDRB |= (1 << 5);  // LED on PB5
    
    // ADC setup
    ADMUX = (1 << REFS0);
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    
    uart_init();
    uart_print_string("Light Sensor Control\r\n");
    
    // Thresholds with hysteresis
    const uint16_t DARK_THRESHOLD = 400;
    const uint16_t LIGHT_THRESHOLD = 600;
    uint8_t led_state = 0;
    
    while (1) {
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
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add LDR → PC0/A0
3. Add LED → PB5/13
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation
7. Open virtual terminal
8. Adjust light level to test

**Key Learnings**:
- Hysteresis implementation
- Preventing oscillation at thresholds
- Environmental sensing
- Automatic control systems

### Key Concepts
- ADC configuration
- Reference voltage
- Sampling rate
- Signal conditioning
- Calibration

---

## Phase 6: I2C Communication (Week 6)

### Objective
Communicate with I2C peripherals.

### Learning Projects

#### Project 16: I2C Scanner
- Implement I2C bus scanning
- Detect connected devices
- Display addresses on serial

**Detailed Implementation**:

**Objective**: Scan I2C bus for connected devices and display their addresses.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- Virtual terminal
- Any I2C device (optional, for testing)

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void uart_print_hex(uint8_t num) {
    char hex[] = "0123456789ABCDEF";
    uart_transmit(hex[num >> 4]);
    uart_transmit(hex[num & 0x0F]);
}

void i2c_init(void) {
    TWSR = 0;  // No prescaler
    TWBR = 72;  // 100kHz @ 16MHz
}

uint8_t i2c_start(uint8_t address) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    
    if ((TWSR & 0xF8) != 0x08) return 1;  // Error
    
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    
    return (TWSR & 0xF8) != 0x18;
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}

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

int main(void) {
    uart_init();
    i2c_init();
    
    while (1) {
        i2c_scan();
        _delay_ms(5000);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Load hex file
3. Tools → Arduino Bootloader
4. Run simulation
5. Open virtual terminal

**Key Learnings**:
- I2C TWI peripheral
- Start/stop conditions
- Slave addressing
- Bus scanning

#### Project 17: OLED Display (SSD1306)
- Initialize I2C OLED display
- Display text and graphics
- Create simple animations
- Integrate with sensor data

**Detailed Implementation**:

**Objective**: Initialize SSD1306 OLED and display text.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- SSD1306 OLED (Spare Parts → Outputs → LCD_ssd1306)

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
#define OLED_ADDR 0x3C

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void i2c_init(void) {
    TWSR = 0;
    TWBR = 72;
}

void i2c_start(uint8_t address) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void oled_init(void) {
    i2c_start(OLED_ADDR << 1);
    i2c_write(0x00);  // Command mode
    
    i2c_write(0xAE);  // Display off
    i2c_write(0xD5);  // Set clock divider
    i2c_write(0x80);
    i2c_write(0xA8);  // Set multiplex ratio
    i2c_write(0x3F);
    i2c_write(0xD3);  // Set display offset
    i2c_write(0x00);
    i2c_write(0x40);  // Set start line
    i2c_write(0x8D);  // Charge pump
    i2c_write(0x14);
    i2c_write(0x20);  // Memory addressing
    i2c_write(0x00);
    i2c_write(0xA1);  // Segment remap
    i2c_write(0xC8);  // COM output scan direction
    i2c_write(0xDA);  // Set COM pins
    i2c_write(0x12);
    i2c_write(0x81);  // Set contrast
    i2c_write(0xCF);
    i2c_write(0xD9);  // Set pre-charge
    i2c_write(0xF1);
    i2c_write(0xDB);  // Set VCOMH
    i2c_write(0x40);
    i2c_write(0xA4);  // Resume display
    i2c_write(0xA6);  // Normal display
    i2c_write(0xAF);  // Display on
    
    i2c_stop();
}

void oled_clear(void) {
    i2c_start(OLED_ADDR << 1);
    i2c_write(0x00);
    i2c_write(0x21);  // Column address
    i2c_write(0);
    i2c_write(127);
    i2c_write(0x22);  // Page address
    i2c_write(0);
    i2c_write(7);
    i2c_stop();
    
    for (uint16_t i = 0; i < 128 * 8; i++) {
        i2c_start(OLED_ADDR << 1);
        i2c_write(0x40);  // Data mode
        i2c_write(0x00);
        i2c_stop();
    }
}

int main(void) {
    uart_init();
    i2c_init();
    oled_init();
    oled_clear();
    
    uart_print_string("OLED Initialized\r\n");
    
    while (1) {
        // Toggle display on/off
        i2c_start(OLED_ADDR << 1);
        i2c_write(0x00);
        i2c_write(0xAE);  // Off
        i2c_stop();
        _delay_ms(1000);
        
        i2c_start(OLED_ADDR << 1);
        i2c_write(0x00);
        i2c_write(0xAF);  // On
        i2c_stop();
        _delay_ms(1000);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add SSD1306 OLED from Spare Parts
3. Configure OLED I2C address
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation

**Key Learnings**:
- OLED initialization sequence
- Display commands
- Memory addressing
- Data vs command mode

#### Project 18: Real-Time Clock (DS1307)
- Read/write time to RTC
- Display current time on OLED
- Implement alarm functionality

**Detailed Implementation**:

**Objective**: Read time from DS1307 RTC and display on OLED with alarm functionality.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- DS1307 RTC (Spare Parts → Others → RTC_ds1307)
- SSD1306 OLED (from Project 17)
- Virtual terminal

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
#define DS1307_ADDR 0x68
#define OLED_ADDR 0x3C

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void uart_print_number(uint8_t num) {
    char buffer[3];
    buffer[0] = '0' + (num / 10);
    buffer[1] = '0' + (num % 10);
    uart_transmit(buffer[0]);
    uart_transmit(buffer[1]);
}

void i2c_init(void) {
    TWSR = 0;
    TWBR = 72;
}

void i2c_start(uint8_t address) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

uint8_t i2c_read_ack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    return (TWSR & 0xF8);
}

uint8_t i2c_read_nack(void) {
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    i2c_stop();
    return TWDR;
}

uint8_t bcd_to_decimal(uint8_t bcd) {
    return ((bcd >> 4) * 10) + (bcd & 0x0F);
}

uint8_t decimal_to_bcd(uint8_t decimal) {
    return ((decimal / 10) << 4) | (decimal % 10);
}

void rtc_read_time(uint8_t *hours, uint8_t *minutes, uint8_t *seconds) {
    i2c_start(DS1307_ADDR << 1);
    i2c_write(0x00);  // Start at seconds register
    i2c_start((DS1307_ADDR << 1) | 1);
    
    *seconds = bcd_to_decimal(i2c_read_ack());
    *minutes = bcd_to_decimal(i2c_read_ack());
    *hours = bcd_to_decimal(i2c_read_nack());
}

void rtc_set_time(uint8_t hours, uint8_t minutes, uint8_t seconds) {
    i2c_start(DS1307_ADDR << 1);
    i2c_write(0x00);
    i2c_write(decimal_to_bcd(seconds));
    i2c_write(decimal_to_bcd(minutes));
    i2c_write(decimal_to_bcd(hours));
    i2c_write(0x00);  // Day of week
    i2c_stop();
}

int main(void) {
    uart_init();
    i2c_init();
    
    // Set initial time (12:00:00)
    rtc_set_time(12, 0, 0);
    
    uart_print_string("RTC Initialized\r\n");
    
    uint8_t hours, minutes, seconds;
    
    while (1) {
        rtc_read_time(&hours, &minutes, &seconds);
        
        uart_print_string("Time: ");
        uart_print_number(hours);
        uart_transmit(':');
        uart_print_number(minutes);
        uart_transmit(':');
        uart_print_number(seconds);
        uart_print_string("\r\n");
        
        _delay_ms(1000);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add DS1307 RTC from Spare Parts
3. Configure RTC I2C address
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation
7. Open virtual terminal

**Key Learnings**:
- BCD (Binary Coded Decimal) conversion
- RTC register structure
- Time keeping in embedded systems
- I2C multi-byte reads

#### Project 19: Temperature/Humidity (DHT11/DHT22)
- Read temperature and humidity
- Display on OLED
- Log data with timestamps

**Detailed Implementation**:

**Objective**: Read DHT22 temperature and humidity sensor and display data.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- DHT22 sensor (Spare Parts → Inputs → DHT22)
- Virtual terminal

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
#define DHT_PIN 2  // PD2

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void uart_print_number(int16_t num) {
    if (num < 0) {
        uart_transmit('-');
        num = -num;
    }
    
    char buffer[6];
    uint8_t i = 0;
    
    if (num == 0) {
        uart_transmit('0');
        return;
    }
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        uart_transmit(buffer[--i]);
    }
}

void dht_start(void) {
    DDRD |= (1 << DHT_PIN);
    PORTD &= ~(1 << DHT_PIN);
    _delay_us(1000);  // 1ms start signal
    PORTD |= (1 << DHT_PIN);
    DDRD &= ~(1 << DHT_PIN);
}

uint8_t dht_read_bit(void) {
    while ((PIND >> DHT_PIN) & 1);  // Wait for low
    while (!((PIND >> DHT_PIN) & 1);  // Wait for high
    _delay_us(40);  // Wait for bit center
    uint8_t bit = ((PIND >> DHT_PIN) & 1);
    while ((PIND >> DHT_PIN) & 1);  // Wait for low
    return bit;
}

uint8_t dht_read_byte(void) {
    uint8_t data = 0;
    for (uint8_t i = 0; i < 8; i++) {
        data <<= 1;
        data |= dht_read_bit();
    }
    return data;
}

uint8_t dht_read_data(int16_t *temp, int16_t *hum) {
    dht_start();
    
    uint8_t data[5];
    for (uint8_t i = 0; i < 5; i++) {
        data[i] = dht_read_byte();
    }
    
    // Verify checksum
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    if (checksum != data[4]) {
        return 1;  // Checksum error
    }
    
    *hum = (data[0] << 8) | data[1];
    *temp = (data[2] << 8) | data[3];
    
    // Handle negative temperature for DHT22
    if (*temp & 0x8000) {
        *temp = -(*temp & 0x7FFF);
    }
    
    return 0;
}

int main(void) {
    uart_init();
    uart_print_string("DHT22 Sensor\r\n");
    
    int16_t temp, hum;
    
    while (1) {
        if (dht_read_data(&temp, &hum) == 0) {
            uart_print_string("Humidity: ");
            uart_print_number(hum / 10);
            uart_transmit('.');
            uart_print_number(hum % 10);
            uart_print_string("%  Temp: ");
            uart_print_number(temp / 10);
            uart_transmit('.');
            uart_print_number(abs(temp % 10));
            uart_print_string("C\r\n");
        } else {
            uart_print_string("Checksum error\r\n");
        }
        
        _delay_ms(2000);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add DHT22 from Spare Parts → Inputs
3. Configure sensor → PD2/2
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation
7. Open virtual terminal

**Key Learnings**:
- Digital sensor protocols
- Bit-banging (software SPI/I2C)
- Checksum validation
- Signed integer handling

### Key Concepts
- I2C protocol and timing
- Slave addressing
- Register mapping
- Device initialization
- Multi-device buses

---

## Phase 7: SPI Communication (Week 7)

### Objective
Communicate with SPI peripherals.

### Learning Projects

#### Project 20: 7-Segment Display (MAX7219)
- Learn SPI protocol
- Control 8-digit 7-segment display
- Create counter and clock display
- Implement scrolling text

**Detailed Implementation**:

**Objective**: Control MAX7219 7-segment display via SPI to show numbers.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- MAX7219 display (Spare Parts → Outputs → 7s_Display_Dec)
- Virtual terminal

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
#define SS_PIN 2  // PD2

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void spi_init(void) {
    DDRB |= (1 << 5) | (1 << 3) | (1 << 2);  // MOSI, SCK, SS
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);  // SPI enable, master, fosc/4
}

void spi_transmit(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
}

void max7219_init(void) {
    // Decode mode: BCD for digits 0-7
    PORTD &= ~(1 << SS_PIN);
    spi_transmit(0x09);
    spi_transmit(0xFF);
    PORTD |= (1 << SS_PIN);
    
    // Intensity: 15/32
    PORTD &= ~(1 << SS_PIN);
    spi_transmit(0x0A);
    spi_transmit(0x0F);
    PORTD |= (1 << SS_PIN);
    
    // Scan limit: 8 digits
    PORTD &= ~(1 << SS_PIN);
    spi_transmit(0x0B);
    spi_transmit(0x07);
    PORTD |= (1 << SS_PIN);
    
    // Shutdown: normal operation
    PORTD &= ~(1 << SS_PIN);
    spi_transmit(0x0C);
    spi_transmit(0x01);
    PORTD |= (1 << SS_PIN);
    
    // Display test: off
    PORTD &= ~(1 << SS_PIN);
    spi_transmit(0x0F);
    spi_transmit(0x00);
    PORTD |= (1 << SS_PIN);
}

void max7219_digit(uint8_t digit, uint8_t value) {
    PORTD &= ~(1 << SS_PIN);
    spi_transmit(digit + 1);
    spi_transmit(value);
    PORTD |= (1 << SS_PIN);
}

void max7219_clear(void) {
    for (uint8_t i = 0; i < 8; i++) {
        max7219_digit(i, 0x0F);  // Blank
    }
}

int main(void) {
    uart_init();
    spi_init();
    max7219_init();
    max7219_clear();
    
    uart_print_string("MAX7219 Initialized\r\n");
    
    uint16_t counter = 0;
    
    while (1) {
        // Display counter on first 4 digits
        uint16_t temp = counter;
        for (uint8_t i = 0; i < 4; i++) {
            max7219_digit(i, temp % 10);
            temp /= 10;
        }
        
        counter++;
        if (counter > 9999) counter = 0;
        
        _delay_ms(1000);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add 7-segment display from Spare Parts
3. Configure SPI pins (SS, MOSI, SCK)
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation
7. Open virtual terminal

**Key Learnings**:
- SPI configuration (SPCR)
- SPI data transfer (SPDR)
- Chip select management
- BCD encoding

#### Project 21: SD Card Module
- Initialize SD card via SPI
- Create text file
- Log sensor data
- Read and display files

**Detailed Implementation**:

**Objective**: Initialize SD card and write/read data files.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- SD card module (Spare Parts → Others → SD card)
- Virtual terminal

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>
#include <string.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
#define SS_PIN 2  // PD2

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void spi_init(void) {
    DDRB |= (1 << 5) | (1 << 3) | (1 << 2);
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

void spi_transmit(uint8_t data) {
    SPDR = data;
    while (!(SPSR & (1 << SPIF)));
}

uint8_t spi_receive(void) {
    SPDR = 0xFF;
    while (!(SPSR & (1 << SPIF)));
    return SPDR;
}

void sd_cs_low(void) {
    PORTD &= ~(1 << SS_PIN);
}

void sd_cs_high(void) {
    PORTD |= (1 << SS_PIN);
}

uint8_t sd_command(uint8_t cmd, uint32_t arg, uint8_t crc) {
    sd_cs_low();
    spi_transmit(cmd | 0x40);
    spi_transmit((arg >> 24) & 0xFF);
    spi_transmit((arg >> 16) & 0xFF);
    spi_transmit((arg >> 8) & 0xFF);
    spi_transmit(arg & 0xFF);
    spi_transmit(crc);
    uint8_t response = spi_receive();
    sd_cs_high();
    return response;
}

uint8_t sd_init(void) {
    // Send 80 clock cycles
    for (uint8_t i = 0; i < 10; i++) {
        spi_transmit(0xFF);
    }
    
    // CMD0: Go to idle state
    if (sd_command(0, 0, 0x95) != 0x01) return 1;
    
    // CMD8: Send interface condition
    if (sd_command(8, 0x000001AA, 0x87) != 0x01) return 2;
    
    // ACMD41: Send host capacity support
    sd_command(55, 0, 0xFF);
    if (sd_command(41, 0x40000000, 0xFF) != 0x00) return 3;
    
    // CMD58: Read OCR
    sd_command(58, 0, 0xFF);
    for (uint8_t i = 0; i < 4; i++) spi_receive();
    
    // CMD16: Set block size to 512
    if (sd_command(16, 0x00000200, 0xFF) != 0x00) return 4;
    
    return 0;
}

void sd_write_block(uint32_t block_addr, uint8_t *data) {
    sd_command(24, block_addr, 0xFF);
    
    sd_cs_low();
    spi_transmit(0xFE);  // Data token
    
    for (uint16_t i = 0; i < 512; i++) {
        spi_transmit(data[i]);
    }
    
    spi_transmit(0xFF);  // CRC
    spi_transmit(0xFF);  // Dummy CRC
    
    while (spi_receive() != 0xFF);  // Wait for not busy
    
    sd_cs_high();
}

int main(void) {
    uart_init();
    spi_init();
    
    DDRD |= (1 << SS_PIN);
    PORTD |= (1 << SS_PIN);
    
    if (sd_init() == 0) {
        uart_print_string("SD Card Initialized\r\n");
        
        uint8_t test_data[512];
        memset(test_data, 0x41, 512);  // Fill with 'A'
        
        sd_write_block(0, test_data);
        uart_print_string("Data written to block 0\r\n");
    } else {
        uart_print_string("SD Card Init Failed\r\n");
    }
    
    while (1) {
        _delay_ms(1000);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add SD card from Spare Parts
3. Configure SPI pins
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation
7. Open virtual terminal

**Key Learnings**:
- SD card initialization sequence
- SPI mode configuration
- Block writing
- Command/response protocol

#### Project 22: Accelerometer (ADXL345)
- Read 3-axis accelerometer
- Display X/Y/Z values
- Implement simple gesture detection

**Detailed Implementation**:

**Objective**: Read ADXL345 accelerometer data and display X/Y/Z acceleration values.

**Hardware Required (PicsimLab)**:
- Arduino Uno board
- ADXL345 accelerometer (Spare Parts → Inputs → ADXL345)
- Virtual terminal

**Code Template**:
```c
#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

#define BAUD 9600
#define UBRR_VALUE ((F_CPU / 16 / BAUD) - 1)
#define ADXL345_ADDR 0x53

void uart_init(void) {
    UBRR0H = (UBRR_VALUE >> 8);
    UBRR0L = UBRR_VALUE;
    UCSR0B = (1 << TXEN0) | (1 << RXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_transmit(char data) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = data;
}

void uart_print_string(const char *str) {
    while (*str) {
        uart_transmit(*str);
        str++;
    }
}

void uart_print_number(int16_t num) {
    if (num < 0) {
        uart_transmit('-');
        num = -num;
    }
    
    char buffer[6];
    uint8_t i = 0;
    
    if (num == 0) {
        uart_transmit('0');
        return;
    }
    
    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }
    
    while (i > 0) {
        uart_transmit(buffer[--i]);
    }
}

void i2c_init(void) {
    TWSR = 0;
    TWBR = 72;
}

void i2c_start(uint8_t address) {
    TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    TWDR = address;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_stop(void) {
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while (TWCR & (1 << TWSTO));
}

void i2c_write(uint8_t data) {
    TWDR = data;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
}

void i2c_write_register(uint8_t reg, uint8_t value) {
    i2c_start(ADXL345_ADDR << 1);
    i2c_write(reg);
    i2c_write(value);
    i2c_stop();
}

uint8_t i2c_read_register(uint8_t reg) {
    i2c_start(ADXL345_ADDR << 1);
    i2c_write(reg);
    i2c_start((ADXL345_ADDR << 1) | 1);
    uint8_t value = TWDR;
    TWCR = (1 << TWINT) | (1 << TWEN);
    while (!(TWCR & (1 << TWINT)));
    i2c_stop();
    return value;
}

void adxl345_init(void) {
    i2c_write_register(0x2D, 0x08);  // Power control
    i2c_write_register(0x31, 0x0A);  // Data format (full res, +/- 16g)
    i2c_write_register(0x2C, 0x08);  // Rate 100 Hz
}

void adxl345_read_xyz(int16_t *x, int16_t *y, int16_t *z) {
    // Read 6 bytes starting at DATAX0 (0x32)
    i2c_start(ADXL345_ADDR << 1);
    i2c_write(0x32);
    i2c_start((ADXL345_ADDR << 1) | 1);
    
    uint8_t data[6];
    for (uint8_t i = 0; i < 6; i++) {
        data[i] = TWDR;
        TWCR = (1 << TWINT) | (1 << TWEN);
        while (!(TWCR & (1 << TWINT)));
    }
    i2c_stop();
    
    *x = (int16_t)((data[1] << 8) | data[0]);
    *y = (int16_t)((data[3] << 8) | data[2]);
    *z = (int16_t)((data[5] << 8) | data[4]);
}

int main(void) {
    uart_init();
    i2c_init();
    adxl345_init();
    
    uart_print_string("ADXL345 Initialized\r\n");
    
    int16_t x, y, z;
    
    while (1) {
        adxl345_read_xyz(&x, &y, &z);
        
        uart_print_string("X: ");
        uart_print_number(x);
        uart_print_string(" Y: ");
        uart_print_number(y);
        uart_print_string(" Z: ");
        uart_print_number(z);
        uart_print_string("\r\n");
        
        _delay_ms(100);
    }
    
    return 0;
}
```

**PicsimLab Setup**:
1. Load Arduino Uno board
2. Add ADXL345 from Spare Parts → Inputs
3. Configure accelerometer I2C address
4. Load hex file
5. Tools → Arduino Bootloader
6. Run simulation
7. Open virtual terminal
8. Move/tilt accelerometer to see values change

**Key Learnings**:
- Multi-byte I2C reads
- Accelerometer data interpretation
- 3-axis sensing
- Raw data processing

### Key Concepts
- SPI modes and timing
- Chip select management
- High-speed communication
- Buffer management

---

## Phase 8: Advanced Projects (Week 8+)

### Objective
Apply learned skills to complex projects.

### Project Options (Choose 2-3)

#### Option A: Digital Oscilloscope
- Read analog input continuously
- Display waveform on OLED
- Add trigger functionality
- Measure frequency and amplitude

**Implementation Outline**:
- Configure ADC for continuous sampling
- Store samples in circular buffer
- Display waveform on OLED as line graph
- Implement trigger (rising/falling edge threshold)
- Calculate frequency from zero-crossings
- Measure min/max amplitude
- Add timebase control via button

#### Option B: Weather Station
- Read temperature, humidity, pressure
- Display on OLED
- Log to SD card
- Add web server (if using ESP32 board later)

**Implementation Outline**:
- Read temperature (LM35 or DHT22)
- Read humidity (DHT22)
- Read pressure (BMP180 if available)
- Display all readings on OLED with labels
- Log to SD card with RTC timestamp every minute
- Implement scrolling display for all data
- Add average/min/max calculations

#### Option C: Game Development
- Create simple game on OLED
- Use accelerometer for control
- Add sound with buzzer
- Implement score tracking

**Implementation Outline**:
- Create simple game (e.g., Snake, Pong)
- Display game elements on OLED
- Use accelerometer X/Y for control
- Generate simple tones with buzzer
- Track score in EEPROM
- Implement game states (menu, playing, game over)
- Use timer for game loop timing

#### Option D: Home Automation Controller
- Read multiple sensors
- Control relays/motors
- Implement scheduling
- Add serial interface for control

**Implementation Outline**:
- Read multiple sensors (temp, light, motion)
- Control relays (simulate with LEDs)
- Implement time-based scheduling (RTC)
- Parse serial commands for manual control
- Display status on OLED
- Store schedule in EEPROM
- Implement automation rules (e.g., "if dark and after 6pm, turn on light")

---

## Phase 9: Real Hardware Transition (Optional)

### Objective
Move from simulation to real hardware.

### Tasks
1. **Acquire Arduino Uno or compatible board**
2. **Set up physical breadboard and components**
3. **Port projects from PicsimLab to real hardware**
4. **Learn debugging with real hardware (logic analyzer, multimeter)**
5. **Understand hardware-specific considerations (noise, power, timing)**

---

## Development Workflow

For each project:
1. Create project directory (or use new_project.sh)
2. Write C code in main.c
3. Create/update Makefile
4. Compile: `make`
5. Load .hex file in PicsimLab (File → Load Hex)
6. **For Arduino Uno boards**: Initialize bootloader (Tools → Arduino Bootloader)
7. Run simulation and test
8. Document learnings in README.md
9. Save PicsimLab workspace (.pzw file)

**Important Note**: For Arduino Uno boards in PicsimLab, the "Arduino Bootloader" step (Tools → Arduino Bootloader) is required after loading the hex file. Without this, the firmware loads but does not execute.

---

## Learning Resources

### Primary
- PicsimLab Examples: https://lcgamboa.github.io/picsimlab_examples/
- PicsimLab Documentation: https://lcgamboa.github.io/picsimlab_docs/stable/
- ATmega328P Datasheet: https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328P_Datasheet.pdf

### Supplementary
- AVR Libc Manual: https://www.nongnu.org/avr-libc/user-manual/
- Embedded C Programming: https://www.embedded.com/category/programming/
- Makefiles for AVR: https://www.microchip.com/en-us/application-notes/an AVR035

### Books (Optional)
- "Make: Electronics" by Charles Platt (basics)
- "Embedded C Programming and the Atmel AVR" by Barnett, Cox, and Full
- "The C Programming Language" by Kernighan and Ritchie (C fundamentals)

---

## Milestones & Success Criteria

### Week 1
- [ ] PicsimLab installed and running
- [ ] Can load Arduino Uno board
- [ ] Can add/remove parts
- [ ] Understand basic workflow

### Week 2
- [ ] Can blink LED using direct register access
- [ ] Can read button input
- [ ] Understand bit manipulation
- [ ] Can implement simple state machine

### Week 3
- [ ] Can use timers for non-blocking delays
- [ ] Can generate PWM
- [ ] Can implement interrupts
- [ ] Understand interrupt priorities

### Week 4
- [ ] Can send/receive serial data
- [ ] Can parse commands
- [ ] Understand UART configuration
- [ ] Can implement simple protocol

### Week 5
- [ ] Can read analog inputs
- [ ] Can convert ADC to physical values
- [ ] Can implement calibration
- [ ] Understand ADC sampling

### Week 6
- [ ] Can use I2C communication
- [ ] Can interface with OLED display
- [ ] Can interface with RTC
- [ ] Understand I2C protocol

### Week 7
- [ ] Can use SPI communication
- [ ] Can interface with 7-segment display
- [ ] Can interface with SD card
- [ ] Understand SPI modes

### Week 8+
- [ ] Complete at least 2 advanced projects
- [ ] Can integrate multiple peripherals
- [ ] Can debug complex issues
- [ ] Can design complete system

---

## Time Commitment
- **Recommended**: 1-2 hours per day, 5 days per week
- **Minimum**: 5 hours per week
- **Total duration**: 8-12 weeks for complete curriculum

---

## Notes & Adjustments

- This plan is flexible - can pause/accelerate based on progress
- Some projects may take longer/shorter than estimated
- Additional resources can be added as needed
- Can switch to real hardware at any point if desired
- Arduino IDE can be introduced later if simpler approach preferred
