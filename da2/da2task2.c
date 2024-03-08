#define F_CPU 20000000
#include <avr/io.h>
#include <util/delay.h>

void delay(int thirds) { // takes input for 1/3 of a second
	for (;thirds > 0; thirds--) {
		_delay_ms(333); // loops to delay
	}
}

int main(void)
{
	
	DDRC = (0 << PINC2); // initialize PINC2 for input
	DDRB = (1 << PINB5); // initialize PINB5 for output
	
	PORTB = (1 << PINB5) | (1 << PINB4) | (1 << PINB3) | (1 << PINB2); // turn off all LEDS
	while (1)
	{
		if ((PINC & (1 << PINC2)) == (0 << PINC2)) { // check if PINC2 = 0
			PORTB ^= (1 << PINB5); // toggle LED on
			delay(6);			// delay 6 * 1/3 seconds
			PORTB ^= (1 << PINB5);	// toggle LED off
		}
	}
}
