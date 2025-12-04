#define F_CPU 1600000UL
#include <avr/io.h>
#include <util/delay.h>
#include "../../include/HAL/lcd.h"
#include "../../include/MCAL/gpio.h"



int main(void)
{
    // Step 1: Initialize GPIO and LCD
    LCD_init(); // Call LCD initialization

    // Step 2: Display a test message on the LCD
    LCD_displayString("Hello, World!");

    // Optional: Test various functions of the LCD

    // Test: Move cursor and display on different lines
    _delay_ms(1000); // Wait for a bit
    LCD_moveCursor(1, 0);  // Move cursor to second row, first column
    LCD_displayString("Test 2nd Line");

    // Test: Clear LCD screen
    _delay_ms(1000); // Wait for a bit
    LCD_clearScreen(); // Clear screen
    LCD_displayString("Cleared!");

    // Infinite loop (as we don't want to halt execution)
    while (1) {
        // Additional code can be added here
    }

    return 0;  // Never reached
}