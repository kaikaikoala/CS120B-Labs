/*
 * Jpark259_Custom_Project.c
 *
 * Created: 8/23/2018 1:05:30 PM
 * Author : ucrcse
 */ 

#include <avr/io.h>
#include "io.c"
//#include "timer.h"
#include "bit.h"
enum States {init,moves} mystates;
unsigned char A = 0x00;

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.
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


void move(){
	switch(mystates){
		case init:
			break;
		case moves:
			mystates = moves;
			break;
	}
	switch(mystates){
		case moves:
			break;
	}
}

int main(void)
{
	
	DDRA = 0xFF; PORTA = 0x00; // LCD data lines
	DDRD = 0xFF; PORTD = 0x00; // LCD control lines
    //Replace with your application code 
	
	LCD_init();
	LCD_build();
	mystates = init;
	TimerSet(10);
	TimerOn();
    while (1) 
    {
		A = ~PINA;
		move();
		while (!TimerFlag);	// Wait 1 sec
		TimerFlag = 0;
    }
}

/*
unsigned char GetKeypadKey();
unsigned long int findGCD( unsigned long int, unsigned long int);

// Struct for Tasks represent a running process in our simple real-time operating system.
typedef struct _task {
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

//--------End Task scheduler data structure-----------------------------------

//////////////////////////////////////////////////////////////////////////
//Tasks
//////////////////////////////////////////////////////////////////////////
enum SM_keypad_states{ SM_keypad_init };
int SM_keypad_tick( int );

enum SM_LCD_states{ SM_LCD_init };
int SM_LCD_tick( int );

enum SM_controller_states{ SM_controller_init };
int SM_controller_tick( int );

//global variables
unsigned char keypad_output = 0x00;
unsigned char LCD_input = 0x00;

int main(void)
{
	//Ports to be used
	DDRA = 0xFF; PORTA = 0x00; // LCD data lines
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00;

	//Periods
	unsigned long int SM_keypad_period = 100;
	unsigned long int SM_LCD_period = 100;
	unsigned long int SM_controller_period = 100;

	//GCD
	unsigned long int gcd;
	gcd = findGCD( SM_keypad_period , SM_LCD_period );
	gcd = findGCD( gcd , SM_controller_period );

	//task array
	static task SM_keypad_task , SM_LCD_task , SM_controller_task ;
	task *tasks[] = { &SM_keypad_task , &SM_controller_task , &SM_LCD_task };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	//SM_keypad_task
	SM_keypad_task.state = -1;
	SM_keypad_task.period = SM_keypad_period/gcd ;
	SM_keypad_task.elapsedTime = SM_keypad_period/gcd ;
	SM_keypad_task.TickFct = &SM_keypad_tick ;
	//SM_LCD_task
	SM_LCD_task.state = -1 ;
	SM_LCD_task.period = SM_LCD_period/gcd;
	SM_LCD_task.elapsedTime = SM_LCD_period/gcd;
	SM_LCD_task.TickFct = &SM_LCD_tick;
	//SM_controller_task
	SM_controller_task.state = -1;
	SM_controller_task.period = SM_controller_period/gcd;
	SM_controller_task.elapsedTime = SM_controller_period/gcd;
	SM_controller_task.TickFct = &SM_controller_tick;

	//timer intialization
	TimerSet(gcd);
	TimerOn(0);

	//Tasks execution while loop
	unsigned char i = 0 ;
	LCD_init();


	while(1) {

		for( i=0 ; i<numTasks ; ++i ){
			if( tasks[i]->elapsedTime == tasks[i]->period ){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0 ;
			}
			tasks[i]->elapsedTime+= 1 ;
		}

		while(!TimerFlag);
		TimerFlag=0;
	}


}


int SM_keypad_tick(int state){
	//only one state in keypad state machine
	switch(state){
		case SM_keypad_init: break;
		default:
		state = SM_keypad_init;
		break;
	}
	switch(state){
		case SM_keypad_init:
		keypad_output = GetKeypadKey();
		break;
		default:
		break;
	}
	return state ;
}

int SM_controller_tick(int state){
	switch(state){
		case SM_controller_init:
		break;
		default:
		state = SM_controller_init;
	}
	switch(state){
		case SM_controller_init:
		if( keypad_output != '\0'){
			LCD_input = keypad_output;
		}
		//else{} do nothing
		break;
		default:
		break;
	}
}

int SM_LCD_tick( int state){
	switch( state ){
		case SM_LCD_init:
		break;
		default:
		state = SM_LCD_init ;
	}
	switch(state){
		case SM_LCD_init:
		LCD_ClearScreen();
		LCD_Cursor(0);
		LCD_WriteData(LCD_input);
		default:
		break;
	}
}

//--------GetKeypadKey() function----------------------------------
unsigned char GetKeypadKey() {
	PORTC = 0xEF; // Enable col 4 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('1'); }
	if (GetBit(PINC,1)==0) { return('4'); }
	if (GetBit(PINC,2)==0) { return('7'); }
	if (GetBit(PINC,3)==0) { return('*'); }

	// Check keys in col 2
	PORTC = 0xDF; // Enable col 5 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	if (GetBit(PINC,0)==0) { return('2'); }
	if (GetBit(PINC,1)==0) { return('5'); }
	if (GetBit(PINC,2)==0) { return('8'); }
	if (GetBit(PINC,3)==0) { return('0'); }
	// ... *****FINISH*****

	// Check keys in col 3
	PORTC = 0xBF; // Enable col 6 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	// ... *****FINISH*****
	if (GetBit(PINC,0)==0) { return('3'); }
	if (GetBit(PINC,1)==0) { return('6'); }
	if (GetBit(PINC,2)==0) { return('9'); }
	if (GetBit(PINC,3)==0) { return('#'); }
	// Check keys in col 4
	PORTC = 0x7F; // Enable col 6 with 0, disable others with 1’s
	asm("nop"); // add a delay to allow PORTC to stabilize before checking
	// ... *****FINISH*****
	if (GetBit(PINC,0)==0) { return('A'); }
	if (GetBit(PINC,1)==0) { return('B'); }
	if (GetBit(PINC,2)==0) { return('C'); }
	if (GetBit(PINC,3)==0) { return('D'); }
	// ... *****FINISH*****

	return('\0'); // default value

}

//--------Find GCD function --------------------------------------------------
unsigned long int findGCD(unsigned long int a, unsigned long int b){
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}
//--------End find GCD function ----------------------------------------------
*/