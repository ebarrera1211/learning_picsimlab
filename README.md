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

## Repository Structure

```
learning_picsimlab/
├── README.md                 # This file
├── common/                   # Shared files (optional)
│   ├── Makefile.template
│   └── avr_io.h
└── projects/                 # Individual project directories
    ├── 01_blink/
    │   ├── main.c
    │   ├── Makefile
    │   ├── README.md        # Project-specific documentation
    │   └── 01_blink.pzw     # PicsimLab workspace file
    ├── 02_multiple_leds/
    │   ├── main.c
    │   ├── Makefile
    │   ├── README.md
    │   └── 02_multiple_leds.pzw
    ├── 03_button/
    │   ├── main.c
    │   ├── Makefile
    │   ├── README.md
    │   └── 03_button.pzw
    ├── 04_traffic_light/
    │   ├── main.c
    │   ├── Makefile
    │   ├── README.md
    │   └── 04_traffic_light.pzw
    └── 05_timer_blink/
        ├── main.c
        ├── Makefile
        ├── README.md
        └── 05_timer_blink.pzw
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
   This compiles `main.c` and generates `main.hex`.

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
   - This step is required for Arduino Uno boards to execute firmware

6. **Run the Simulation**
   - Click the `Run` button (green triangle) in the main window
   - The simulation will start and the firmware will execute

7. **Observe Behavior**
   - LEDs will blink, buttons will respond, etc.
   - Use `Tools` → `Pin Viewer` to inspect pin states for debugging

8. **Save Workspace (Optional)**
   - If you make hardware configuration changes:
   - `File` → `Save Workspace` to update the `.pzw` file

### Project-Specific Details

Each project has its own `README.md` file with detailed information:

- **Project 1**: [01_blink/README.md](projects/01_blink/README.md) - Basic GPIO and LED blinking
- **Project 2**: [02_multiple_leds/README.md](projects/02_multiple_leds/README.md) - Multiple GPIO pins and binary representation
- **Project 3**: [03_button/README.md](projects/03_button/README.md) - Digital input and button debouncing
- **Project 4**: [04_traffic_light/README.md](projects/04_traffic_light/README.md) - State machines and non-blocking timing
- **Project 5**: [05_timer_blink/README.md](projects/05_timer_blink/README.md) - Hardware timers and interrupts

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
