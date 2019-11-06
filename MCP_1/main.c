/*
 * MCP Pflichtübung 1
 * Gruppe Phan, Pörling, Scheibner
 * Letzte Aktuallisierung: 05.11.2019
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart_atmega168a.h"
#include <stdlib.h> //used for function atoi(const char *__s)
#include <string.h> //used for function strlen(const char *)
#include <ctype.h>  //used for function isdigit(int __c)

#define F_CPU 3686400UL
#include <util/delay.h>
#define BAUD 9600UL
#define MYUBBR F_CPU/16/BAUD-1

volatile int cntr;
volatile int cntr_O;

volatile int cntr2;
volatile int cntr2_O;

volatile int rdz;

void countdown(int i){
	printf("Das Rennen Startet in...");
	for(;i>0;i--){
	printf("\n\t...%i",i);
		_delay_ms(1000);}
	printf("\n\tL O S !\n");
}

int rundenzahl(){
	char iin[100] = ""; //expected no more than 100 characters as input
	int length, i;

	enternumber: //label for going back after incorrect input
	printf("\nGeben sie die Rundenzahl ein:");

	scanf("%s",iin); //input from terminal
	length = strlen(iin); //length of string entered
	for(i = 0; i< length;i++){
		if (!isdigit(iin[i])) //checks if any character input is not a digit
		{
			printf("\nungueltige Eingabe!");
			goto enternumber; //forces new, correct input from user
		}
	}

	printf("\nEin Rennen geht %i Runden!\n",atoi(iin));
	
	return 	atoi(iin); //array to integer
}

void wartestart(){
	printf("\nDruecke 's', um das Rennen zu Starten!\nDruecke 'c', um das laufende Rennen abzubrechen!\n\n");
	while(UDR0 != 's'){} //will break when 's' is pressed on the keyboard
}

int main(void)
{
	USART_Init(MYUBBR);

	//External Interrupt Control Register A 
	EICRA |= (1 << ISC01)|(1 << ISC00);
	EICRA |= (1 << ISC10)|(1 << ISC11);		// Sets Interrupts to be triggered on any rising edge

	//External Interrupt Mask Register
	EIMSK |= (1<<INTF0)|(1<<INTF1);			// Turns on INT0 and INT1
	
	sei();									// Turn on interrupts (set global I-bit Flag)

	main:
	rdz = cntr_O = cntr2_O = rundenzahl()+1;		// initialize counters with numbers of rounds.
													// +1 since numbers are deducted before any prints.
	wartestart();
	countdown(5); //Countdown for race to start. Parameter is length in seconds

	//will break when character 'c' is pressed on the keyboard
	
	cntr = cntr2 = rdz;
	while (UDR0!='c')
	{
		//Caused by interrupt event
		if (cntr==cntr_O-1)
		{
			cntr_O=cntr;
			if (cntr== rdz-1){printf("Spur 1 - Rennen gestartet\n");}     //Race starts after first passing of the beam sensor
			if (cntr==0)
			{
				printf("Spur 1 - Rennen beendet\n");

				goto main;				//Will effectively restart the program
			}else {printf("Spieler 1 noch: %i Runden\n",cntr);}
		}
		if (cntr2==cntr2_O-1)
		{
			cntr2_O=cntr2;
			if (cntr2 == rdz-1){printf("Spur 2 - Rennen gestartet\n");}
			if (cntr2==0)
			{
				printf("Spur 2 - Rennen beendet\n");
				goto main;
			}else {printf("Spieler 2 noch: %i Runden\n",cntr2);}
		}
	}
	//Restart the program when race is aborted
	
	printf("Renne abgebochen!\n\n");
	goto main;
}


ISR (INT0_vect)
{
	cntr = cntr_O-1;
}


ISR (INT1_vect)
{
	cntr2 = cntr2_O-1;
}