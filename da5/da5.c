#define F_CPU 16000000UL //define the XTAL in the MicroController

// Ultra-Sonic Sensor
#define Trigger_pin PINB1	//This is the UltraSonic Sensors Trigger Pin

// USART output
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

// decimal to string dtostrf
#define PRECISION 0
#define MINWIDTH 1

// servo
#define SERVO180 600 // OCR1A value for 180 degrees
#define SERVO0 100 // OCR1A value for 0 degrees
#define DEGSTEP (SERVO180 - SERVO0)/180 // 1 degree step
#define STEPDELAY 50
#define WAITDELAY 1000

// libraries
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Global Variables
const double ANGLE_SCALE = 0.72; // 180 deg divided by number of servo steps
int TimerOverflow = 0;

//Interrupt Subroutines (for USS)
ISR(TIMER1_OVF_vect)
{
	TimerOverflow++;	//Increment Timer Overflow count
}
// initialize ultrasonic sensor
void UltrasonicInit() {
	/*Ultrasonic Initialization
	PB0 is the Echo Pin & PB1 is the Trigger*/
	
	//GPIO Programming
	DDRB = 0x02;	//Output for Ultrasonic Trigger Pin
	DDRC = 0xFF;	//PORTC as outputs for LEDs
	
	//Timer 1 Initialization
	TIMSK1 = (1 << TOIE1);	//Enable Timer1 overflow interrupts
	TCCR1A = 0;				//Set all bit to zero Normal operation
	
	//Enable Global Interrupts
	sei();
}

// Function to determine distance from ultrasonic sensor
double distance() {
	TCCR1B |= (1 << CS10);  //Pre-Scalar, Start Timer
	
	/*Declare variables*/
	long count;				//var to store the received input from ultrasonic
	double distance = 0;	//var to store the received distance from the USART
	
	/*Receive the UltraSonic sensors values*/
	PORTB |= (1 << Trigger_pin);			//Begin Trigger
	_delay_us(10);
	PORTB &= (~(1 << Trigger_pin));			//Cease Trigger
	TCNT1 = 0;								//Clear Timer counter
	TCCR1B = (1 << ICES1) | (1 << CS10);	//Capture rising edge, Pre-Scalar 1
	TIFR1 = (1 << ICF1) | (1 << TOV1);		// Clear ICP flag & Clear Timer Overflow flag
	
	/*Calculate width of Echo by Timer 1 ICP*/
	while ((TIFR1 & (1 << ICF1)) == 0);		// Wait for rising edge
	TCNT1 = 0;								// Clear Timer counter
	TCCR1B = (1 << CS10);					// Capture falling edge
	TIFR1 = (1 << ICF1) | (1 << TOV1);		// Clear ICP flag & Clear Timer Overflow flag
	TimerOverflow = 0;						// Clear Timer overflow count
	while ((TIFR1 & (1 << ICF1)) == 0);		//Wait for falling edge
	count = ICR1 + (65535 * TimerOverflow);	//Take value of capture register and calculate width
	distance = (double)count / (58*16);		//Calculate Distance
	
	TCCR1B ^= (1 << CS10);  //No Pre-Scalar, Stop Timer
	return distance;			//Return distance
}

//initialize USART
void USART_init(unsigned int ubrr)
{
	//Set baud rate
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char) ubrr;
	// enable transmitter
	UCSR0B = (1<<TXEN0);
	// Set frame format: async, no parity, 1 stop bit, , 8 data bits
	UCSR0C = (0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00);
}
//send strings thru USART
void USART_transmit(const char* data)
{
	while (*data) {
		//check if buffer is empty so that data can be written to transmit
		while (!(UCSR0A & (1 << UDRE0)));
		UDR0 = *data; //copy “data” to be sent to UDR0
		++data;
	}
}
//send characters thru USART
void USART_transmitChar(const char data)
{
	//check if buffer is empty so that data can be written to transmit
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data; //copy character to be sent to UDR0
}

int main()
{
	USART_init(MYUBRR);
	UltrasonicInit();
	//Configure TIMER1
	// OC3A is output to PIN D0
	TCCR3A|=(1<<COM3A1)|(1<<COM3B1)|(1<<WGM31);        // clear OC1A/OC1B on compare match, NON Inverted PWM (period resets OC1A to high at BOTTOM)
	TCCR3B|=(1<<WGM33)|(1<<WGM32)|(1<<CS31)|(1<<CS30); // PRESCALER=64, MODE 14(FAST PWM)

	ICR3=4999;  //fPWM=50Hz (Period = 20ms Standard).

	DDRD |= (1<<PIND0);   //PWM Pins as Out
	
	OCR3A = SERVO0;
	_delay_ms(WAITDELAY);
	
	while(1)
	{
		char strAng[20];
		char strDist[10];
		double angle; 
		double dist;
		
		int j = 0; // counter tracking servo steps
		for(int i = SERVO0; i < SERVO180; i+= DEGSTEP) {
			_delay_ms(STEPDELAY);
			OCR3A = i; // servo position
			angle = j * ANGLE_SCALE; // calculate angle
			dtostrf(angle, MINWIDTH, PRECISION, strAng); // turn angle into integer string
			USART_transmit(strAng);
			USART_transmitChar(',');
			++j;
			
			dist = distance(); // get distance from ultra-sonic sensor
			dtostrf(dist, MINWIDTH, PRECISION, strDist); // turn distance into integer string
			USART_transmit(strDist);
			USART_transmitChar('.');
		}
		_delay_ms(WAITDELAY);
		for(int i = SERVO180; i > SERVO0; i-= DEGSTEP) {
			_delay_ms(STEPDELAY);
			OCR3A = i; // servo position
			angle = j * ANGLE_SCALE; // calculate angle
			dtostrf(angle, MINWIDTH, PRECISION, strAng); // turn angle into integer string
			USART_transmit(strAng);
			USART_transmitChar(',');
			--j;
			
			dist = distance();
			dtostrf(dist, MINWIDTH, PRECISION, strDist); // turn distance into integer string
			USART_transmit(strDist);
			USART_transmitChar('.');
		}
		_delay_ms(WAITDELAY);
	}
	return 0;
}
