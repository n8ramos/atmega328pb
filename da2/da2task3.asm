.org 0x0000
	jmp setup
.org 0x0004
	jmp INT1_ISR

setup:
	// initialization of ports and Interrupts
	ldi r17, 0x00			// load r17 to all 0s
	ldi r18, 0b00110000		// load register for DDRD
	out DDRB, r18			// initialize pins 5 and 4 for output
	out DDRD, r17			// initialize all PORT D for input
	out PORTB, r18			// initialize LEDs off
	out PORTD, r16			// activate pull-ups for all PORT D
	ldi r19, 2				// load register for EIMSK
	out EIMSK, r19			// enable interrupt 1
	ldi r19, 0b00001100		// load register for EICRA
	sts EICRA, r19			// trigger INT1 on rising edge
	// initialization of timer/counter1 
	ldi r23, 0x02			// register used for OCF1A flag
	ldi r20, 0 << WGM10		// sets WGM1[1:0] for ctc
	ldi r19, 5				// sets CS1[2:0] for prescalar
	ori r19, 1 << WGM12		// sets WGM1[3:2] for ctc
	sts TCCR1A, r20			// sets CTC bits 
	sts TCCR1B, r19			// Sets prescalar = 1024 and CTC bits
	sts TIMSK1, r23			// sets OCIE1A bit to enable CTC comparator A
	// sets the high and low bits of OCR1A for CTC mode
	ldi r20, 0x19			// 1/3 second delay
	ldi r19, 0x6D			// 1/3 second delay
	sts OCR1AH, r20			// 1/3 second delay
	sts OCR1AL, r19			// 1/3 second delay
	ldi r18, 1 << PINB4		// load register for toggling LED
	mov r16, r18			// set register for data output to PORT B
	sei						// enable interrupts

main:
	rjmp main		// normal status
	
delay:
	in r28, TIFR1	// load value of TIFR1
	and r28, r23	// mask all bits except OCF1A
	cp r28, r23		// check if OCF1A flag is set
	brne delay		// check if 1/3 second is reached
	out TIFR1, r23	// reset OCF1A flag
	ret

INT1_ISR:
	cli				// clear interrupts
	eor r16, r18	// toggle LED on
	out PORTB, r16	// output to LED
	ldi r21, 9		// load num of 1/3 second delays desired
loopdelay:
	cp r21, r17		// check if counter = 0
	breq done		
	rcall delay		// call delay
	subi r21, 1		// decrement counter
	rjmp loopdelay	// loop
done:
	eor r16, r18	// toggle LED off
	out PORTB, r16	// output to LED
	sei				// enable interrupts
	rjmp main		// return