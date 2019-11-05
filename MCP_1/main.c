/*
 * MCP Pflichtübung 1
 * Gruppe Phan, Pörling, Scheibner
 * Letzte Aktuallisierung: 05.11.2019
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#include "uart_atmega168a.h"
#include <stdlib.h>

#define F_CPU 3686400UL
#include <util/delay.h>
#define BAUD 9600UL
#define MYUBBR F_CPU/16/BAUD-1


volatile int cntr;
volatile int cntr_O;

volatile int cntr2;
volatile int cntr2_O;

volatile int rdz;

volatile uint8_t uart_rx_flag=0;            // Flag

void countdown(int i){

	printf("Das Rennen Startet in...");
	for(;i>0;i--){
	printf("\n\t...%i",i);
		_delay_ms(1000);}
	printf("\n\tL O S !\n");
}

int rundenzahl(){
	char iin[100] = "";
	int length, i;
	
	back:
	printf("\nGeben sie die Rundenzahl ein:");

	scanf("%s",iin);
	length = strlen(iin);
	for(i = 0; i< length;i++){
	if (!isdigit(iin[i]))
	{
		printf("ungueltige Eingabe!");
		goto back;
	}}

	printf("\nEin Rennen geht %i Runden!\n",atoi(iin));
	
	return 	atoi(iin);
}

void wartestart(){
	printf("\nDruecke 's', um das Rennen zu Starten!\nDruecke 'c', um das laufende Rennen abzubrechen!\n\n");
	while(UDR0 != 's'){}
}

int main(void)
{
	EICRA |= (1 << ISC01)|(1 << ISC00);
	EICRA |= (1 << ISC10)|(1 << ISC11);

	USART_Init(MYUBBR);
	EIMSK |= (1<<INTF0)|(1<<INTF1);
	
	sei();									// turn on interrupts (set global I-bit Flag)
	
	
	main:
	rdz = cntr = cntr2 = cntr_O = cntr2_O = rundenzahl()+1;		// Zähler mit Rundenzahl initiallisieren
	wartestart();
	countdown(5); //Countdown zum Start des Rennens
	/* Replace with your application code */
	while (UDR0!='c')
	{
		//Ausgelöst durch Interrupt
		if (cntr==cntr_O-1)
		{
			cntr_O=cntr;
			//Beim ersten Überfahren der Lichtschranke
			if (cntr== rdz-1){printf("Spur 1 - Rennen gestartet\n");}
			if (cntr==0)
			{
				printf("Spur 1 - Rennen beendet\n");
				goto main;
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