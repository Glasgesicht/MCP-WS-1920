
/*
* Abgabe Gruppe Scheibner, Pörrling, Phan
*
*
* Created: 02.10.2011 15:01:10
*  Author: mf
*  Updates
*  21.05.2019 Anpassung auf myAVR MK2, dominic lukwata
*  06.12.2019 Anpassung fuer Gruppenfrequenzen, dominic lukwata
*
*  10.12.2019 Erweiterung des Programmes zur Steuerung des Ampelprogrammes
*  entsprechend dem 3. Übungsblatt
*
*/

/********************************************************************
Beispiel f?r RFM12-Funkmodul (Senden)
IAR Embedded Workbench IDE, C
Prozessor: ATMEGA168a
Frequenz: 433.05MHz
Datenrate: 4.8kbps

Dieses Programm enthält eine Ampelsteuerung und eine Routine zum Empfang von Steuersignalen.
Über die Steuersignale lässt sich die Ampel in die Modi 'an' und 'aus versetzen'.

Im 'an' Modus verhält sich die Ampel so, wie schon im 2. Übungsblatt.

Empfängt das Programm ein Steuersignal, um in dem Modus 'aus' versetzt zu werden, blinkt das Gelbe Licht der Ampel im Sekundentakt.

Pinbelegung
-----------------------
ATMEGA168A        RFM12
-----------------------
SCK               SCK
MISO              SDO
MOSI              SDI
SS                nSEL
INT0              nIRQ
PC0               LED1
PC1               LED2
PD4               Module-Power

PD7               FSK
PD3               DCLK,FFIT
PB0               NINT, VDI
**********************************************************************/

// Please enter your group frequenz in MHz here. Default is 434.7825 MHz
#define GROUPFREQUENZ 433.05

#define RFM12_FREQUENCY_CALC_433()	((uint16_t)((GROUPFREQUENZ-430.0)*400))

#define F_CPU 3686400		// CPU-Takt
#define DDR_IN 0
#define DDR_OUT 1
#define PORT_SEL PORTB
#define PIN_SEL PINB
#define DDR_SEL DDRB
#define PORT_SDI PORTB
#define PIN_SDI PINB
#define DDR_SDI DDRB
#define PORT_SCK PORTB
#define PIN_SCK PINB
#define DDR_SCK DDRB
#define PORT_SDO PORTB
#define PIN_SDO PINB
#define DDR_SDO DDRB
#define PORT_IRQ PORTD
#define PIN_IRQ PIND
#define DDR_IRQ DDRD
#define PORT_DATA PORTD
#define PIN_DATA PIND
#define DDR_DATA DDRD
#define PB7 7 //
#define PB6 6 // |
#define RFXX_SCK 5// |
#define RFXX_SDO 4// |RF_PORT
#define RFXX_SDI 3// |
#define RFXX_SEL 2// |
#define NC 1// |
#define PB0 0//--/
#define SEL_OUTPUT() DDR_SEL |= (1<<RFXX_SEL)
#define HI_SEL() PORT_SEL|= (1<<RFXX_SEL)
#define LOW_SEL() PORT_SEL&=~(1<<RFXX_SEL)
#define SDI_OUTPUT() DDR_SDI |= (1<<RFXX_SDI)
#define HI_SDI() PORT_SDI|= (1<<RFXX_SDI)
#define LOW_SDI() PORT_SDI&=~(1<<RFXX_SDI)
#define SDO_INPUT() DDR_SDO&= ~(1<<RFXX_SDO)
#define LOW_SDO() PORT_SDO&= (1<<RFXX_SDO)
#define SDO_HI() PIN_SDO&(1<<RFXX_SDO)
#define SCK_OUTPUT() DDR_SCK |= (1<<RFXX_SCK)
#define HI_SCK() PORT_SCK|= (1<<RFXX_SCK)
#define LOW_SCK() PORT_SCK&=~(1<<RFXX_SCK)
#define RF12_IRQ 2
#define IRQ_IN() DDR_IRQ &=~(1<<RF12_IRQ)
#define WAIT_IRQ_LOW() while(PIND&(1<<RF12_IRQ))
#define RF12_DATA 7//PD7
#define DATA_OUT() DDR_DATA|=1<<RF12_DATA
#define HI_DATA() PORT_DATA|=1<<RF12_DATA
#define MODULE_OUTPUT() PORTD&=~(1<<4) //+Module-Power
#define MODULE_ON() PORTD|=(1<<4)
#define MODULE_OFF() PORTD&=~(1<<4)

