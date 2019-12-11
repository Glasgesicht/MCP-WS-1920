
/*
*	Deklaration der globalen Variablen 
*	aus baudy.c
*	File:	uart.h --> uart_atmega168a.h
*	Author:	d. lukwata
*			updated 25.4.2019
*/

#ifndef _UART_H_
#define _UART_H_	

#include <stdio.h>

/*#define FOSC 12000000UL // Clock Speed*/
#define FOSC 3686400UL	/* Clock Speed */
#define BAUD 9600UL
#define MYUBRR FOSC/16/BAUD-1


/* Baudy Comfort: Definition der Basis-Ausgabefunktion*/
/* für Ausgaben über printf und umgeleitetem stdout*/
extern int uart_putchar( char c, FILE *stream );

/* Baudy Comfort: Definition der Basis-Einlesefunktion*/
/* für Einlesen über scanf und umgeleitetem stdin*/

extern int uart_getchar(FILE *stream);

/* Initialisierung des USART (braucht man immer)*/
extern void USART_Init( unsigned int ubrr);

/* Baudy Light: primitive Ausgabefunktion*/
extern void USART_Transmit(unsigned char data);

/* Baudy Light: primitive Eingabefunktion */
extern unsigned char USART_Receive(void);

extern FILE uart_str;

#endif /* _UART_H_ */