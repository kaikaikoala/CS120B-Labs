/*
 * pkawa001_lab2_part2.c
 *
 * Created: 7/31/2018 1:15:15 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>

unsigned char SetBit(unsigned char x, unsigned char k, unsigned char b) {
	return (b ? x | (0x01 << k) : x & ~(0x01 << k));
}
unsigned char GetBit(unsigned char x, unsigned char k) {
	return ((x & (0x01 << k)) != 0);
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
    
	unsigned char level = 0x00;
    while (1) 
    {
		level = PINB;
		if(PINA > 13){
			level = SetBit(level,0,1);
		}
		if(PINA > 10){
			level = SetBit(level,1,1);	
		}
		if(PINA > 7){
			level =SetBit(level,2,1);
		}
		if(PINA > 5){
			level = SetBit(level,3,1);
		}
		if(PINA > 3){
			level = SetBit(level,4,1);
		}
		if(PINA > 1){
			level =SetBit(level,5,1);
		}
		if(PINA < 5){
			level = SetBit(level,6,1);
		}
		PORTC = level;
    }
}

