#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void delay(int thirds) {
	for (;thirds > 0; thirds--) {
		_delay_ms(333);
	}
}

ISR(INT1_vect)
{
	cli();
	PORTB ^= (1 << PINB4);
	delay(9);
	PORTB ^= (1 << PINB4);
	sei();
}

int main(void)
{
	
	DDRB = (1 << PINB5) | (1 << PINB4);
	DDRD = (0 << PIND3);
	EIMSK |= (1 << INT1);
	EICRA |= (1 << ISC11) | (1 << ISC10);
	PORTB = (1 << PINB5) | (1 << PINB4) | (1 << PINB3) | (1 << PINB2);
	PORTD |= (1 << PIND3);
	sei();
	while (1);
}
