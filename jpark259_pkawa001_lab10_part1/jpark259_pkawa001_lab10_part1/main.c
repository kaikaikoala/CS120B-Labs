#include <avr/io.h>
#include "bit.h"
#include "io.c"
#include "timer.h"

//Adding SM check list
// enumerate SM states, create tick function
//add period and recalculate gcd
//update task list
//instantiate task

//change io.c databus c to data bus b


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


//global variables
unsigned char keypad_output = 0x00;
unsigned char LCD_input = 0x00;

int main(void)
{
	//Ports to be used
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00;
	
	//Periods
	unsigned long int SM_keypad_period = 100;
	unsigned long int SM_LCD_period = 100;
	
	//GCD
	unsigned long int gcd;
	gcd = findGCD( SM_keypad_period , SM_LCD_period );	
	
	//task array
	static task SM_keypad_task , SM_LCD_task ;
	task *tasks[] = { &SM_keypad_task , &SM_LCD_task };
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

	
	//timer intialization
	TimerSet(gcd);
	TimerOn(0);
	//Tasks execution while loop
	unsigned char i = 0 ;
	
			
	while(1) {

		
		for( i=0 ; i<numTasks ; ++i ){
			if( tasks[i]->elapsedTime == tasks[i]->period ){
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				tasks[i]->elapsedTime = 0 ;
			}
			tasks[i]->elapsedTime+= 1 ;
		}

						switch (keypad_output) {
							case '\0': PORTB = 0x1F; break; // All 5 LEDs on
							case '1': PORTB = 0x01; break; // hex equivalent
							case '2': PORTB = 0x02; break;

							// . . . ***** FINISH *****

							case 'D': PORTB = 0x0D; break;
							case '*': PORTB = 0x0E; break;
							case '0': PORTB = 0x00; break;
							case '#': PORTB = 0x0F; break;
							default: PORTB = 0x1B; break; // Should never occur. Middle LED off.
						}	
		LCD_input = keypad_output;
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

