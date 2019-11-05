/*
 * BaudyLight1.c
 *
 * Created: 25.10.2019 08:16:20
 * Author : Bernhardt
 */ 

#include <avr/io.h>
#include <stdlib.h>
#include <stdio.h>

#define F_CPU 3686400UL
#define BAUD 9600UL      // Baudrate

#define MYUBRR (F_CPU/16/BAUD-1)  // Reale Baudrate

void uart_init(void);
int uart_putc(char c,FILE *stream);
int uart_recc(FILE *stream);

FILE uart_str = FDEV_SETUP_STREAM(uart_putc,uart_recc,_FDEV_SETUP_RW);




void uart_init(void)
{
	
	UBRR0H = (unsigned char)(MYUBRR>>8);
	UBRR0L = (unsigned char)MYUBRR;
		
	UBRR0 = MYUBRR;
	UCSR0B |= (1<<RXEN0)|(1<<TXEN0);
	// Frame Format: Asynchron 8N1
	UCSR0C = (1<<UCSZ01)|(1<<UCSZ00);
	

}

int uart_putc(char c,FILE *stream)
{
	 
	if(c == '\n')
	  uart_putc('\r',stream);
	  
	while(!(UCSR0A & (1<<UDRE0)))  /* warten bis Senden moeglich */
	{
		
	}

	UDR0 = c;                      /* sende Zeichen */
	return 0;
}



int uart_recc(FILE *stream)
{
	unsigned char ch;
	
	while (!(UCSR0A & (1<<RXC0)))  /* warten bis Senden moeglich */
	{
	}
	
	ch = UDR0;
	uart_putc(ch,stream);

	return UDR0; //received char
}



int main(void)
{
    /* Replace with your application code */
	
	int iin;
	
	uart_init();
	
	stdout = stdin = &uart_str;
	printf("Hallo Philipp\n");
	
    while (1) 
    {
        printf("Entar any number: ");
		scanf("%d",&iin);
		
		printf("\nYa Number is: %d\n",iin);
    }
}

