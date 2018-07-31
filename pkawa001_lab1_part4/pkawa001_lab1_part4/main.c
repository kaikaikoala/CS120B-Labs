/*
 * pkawa001_lab1_part4.c
 *
 * Created: 7/31/2018 11:48:08 AM
 * Author : ucrcse
 */ 

#include <avr/io.h>


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF; 
	DDRB = 0x00; PORTB = 0xFF; 
	DDRC = 0x00; PORTC = 0xFF;
	DDRD = 0xFF; PORTD = 0x00;

	unsigned short total = 0x00;
	unsigned char over = 0x00;
	signed short balance = 0x00;
	while(1){
		balance = PINA - PINC;
		total = (PINA + PINB + PINC) ;
		if((-80 > balance) || (balance > 80)){
			over = ( over & 0xFC) | 0x02; 
		}
		if(total > 140){
			over = ( over & 0xFE) | 0x01;
		} 
		total = total >> 2;
		over = (total & 0xFC) | over;
		PORTD = over;
	}
}

