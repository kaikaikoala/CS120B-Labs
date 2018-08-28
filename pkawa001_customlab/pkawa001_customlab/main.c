#include <avr/io.h>
#include "bit.h"
#include "io.c"
#include "timer.h"
#include <time.h>
#include <stdlib.h>

//Adding SM check list
// enumerate SM states, create tick function
//add period and recalculate gcd
//update task list
//instantiate task

//([1,16],[0,1])
struct object{
	unsigned char exist;
	unsigned char posX;
	unsigned char posY;
	unsigned char shape;
};

typedef struct _task {
	signed char state; //Task's current state
	unsigned long int period; //Task period
	unsigned long int elapsedTime; //Time elapsed since last task tick
	int (*TickFct)(int); //Task tick function
} task;

unsigned char GetKeypadKey();
void render( struct object* , unsigned char );
unsigned char calcPos( unsigned char , unsigned char );
unsigned long int findGCD( unsigned long int, unsigned long int);
unsigned char update_player( unsigned char );
unsigned char update_env( ) ;
void level_one_gen();
unsigned char collision() ;
void update_score() ;


// Struct for Tasks represent a running process in our simple real-time operating system.


//--------End Task scheduler data structure-----------------------------------

//////////////////////////////////////////////////////////////////////////
//Tasks
//////////////////////////////////////////////////////////////////////////
enum SM_keypad_states{ SM_keypad_init };
int SM_keypad_tick( int );

enum SM_LCD_states{ SM_LCD_init };
int SM_LCD_tick( int );

enum SM_controller_states{ SM_controller_init, SM_controller_wait , SM_controller_press ,
	SM_controller_press_wait };
int SM_controller_tick( int );

enum SM_game_engine{ SM_game_engine_init , SM_game_engine_menu , SM_game_engine_play ,
	 SM_game_engine_win , SM_game_engine_lose, SM_game_engine_boot };
int SM_game_engine_tick( int );

//global variables
unsigned char keypad_output = 0x00;
unsigned char LCD_input = 0x00;
unsigned char player_input = 0x00;
unsigned char num_players = 0;
unsigned char env_cnt = 0 ;
unsigned char score = '0' ;
const unsigned char LEVEL = 5;
unsigned char score_str[3] = {'0','0','0'};
const unsigned char MENU_DISPLAY[]
 ={'A',' ','T','o',' ','p','l','a','y',' ',' ',' ',' ',' ',' ',' ',
	'B',' ','T','o',' ','m','a','k','e',' ','a','v','a','t','a','r'};
