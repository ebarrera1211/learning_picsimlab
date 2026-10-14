# Embedded C Learning with PICSimLab

Using PICSimLab with virtual Arduino Uno to learn embedded C programming from beginner level.

## ⚠️ Platform Warning

**This learning procedure was created and tested on Ubuntu 26.04.** While it may work on other Linux distributions, Windows, or macOS, the installation steps and paths may differ. Adjust instructions accordingly for your platform.

## Prerequisites

### 1. Operating System
- Linux (tested on Ubuntu 26.04)
- Root/sudo access for dependency installation

### 2. Required Software

#### Build Tools
```bash
sudo apt-get update
sudo apt-get install -y \
    gcc \
    g++ \
    make \
    cmake \
    git
```

#### AVR Toolchain
```bash
sudo apt-get install -y \
    gcc-avr \
    avr-libc
```

- `gcc-avr`: AVR cross-compiler (provides `avr-gcc`, `avr-objcopy`, etc.)
- `avr-libc`: AVR C library and headers (provides `<avr/io.h>`, `<util/delay.h>`)

#### PICSimLab Dependencies
```bash
sudo apt-get install -y \
    libelf-dev \
    freeglut3-dev
```

### 3. PICSimLab Installation

#### Clone PICSimLab Repository
```bash
cd ~
git clone https://github.com/lcgamboa/picsimlab.git
cd picsimlab
```

#### Install Dependencies
```bash
cd bscripts
sudo bash install_deps.sh
```

#### Build and Install PICSimLab
```bash
cd ..
bash bscripts/build_all_and_install.sh
```

This will build all dependencies (picsim, lxrad, simavr, etc.) and install PICSimLab.

#### Launch PICSimLab
```bash
picsimlab
```


## How to Run Each Project

### General Procedure

For each project, follow these steps:

1. **Navigate to Project Directory**
   ```bash
   cd ~/learning_picsimlab/projects/XX_project_name
   ```

2. **Build the Firmware**
   ```bash
   make
   ```
   This compiles `main.c` and generates `main.hex`. If no changes to .c from source, use .hex included in repo.

3. **Open PICSimLab**
   ```bash
   picsimlab
   ```

4. **Load the Workspace File**
   - In PICSimLab main window: `File` → `Load Workspace`
   - Navigate to the project directory
   - Select the `.pzw` file (e.g., `01_blink.pzw`)
   - Click `Open`

   **Note**: The `.pzw` workspace file contains the pre-configured hardware setup (LEDs, buttons, etc.) for that project.

5. **Initialize Bootloader (Arduino Uno)**
   - In PICSimLab main window: `Tools` → `Arduino Bootloader`
   - This step sets the Arduino Uno in a state ready to upload firmware.
   - Click `File` → `Load Hex` and load the firmware .hex file for the project 

6. **Observe Behavior**
   - LEDs will blink, buttons will respond, etc.
   - Use `Tools` → `Pin Viewer` to inspect pin states for debugging

7. **Save Workspace (Optional)**
   - If you make hardware configuration changes:
   - `File` → `Save Workspace` to update the `.pzw` file

### Project-Specific Details

Each project has its own `README.md` file with detailed information:

- **Project 1**: [01_blink/README.md](projects/01_blink/README.md) - Basic GPIO and LED blinking
- **Project 2**: [02_multiple_leds/README.md](projects/02_multiple_leds/README.md) - Multiple GPIO pins and binary representation
- **Project 3**: [03_button/README.md](projects/03_button/README.md) - Digital input and button debouncing
- **Project 4**: [04_traffic_light/README.md](projects/04_traffic_light/README.md) - State machines and non-blocking timing
- **Project 5**: [05_timer_blink/README.md](projects/05_timer_blink/README.md) - Hardware timers and interrupts
- **Project 6**: [06_pwm/README.md](projects/06_pwm/README.md) - Configure Pulse Width Modulation
- **Project 7**: [07_external_interrupt/README.md](projects/07_external_interrupt/README.md) - Setup interrupt and use ISR()
- **Project 8**: [08_timer_interrupt/README.md](projects/08_timer_interrupt/README.md) - Use interrupt to setup counter based on timer and use button to reset counter
- **Project 9**: [09_uart_basics/README.md](projects/09_uart_basics/README.md) - UART communication, string formatting, and interactive input

**Refer to each project's README.md for:**
- Hardware configuration details
- Code explanations
- Key concepts learned
- Troubleshooting tips

## Learning Path

This curriculum progresses from basic to advanced:

1. **GPIO Fundamentals** (Projects 1-4)
   - Digital output
   - Multiple pins
   - Digital input
   - State machines

2. **Timers & Interrupts** (Projects 5-8)
   - Hardware timers
   - Interrupt service routines
   - PWM
   - External interrupts

3. **Serial Communication** (Projects 9-11)
   - UART basics
   - Command parsing
   - Data logging

4. **Analog Input** (Projects 12-15)
   - ADC reading
   - Potentiometers
   - Sensors (temperature, light)

5. **I2C Communication** (Projects 16-19)
   - I2C protocol
   - OLED displays
   - Real-time clocks
   - Temperature/humidity sensors

6. **SPI Communication** (Projects 20-22)
   - SPI protocol
   - 7-segment displays
   - SD cards
   - Accelerometers

## Troubleshooting

### PICSimLab Won't Start
- Verify all dependencies are installed
- Check that PICSimLab was built and installed successfully
- Try running from terminal to see error messages

### Build Errors
- Ensure `gcc-avr` and `avr-libc` are installed
- Check Makefile recipe lines start with TAB characters (not spaces)
- Verify `-mmcu=atmega328p` flag is present

### Firmware Not Running
- **Critical**: Run `Tools` → `Arduino Bootloader` after loading hex file
- Ensure `.hex` file was generated successfully (`make` command)
- Check that simulation is running (Run button active)

### Hardware Not Responding
- Verify signals are configured correctly in Spare Parts window
- Check Pin Viewer to see actual pin states
- Ensure hardware components are added to the workspace
- Verify workspace file (.pzw) is loaded

## Resources

- [PICSimLab GitHub](https://github.com/lcgamboa/picsimlab)
- [PICSimLab Documentation](https://lcgamboa.github.io/picsimlab_docs/stable/)
- [PICSimLab Examples](https://lcgamboa.github.io/picsimlab_examples/examples_index.html)
- [ATmega328P Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/ATmega328P_Datasheet.pdf)
- [AVR Libc Documentation](https://www.nongnu.org/avr-libc/user-manual/)

## License

This learning material is provided for educational purposes. PICSimLab is licensed under GPL-2.0.
