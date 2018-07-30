/*
 * pkawa001_lab1_part2.c
 *
 * Created: 7/30/2018 2:58:48 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0xFF; PORTB = 0x00; // Configure port B's 8 pins as outputs,
	// initialize to 0s
	unsigned char spot1 = 0x00; 	
	unsigned char spot2 = 0x00; 
	unsigned char spot3 = 0x00;
	unsigned char spot4 = 0x00;
	unsigned char cntavail = 0x04;
    while (1) 
    {
		spot1 = PINA & 0x01;	//Port A's pins 3 to 0
		spot2 = PINA & 0x02;
		spot3 = PINA & 0x04;
		spot4 = PINA & 0x08;
		cntavail = 0x04;
		if ( spot1 == 0x01){
			cntavail -= 0x01;
		}
		if ( spot2 == 0x02){
			cntavail -= 0x01;
		} 
		if ( spot3 == 0x04){
			cntavail -= 0x01;
		}
		if ( spot4 == 0x08){
			cntavail -= 0x01;
		}
		PORTB = cntavail;
    }
}

