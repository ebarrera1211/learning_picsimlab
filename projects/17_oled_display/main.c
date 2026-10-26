#include "../common/avr_io.h"
#include "../common/i2c.h"

#define OLED_ADDR 0x78

// OLED initialization (simplified)
void oled_init(void) {
    i2c_start(OLED_ADDR);
    i2c_write(0x00);  // Command mode
    
    i2c_write(0xAE);  // Display OFF
    i2c_write(0xD5);  // Set clock divide
    i2c_write(0x80);
    i2c_write(0xA8);  // Set multiplex ratio
    i2c_write(0x3F);
    i2c_write(0xD3);  // Set display offset
    i2c_write(0x00);  // No offset
    i2c_write(0x40);  // Set display start line
    i2c_write(0x8D);  // Charge pump setting
    i2c_write(0x14);  // Enable charge pump (powers OLED panel)
    i2c_write(0x20);  // Set memory addressing mode
    i2c_write(0x00);  // Horizontal addressing mode
    i2c_write(0xA1);  // Segment remap
    i2c_write(0xC8);  // COM output scan direction
    i2c_write(0xDA);  // Set COM pins
    i2c_write(0x12);
    i2c_write(0x81);  // Set contrast
    i2c_write(0xCF);
    i2c_write(0xD9);  // Set pre-charge period
    i2c_write(0xF1);
    i2c_write(0xDB);  // Set VCOMH deselect level
    i2c_write(0x40);
    i2c_write(0xA4);  // Resume display content
    i2c_write(0xA6);  // Normal display
    i2c_write(0xAF);  // Display ON
    
    i2c_stop();
    
    _delay_ms(100);
}

// Clear display
void oled_clear(void) {
    i2c_start(OLED_ADDR);
    i2c_write(0x00);  // Command mode
    i2c_write(0x21);  // Set column address
    i2c_write(0x00);  // Start column
    i2c_write(0x7F);  // End column
    i2c_write(0x22);  // Set page address
    i2c_write(0x00);  // Start
    i2c_write(0x07);  // End page (7)
    i2c_stop();

    i2c_start(OLED_ADDR);
    i2c_write(0x40);  // Data mode
	
    for (uint32_t i = 0; i < 1024; i++) {
        i2c_write(0x00);  // Clear pixel (black)
	
	if ((i + 1) % 128 == 0) {
            i2c_stop();
	    _delay_ms(5);
	    i2c_start(OLED_ADDR);
	    i2c_write(0x40);
	}
    }
    i2c_stop();
}

// 8x8 font for "Hello World"
// Each character is 8 bytes (8 columns × 8 rows)
const uint8_t font_H[8] = {0xFF, 0x18, 0x18, 0x18, 0x18, 0x18, 0xFF, 0x00};
const uint8_t font_e[8] = {0xFF, 0x99, 0x99, 0x99, 0x81, 0x81, 0x81, 0x00};
const uint8_t font_l[8] = {0xFF, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x00};
const uint8_t font_o[8] = {0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF, 0x00};
const uint8_t font_W[8] = {0xFF, 0x80, 0x80, 0xF0, 0x80, 0x80, 0xFF, 0x00};
const uint8_t font_r[8] = {0xFF, 0x09, 0x19, 0x29, 0x49, 0x89, 0x0F, 0x00};
const uint8_t font_d[8] = {0xFF, 0x81, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00};
const uint8_t font_space[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
 
// Array of font pointers
const uint8_t* fonts[] = {font_H, font_e, font_l, font_o, font_W, font_r, font_d, font_space};

// Write character to display at position (x, y)
void oled_draw_char(uint8_t c, uint8_t x, uint8_t y) {
    uint8_t char_index = 0;	// Initializing a default

    if (c == 'H') char_index = 0;
    else if (c == 'e') char_index = 1;
    else if (c == 'l') char_index = 2;
    else if (c == 'o') char_index = 3;
    else if (c == 'W') char_index = 4;
    else if (c == 'r') char_index = 5;
    else if (c == 'd') char_index = 6;
    else if (c == ' ') char_index = 7;
    else return;
    
    const uint8_t* font = fonts[char_index];

    // Set window to character position
    i2c_start(OLED_ADDR);
    i2c_write(0x00);		// Command mode
    i2c_write(0x21);		// Set column address
    i2c_write(x);		// Start column
    i2c_write(x + 7);		// End column (x + 7 for 8-pixel wide char)
    i2c_write(0x22);		// Set page address
    i2c_write(y / 8);		// Start page
    i2c_write(y / 8);		// End page (same page for 8-row char)
    i2c_stop();

    // Send character data
    i2c_start(OLED_ADDR);
    i2c_write(0x40);		// data mode
    
    for (uint8_t col = 0; col < 8; col++) {
	    i2c_write(font[col]);
    }
    i2c_stop();
}

// Write "Hello World" at position (20, 28)
void oled_write_hello_world(void) {
    const char* text = "Hello World";
    uint8_t x = 10;  // Start x position
    uint8_t y = 24;  // Start y position (centered vertically)
    
    for (uint8_t i = 0; text[i] != '\0'; i++) {
        oled_draw_char(text[i], x, y);
        x += 8;  // Move to next character position
    }
}

void oled_fill_white(void) {
    i2c_start(OLED_ADDR);
    i2c_write(0x00);  // Command mode
    i2c_write(0x21);  // Set column
    i2c_write(0x00);  // Start column
    i2c_write(0x7F);  // End column (127)
    i2c_write(0x22);  // Set page address
    i2c_write(0x00);  // Start page
    i2c_write(0x07);  // End page (7)
    i2c_stop();

    i2c_start(OLED_ADDR);
    i2c_write(0x40);  // Data mode
        
    for (uint32_t col = 0; col < 1024; col++) {
        i2c_write(0xFF);  // Set all pixels white
        
	if ((col + 1) % 128 == 0) {
	    i2c_stop();
	    _delay_ms(5);
	    i2c_start(OLED_ADDR);
	    i2c_write(0x40);
	}
    }
    i2c_stop();
}

int main(void) {
    uart_init();
    i2c_init();
    
    uart_print_string("Initializing OLED...\r\n");
    oled_init();
    uart_print_string("Clearing display...\r\n");
    oled_clear();
    _delay_ms(5000);
    uart_print_string("Making whole screen light up...\r\n");
    oled_fill_white();
    _delay_ms(5000);
    uart_print_string("Clearing display...\r\n");
    oled_clear();
    _delay_ms(5000);
    uart_print_string("Writing Hello World...\r\n");
    oled_write_hello_world();
    uart_print_string("Done!\r\n");
    
    while(1) {
        _delay_ms(10000);
    }
    
    return 0;
}

