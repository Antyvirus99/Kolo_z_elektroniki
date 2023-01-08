/*
 * MFRC522.c
 *
 * Created: 08.06.2017 18:28:04
 *  Author: Donat
 */ 
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>


#include "W25Q32BV.h"
#include "SSD1306.h"
#include "main.h"
#include "MFRC522.h"
#include "MFRC522_CMD.h"
#include "MFRC522_REG.h"

void MFRC522_Ustaw_piny (void)
{
	DDRC |= (1<<MFRC522_SS);
	PORTC |= (1<<MFRC522_SS);
}

void MFRC522_High (void)
{
	PORTC |= (1<<MFRC522_SS);
}

void MFRC522_low (void)
{
	PORTC &=~ (1<<MFRC522_SS);
}

unsigned char MFRC522_Transmit (char* data, unsigned int data_size)				//wysyl do fifo i wysyl danych na karte z fifo 
{
	for(int i = 0; i < data_size; i++)
	{
		MFRC522_write_data(FIFODataReg, data+i);
	}
	
	MFRC522_write_data(CommandReg, Transmit_CMD);
	
}


unsigned char MFRC522_Read_data (char adres)				//adres = <0;63>
{
	
	MFRC522_low();
	
	SPDR = (adres<<1) | MFRC522_READ;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	
	MFRC522_High();
	return SPDR;
}

void MFRC522_write_data (char adres,char data)					//adres = <0;63>
{
	MFRC522_low();
	
	SPDR = (adres<<1) | MFRC522_WRITE;
	while(!(SPSR & (1<<SPIF)));
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	MFRC522_High();
}


void MFRC522_inicjuj (void)
{
	
	MFRC522_Ustaw_piny();	
	
	MFRC522_write_data (TModeReg,0x8D);				//w³¹cz timer autpomatyczny 0x80 oraz 0X0d to czêœæ preskalera
	MFRC522_write_data (TPrescalerReg, 0x3E);		//pozosta³a czêœæ presklera z rozkazu powy¿ej - st¹ preskaler jest ustawiony na 0xd3e czyli 3390
	MFRC522_write_data (TReloadReg_1, 30);			//wartoœæ reload timera bajt High
	MFRC522_write_data (TReloadReg_2, 0);			//wartoœæ reload timera bajt Low czyli Reload ustawione na 30*256 + 0 = 0x3000
	MFRC522_write_data (TxASKReg, 0x40);			//w³¹cza Force100ASK
	MFRC522_write_data (ModeReg, 0x3D);			

	char dana = MFRC522_Read_data(TxControlReg);
	if(!(dana & 0x03))								//jesli ani tx1 ani tx2 nie jest wlaczony to wlacz oba
	{
		MFRC522_write_data(TxControlReg, dana | 0x03);	// w³¹cz wyjœcia TX1 i TX2
	}
	
}

void MFRC522_timer (void)
{
	////////odlicza 100 milisekund////////////
	//MFRC522_write_data(TModeReg,0b10000000);						//AUTO
	MFRC522_write_data(TPrescalerReg,0b00010100);					//lower bits	preskaler 20
	MFRC522_write_data(TReloadReg_1,0b11111111);					//higher bits
	MFRC522_write_data(TReloadReg_2,0b11111111);					//lower bits	reload 65535
	
}

void MFRC522_interrupt (void)
{
	MFRC522_write_data(ComIEnReg,0b00000001);						//wlacz przerwania
	MFRC522_write_data(DivIEnReg,0b10000000);						//standard CMOS output	
}



