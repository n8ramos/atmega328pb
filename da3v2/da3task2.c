#include <avr/io.h>
#include <avr/interrupt.h>
#define F_CPU 16000000;

unsigned int counter = 0;

ISR(TIMER1_COMPA_vect) {
	cli();
	counter++;
	sei();
}

int main(void)
{
	DDRB |= (1 << PINB4); // PB4 is an output
	// Set the Timer Mode to CTC
	// Sets CS1 to 0b101 -> prescalar 256; finish WGM1 to be 0b0100 for CTC
	TCCR1B |= (1 << WGM12) | (1 << CS12);
	TIMSK1 |= (1 << OCIE1A); // sets bit to enable Comparator A
	OCR1A = 0x1F; //0x7A11 for 0.1ms delays
	// initialize counter
	TCNT1 = 0;
	sei();
	while(1) {
		if(counter == 6000) {
			PORTB ^= (1 << PINB4); // blink PINB4, LED D2
			counter = 0;
		}
	}
}
