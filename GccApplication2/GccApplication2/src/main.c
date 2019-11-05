/*
 * GccApplication2.c
 */ 

#include <avr/io.h>
#define F_CPU 3686400
#include "util/delay.h"


int main(void)
{
	DDRB |= (1<<DDB2)|(1<<DDB1)|(1<<DDB0);
	
	int8_t iLEDcnt =0;
	int8_t vz = 1;


    while (1) 
    {
	PORTB = (1<<iLEDcnt);
	_delay_ms(500);
	iLEDcnt+=vz;
	
	if(iLEDcnt>2){
		iLEDcnt =1;
		vz =-1;
	}else if (iLEDcnt<0){
		iLEDcnt=1;
		vz=1;
	}

    }
return 0;
}

