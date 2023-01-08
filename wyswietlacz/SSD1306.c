/*
 * SSD1306.c
 *
 * Created: 21.03.2017 19:37:42
 *  Author: Donat
 */ 
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

#include "SSD1306.h"
#include "czcionki.h"
#include "czcionka+8pts.h"
unsigned int a = 0;


char napis[] = "cokolwiek";

void ustaw_master (void)
{
	DDRB |= (1<<SSD1306_PIN_DC);
	DDRB |= (1<<SSD1306_PIN_CS);	//ustaw pin SS na wyjscie
	PORTB |= (1<<SSD1306_PIN_CS);	//ustaw stan wysoki SS
	DDRB |= (1<<PB3);	//ustaw pin MOSI
	DDRB |= (1<<PB5);	//ustaw pin SCK
	DDRB &=~ (1<<PB4);	//ustaw miso
	SPCR |= (1<<MSTR);	//ustaw tryb MASTER
	SPCR |= (1<<CPHA) | (1<<CPOL);	//ustaw przesyl  domyslnie 1 zbocze narastajace
	//wysyl MSB nie trzeba ustawiac
	//Preskaler 4 nie trzeba ustawaic
	//SPCR |= (1<<SPR1) | (1<<SPR0);	//preskaler 128
	SPSR |= (1<<SPI2X);	//2 razy szybciej
	SPCR |= (1<<SPE);	//wlacz SPI
	SPSR;
	SPDR;
}
void ustaw_slave (void)
{
	DDRB &=~ (1<<SSD1306_PIN_CS);	//ustaw pin SS na wejsce
	DDRB &=~ (1<<PB3);	//ustaw pin MOSI
	DDRB &=~ (1<<PB5);	//ustaw pin SCK
	DDRB |= (1<<PB4);	//ustaw miso
	SPCR &=~ (1<<MSTR);	//ustaw tryb MASTER
	SPCR |= (1<<CPHA) | (1<<CPOL);	//ustaw przesyl  domyslnie 1 zbocze narastajace
	//wysyl MSB nie trzeba ustawiac
	SPCR |= (1<<SPE);	//wlacz SPI
}
inline void CS_high (void)
{
	PORTB |= (1<<SSD1306_PIN_CS);
}

inline void CS_low (void)
{
	PORTB &=~ (1<<SSD1306_PIN_CS);
}

inline void DC_high (void)
{
	PORTB |= (1<<SSD1306_PIN_DC);
}

inline void DC_low (void)
{
	PORTB &=~ (1<<SSD1306_PIN_DC);
}



void ssd1306_inicjuj (void)
{
	DC_low();
	CS_low();
	
	SPDR = CHARGE_PUMP_SETTING;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 0x14;
	while(!(SPSR & (1<<SPIF)));
	SPDR = SET_DISPLAY_ON;
	while(!(SPSR & (1<<SPIF)));
	
	CS_high();
}

void ssd1306_wyslij_3_bajty (unsigned char komenda, unsigned char bajt1, unsigned char bajt2)
{
	DC_low();
	CS_low();
	
	SPDR = komenda;
	while(!(SPSR & (1<<SPIF)));
	SPDR = bajt1;
	while(!(SPSR & (1<<SPIF)));
	SPDR = bajt2;
	while(!(SPSR & (1<<SPIF)));
	
	CS_high();
}

void ssd1306_wyslij_2_bajty (unsigned char komenda, unsigned char bajt1)
{
	DC_low();
	CS_low();
	
	SPDR = komenda;
	while(!(SPSR & (1<<SPIF)));
	SPDR = bajt1;
	while(!(SPSR & (1<<SPIF)));
	
	CS_high();
}
void ssd1306_wyslij_1_bajt (unsigned char komenda)
{
	DC_low();
	CS_low();
	
	SPDR = komenda;
	while(!(SPSR & (1<<SPIF)));

	CS_high();
}

