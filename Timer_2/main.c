/*
 * MCP_2Ampel.c
 * Gruppe Phan, Pörling, Scheibner
 * Created: 13.11.2019 15:37:46
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t counter = 0;

int main(void)
{
	// Overflow Interrupt erlauben
	TIMSK1 |= (1<<OCIE1A);
		
	// Timer 0 konfigurieren
	TCCR1B = (1<<WGM12);
	TCNT1 = (0b1111111111111111) - (0b0000000101101000) + (0b0000000000000001) ; //Same as 0xFFFF-360+1!!  // for 0.1 sec at PS 1024
	//will internally handled as 65176 (0xFE98) and count to 65535 (0xFFFF) to overflow
	
	TCCR1B = (1<<CS12)|(1<<CS10); //PS -> 1024
	
	//Richtungsregister PORTB
	DDRB |= (1<<DDB0)|(1<<DDB1)|(1<<DDB2)|(1<<DDB3);
    // PB0 are now output with pull-up enabled
	
	PORTB |= 1;

	// Global Interrupts aktivieren
	sei();
	
    while (1) 
    {
    }
}


ISR (TIMER1_COMPA_vect)
{
	TCNT1 = -360;
	if (counter<100){
	counter++;}else{
		PORTB ^=(0b111);
	}
	
}

ISR (PCINT0_vect)
{

}




