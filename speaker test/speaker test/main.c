/*
 * speaker test.c
 *
 * Created: 8/14/2018 1:05:45 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
	DDRB = 0xFF; PORTB = 0x00;
    /* Replace with your application code */
    while (1) 
    {
		PORTB = 0xFF;
    }
}

