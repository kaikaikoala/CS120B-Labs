/*
 * jpark259_lab6_part1.c
 *
 * Created: 8/9/2018 12:57:49 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
#include "io.c"

int main(void)
{
	DDRC = 0xFF; PORTC = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
	
	// Initializes the LCD display
	LCD_init();
	
	// Starting at position 1 on the LCD screen, writes Hello World
	LCD_DisplayString(1, "Hello World");
	
	while(1) {continue;}
}

