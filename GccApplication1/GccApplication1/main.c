/*
 * GccApplication1.c
 *
 * Created: 11.10.2019 07:55:26
 * Author : Bernhardt
 */ 

#include <avr/io.h>
#define F_CPU 3686400
#include "util/delay.h"

int main (void)
{
	DDRB = 0xFF;
	/* Insert system clock initialization code here (sysclk_init()). */
	/* Insert application code here, after the board has been initialized. */
	
	while(1)
	{
		PORTB=0x07;
		_delay_ms(1000);
		PORTB=0x00;
		_delay_ms(1000);
		PORTB=0x00;
	}
	return 0;
}

