
#include <avr/io.h>
#include <avr/interrupt.h>


int main(void)
{
		// Overflow Interrupt erlauben
		TIMSK1 |= (1<<OCIE1A);
		
		
	DDRB = (1<<PB1);
	DDRB |= (1<<PB2);	
	
	TCCR1A = (1<<COM1A1)|(1<<WGM11);
	TCCR1B = (1<<WGM13)|(1<<WGM12)|(1<<CS10);
	
	sei();
	
	ICR1=0x0999;
	OCR1A = ICR1/2;
    /* Replace with your application code */
    while (1) 
    {
    }
}


ISR (TIMER1_COMPA_vect)
{
	if (ICR1> 0x9999){
		ICR1--;}
		else{
			ICR1++;
		}
}
