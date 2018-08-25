#ifndef MENU_H
#define MENU_H

char msg[] = "Welcome to The Game. Press 1 to start!";
enum keypad_states {title,menu,game,score,character} keypad_state = 0;
unsigned char x;
unsigned char selection = 0;
unsigned char ind = 0;
char reading[25];
unsigned char gamebool = 0;

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
			selection = 0;
			state = game;
			} else if (selection == 2){
			selection = 0;
			state = score;
			} else if (selection == 3){
			selection = 0;
			state = character;
			} else {
			state = menu;
		}
		break;
		case game:
		selection = 0;
		state = game;
		break;
		case score:
		selection = 0;
		break;
		case character:
		selection = 0;
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
		LCD_DisplayString(1,"A-Play! B-Score  C-Character");
		LCD_Cursor_Off();
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
			case 'C':
			selection = 3;
			default:
			LCD_DisplayString(1,"A-Play! B-Score  C-Character");
			LCD_Cursor_Off();
			break;
		}
		break;
		case game:
		gamebool = 1;
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


#endif 