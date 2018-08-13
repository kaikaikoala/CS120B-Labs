/*
 * jpark259_pkawa001_lab6_part1.c
 *
 * Created: 8/13/2018 12:04:01 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
#include "io.c"
#include <avr/interrupt.h>

unsigned char button = 0x00;
enum States {init, wait, increment, decrement, autoincrement, autodecrement, max, min} mystate;
unsigned char cnt = 0;
const unsigned char sec1 = 10;
unsigned char output = 0;

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

void tick(){
	switch(mystate){
		case init:
			mystate = wait;
			break;
		case wait:
			if(button == 0x00 ){
				mystate = wait;
			} else if (button == 0x01 && (output != 9)){
				output++;
				mystate = increment;
			} else if (button == 0x02 && (output != 0)){
				output--;
				mystate = decrement;
			} else if (button == 0x03) {
				mystate = init;	
			}
			break;
		case increment:
			if(button == 0x01 && cnt < sec1){
				mystate = increment;
			} else if (button == 0x01 && cnt>= sec1){
				mystate = autoincrement;
				cnt = 0;
				output++;
				if(output > 9){
					mystate = max;
				}
			} else if (button == 0x00){
				mystate = wait;
			} else if (button == 0x03){
				mystate = init;
			}
			break;
		case decrement:
			if(button == 0x02 && cnt < sec1){
				mystate = decrement;
			} else if (button == 0x02 && cnt>= sec1){
				mystate = autodecrement;
				cnt = 0;
				output--;
				if(output < 0){
					mystate = min;
				}
			} else if (button == 0x00){
				mystate = wait;
			} else if (button == 0x03){
				mystate = init;
			}
			break;
		case autoincrement:
			if(button == 0x01 && (output != 9)){
				mystate = autoincrement;
			} else if(button == 0x00){
				mystate = wait;
				cnt = 0;
			} else if (button == 0x03){
				mystate = init;
			} else if (output == 9){
				mystate = max;
			} else if (output == 0){
				mystate = min;
			}
			break;
		case autodecrement:
			if(button == 0x02 && (output != 0)){
				mystate = autodecrement;
			} else if(button == 0x00){
				mystate = wait;
				cnt = 0;
			} else if (button == 0x03){
				mystate = init;
			} else if (output == 9){
				mystate = max;
			} else if (output == 0){
				mystate = min;
			}
			break;
		case max:
			if (button == 0x02){
				mystate = decrement;
			} else if (button == 0x04){
				mystate = init;
			} else if (button == 0x01){
				mystate = max;
			}
			break;
		case min:
			if(button == 0x01){
				mystate = increment;
			} else if (button == 0x04){
				mystate = init;
			}
			break;
	}
	switch(mystate){
		case init:
			cnt = 0;
			output = 0;
			break;
		case increment:
			cnt++;
			break;
		case decrement:
			cnt++;
			break;
		case autoincrement:
			cnt++;
			if(cnt%sec1 == 0 && output <= 9){
				output++;
			}
			break;
		case autodecrement:
			cnt++;
			if(cnt%sec1 == 0 && output >= 0){
				output--;
			}
			break;
		case max:
			break;
		case min:
			break;
		default:
		break;
	}
}

int main(void)
{
   DDRA = 0x00; PORTA = 0xFF;
   DDRC = 0xFF; PORTC = 0x00; // LCD data lines
   DDRD = 0xFF; PORTD = 0x00; // LCD control lines
   TimerSet(100);
   TimerOn();
   
   // Initializes the LCD display
   LCD_init();
   
   // Starting at position 1 on the LCD screen, writes Hello World
 //  LCD_DisplayString(1, "Hello World");
   
   while(1) {
	   button = ~PINA & 0x03;
	   tick();
	   LCD_ClearScreen();
	   LCD_Cursor(1);
	   LCD_WriteData(output+'0');
	   while(!TimerFlag);
	   TimerFlag = 0;
	   continue;}
}

