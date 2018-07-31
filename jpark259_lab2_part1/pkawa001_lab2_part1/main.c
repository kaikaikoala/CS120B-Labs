/*	Partner(s) Name & E-mail: Philip Kaishin Kawada pkawa001@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab # 2 Exercise # 1
 *	Exercise Description:
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */

#include <avr/io.h>

// Bit-access function
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; // Configure port A's 8 pins as inputs
	DDRB = 0x00; PORTB = 0xFF; // Configure port B's 8 pins as outputs,
	DDRC = 0xFF; PORTC = 0x00;
	// initialize to 0s
	unsigned char count = 0x00; // intermediate variable used for port updates
	unsigned char pinsa = 0x00;
	unsigned char pinsb = 0x00;
	
    while (1) 
    {
		count = 0;
		pinsa = PINA;
		pinsb = PINB;
		for(unsigned char i = 0; i < 8; i++){
			if(GetBit(pinsa,i)){
				count += 1;
			}
			if(GetBit(pinsb,i)){
				count += 1;
			}		
		}
		PORTC = count;
    }
}

