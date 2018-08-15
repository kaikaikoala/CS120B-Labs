/*
 * jpark259_pkawa001_lab8_part1.c
 *
 * Created: 8/15/2018 11:50:37 AM
 * Author : ucrcse
 */ 

#include <avr/io.h>


void ADC_init();

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	

	ADC_init();
    /* Replace with your application code */
    while (1) 
    {
		unsigned short adcregister = ADC;  // Value of ADC register now stored in variable x
		unsigned char lowerbyte = (char)adcregister;
		unsigned char upper2 = (char)(adcregister>>8);
		
		PORTB = lowerbyte;
		PORTD = upper2;
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