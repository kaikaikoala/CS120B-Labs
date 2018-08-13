#include <avr/io.h>
#include <avr/interrupt.h>
#include "io.c"

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.
enum states{ led0,led1, led2, press,release,victory,restart} myState ;
unsigned char cnt = 0 ;
unsigned char b = 0x00;
unsigned char button = 0x00;
unsigned char score = 5 ;

// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

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
	switch( myState ){
		case led0:
			if(button == 0x01){
				myState = press;
				score --;
			}
			else{
				myState=led1;
			}
			break;
		case led1:
			if(button == 0x01){
				if( score < 9){
					myState = press;
				}
				else{
					myState = victory;
				}
				++score;
			}
			else{
				myState = led2 ;
			}
			break;
		case led2:
			if(button==0x01){
				myState = press ;
				--score;
			}
			else{
				myState = led0;
			}
			break;
		case press:
			if(button == 0x01){
				myState = press;
				}else{
				myState = release;
			}
			break;
		case release:
			if(button == 0x01){
				myState = led0;
				}else{
				myState = release;
			}
			break;
		case victory:
			if(button ==0x01){
				myState = victory;
				
			}
			else if( button == 0x00){
				myState = restart;
			}
			break;
		case restart:
			if(button == 0x00 ){
				myState = restart;
			}
			else if( button ==0x00){
				myState = led0;
			}
	}
	switch( myState ){
		case led0:
		b=0x01;
		break;
		case led1:
		b=0x02;
		break;
		case led2:
		b=0x04;
		break ;
		case press:
		break;
		case release:
		break;
		case victory:
			score = 10;
			break;
		case restart:
			score = 5;
			break ;
	}
}

int main()
{
	DDRB = 0xFF; // Set port B to output
	PORTB = 0x00; // Init port B to 0s
	DDRA = 0x00;
	PORTA = 0xFF;
	DDRC = 0xFF ; PORTC=0x00;
	DDRD = 0xFF ; PORTD =0x00;
	TimerSet(300);
	TimerOn();
	LCD_init();
	
	while(1) {
		// User code (i.e. synchSM calls)
		button = ~PINA & 0x01;
		tick();
		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;

		PORTB = b;
		LCD_ClearScreen();
		LCD_Cursor(1);
		if( score < 10 ){
			LCD_WriteData( score + '0' );
		}
		else{
			LCD_DisplayString(1,"Victory");
		}
	}
}