void wyczysc_ekran (void)
{
	ssd1306_wyslij_3_bajty(SET_COLUMN_ADDRESS, 0, 127);		//ustaw kolumny
	ssd1306_wyslij_3_bajty(SET_PAGE_ADDRESS, 0, 7);			//ustaw adresy	
	DC_high();
	CS_low();
	for (a=0; a<1024; a++)
	{
		SPDR = 0;
		while(!(SPSR & (1<<SPIF)));
	}
	CS_high();
}



void tekst_z_flash (const __flash char* napis, char linia, char kolumna)
{
	char page = linia / 8;
	char wiersz = linia % 8;
	ssd1306_wyslij_3_bajty(SET_COLUMN_ADDRESS, kolumna, 127);		//ustaw kolumny
	ssd1306_wyslij_3_bajty(SET_PAGE_ADDRESS, page, page);			//ustaw adresy
	
	DC_high();
	CS_low();
	
	for(int index_tekstu = 0; index_tekstu < strlen_P(napis); index_tekstu++ )
	{
		
		int index_czcionki = info[*(napis+index_tekstu) - '!'][1];
		int index_bajtow = info[*(napis+index_tekstu) - '!'][0];

		for(int f = 0; f < index_bajtow ; f++)
		{

			SPDR = czcionka[index_czcionki + f]<<wiersz;
			while(!(SPSR & (1<<SPIF)));

		}
		//_delay_ms(100);

		SPDR = 0;
		while(!(SPSR & (1<<SPIF)));			

	}
	CS_high();
	//############	wyslij 2 page	##############
	ssd1306_wyslij_3_bajty(SET_COLUMN_ADDRESS, kolumna, 127);		//ustaw kolumny
	ssd1306_wyslij_3_bajty(SET_PAGE_ADDRESS, page + 1, page + 1);			//ustaw adresy
	
	DC_high();
	CS_low();
	
	for(int index_tekstu = 0; index_tekstu < strlen_P(napis); index_tekstu++ )
	{
		
		int index_czcionki = info[*(napis+index_tekstu) - '!'][1];
		int index_bajtow = info[*(napis+index_tekstu) - '!'][0];
		for(int f = 0; f < index_bajtow ; f++)
		{
		
			SPDR = czcionka[index_czcionki + f]>>(8-wiersz);
			while(!(SPSR & (1<<SPIF)));

		}
		//_delay_ms(100);

		SPDR = 0;
		while(!(SPSR & (1<<SPIF)));
		
	}
	CS_high();		
}

void tekst_z_sram (char* napis, char linia, char kolumna)
{
	char page = linia / 8;
	char wiersz = linia % 8;
	ssd1306_wyslij_3_bajty(SET_COLUMN_ADDRESS, kolumna, 127);		//ustaw kolumny
	ssd1306_wyslij_3_bajty(SET_PAGE_ADDRESS, page, page);			//ustaw adresy
	
	DC_high();
	CS_low();
	
	for(int index_tekstu = 0; index_tekstu < strlen(napis); index_tekstu++ )
	{
		
		int index_czcionki = info[*(napis+index_tekstu) - '!'][1];
		int index_bajtow = info[*(napis+index_tekstu) - '!'][0];

		for(int f = 0; f < index_bajtow ; f++)
		{

			SPDR = czcionka[index_czcionki + f]<<wiersz;
			while(!(SPSR & (1<<SPIF)));

		}
		//_delay_ms(100);

		SPDR = 0;
		while(!(SPSR & (1<<SPIF)));

	}
	CS_high();
	//############	wyslij 2 page	##############
	ssd1306_wyslij_3_bajty(SET_COLUMN_ADDRESS, kolumna, 127);		//ustaw kolumny
	ssd1306_wyslij_3_bajty(SET_PAGE_ADDRESS, page + 1, page + 1);			//ustaw adresy
	
	DC_high();
	CS_low();
	
	for(int index_tekstu = 0; index_tekstu < strlen(napis); index_tekstu++ )
	{
		
		int index_czcionki = info[*(napis+index_tekstu) - '!'][1];
		int index_bajtow = info[*(napis+index_tekstu) - '!'][0];
		for(int f = 0; f < index_bajtow ; f++)
		{
			
			SPDR = czcionka[index_czcionki + f]>>(8-wiersz);
			while(!(SPSR & (1<<SPIF)));

		}
		//_delay_ms(100);

		SPDR = 0;
		while(!(SPSR & (1<<SPIF)));
		
	}
	CS_high();
}


