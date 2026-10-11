#ifndef AVR_IO_H
#define AVR_IO_H

#include <avr/io.h>
#include <util/delay.h>

// Bit manipulation macros
#define SET_BIT(port, bit)    (port |= (1 << bit))
#define CLEAR_BIT(port, bit)  (port &= ~(1 << bit))
#define TOGGLE_BIT(port, bit) (port ^= (1 << bit))
#define READ_BIT(port, bit)   ((port >> bit) & 1)

// Common pin definitions for Arduino Uno
#define LED_PIN      5  // PB5 (D13)
#define BUTTON_PIN   2  // PD2 (D2)

#endif // AVR_IO_H
