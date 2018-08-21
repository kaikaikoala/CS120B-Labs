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

enum SM_LCD_states{ SM_LCD_init };
int SM_LCD_tick( int );

enum SM_controller_states{ SM_controller_begin , SM_controller_end };
int SM_controller_tick( int );

//global variables
unsigned char* LCD_input = 0x00;
unsigned char A = 0x00;


int main(void)
{
	//Ports to be used
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00;

	//Periods

	unsigned long int SM_LCD_period = 100;
	unsigned long int SM_controller_period = 100;

	//GCD
	unsigned long int gcd;
	gcd = findGCD( SM_controller_period , SM_LCD_period );

	//task array
	static task  SM_LCD_task , SM_controller_task ;
	task *tasks[] = { &SM_controller_task , &SM_LCD_task };
	const unsigned short numTasks = sizeof(tasks)/sizeof(task*);

	
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
		A = ~PINA;
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

int SM_controller_tick(int state){
	switch(state){
		case SM_controller_begin:
			if( (A&0x03) == 0x02){
				state = SM_controller_end ;
			}
			break;
		case SM_controller_end:
			if( (A&0x03) == 0x01){
				state=SM_controller_begin;
			}
			break;
		default:
			state = SM_controller_begin;
			break;
	}
	switch(state){
		case SM_controller_begin:
			LCD_input = "CS120B is Legend...";
			break;
		case SM_controller_end:
			LCD_input = "wait for it DARY!";
			break;
		default:
			LCD_input = "default";
			break;
	}
	return state;
}

int SM_LCD_tick( int state){
	switch( state ){
		case SM_LCD_init:
			break;
		default:
			state = SM_LCD_init ;
			break;
	}
	switch(state){
		case SM_LCD_init:
			LCD_ClearScreen();
			LCD_DisplayString(1,LCD_input);
		default:
		break;
	}
	return state;
}

//--------GetKeypadKey() function----------------------------------

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



