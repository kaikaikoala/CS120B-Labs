/*
 * jpark259_pkawa001_lab8_part1.c
 *
 * Created: 8/15/2018 11:50:37 AM
 * Author : ucrcse
 */ 

#include <avr/io.h>

unsigned char level = 0x00;

void ADC_init();
void led(short adc, short maximum);

int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned short max = 0x0027; 

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
	if(adc < 4){
		level = 0x00;
	} else if( adc < 8 ) {
		level = 0x01;
	} else if ( adc < 12 ) {
		level = 0x03;
	} else if ( adc < 16 ) {
		level = 0x07;
	} else if ( adc < 20 ) {
		level = 0x0F;
	} else if ( adc < 24 )  {
		level = 0x1F;
	} else if ( adc < 28 ) {
		level = 0x3F;
	} else if ( adc < 32 ) {
		level = 0x7F;
	} else if ( adc < 36 ) {
		level = 0xFF;
	}
	PORTB = level;
}

void ADC_init() {
	ADCSRA |= (1 << ADEN) | (1 << ADSC) | (1 << ADATE);
	// ADEN: setting this bit enables analog-to-digital conversion.
	// ADSC: setting this bit starts the first conversion.
	// ADATE: setting this bit enables auto-triggering. Since we are
	//        in Free Running Mode, a new conversion will trigger whenever
	//        the previous conversion completes.
}