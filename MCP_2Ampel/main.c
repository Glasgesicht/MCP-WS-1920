/*
 * MCP_2Ampel.c
 *
 * Created: 13.11.2019 15:37:46
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart_atmega168a.h"

				   //GR-GYR 
#define a_gruen		(0b01100)
#define a_gelb		(0b01010)
#define a_rot		(0b01001)
#define f_gruen		(0b10001)
#define f_rot		(0b01001)
#define a_gelbrot	(0b01011)

volatile uint8_t counter = 0;

int main(void)
{
	// Overflow Interrupt erlauben
	TIMSK1 |= (1<<OCIE1A);
		
	// Timer 0 konfigurieren
	TCCR1B = (1<<WGM12); 
	TCNT1 = -3600;   // for 1 sec at 16 MHz	
	TCCR1A = 0x00;
	TCCR1B = (1<<CS10)|(1<<CS12);
	
	
	//Richtungsregister PORTC
	DDRC |= (1<<DDC0)|(1<<DDC1)|(1<<DDC2)|(1<<DDC3)|(1<<DDC4);
	PORTC = a_gruen; //Autofahrer hat Gruen
	
    DDRB &= ~(1 << DDB0); // Clear the PB0, PB1, PB2 pin
    // PB0,PB1,PB2 (PCINT0, PCINT1, PCINT2 pin) are now inputs

    PORTB |= (1 << PB0); // turn On the Pull-up
    // PB0, PB1 and PB2 are now inputs with pull-up enabled
    
    PCICR |= (1 << PCIE0);     // set PCIE0 to enable PCMSK0 scan
    PCMSK0 |= (1 << PCINT0);   // set PCINT0 to trigger an interrupt on state change

	
	// Global Interrupts aktivieren
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
    }
}


ISR (TIMER1_COMPA_vect)
{
	counter++;
	TCNT1 = -3600;   // for 1 sec at 16 MHz	
	
	switch(counter){
	case 14: PORTC = a_gelb; break;
	case 15: PORTC = a_rot; break;
	case 16: PORTC = f_gruen; break;
	case 21: PORTC = f_rot; break;
	case 26: PORTC = a_gelbrot; break;
	case 27: PORTC = a_gruen; counter=0;
	}
}

ISR (PCINT0_vect)
{
	if(counter<=14||counter>=26){
		counter = 13;
	}
}




