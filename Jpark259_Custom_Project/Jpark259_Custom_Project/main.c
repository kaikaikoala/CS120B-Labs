#include <avr/io.h>
#include <bit.h>
#include <timer.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "io.c"
#include <string.h>

unsigned short numTasks = 2;

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



// Returns '\0' if no key pressed, else returns char '1', '2', ... '9', 'A', ...
// If multiple keys pressed, returns leftmost-topmost one
// Keypad must be connected to port C
/* Keypad arrangement
        PC4 PC5 PC6 PC7
   col  1   2   3   4
row
PC0 1   1 | 2 | 3 | A
PC1 2   4 | 5 | 6 | B
PC2 3   7 | 8 | 9 | C
PC3 4   * | 0 | # | D
*/
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

enum keypad_states {getkey} keypad_state = -1; 

unsigned char x;

unsigned char in;

int keypad_tick (int state) {
	switch(state) {
		case getkey:
			break;
		default:
			state = getkey;
			break;
	}
	
	switch(state) {
		case getkey:
			x = GetKeypadKey();
			switch (x) {
				case '\0': break; // All 5 LEDs on
				case '1':
					in++;
					break;
	/*			case '1': from_key[0] = '1'; break; // hex equivalent
				case '2': from_key[0] = '2'; break;
				case '3': from_key[0] = '3'; break;
				case '4': from_key[0] = '4'; break;
				case '5': from_key[0] = '5'; break;
				case '6': from_key[0] = '6'; break;
				case '7': from_key[0] = '7'; break;
				case '8': from_key[0] = '8'; break;
				case '9': from_key[0] = '9'; break;
				// . . . ***** FINISH *****
				case 'A': from_key[0] = 'A'; break;
				case 'B': from_key[0] = 'B'; break;
				case 'C': from_key[0] = 'C'; break;
				case 'D': from_key[0] = 'D'; break;
				case '*': from_key[0] = '*'; break;
				case '0': from_key[0] = '0'; break;
				case '#': from_key[0] = '#'; break;*/
				default: 
					LCD_Cursor(in + 1);
					LCD_WriteData(x); 
					in = (in + 1) % 16;
					break; // Should never occur. Middle LED off.
			}
			break;
	}
	
	return state;
}

unsigned char in;

enum msg_states { read } msg_state = -1;

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
}
unsigned char A = 0x00;

int main(void)
{
	DDRA = 0xFF; PORTA = 0x00;
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0xFF;
	static task task1, task2;
	
	task1.state = keypad_state;
	task1.period = 300;
	task1.elapsedTime = 0;
	task1.TickFct = &keypad_tick;
	
	task2.state = msg_state;
	task2.period = 100;
	task2.elapsedTime = 0;
	task2.TickFct = &msg_Tick;
	
	LCD_init();

	in = 0;
	
	task *tasks[] = { &task1, &task2 };
	
	TimerSet(1);
	TimerOn();
	LCD_DisplayString(1, "Congratulations");
	LCD_Cursor(1);
	
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