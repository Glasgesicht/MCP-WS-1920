/*
 * GccApplication2.c
 *
 * Created: 11.10.2019 08:16:40
 * Author : Bernhardt
 */ 

#include <avr/io.h>
#define F_CPU 3686400
#define F_MIN F_CPU/50000
#define F_MAX F_CPU/200000

#define PS1 32
#define PS2 64


void ADC_Init(void) {
	ADMUX = (1<<REFS0); // AVcc Referenzspannung nutzen
	ADCSRA = (1<<ADPS2) | (1<<ADPS0); // Frequenzvorteiler (Faktor 32)
	ADCSRA |= (1<<ADEN); // ADC aktivieren
	/* nach Aktivieren des ADC wird ein "Dummy-Readout" empfohlen, man liest
	also einen Wert und verwirft diesen, um den ADC "warmlaufen zu lassen" */
	ADCSRA |= (1<<ADSC); // eine ADC-Wandlung
	while (ADCSRA & (1<<ADSC) ) {} // auf Abschluss der Konvertierung warten
	/* ADCW muss einmal gelesen werden, sonst wird Ergebnis der nächsten
	Wandlung nicht übernommen. */
	
	ADCW = ADCW;
}


/* ADC Einzelmessung */
uint16_t ADC_Read( uint8_t channel )
{
	// Kanal waehlen, ohne andere Bits zu beeinflußen
	ADMUX = (ADMUX & ~(0x1F)) | (channel & 0x1F);
	ADCSRA |= (1<<ADSC); // eine Wandlung "single conversion"
	while (ADCSRA & (1<<ADSC) ) {} // auf Abschluss der Konvertierung warten
	return ADCW; // ADC auslesen und zurückgeben
}


/* Hauptprogramm*/
int main()
{
	DRB |= ((1<<DDB3)|(1<<DDB2)|(1<<DDB1)|(1<<DDB0));// Setze PORTB Bit 0..3 auf Ausgang (LED 1.. LED3)
	uint16_t adcval;
	uint8_t kanal = 3; // Kanal 3: ADC3 an PortC3 anschließen: MUX3..0: 0011 ? 3
	ADC_Init(); // Initialisiere ADC
	while( 1 ) {
		adcval = ADC_Read(kanal); // Konvertiere (Kanal 3) – Single Conversion
		// TODO: LED ansteuern
	}
}

