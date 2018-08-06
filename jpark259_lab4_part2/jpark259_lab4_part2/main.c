

/*
 * jpark259_lab3_part2.c
 *
 * Created: 8/1/2018 1:26:21 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
enum States {init,base,up,down,max,min,zero} myState;

unsigned char PA , cnt=7 ;
	
void counter(){
	//transitions
	switch(myState){
		case( init ):
			myState = base;
			break;
		case(base):
			if( cnt == 9){
				myState = max;
			}
			else if( cnt ==0 || PA==0x03 ){
				myState = min ;	
			}
			else if (PA == 0x03){
				cnt = 0;
				myState = zero;
			}
			else if( PA == 0x01 ){
				myState = up;
			}
			else if( PA == 0x02 ){
				myState = down;
			}
			else{
				myState = base;
			}
			break;
		case(up):
			if(PA==0x01){
				myState=up;
			}
			else if(PA==0x03){
				myState=min;
			}
			else if(PA!=0x01){
				myState=base;
				++cnt;
			}
			break;
		case(down):
			if(PA==0x02){
				myState=down;
			}
			else if(PA==0x03){
				myState=min;
			}
			else if(PA!=0x02){
				myState=base;
				--cnt;
			}
			break;
		case(max):
			if(PA==0x03){
				myState=min;
			}
			else if(PA==0x02){
				myState=down;
			}
			else{
				myState=max;
			}
			break;
		case(min):
			if(PA==0x01){
				myState=up;
			}
			else{
				myState=min;
			}
			break;
		case(zero):
			cnt = 0;
			if(PA==0x01){
				myState=up;
			}else{
				myState=min;
			}
			break;
	}
	//action
	switch(myState){
		case(min):
			cnt=0;
			break;
		default:
			break;
	}
}

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	myState=init;
    /* Replace with your application code */
    while (1) 
    {
		PA = ~PINA & 0x03;
		counter();
		PORTB = cnt ;
    }
}
