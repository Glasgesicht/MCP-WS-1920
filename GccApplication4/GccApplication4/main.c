/*
 * blinklauf.c
 *
 * Created: 10.10.2019 18:19:08
 * Author : d. lukwata
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart_atmega168a.h"

volatile int8_t iLEDcnt;	// Zaehler bzw Bit, das auf 1 zu setzen ist
volatile int8_t vz;			// Vorzeichen

#define F_CPU 3686400UL
#define BAUD 9600UL
#define MYUBBR F_CPU/16/BAUD-1


volatile int cntr;
volatile int cntrO;


int main(void)
{
	DDRD &= ~ (1 << DDD2);					// Clear PD2 pin, PD2 (INT0 pin) are now inputs
	PORTD |= (1 << PD2);					// Turn On the Pull-up on PD2
	EIMSK &= ~( 1<< INT0); 
	EICRA |= (1 << ISC01);		// set INT0 to trigger on rising Edge


	iLEDcnt	= cntr = cntrO = 0;		// Z�hler mit 0 starten bzw mit Bit 0 beginnen

	USART_Init(MYUBBR);
	printf("Hallo Philipp");
	
	EIFR |= (INTF0<<1);
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
