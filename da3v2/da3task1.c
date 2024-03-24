#include <avr/io.h>
#define F_CPU 16000000;

unsigned int counter = 0;

int main(void)
{
	DDRB |= (1 << PINB5); // PB5 is an output
	// Set the Timer Mode to normal
	// prescalar set to 64
	TCCR0B |= (1 << CS01) | (1 << CS00);
	// initialize counter
	TCNT0 = 0;
	while(1) {
		
		// check if 0.1ms has been reached
		if (TCNT0 == 24) {
			counter++;
			TCNT0 = 0;
		} 
		// check for 10,000 ticks of 0.1ms (aka 1 second delay)
		if(counter == 10000) {
			PORTB ^= (1 << PINB5); // EORs PINB5, thus blinking it
			counter = 0;
		}
	}
}
