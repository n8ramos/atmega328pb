#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000;

unsigned int counter = 0;

ISR(TIMER2_OVF_vect) {
	cli();
	counter++;
	TCNT2 = 194;
	sei();
}

int main(void)
{
	DDRB |= (1 << PINB3); // PB3 is an output
	// Set the Timer Mode to Normal
	// Sets prescalar 64 
	TCCR2B |= (1 << CS22);
	// enable overflow interrupt
	TIMSK2 |= (1 << TOIE2);
	// initialize counter so that there is 0.25ms left before it overflows
	TCNT2 = 194;
	sei();
	while(1) {
		// check for 8000 ticks of 0.25ms (aka 2 seconds)
		if(counter == 8000) {
			PORTB ^= (1 << PINB3); // blink PINB3, LED D3
			counter = 0;
		}
	}
}
