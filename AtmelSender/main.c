/*
 * ATmega8_RFM12_Receive_fg.c
 * Version fuer Gruppenfrequenzen
 *
 * Created: 02.10.2011 15:01:10
 *  Author: mf
 *  Updates
 *  21.05.2019 Anpassung auf myAVR MK2, dominic lukwata
 *  06.12.2019 Anpassung fuer Gruppenfrequenzen, dominic lukwata
 */

 /********************************************************************
Beispiel f?r RFM12-Funkmodul (Senden)
IAR Embedded Workbench IDE, C
Prozessor: ATMEGA168a
Frequenz: 434MHz
Datenrate: 4.8kbps

Diese Programm stellt eine kurze Testroutine f?r den ATMEGA168a in
Verbindung mit einem externen 3.6864MHz Quarz, einem RFM12-Funkmodul, und
dem Pollin Electronic Funk-AVR-Evaluations-Board dar.
Es handelt sich ausdr?cklich um eine Testroutine und ein Anwendungs-
beispiel. Elementare Elemente wie Fehlerbehandlungen usw. sind nicht
vorhanden. Das Originalprogramm heisst ATmega8_RFM12_Transmit.c

Das Funkmodul RFm12 ist auf MyAVRLaborkarte C verbaut.

Ablauf:
Sendet Preamble, Frame-Recognition, 16 Datenbyte und ChkSumme der
16 Datenbyte. Anschlie?end erfolgt eine Signalisierung mit LED1.
Dies geschieht in einer Endlosschleife.

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
#define PORT_DATA PORTD
#define PIN_DATA PIND
#define DDR_DATA DDRD
#define PB7 7 //
#define PB6 6 //
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
#define SDO_HI() PIN_SDO&(1<<RFXX_SDO)
#define SCK_OUTPUT() DDR_SCK |= (1<<RFXX_SCK)
#define HI_SCK() PORT_SCK|= (1<<RFXX_SCK)
#define LOW_SCK() PORT_SCK&=~(1<<RFXX_SCK)
#define RF12_DATA 7 //PD7
#define DATA_OUT DDR_DATA|=1<<RF12_DATA
#define HI_DATA PORT_DATA|=1<<RF12_DATA
#define MODULE_OUTPUT() PORTD&=~(1<<4) //+Module-Power
#define LED1_OUTPUT() DDRC|=(1<<0)
#define LED1_OFF() PORTC&=~(1<<0)
#define LED1_ON() PORTC|=(1<<0)
#define MODULE_OFF() PORTD&=~(1<<4)
#define MODULE_ON() PORTD|=(1<<4)

#include <avr/io.h>
#include "uart_atmega168a.h"
#include "util/delay.h"		// enthaelt wartezeitroutinen
#include <stdlib.h>




void RFXX_PORT_INIT(void){
  HI_SEL();
  HI_SDI();
  LOW_SCK();
  SEL_OUTPUT();
  SDI_OUTPUT();
  SDO_INPUT();
  SCK_OUTPUT();
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


// Aenderung fuer mehere Frequenzen
// lukwata 06.12.2019

void RF12_INIT(void){
  RFXX_WRT_CMD(0x80D7);//EL,EF,12.0pF
  RFXX_WRT_CMD(0x8239);//!er,!ebb,ET,ES,EX,!eb,!ew,DC

  //  RFXX_WRT_CMD(0xA640);//A640=434 MHz
  RFXX_WRT_CMD(0xA000| RFM12_FREQUENCY_CALC_433());// Gruppenfrequenz einstellen

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



void RF12_SEND(unsigned char aByte){
  while(PIND&(1<<2));//wait for previously TX over
    RFXX_WRT_CMD(0xB800+aByte);
  }

//void main(void)
int main(void)

{
  unsigned int i;
  unsigned char ChkSum;
  asm("cli");
  DDRB=0x00;//PB INPUT;
  DDRD=0x00;//PD INPUT;
  //POWER ON indication: LED blink 3 times
 USART_Init ( MYUBRR );
  
  MODULE_OUTPUT();
  LED1_OUTPUT();
  LED1_OFF();
  MODULE_OFF(); //for reset
 printf ("RFM12 Transmit\n");
  for(i=0;i<3;i++){
    _delay_ms(200);
    LED1_ON();
    _delay_ms(200);
    LED1_OFF();
  }
  LED1_OFF();
  MODULE_ON();
  _delay_ms(200);

  
  // Initialisierung des RFM12-Moduls
  RFXX_PORT_INIT();
  RF12_INIT();
  DDRD|=(1<<RF12_DATA);		// set nFFS pin to Output
  PORTD|=(1<<RF12_DATA);	// SET nFFS pin HI when using TX register
  DDRD&=~(1<<2);			// set PD2(INT0) to input
  
  while(1){
	  
	char iin[10];
	int a;
	printf("gebe '0' oder '1' in die Konsole ein, um die Ampel zu steuern.\n");
	scanf("%s",iin);
	
	a = atoi(iin); //array to integer
	
	if (a==0||a==1)
	{
	
	for (int i = 0; i < 10; i++)
	{
    LED1_ON();
    RFXX_WRT_CMD(0x0000);//read status register
	
	// Power Management: Switch on transceiver to send data (no receive!)
    RFXX_WRT_CMD(0x8239);//!er,!ebb,ET,ES,EX,!eb,!ew,DC
	
    ChkSum=0;
    RF12_SEND(0xAA);//PREAMBLE
    RF12_SEND(0xAA);//PREAMBLE
    RF12_SEND(0xAA);//PREAMBLE
    RF12_SEND(0x2D);//SYNC HI BYTE
    RF12_SEND(0xD4);//SYNC LOW BYTE
    
	
	RF12_SEND(0x30);//DATA BYTE 0
    ChkSum+=0x30;
    RF12_SEND(0x31);//DATA BYTE 1
    ChkSum+=0x31;
    RF12_SEND(0x32);
    ChkSum+=0x32;
    RF12_SEND(0x33);
    ChkSum+=0x33;
    RF12_SEND(0x34+a);
    ChkSum+=0x34+a;
    RF12_SEND(0x35);
    ChkSum+=0x35;
    RF12_SEND(0x36);
    ChkSum+=0x36;
    RF12_SEND(0x37);
    ChkSum+=0x37;
    RF12_SEND(0x38);
    ChkSum+=0x38;
    RF12_SEND(0x39);
    ChkSum+=0x39;
    RF12_SEND(0x3A);
    ChkSum+=0x3A;
    RF12_SEND(0x3B);
    ChkSum+=0x3B;
    RF12_SEND(0x3C);
    ChkSum+=0x3C;
    RF12_SEND(0x3D);
    ChkSum+=0x3D;
    RF12_SEND(0x3E);
    ChkSum+=0x3E;
    RF12_SEND(0x3F); //DATA BYTE 15
    ChkSum+=0x3F;
    RF12_SEND(ChkSum); //send chk sum
   
    RF12_SEND(0xAA);//DUMMY BYTE
    RF12_SEND(0xAA);//DUMMY BYTE
    RF12_SEND(0xAA);//DUMMY BYTE
	
	// Power Management: switch off transceiver
    RFXX_WRT_CMD(0x8201);
    LED1_OFF();
	_delay_ms(300);
	}
		printf("%X \n",ChkSum);
		printf ("Steuersignal uebertragen!\n");
	}
  };
}