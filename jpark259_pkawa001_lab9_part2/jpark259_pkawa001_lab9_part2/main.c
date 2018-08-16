/*
 * jpakr259_pkawa001_lab9_part1.c
 *
 * Created: 8/16/2018 1:23:35 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>


// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	
	if (frequency != current_frequency) {
		if (!frequency) { TCCR0B &= 0x08; } //stops timer/counter
		else { TCCR0B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR0A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR0A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else {OCR0A = (short)(8000000 / (128 * frequency)) - 1;}

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}

}

void PWM_on() {
	TCCR0A = (1 << COM0A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR0B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

enum States {init,onhold,on,offhold,off,up,down} mystate;
double note = 0.0;
double notes[] = {261.63, 293.66,329.63,349.23,392.00,440.00,493.88,523.25};
unsigned char A = 0x00;
unsigned char i = 0;

void tick(){
	switch(mystate){
		case init:
			break;
		case onhold:
			if(A == 0x04){
				mystate = onhold;
			} else {
				mystate = on;
			}
			break; 
		case on:
			if(A == 0x01){
				mystate = up;
			} else if ( A == 0x02) {
				mystate = down;
			} else if ( A == 0x04){
				mystate = offhold;
			}
			break;
		case offhold:
			break;
		case off:
			if(A == 0x04){
				mystate = onhold;
			} else {
				mystate = offhold;
			}
			break;
		case up:
			break;
		case down:
			break;
	}
	switch(mystate){
		case init:
			i = 0;
			break;
		case off:
			note = 0;
			break;
		case on:
			note = note[i];
	}
}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	
	PWM_on();
	
    /* Replace with your application code */
    while (1) 
    {
		A = ~PINA;
		tick();
		set_PWM(note);
    }
}