unsigned char LOSE_DISPLAY[32] = 
{'y','o','u',' ' ,'l','o','s','e',' ',' ',' ',' ',' ',' ',' ',' ',
	'S','o','r','e',':',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '};

struct object myObjects[32] ;
unsigned char size_myObjects = sizeof(myObjects)/sizeof(struct object);

int main(void)
{
	//Ports to be used
	DDRB = 0xFF; PORTB = 0x00; // PORTB set to output, outputs init 0s
	DDRC = 0xF0; PORTC = 0x0F; // PC7..4 outputs init 0s, PC3..0 inputs init 1s
	DDRD = 0xFF; PORTD = 0x00;

	//Periods
	unsigned long int SM_keypad_period = 100;
	unsigned long int SM_LCD_period = 100;
	unsigned long int SM_controller_period = 100;
	unsigned long int SM_game_engine_period = 100;

	//GCD
	unsigned long int gcd;
	gcd = findGCD( SM_keypad_period , SM_LCD_period );
	gcd = findGCD( gcd , SM_controller_period );
	gcd = findGCD( gcd , SM_game_engine_period );

	//task array
	static task SM_keypad_task , SM_LCD_task , SM_controller_task , SM_game_engine_task ;
	task *tasks[] = { &SM_keypad_task , &SM_controller_task ,
		&SM_game_engine_task, &SM_LCD_task  };
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
	//SM_game_engine_task
	SM_game_engine_task.state = -1;
	SM_game_engine_task.period = SM_game_engine_period/gcd;
	SM_game_engine_task.elapsedTime = SM_game_engine_period/gcd;
	SM_game_engine_task.TickFct= &SM_game_engine_tick ;
	//timer intialization
	TimerSet(gcd);
	TimerOn(0);

	//Tasks execution while loop
	unsigned char i = 0 ;
	unsigned char myAvatar[8] = {0x1b,0x00,0xa,0x0,0x0,0x11,0xe,0x0};
	LCD_build( &myAvatar , 1 );
	LCD_init();
	
	time_t t;
	srand( (unsigned) time(&t) );
	
	for(i=0 ; i< size_myObjects ; ++i ){
		myObjects[i].exist = 0 ;
	}

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

//wait, init, press
int SM_controller_tick(int state){
	switch(state){
		case SM_controller_init:
			state=SM_controller_wait;
			break;
		case SM_controller_wait:
			if( keypad_output != '\0'){
				player_input = keypad_output;
				state=SM_controller_press;
			}
			break;
		case SM_controller_press:
			state=SM_controller_press_wait;
			break;
		case SM_controller_press_wait:
			if(keypad_output=='\0'){
				state=SM_controller_wait;
			}
			break;
		default:
			state = SM_controller_init;
			break;
	}
	switch(state){
		case SM_controller_press_wait:
			player_input ='\0';
			break;
		default:
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
	}
	switch(state){
		case SM_LCD_init:
		LCD_ClearScreen();
		render(&myObjects, size_myObjects);
		default:
		break;
	}
	return state;
}

//init , menu , boot ,play , win , lose }
int SM_game_engine_tick( int state){
	//temporary game_engine_tick function
	int i=0;
	switch(state){
		case SM_game_engine_init:
			state = SM_game_engine_menu;
			break;
		case SM_game_engine_menu:
			if(player_input == 'A'){
				state = SM_game_engine_boot;
			}
			break;
		case SM_game_engine_boot:
			state = SM_game_engine_play;
			break;
		case SM_game_engine_play:
			if( !myObjects[1].exist ){
				state = SM_game_engine_lose;
			}
			break;
		case SM_game_engine_lose:
			if( player_input =='A'){
				state = SM_game_engine_init;
			}
			break;
		default:
			state = SM_game_engine_init;
			break;			
	}
	switch(state){
		//myObjects indecies
		//0 place holder
		//1-2 players
		//3-9 food
		//10-20 environment
		//20-31??
		case SM_game_engine_play:
			if( (env_cnt % 3) == 0 ){
				update_env();				
			}
			update_player( 1 );
			++env_cnt;
			
			break;
		case SM_game_engine_boot:
			for(i=0;i<32;++i){
				myObjects[i].exist = 0;
			}
			num_players = 1 ;
			//bug must have something at cursor 1?
			myObjects[0].exist = 1;
			myObjects[0].posX = 1;
			myObjects[0].posY = 0 ;
			myObjects[0].shape = ' ';
			//make evn
			level_one_gen();
			break;
		case SM_game_engine_menu:
			for(i=0;i<32;++i){
				myObjects[i].exist = 1;
				myObjects[i].shape = MENU_DISPLAY[i];
				myObjects[i].posX = i+1; //LCD Screen indexed at 1
				myObjects[i].posY = 0;
			}
			break;
		case SM_game_engine_init:
			for( i = 0 ; i < 3 ; ++i){
				score_str[i]='0';
			}
			score = 0 ;
			break;
		case SM_game_engine_lose:
			LOSE_DISPLAY[29]=score_str[0];
			LOSE_DISPLAY[30]=score_str[1];
			LOSE_DISPLAY[31]=score_str[2];
			for( i = 0 ; i<32 ; ++i ){
				myObjects[i].exist = 1;
				myObjects[i].shape = LOSE_DISPLAY[i];
				myObjects[i].posX = i+1;
				myObjects[i].posY = 0;
			}
			break;
		default:
			break;
	}
	return state;
}
//16

unsigned char update_player( unsigned char myPlayer ){
	//myObjects[0].shape++;
	if(player_input == '1'){
		myObjects[myPlayer].posY=0;
	}
	else if(player_input == '4'){
		myObjects[myPlayer].posY=1;
	}
	else if(player_input == '7' && myObjects[myPlayer].posX >1){
		myObjects[myPlayer].posX-=1;
	}
	else if(player_input =='8' && myObjects[myPlayer].posX <16 ){
		myObjects[myPlayer].posX+=1;
	}
	collision();
	return 0; //1 for player alive 0 for player dead
}

unsigned char collision(){
	for( char i = 10 ; i < 15 ; ++i ){ //thank you friend 
		if( myObjects[i].exist == 1 ){
			if( (myObjects[1].posX == myObjects[i].posX) && 
					(myObjects[1].posY == myObjects[i].posY) ){
				myObjects[1].exist=0;	
			}
		}
		//else nothing to check if there's collision
	}
	
	for( char i = 3 ; i < 4 ; ++i ){ //3-9 has food
		if( myObjects[i].exist == 1 ){
			if( (myObjects[1].posX == myObjects[i].posX) &&
			(myObjects[1].posY == myObjects[i].posY) ){
				myObjects[i].exist=0;
			}
		}
	}
	return 0;
}

void level_one_gen(){
	int i =0;
	
	//make world
	for( i = 10 ; i < 15 ; ++i ){
		myObjects[i].exist = 1 ;
		myObjects[i].shape = 'X' ;
	}
	myObjects[10].posX = 3;
	myObjects[10].posY = rand()%2;
	myObjects[11].posX = 5;
	myObjects[11].posY = rand()%2;
	myObjects[12].posX = 8;
	myObjects[12].posY = rand()%2;
	myObjects[13].posX = 12;
	myObjects[13].posY = rand()%2;
	myObjects[14].posX = 14;
	myObjects[14].posY = rand()%2;
	
	//make food		
	myObjects[3].exist = 1;
	myObjects[3].posX=2;
	myObjects[3].posY=1;
	myObjects[3].shape = 'f';
	
	//make first playerr
	myObjects[1].exist = 1;
	myObjects[1].posX = 1;
	myObjects[1].posY = 1 ;
	//Charater avatar in pos1
	myObjects[1].shape = num_players;
}

//10-20 environment //3-9 food
unsigned char update_env( ){
	int i=0;
	update_score();
	//make environment
	for( i = 10 ; i < 15 ; ++i ){
		if( myObjects[i].exist == 1 ){
			if( rand() % LEVEL == 0 ){
				//MOVement function could be random
				myObjects[i].posX -= 1;
				if(myObjects[i].posX < 3 ){
					myObjects[i].exist = 0;
				}
			}
		}
		else{
			myObjects[i].exist = 1;
			myObjects[i].posY = rand() % 2 ;
			myObjects[i].posX = 14;
		}
	}
	//feed food pls 3-9
	for( i = 3 ; i< 4 ; ++i ){
		if( myObjects[i].exist == 0 ){
			myObjects[i].exist = 1 ;
			myObjects[i].posY = rand() %2 ;
			if(myObjects[i].posX < 6 ){
				myObjects[i].posX =  15;
			}
			else{
				myObjects[i].posX = 1;
			}
		}
	}
	collision();
	return 0; ////////////////////////////////////NANI
}

void update_score(){
	++score;
	for(char i = 3 ; i< 4 ; ++i ){
		if( myObjects[i].exist == 0 ){
			++score_str[2];
			if( score_str[2] == '9'){
				score_str[2] = '0';
				++score_str[1];
				if( score_str[1] == '9' ){
					score_str[1] = '0';
					++score_str[0];
				}
			}
			
		}
	}
 
}

void render( struct object* objects , unsigned char size_objects){
	int i;
	 
	for( i = 0 ; i < size_objects ; ++i ){
		if( objects[i].exist ){
			LCD_Cursor( calcPos( objects[i].posX , objects[i].posY ) );
			LCD_WriteData(objects[i].shape );
		}
	}
}

unsigned char calcPos( unsigned char x , unsigned char y){
	if( y == 1 ){
		return x + 16;
	}
	else{ //y=0
		return x;
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