/*
 * pkawa001_lab1_part1.c
 *
 * Created: 7/30/2018 2:28:49 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
    /* Replace with your application code */
   	DDRA = 0x00; PORTA = 0xFF;
   	DDRB = 0xFF; PORTB = 0x00;
   			
   	unsigned char tmpA = 0x00;
   	unsigned char tmpB = 0x00;
   
    while (1) 
    {
			//if it is dark outside and the door is open turn the led on
			//light outside PA1 = 1
			//door is open PA0 = 1	
			tmpA = PINA & 0x01; //door
			tmpB = PINA & 0x02; //light sensor
			
		if (tmpA == 0x01 && tmpB == 0x00) { // True if PA0 is 1
			tmpB = (tmpA & 0xFC) | 0x01; // Sets tmpB to bbbbbb01
			// (clear rightmost 2 bits, then set to 01)
		}
		else {
			tmpB = (tmpB & 0xFC) | 0x00; // Sets tmpB to bbbbbb10
			// (clear rightmost 2 bits, then set to 10)
		}
		// (3) Write output
		PORTB = tmpB;			
    }
}

