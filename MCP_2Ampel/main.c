/*
 * MCP_2Ampel.c
 * Gruppe Phan, Pörling, Scheibner
 * Created: 13.11.2019 15:37:46
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

				            //GR-GYR - BOGYR
#define a_gruen		PORTC =  (1<<PINC3)|(1<<PINC2)			  // (0b01100)
#define a_gelb		PORTC =  (1<<PINC3)|(1<<PINC1)			  // (0b01010)
#define a_rot		PORTC =  (1<<PINC3)|(1<<PINC0)			  // (0b01001)
#define f_gruen		PORTC =  (1<<PINC4)|(1<<PINC0)			  // (0b10001)
#define f_rot		PORTC =  (1<<PINC3)|(1<<PINC0)			  // (0b01001)
#define a_gelbrot	PORTC =  (1<<PINC3)|(1<<PINC1)|(1<<PINC0) // (0b01011)

volatile uint8_t counter = 0;

int main(void)
{
	// Timer Interrupt Mask Register
	// Overflow Interrupt erlauben
	TIMSK1 |= (1<<OCIE1A);
		
	// Timer 0 konfigurieren
	TCCR1B = (1<<WGM12);
	// for 1 sec at PS 1024, will internally handled as 61936 (0xF1EF) and count to  (0xFFFF)
	OCR1A = 3600-1;   
	TCCR1B = (1<<CS10)|(1<<CS12); //PS -> 1024
	
	//Richtungsregister PORTC
	DDRC |= (1<<DDC0)|(1<<DDC1)|(1<<DDC2)|(1<<DDC3)|(1<<DDC4);
	a_gruen; //Autofahrer hat zu Begin Gruen

    DDRB &= ~(1 << DDB0); // Clear the PB0 pin
    // PB0 are now inputs

    PORTB |= (1 << PB0); // turn On the Pull-up
    // PB0 are now inputs with pull-up enabled

    PCICR |= (1 << PCIE0);     // set PCIE0 to enable PCMSK0 scan
    PCMSK0 |= (1 << PCINT0);   // set PCINT0 to trigger an interrupt on state change

	// Global Interrupts aktivieren
	sei();
	
    while (1) 
    {
    }
}


ISR (TIMER1_COMPA_vect)
{
	counter++;
	//TCNT1 = 61936;   // for 1 sec // 3686400/1024 = 3600

	switch(counter){
	case 14: a_gelb; break;
	case 15: a_rot; break;
	case 16: f_gruen; break;
	case 21: f_rot; break;
	case 26: a_gelbrot; break;
	case 27: a_gruen; counter=0;
	}
}

ISR (PCINT0_vect)
{
	if(counter<13||counter>26){
		counter = 13;
	}
}




