#include <avr/io.h>
#include <avr/interrupt.h>


// Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms.
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks

enum ThreeStates {led1,led2,led3} threestate;
enum BlinkStates {led01,led04} blinkstate;
enum SoundStates {up,down,wait} soundstate;

unsigned char cnt = 0;
unsigned char threeled = 0x00;
unsigned char blinkled = 0x00;
unsigned char combinedled = 0x00;
unsigned char A = 0x00;
unsigned char B4 = 0x00;
unsigned char tune = 1;

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

typedef struct {
	int state;				// Task’s current state
	unsigned long period;			// Task period
	unsigned long elapsedTime;		// Time elapsed since last task tick
	int (*TickFct)(int);			// Task tick function
}Task;

const unsigned char tasksSize = 4;
Task tasks[4];

void TimerISR() {
	unsigned char i;
	A = ~PINA;
	for (i = 0;i < tasksSize;++i) {
		if (tasks[i].elapsedTime >= tasks[i].period) {
			tasks[i].state = tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += 1;
	}
	PORTB = combinedled;
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


int ThreeLED(int state){
	switch(state){
		case led1:
		threeled = 0x01;
		state = led2;
		break;
		case led2:
		threeled = 0x02;
		state = led3;
		break;
		case led3:
		threeled = 0x04;
		state = led1;
		break;
	}
	return state;
}

int BlinkingLED(int state){
	switch(state){
		case led04:
		blinkled = 0x08;
		state = led01;
		break;
		case led01:
		blinkled = 0x00;
		state = led04;
		break;
	}
	return state;
}

int CombinedLED(int state){
	combinedled = blinkled | threeled | B4;
	return state;
}

int Sound(int state){
	switch(state){
		case up:
			if(A==0x01){
				state = up;
			}
			else if(A==0x00){
				state = wait;
			}
			break;
		case down:
			if( A==0x02){
				state=down;
			}
			else if(A==0x00){
				state = wait;
			}
			break;
		case wait:
			if(A==0x02){
				state = down;
				if(tasks[3].period!=0){
					tasks[3].period--;
				}
			}
			else if(A==0x01){
				state=up;
				if(tasks[3].period!=255){	
					tasks[3].period+=1;
				}
			} 
			break;
	}
	switch(state){
		case wait:
			B4 = 0x00;
			break;
		default:
			cnt++;
			B4=cnt%2?0x00:0x10;
			break;	
	}
	return state;
}
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	/* Replace with your application code */
	unsigned char i = 0;

	tasks[i].state = led1;
	tasks[i].period = 300;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &ThreeLED;
	i++;
	tasks[i].state = led04;
	tasks[i].period = 1000;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &BlinkingLED;
	i++;
	tasks[i].state = 0;
	tasks[i].period = tune;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &CombinedLED;
	i++;
	tasks[i].state = wait;
	tasks[i].period = tune;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &Sound;
	TimerSet(1);
	TimerOn();
	while (1){}
}