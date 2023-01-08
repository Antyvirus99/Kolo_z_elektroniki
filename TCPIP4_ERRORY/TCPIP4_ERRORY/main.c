/*
 * Komendy_AT.c
 *
 * Created: 2016-12-31 10:56:32
 *  Author: Dondu
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#include <string.h>
#include <stdlib.h>

#include "ESP8266.h"







int main(void)
{
	
	DDRD |= (1<<PD1); // ustaw txd na wyjscie
	
	//diody pomocnicze
	DDRD |= (1<<PD7);
	DDRD |= (1<<PD3);
	DDRD |= (1<<PD5);
	DDRB |= (1<<PB1);
	//Pin reset 8266
	DDRD |= (1<<PD6);
	
	//PORTD |= (1<<PD2);
	PORTD |= (1<<PD3);
	//PORTD |= (1<<PD5);
	PORTB |= (1<<PB1);
	PORTD |= (1<<PD7);
	
	ustaw_w_tryb_at();
	
	
	
	esp8866_resetuj();
	
	
	polacz_z_router();
	
	


	_delay_ms(8000);
	

	sei();
	
	
	
	


	
    while(1)
    {
		/*
		wyslij_tekst();
		//index_odebrany=4;
		if (index_odebrany >= (strlen_P(komenda_SZUKANA)))
		{
			if (funkcja_szukajaca() == 1)
			{
				PORTD ^= (1<<PD2);
				
			}
				
		}	
		*/
		
/*
		polacz_z_NTP();
		
		czekaj_ms(1000);
		
		if((		memmem_P(	(const void *) odbiorcza,						//tablica w której bêdziemy szukaæ
							index_odebrany,										//ile tekstu z tej tablicy ma przeszukaæ    (-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
							komenda_SZUKANA_OK,									//szukany tekst
							strlen_P(komenda_SZUKANA_OK)						//d³ugoœæ tekstu szukanego					(-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
							)
		)
		||
		(			memmem_P(	(const void *) odbiorcza,						//tablica w której bêdziemy szukaæ
							index_odebrany,										//ile tekstu z tej tablicy ma przeszukaæ    (-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
							juz_polaczono,										//szukany tekst
							strlen_P(juz_polaczono)								//d³ugoœæ tekstu szukanego					(-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
								)
		))
		{			
			wyslij_komenda_bajty();
			czekaj_ms(2000);
			if(		memmem_P(	(const void *)odbiorcza,								//tablica w której bêdziemy szukaæ
								index_odebrany,											//ile tekstu z tej tablicy ma przeszukaæ    (-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
								komenda_SZUKANA_kontynuuj,								//szukany tekst
								strlen_P(komenda_SZUKANA_kontynuuj)						//d³ugoœæ tekstu szukanego					(-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
								)
			)
			{ 

							
				wyslij_bajty();	
				czekaj_ms(3000);	
				wynik = memmem_P(	(const void *)odbiorcza,						//tablica w której bêdziemy szukaæ
									index_odebrany,									//ile tekstu z tej tablicy ma przeszukaæ    (-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
									komenda_szukana_IPD,							//szukany tekst
									strlen_P(komenda_szukana_IPD)					//d³ugoœæ tekstu szukanego					(-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
									);

			}
			
						
		}
		
		if (wynik != 0)
		{														
			//... a jeœli nie znajdzie IPD??? - brakuje reakcji
			wynik += 48; // ustaw wskaznik na date wyslania czasu unix
			pobierz_czas();
								
								
			//tutaj przenios³em wysy³anie +++ i wszystko dzia³a OK
			_delay_ms(500);
			while (!(UCSRA & (1<<UDRE)));
			UDR = '+';
			while (!(UCSRA & (1<<UDRE)));
			UDR = '+';
			while (!(UCSRA & (1<<UDRE)));
			UDR = '+';
			_delay_ms(1000);			//niezbêdny zgodnie z dokumentacj¹
								
			rozlacz();
			czekaj_ms(1000);
			PORTD ^= (1<<PD7);
			//wyslij_czas();
			//czas_UNIX = 4732;
			dana_do_wysylki = czas_UNIX;
			wynik = 0;
		}
		else
		{
			dana_do_wysylki = 666;
			wynik = 0;
		}
		*/
		//polacz_i_pobierz_czas_UNIX();
		dana_do_wysylki = polacz_i_pobierz_czas_UNIX();
		
		_delay_ms(1000);
		PORTB ^=(1<<PB1);
		
		if (!wyslij_na_serwer())
		{
			
			esp8866_resetuj();			//wchodzi tutaaj i resetuje esp
		
		}
		wyzeruj_dane_po_wyslaniu_na_serwer();
		_delay_ms(10000);

	}  
	
}