void tekst_duzy_z_flash (const __flash char* napis, char page, char kolumna)
{
	char linia = 0;		//linia od ktorej zaczyna pisac tekst
	char wiersz = linia % 8;

	

	for(int index_page = 0; index_page < 2; index_page++) 
	{
		ssd1306_wyslij_3_bajty(SET_COLUMN_ADDRESS, kolumna, 127);		//ustaw kolumny
		ssd1306_wyslij_3_bajty(SET_PAGE_ADDRESS, page + index_page, page + index_page);	//ustaw adresy

		for(int index_tekstu = 0; index_tekstu < strlen_P(napis); index_tekstu++ )
		{
		
			int index_czcionki = info_8pts[*(napis+index_tekstu) - '!'][1];
			int index_bajtow = 2*(info_8pts[*(napis+index_tekstu) - '!'][0]);
		
			DC_high();
			CS_low();
			for(int f = 1 - index_page; f < index_bajtow ; f +=2)
			{			
				SPDR = czcionka_8pts[index_czcionki + f]<<wiersz;
				while(!(SPSR & (1<<SPIF)));			
			}
			SPDR = 0;
			while(!(SPSR & (1<<SPIF)));
			CS_high();
				
		}
	}
		//_delay_ms(100);
}
		
void tekst_duzy_z_sram (char* napis, char page, char kolumna)
{
	char linia = 0;		//linia od ktorej zaczyna pisac tekst
	char wiersz = linia % 8;

	

	for(int index_page = 0; index_page < 2; index_page++)
	{
		ssd1306_wyslij_3_bajty(SET_COLUMN_ADDRESS, kolumna, 127);		//ustaw kolumny
		ssd1306_wyslij_3_bajty(SET_PAGE_ADDRESS, page + index_page, page + index_page);	//ustaw adresy

		for(int index_tekstu = 0; index_tekstu < strlen(napis); index_tekstu++ )
		{
			
			int index_czcionki = info_8pts[*(napis+index_tekstu) - '!'][1];
			int index_bajtow = 2*(info_8pts[*(napis+index_tekstu) - '!'][0]);
			
			DC_high();
			CS_low();
			for(int f = 1 - index_page; f < index_bajtow ; f +=2)
			{
				SPDR = czcionka_8pts[index_czcionki + f]<<wiersz;
				while(!(SPSR & (1<<SPIF)));
			}
			SPDR = 0;
			while(!(SPSR & (1<<SPIF)));
			CS_high();
			
		}
	}
	//_delay_ms(100);
}	






		
/*
		
	}
	CS_high();
	//############	wyslij 2 page	##############
	ssd1306_wyslij_3_bajty(SET_COLUMN_ADDRESS, kolumna, 127);		//ustaw kolumny
	ssd1306_wyslij_3_bajty(SET_PAGE_ADDRESS, page + 1, page + 1);			//ustaw adresy
	
	DC_high();
	CS_low();
	
	for(int index_tekstu = 0; index_tekstu < strlen(napis); index_tekstu++ )
	{
		
		int index_czcionki = info_8pts[napis[index_tekstu] - '!'][1];
		int index_bajtow = info_8pts[napis[index_tekstu] - '!'][0];
		for(int f = 0; f < index_bajtow ; f++)
		{
			
			SPDR = czcionka[index_czcionki + f]>>(8-wiersz);
			while(!(SPSR & (1<<SPIF)));

		}
		//_delay_ms(100);

		SPDR = 0;
		while(!(SPSR & (1<<SPIF)));
		
	}
	CS_high();
}
*/