#include <avr/io.h>
#include "uart_atmega168a.h"
#include "util/delay.h"		// enthaelt wartezeitroutinen
#include <avr/interrupt.h>


//GR-GYR - BOGYR  Belegung der Pins zum Ansteuern der Ampel
#define a_gruen		PORTC =  (1<<PINC3)|(1<<PINC2)			  // (0b01100)
#define a_gelb		PORTC =  (1<<PINC3)|(1<<PINC1)			  // (0b01010)
#define a_rot		PORTC =  (1<<PINC3)|(1<<PINC0)			  // (0b01001)
#define f_gruen		PORTC =  (1<<PINC4)|(1<<PINC0)			  // (0b10001)
#define f_rot		PORTC =  (1<<PINC3)|(1<<PINC0)			  // (0b01001)
#define a_gelbrot	PORTC =  (1<<PINC3)|(1<<PINC1)|(1<<PINC0) // (0b01011)

//Variable, mit der de Ampel an und aus geschaltet wird
volatile uint8_t ampelan = 1;
//Zeitmesser fuer interne Ampelsteuerung
volatile uint8_t counter = 0;
//ID unserer geliebten Ampel!
const unsigned char AMPELID = 11;

void RFXX_PORT_INIT(void){
	HI_SEL();
	HI_SDI();
	LOW_SCK();
	//SET nFFS pin HI when using FIFO
	HI_DATA();
	SEL_OUTPUT();
	SDI_OUTPUT();
	SDO_INPUT();
	SCK_OUTPUT();
	IRQ_IN();
	DATA_OUT();
}

unsigned int RFXX_WRT_CMD(unsigned int aCmd){
	unsigned char i;
	unsigned int temp;
	temp=0;
	LOW_SCK();
	LOW_SEL();
	

	for(i=0;i<16;i++){
		
		// writing data via serial line to RFM12
		if(aCmd&0x8000){
			HI_SDI();
			}else{
			LOW_SDI();
		}
		HI_SCK();
		
		// this is for reading incoming data from RFM12 via serial line
		temp<<=1;
		if(SDO_HI()){
			temp|=0x0001;
		}
		
		LOW_SCK();
		aCmd<<=1;
	};
	HI_SEL();
	return(temp);
}


void RF12_INIT(void){
	RFXX_WRT_CMD(0x80D7);//EL,EF,11.5pF
	RFXX_WRT_CMD(0x82D9);//!er,!ebb,ET,ES,EX,!eb,!ew,DC

	//	RFXX_WRT_CMD(0xA640);//434MHz
	RFXX_WRT_CMD(0xA000| RFM12_FREQUENCY_CALC_433());// Gruppenfrequenz einstellen

	RFXX_WRT_CMD(0xC647);//4.8kbps
	
	// 67kHz Bandbreite pro Frequenz / Seite
	RFXX_WRT_CMD(0x9000|0x400|0xC0|0x10|0x04);//VDI,FAST,134kHz,0dBm,-103dBm
	
	RFXX_WRT_CMD(0x94A0);//VDI,FAST,134kHz,0dBm,-103dBm
	RFXX_WRT_CMD(0xC2AC);//AL,!ml,DIG,DQD4
	RFXX_WRT_CMD(0xCA81);//FIFO8,SYNC,!ff,DR
	RFXX_WRT_CMD(0xC483);//@PWR,NO RSTRIC,!st,!fi,OE,EN
	RFXX_WRT_CMD(0x9850);//!mp,9810=30kHz,MAX OUT
	RFXX_WRT_CMD(0xE000);//NOT USE
	RFXX_WRT_CMD(0xC800);//NOT USE
	RFXX_WRT_CMD(0xC400);//1.66MHz,2.2V
}



