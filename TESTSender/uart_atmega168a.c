/*
 *	aus baudy.c
 *	File:		uart.c --> uart_atmega168a.c
 *
 *	Created:	10.10.2011 20:09:40
 *	Author:		mf
 *	Verwendung des USART, serielle Schnittstelle in mehreren Varianten, Baudy light und Baudy Comfort
 *	
 *	Changed:	
 *	Author:		lukwata	
 *				for ATmega168A 12.11.2013
 *				11.5.2016
 */ 

#include <avr/io.h>
//#include <string.h>
#include "uart_atmega168a.h"	


/*  Assign I/O stream to UART stdout and stdin*/
FILE uart_str = FDEV_SETUP_STREAM(uart_putchar, uart_getchar, _FDEV_SETUP_RW); 
 

/* Baudy Comfort: Definition der Ausgabefunktion */
int uart_putchar( char c, FILE* stream )
{
    if( c == '\n' )
        uart_putchar( '\r', stream );
 
    loop_until_bit_is_set( UCSR0A, UDRE0 );
    UDR0 = c;
    return 0;
}

/* Baudy Comfort: für Eingabe über scanf und umgeleitetem stdout */
int uart_getchar(FILE *stream)
{
   unsigned char ch;    
   while (!(UCSR0A & (1<<RXC0)));
   ch=UDR0;  

   /* Echo the output back to the terminal */
   uart_putchar(ch,stream);      

   return ch;
}

/* Initialisierung des USART (braucht man immer) */
void USART_Init( unsigned int ubrr)
{
	/* Set baud rate */
	UBRR0H = (unsigned char)(ubrr>>8);
	UBRR0L = (unsigned char)ubrr;
	/* Enable receiver and transmitter */
	UCSR0B = (1<<RXEN0)|(1<<TXEN0);

	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);  /* Asynchron 8N1 */
	
	stdout = stdin = &uart_str;

}

/* Baudy Light: primitive Ausgabefunktion */
void USART_Transmit(unsigned char data)
{
	/* wait for empty transmit buffer */
	while (!(UCSR0A & (1<<UDRE0)))
	{
		
	}
	UDR0=data;
}


/* Baudy Light: primitive Eingabefunktion */
unsigned char USART_Receive(void)
{
	/*wait for data to be received */
	while (! (UCSR0A & (1<<RXC0)))
	;
	/* get and return received data from buffer */
	return UDR0;
}


