/*
 * jpark259_pkawa001_lab8_part1.c
 *
 * Created: 8/15/2018 11:50:37 AM
 * Author : ucrcse
 */ 

#include <avr/io.h>

enum States {on,off} mystate;


void ADC_init();
void led(short adc, short maximum);

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned short max = 0x0022; // When covered with finger, measured 0x1F. We picked a value that is slightly greater than that.
								// tested 0x0021 & 0x0020, both did not work. Therefore, picked a value greater.
	
	mystate = off;

	ADC_init();
    /* Replace with your application code */
    while (1) 
    {
		unsigned short adcregister = ADC;  // Value of ADC register now stored in variable x
		//unsigned char lowerbyte = (char)adcregister;
		//unsigned char upper2 = (char)(adcregister>>8);
		led(adcregister,max);
    }
}

void led(short adc,short maximum){
	switch(mystate){
		case on:
			if(adc <= maximum) {
				mystate = off;
			}
			break;
		case off:
			if(adc >= maximum){
				mystate = on;
			}
			break;
	}
	switch(mystate){
		case on:
			PORTB = 0x01;
			break;
		case off:
			PORTB = 0x00;
			break;
	}
}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}