unsigned char RF12_RECV(void){
	unsigned int FIFO_data;
	WAIT_IRQ_LOW();
	RFXX_WRT_CMD(0x0000); // read internal status register
	FIFO_data=RFXX_WRT_CMD(0xB000); // Fifo read command
	return(FIFO_data&0x00FF);
}

int main(void)
{
	unsigned char i;
	unsigned char ChkSum;
	unsigned char val;
	unsigned char id;
	//POWER ON indication: LED blink 3 times
	
	USART_Init ( MYUBRR );
	MODULE_OUTPUT();
	MODULE_OFF(); //for reset
	
	printf ( "AMPEL START!\n\n" );
	
	MODULE_ON(); //for reset
	_delay_ms(200);
	
	//Initialize command port
	RFXX_PORT_INIT();
	//Initialize RF12 chip
	RF12_INIT();
	//Init FIFO
	RFXX_WRT_CMD(0xCA81);
	
	
	// Timer Interrupt Mask Register
	// Overflow Interrupt erlauben
	TIMSK1 |= (1<<TOIE1);
	
	// Entspricht einer Sekunde bis Overflow bei Prescaler 1024
	TCNT1 = -3600;
	TCCR1B = (1<<CS10)|(1<<CS12); // Setze Prescaler für Timer auf 1024
	
	//Richtungsregister PORTC
	DDRC |= (1<<DDC0)|(1<<DDC1)|(1<<DDC2)|(1<<DDC3)|(1<<DDC4);
	a_gruen; //Autofahrer hat zu Begin Gruen
	
	/* Konfiguriere Taster */

	DDRD  &= ~(1 << DDD6);     // Lösche Belegung von PD6
	PORTD |=  (1 << PD6);      // turn On the Pull-up on PD6
	
	// Pin Change Interrupt Control Register
	PCICR |= (1 << PCIE2);     // Setze PCIE2 um PCMSK2 Scan zu aktivieren
	
	// Pin Change Mask Register 2
	PCMSK2 = (1 << PCINT22);   //Aktiviere Interrupt auf PCINT22 (PIND6)
	
	// Global Interrupts aktivieren
	sei();
	

	while(1){
		
		//Enable FIFO
		RFXX_WRT_CMD(0xCA83);
		
		ChkSum=0;
		
		
		//Receive payload data
		id = RF12_RECV();
		val = RF12_RECV();
		//Receive Check sum
		i=RF12_RECV();
		
		ChkSum+= id;
		ChkSum+= val;
		//Disable FIFO
		RFXX_WRT_CMD(0xCA81);
		//Package chkeck
		
		printf("ID: %i val: %i", id, val);
		
		if(ChkSum==i && id == AMPELID && (val==0||val==0x0F)){
			if (val == 0x0F)
			{
				ampelan = 1;
			} else if (val == 0)
			{
				ampelan = 0;
			}
			printf( " Received Data Package OK, Data Code: 0x0%X",val);
			_delay_ms(10);
		} else {
			printf(" Discarded Package");
		}
		printf("\n");
	}
}

ISR (TIMER1_OVF_vect)
{
	TCNT1 = -3600;   // for 1 sec // 3686400/1024 = 3600
	
	if (ampelan)
	{
		counter++;
		switch(counter){
			case 14: a_gelb; break;
			case 15: a_rot; break;
			case 16: f_gruen; break;
			case 21: f_rot; break;
			case 26: a_gelbrot; break;
			case 27: a_gruen; counter=0;
		}
		} else {
		counter = 20;
		PORTC &= (0b00010); // Deaktiviere alle Lampen, bis auf die Gelbe
		PORTC ^= (0b00010); // XOR auf der Gelben Lampe, damit sie im Sekunden
	}
}

/*ISR is Activated on any state change.
since this will switch the variable counter to a state in which the if-condition is not being met anymore,
an actual de-bounce is not required here.*/
ISR(PCINT2_vect) {
	if(counter<13||counter>26){
		counter = 13;
	}
}
