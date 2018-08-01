/*
 * jpark259_lab3_part1.c
 *
 * Created: 8/1/2018 12:21:09 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>

enum Light_State {init, zero_offpressed, zero_offrelease, zero_onpressed} My_State;
unsigned char PB;
unsigned char PA;

void ledbutton(){
	PA = PINA & 0x03;
	switch(My_State){
		case(init):
			if(PA == 0x00){
				My_State = init;
			} else if (PA == 0x01) {
				My_State = zero_offpressed;
			}
			break;
		case(zero_offpressed):
			if(PA == 0x01){
				My_State = zero_offpressed;
			} else {
				My_State = zero_offrelease;
			}
			break;
		case(zero_offrelease):
			if(PA){
				My_State = zero_onpressed;
			} else {
				My_State = zero_offrelease;
			}
			break;
		case(zero_onpressed):
			if(PA){
				My_State = zero_onpressed;
			} else {
				My_State = init;
			}
			break;
		default:
			PB = 0x03;
	}
	switch(My_State){
		case(init):
			PB = 0x01 & 0x03;  //PB0 = 1 PB1 = 0			
			break;
		case(zero_offpressed):
			PB = 0x02 & 0x03;  //PB0 = 0 PB1 = 1
			break;
		case(zero_offrelease):
			PB = 0x02 & 0x03; //PB0 = 0 PB1 = 1
			break;
		case(zero_onpressed):
			PB = 0x01 & 0x03; //PB0 =1 PB1 = 0
			break;		
		default:
			PB = 0x03;
			break;
		
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
    /* Replace with your application code */
	PB = 0x00;
	PA = 0x00;
	My_State = init;
    while (1) 
    {
		ledbutton();
		PORTB = PB;
    }
}

