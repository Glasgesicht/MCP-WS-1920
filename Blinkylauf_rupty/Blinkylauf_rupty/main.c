/*
 * blinklauf.c
 *
 * Created: 10.10.2019 18:19:08
 * Author : d. lukwata
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile int8_t iLEDcnt;	// Zaehler bzw Bit, das auf 1 zu setzen ist
volatile int8_t vz;			// Vorzeichen

int main(void)
{
	DDRB |= ((1<<DDB2)|(1<<DDB1)|(1<<DDB0));// PB2 ... PB0 as output
	DDRD &= ~ (1 << DDD2);					// Clear PD2 pin, PD2 (INT0 pin) are now inputs
	PORTD |= (1 << PD2);					// Turn On the Pull-up on PD2
	EICRA |= (1 << ISC01)|(1 << ISC00);		// set INT0 to trigger on rising Edge
	PORTB &= ~((1<<PB2)|(1<<PB1)|(1<<PB0));	//Loesche PB2..PB0

	iLEDcnt	= 0;		// Zähler mit 0 starten bzw mit Bit 0 beginnen
	vz = 1;				// es wird zuerst incrementiert
	PORTB |= (1<<iLEDcnt);					// LED an Pin iLEDcnt an.

	EIFR |=	(1<<INTF0);						// Interrupstaufrufe von vorigen Flanken loeschen
	EIMSK |= (1 << INT0);					// Turns on INT1 and INT0
	sei();									// turn on interrupts (set global I-bit Flag)

	/* Replace with your application code */
	while (1)
	{
	}
}

ISR (INT0_vect)
{
	iLEDcnt += vz;			// naechste LED
	if (iLEDcnt>2)			// Da hier nur drei LEDs: Falls Bit 3,
	{
		iLEDcnt = 1;		// dann zurueck zu Bit 1
		vz = -1;			// und decrementieren (Vorzeichen: -)
	}
	else if (iLEDcnt<0)		// Falls Bit < 0,
	{
		iLEDcnt = 1;		// dann zurueck zu Bit 1
		vz = 1;				// und incrementieren (Vorzeichen: +)
	}
	PORTB =	(1<<iLEDcnt);	// LED an Pin iLEDcnt an

}


ISR (INT1_vect)
{
	/* Interrupt Code */
}



ISR(TIMER0_OVF_vect)
{
	/* Interrupt Code */
}

ISR(USART_RX_vect)
{
	/* Interrupt Code */
}
