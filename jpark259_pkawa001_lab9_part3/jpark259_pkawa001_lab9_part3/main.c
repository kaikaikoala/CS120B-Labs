/*
 * jpakr259_pkawa001_lab9_part1.c
 *
 * Created: 8/16/2018 1:23:35 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks
volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

void TimerOn() {
	// AVR timer/counter controller register TCCR1
	TCCR1B = 0x0B;// bit3 = 0: CTC mode (clear timer on compare)
	// bit2bit1bit0=011: pre-scaler /64
	// 00001011: 0x0B
	// SO, 8 MHz clock or 8,000,000 /64 = 125,000 ticks/s
	// Thus, TCNT1 register will count at 125,000 ticks/s

	// AVR output compare register OCR1A.
	OCR1A = 125;	// Timer interrupt will be generated when TCNT1==OCR1A
	// We want a 1 ms tick. 0.001 s * 125,000 ticks/s = 125
	// So when TCNT1 register equals 125,
	// 1 ms has passed. Thus, we compare to 125.
	// AVR timer interrupt mask register
	TIMSK1 = 0x02; // bit1: OCIE1A -- enables compare match interrupt

	//Initialize avr counter
	TCNT1=0;

	_avr_timer_cntcurr = _avr_timer_M;
	// TimerISR will be called every _avr_timer_cntcurr milliseconds

	//Enable global interrupts
	SREG |= 0x80; // 0x80: 1000000
}

void TimerOff() {
	TCCR1B = 0x00; // bit3bit1bit0=000: timer off
}

void TimerISR() {
	TimerFlag = 1;
}

// In our approach, the C programmer does not touch this ISR, but rather TimerISR()
ISR(TIMER1_COMPA_vect) {
	// CPU automatically calls when TCNT1 == OCR1 (every 1 ms per TimerOn settings)
	_avr_timer_cntcurr--; // Count down to 0 rather than up to TOP
	if (_avr_timer_cntcurr == 0) { // results in a more efficient compare
		TimerISR(); // Call the ISR that the user uses
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

// Set TimerISR() to tick every M ms
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}


// 0.954 hz is lowest frequency possible with this function,
// based on settings in PWM_on()
// Passing in 0 as the frequency will stop the speaker from generating sound
void set_PWM(double frequency) {
	static double current_frequency; // Keeps track of the currently set frequency
	// Will only update the registers when the frequency changes, otherwise allows
	// music to play uninterrupted.
	
	if (frequency != current_frequency) {
		if (!frequency) { TCCR3B &= 0x08; } //stops timer/counter
		else { TCCR3B |= 0x03; } // resumes/continues timer/counter
		
		// prevents OCR3A from overflowing, using prescaler 64
		// 0.954 is smallest frequency that will not result in overflow
		if (frequency < 0.954) { OCR3A = 0xFFFF; }
		
		// prevents OCR0A from underflowing, using prescaler 64					// 31250 is largest frequency that will not result in underflow
		else if (frequency > 31250) { OCR3A = 0x0000; }
		
		// set OCR3A based on desired frequency
		else {OCR3A = (short)(8000000 / (128 * frequency)) - 1;}

		TCNT0 = 0; // resets counter
		current_frequency = frequency; // Updates the current frequency
	}

}

void PWM_on() {
	TCCR3A = (1 << COM0A0);
	// COM3A0: Toggle PB3 on compare match between counter and OCR0A
	TCCR3B = (1 << WGM02) | (1 << CS01) | (1 << CS00);
	// WGM02: When counter (TCNT0) matches OCR0A, reset counter
	// CS01 & CS30: Set a prescaler of 64
	set_PWM(0);
}

void PWM_off() {
	TCCR0A = 0x00;
	TCCR0B = 0x00;
}

enum States {init,off,play} mystate;
double note = 0.0;
#define notesize 27
double notes[] = {0,329.63,293.66,261.63,293.66,329.63,329.63,329.63,293.66,293.66,293.66,329.63,392.00,392.00,329.63,293.66,261.63,293.66,329.63,329.63,329.63,329.63,293.66,293.66,329.63,293.66,261.63};
unsigned char A = 0x00;
unsigned char i = 0;
unsigned char buttoncount = 0;

void tick(){
	switch(mystate){
		case init:
			mystate = off;
			break;
		case off:
			if(A == 0x01 && buttoncount == 0){
				buttoncount = 1;
				mystate = play;
			} else if (buttoncount == 1){
				mystate = init;
			} else {
				mystate = off;
			}
			break;
		case play:
			if(i != (notesize)){
				i++;
				mystate = play;
			}  else {
				mystate = init;
			}
			break;
	}
	switch(mystate){
		case init:
			i = 0;
			note = 0;
			break;
		case off:
			i = 0;
			note = 0;
			buttoncount = 0;
			break;
		case play:
			note = notes[i];
			break;
	}
}


int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(600);
	TimerOn();
	PWM_on();
	mystate = init;
    /* Replace with your application code */
    while (1) 
    {
		A = ~PINA;
		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;
		tick();
		set_PWM(note);
    }
}