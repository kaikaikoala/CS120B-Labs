#include <avr/io.h>
#include <bit.h>
#include <timer.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "io.c"
#include <string.h>

unsigned short numTasks = 2;
unsigned char A = 0x00;

unsigned long int findGCD(unsigned long int a, unsigned long int b)
{
	unsigned long int c;
	while(1){
		c = a%b;
		if(c==0){return b;}
		a = b;
		b = c;
	}
	return 0;
}

typedef struct _task {
	/*Tasks should have members that include: state, period,
		a measurement of elapsed time, and a function pointer.*/
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;


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
	PORTC = 0x7F;
	asm("nop");
	
	if (GetBit(PINC,0)==0) { return('A'); }
	if (GetBit(PINC,1)==0) { return('B'); }
	if (GetBit(PINC,2)==0) { return('C'); }
	if (GetBit(PINC,3)==0) { return('D'); }
	// ... *****FINISH*****

	return('\0'); // default value

}

char* from_key = " ";
char msg[] = "Welcome to The Game. Press 1 to start!";
enum keypad_states {title,menu,game,score,character} keypad_state = 0; 

unsigned char x;
unsigned char selection = 0;
unsigned char in;
unsigned char lastlocation = 0;
unsigned char currlocation = 0;
unsigned char map = 32;
unsigned char ind = 0;
char reading[25];

int keypad_tick (int state) {
	switch(state) {
		case title:
			if(selection == 4){
				selection = 0;
				state = menu;
			} else {
				state = title;
			}
			break;
		case menu:
			if(selection == 1){
				state = game;
			} else if (selection == 2){
				state = score;
			} else if (selection == 3){
				state = character;
			} else {
				state = menu;
			}
			break;
		case game:
			break;
		case character:
			break;
		default:
			state = menu;
			break;
	}
	
	switch(state) {
		case title:
			x = GetKeypadKey();
			switch(x){
				case '/0':
					break;
				case '1':
					x = 0;
					selection = 4;
					break;
				default:
					strncpy(reading, msg + ind, 16);
					reading[16] = '\0';
					LCD_ClearScreen();
					LCD_DisplayString(1, (const) reading);
					ind++;
					ind = ind % 23;
					
					break;
			}
			
			break;
		case menu:
			x = GetKeypadKey();
			switch (x) {
				case '\0': 
					break; 
				case 'A':
					selection = 1;
					
					break;
				case 'B':
					selection = 2;
					break;
				default: 
					LCD_DisplayString(1,"A-Play! B-Score  C-Character");
					LCD_Cursor_Off();
					break; 
			}
			break;
		case game:
			LCD_DisplayString(1,"GAME");
			break;
		case score:
			LCD_DisplayString(1,"SCORE");
			break;
		case character:
			LCD_DisplayString(1,"Character");
			break;
		default:
			state = title;
			break;
	}
	
	return state;
}

unsigned char in;

enum msg_states { read } msg_state = -1;
/*
int msg_Tick(int state) {
	switch(state) {
		case read:
			break;
		default:
			state = read;
			break;
	}
	switch(state) {
		case read:
			//LCD_ClearScreen();
			LCD_WriteData(from_key[0]);
			break;
	}
	return state;
}*/


int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0xFF;
	static task task1, task2;
	LCD_build();
	task1.state = keypad_state;
	task1.period = 250;
	task1.elapsedTime = 0;
	task1.TickFct = &keypad_tick;
	/*
	task2.state = msg_state;
	task2.period = 100;
	task2.elapsedTime = 0;
	task2.TickFct = &msg_Tick;
	*/
	LCD_init();

	in = 0;
	
	task *tasks[] = { &task1, &task2 };
	
	TimerSet(1);
	TimerOn();
	unsigned short i;
	while(1) {
		A = ~PINA;
		for ( i = 0; i < 1; i++ ) {
			// Task is ready to tick
			if ( tasks[i]->elapsedTime == tasks[i]->period ) {
				// Setting next state for task
				tasks[i]->state = tasks[i]->TickFct(tasks[i]->state);
				// Reset the elapsed time for next tick.
				tasks[i]->elapsedTime = 0;
			}
			tasks[i]->elapsedTime += 1;
		}
		while(!TimerFlag);
		TimerFlag = 0;
	}
}