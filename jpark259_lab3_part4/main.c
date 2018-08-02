/*
 * jpark259_Lab3_part3.c
 *
 * Created: 8/2/2018 11:41:43 AM
 * Author : ucrcse
 */ 

#include <avr/io.h>
enum States {init,hpress,hrelease,ypress,yrelease,open,wait} lock;
unsigned char A;
unsigned char B;
void door(){

	switch(lock){
		case(init):
			if(A == 0x04){
				lock = hpress;
			} else if (A == 0x80){
				B = 0;
				lock = wait;
			} else {                        //A7 implicitly defined in else
				lock = wait;
			}
			break;
		case(hpress):
			if(A == 0x04){
				lock = hpress;
			} else if(A == 0x00) {
				lock = hrelease;
			}  else if (A == 0x80){
				B = 0;
				lock = wait;
			} else {
				lock = wait;
			}
			break;
		case(hrelease):
			if(A == 0x02){
				lock = ypress;
			} else if(A == 0x00){
				lock = hrelease;
			}  else if (A == 0x80){
				B = 0;
				lock = wait;
			} else {
				lock = wait;
			}
			break;
		case(ypress):
			if(A != 0x02){
				lock = yrelease;
			} else if ( A == 0x02){
				lock = ypress;
			} else if (A == 0x80){
				B = 0;
				lock = wait;
			} else {
				lock = wait;
			}
			break;
		case(yrelease):
			lock = open;
			break;
		case(open):
			if(B == 0){
				B = 1;
			} else if (A == 0x80){
				B = 0;
				lock = wait;
			} else {
				B = 0;
			}
			lock = wait;
			break;
		case(wait):
			if( A == 0x04){
				lock = hpress;
			}  else if (A == 0x80){
				B = 0;
				lock = wait;
			} else {
				lock = wait;
			}
	}
	switch(lock){
		case(init):
			B = 0;
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	lock = init;
	B = 0x00;
    /* Replace with your application code */
    while (1) 
    {
		A = PINA;
		door();
		PORTB = B;
    }
}

