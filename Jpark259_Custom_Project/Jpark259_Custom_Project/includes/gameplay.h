#ifndef GAMEPLAY_H
#define GAMEPLAY_H

enum game_states {wait,player} game_state = 0;
enum map_states {waitmap, object} map_state = 0;
	
// FOR PLAYER
unsigned char lastlocation = 0;
unsigned char currlocation = 0;

int gameplay_tick(int state){
	switch(state){
		case wait:
			if(gamebool == 1){
				state = player;
			} else {
				state = wait;
			}
			break;
		case player:
			break;
	}
	switch(state){
		case waitmap:
			break;
		case player:
			LCD_DisplayString(1,"YEAHHHHHH");
			break;
	}
	return state;
}

int map_tick (int state){
	switch(state){
		case waitmap:
			if(gamebool == 1){
				state = object;
			} else {
				state = wait;
			}
			break;
		case object:
			break;
	}
	switch(state){
		case wait:
			break;
		case object:

			break;
	}
	return state;
}

#endif