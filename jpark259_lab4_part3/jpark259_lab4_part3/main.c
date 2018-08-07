/*
 * jpark259_lab4_part3.c
 *
 * Created: 8/7/2018 1:46:04 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
enum state {press, release} mystate;
unsigned char A = 0x00;
unsigned char i = 0;
unsigned char b = 0x00;
unsigned char light[] = {0xf3, 0x44 , 0xdd , 0x11 , 0x23 , 0xff , 0xd2 , 0x45 , 0xaa};
void tick(){
	switch(mystate){
		case press:
			if(A==0x00){
				
					++i;

				mystate = release;
			} else {
				mystate = press;
			}
			break;
		case release:
			if( A==0x00){
				mystate = release;
			}
			else if( A==0x01 ){
				b = light[i%9];
				mystate = press;
				
			}
			break;
	}
// 	switch(mystate){
// 		case press:
// 			//b = light[i%3];
// 		case release:
// 			//b = light[i];
// 	}
	
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	mystate = release;
    /* Replace with your application code */
    while (1) 
    {
		A = ~PINA;
		tick();
		PORTB = b;
    }
}

