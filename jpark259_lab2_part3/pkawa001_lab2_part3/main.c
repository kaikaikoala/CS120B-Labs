/*	Partner(s) Name & E-mail: Philip Kaishin Kawada pkawa001@ucr.edu
 *	Lab Section: 21
 *	Assignment: Lab # 2 Exercise # 3
 *	Exercise Description:
 *	
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
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
	unsigned char fuel = 0x00;
	unsigned char condition = 0x00;
    while (1) 
    {
		fuel = PINA & 0x0F;
		condition = (PINA & 0xF0)>>4;
		if(fuel > 13){
			level = SetBit(level,0,1);
		}
		if(fuel > 10){
			level = SetBit(level,1,1);	
		}
		if(fuel > 7){
			level =SetBit(level,2,1);
		}
		if(fuel > 5){
			level = SetBit(level,3,1);
		}
		if(fuel > 3){
			level = SetBit(level,4,1);
		}
		if(fuel > 1){
			level =SetBit(level,5,1);
		}
		if(fuel < 5){
			level = SetBit(level,6,1);
		} else {
			level = SetBit(level,6,0);
		}
		if(GetBit(condition,0) == 1 && GetBit(condition,1) == 1  && GetBit(condition,2) == 0){
			level = SetBit(level,7,1);
		} else {
			level = SetBit(level,7,0);
		}
		PORTC = level;
    }
}