/*
 * ESP8266.c
 *
 * Created: 14.03.2017 19:09:23
 *  Author: Donat
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>


#include <string.h>
#include <stdlib.h>

#include "ESP8266.h"

const __flash char juz_polaczono[] = "ALREADY CONNECTED\r\n";
const __flash char polaczono_z_serwerem[] = "CONNECT\r\n\r\nOK\r\n";
const __flash char zapisano_na_serwerze[] = "zapisano_dane";
const __flash char odebrano_bajty[] = "bytes\r\n";
const __flash char komenda_wlacz_robocza[] = "AT+CWMODE_CUR=3\r\n";
const __flash char komenda_polacz_do_serwer[] = "AT+CIPSTART=\"TCP\",\"192.168.1.51\",80\r\n";
const __flash char komdena_wyslij_ilosc_do_serwer[] = "AT+CIPSEND=";
const __flash char komenda_wyslij_adresI[] = "GET http://192.168.1.51/JUR/php/notpad_do_TCPIP.php?dana=";
const __flash char komenda_wyslij_adresII[] = " HTTP/1.1\r\nHost: 192.168.1.51\r\n\r\n";
int liczba_errorow = 0;
char dane[15];
char dane2[10];
int ilosc_znakow;
unsigned long dana_do_wysylki = 0;

const __flash char komenda_polacz[] = "AT+CIPSTART=\"UDP\",\"ntp1.tp.pl\",123";
const __flash char komenda_rozlacz[] = "AT+CIPCLOSE";
const __flash char komenda_bajty[] = "AT+CIPSEND=48";
const __flash char komenda_wyslij[] = "AT+CMGS=\"796163560\"";
volatile char odbiorcza[100];
const __flash char komenda_SZUKANA_OK[] = "\r\nOK\r\n";
const __flash char komenda_szukana_IPD[] = "+IPD,48:";
const __flash char komenda_SZUKANA_kontynuuj[] = "\r\nOK\r\n> ";
const __flash char komenda_polacz_z_router[] = "AT+CWJAP_CUR=\"Buba_wifi_G\",\"MoiGosciE#2017\"";
volatile unsigned int opoznienie = 0;
unsigned long czas_odebrany = 0;
unsigned long czas_UNIX = 0;
char * wynik;


volatile unsigned int index_odebrany = 0;
signed int index_szukany = 0;
unsigned int index_zapisu = 0;
unsigned int index_tekstu = 0;

ISR (USART_RXC_vect)
{
	odbiorcza[index_odebrany] = UDR;
	index_odebrany++;
}

void czekaj_ms (int czas)
{
	czas/=10;
	
	while ((index_odebrany == 0) && (czas > 0))		//czekaj 2 sekundy lub az odbierze informacje zwrotna
	{
		_delay_ms(10);
		czas--;						
	}
	_delay_ms(500);
}
char potwierdz_zapis_na_serwerze (void)
{
	if(memmem_P(	(const void *) odbiorcza,						//tablica w której bêdziemy szukaæ
					index_odebrany,									//ile tekstu z tej tablicy ma przeszukaæ    (-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
					zapisano_na_serwerze,								//szukany tekst
					strlen_P(zapisano_na_serwerze)					//d³ugoœæ tekstu szukanego					(-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
					) != NULL
	) return 1;		
	else return 0;
}
char potwierdz_polaczenie_z_serwerem (void)
{
	if(memmem_P(	(const void *) odbiorcza,						//tablica w której bêdziemy szukaæ
					index_odebrany,									//ile tekstu z tej tablicy ma przeszukaæ    (-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
					polaczono_z_serwerem,								//szukany tekst
					strlen_P(polaczono_z_serwerem)					//d³ugoœæ tekstu szukanego					(-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
					) != NULL
	) return 1;
	else return 0;	
}
char potwierdz_przesyl_adresu (void)
{
	if(memmem_P(	(const void *) odbiorcza,						//tablica w której bêdziemy szukaæ
					index_odebrany,									//ile tekstu z tej tablicy ma przeszukaæ    (-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
					odebrano_bajty,								//szukany tekst
					strlen_P(odebrano_bajty)					//d³ugoœæ tekstu szukanego					(-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
					) != NULL
	) return 1;	
	else return 0;
}
char potwierdz_przeslane_bajty (void)
{
	if(memmem_P(	(const void *) odbiorcza,						//tablica w której bêdziemy szukaæ
					index_odebrany,									//ile tekstu z tej tablicy ma przeszukaæ    (-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
					komenda_SZUKANA_kontynuuj,								//szukany tekst
					strlen_P(komenda_SZUKANA_kontynuuj)					//d³ugoœæ tekstu szukanego					(-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
					) != NULL
	) return 1;
	else return 0;
}
char znajdz_ok (void)
{

	if(memmem_P(	(const void *) odbiorcza,						//tablica w której bêdziemy szukaæ
			index_odebrany,									//ile tekstu z tej tablicy ma przeszukaæ    (-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
			komenda_SZUKANA_OK,								//szukany tekst
			strlen_P(komenda_SZUKANA_OK)					//d³ugoœæ tekstu szukanego					(-1 bo do tablicy dodawany jest znak koñca tekstu, którym jest bajt o wartoœci zero)
			) != NULL
	) return 1;
	else return 0;
	
}
void polacz_z_router (void)
{
	index_odebrany = 0; //Zeby odpowiedz byla pakowana na poczatek buforu
	for (index_tekstu = 0; index_tekstu < strlen_P(komenda_polacz_z_router); index_tekstu++)
	{
		while (!(UCSRA & (1<<UDRE)));
		UDR = komenda_polacz_z_router[index_tekstu];
	}
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0d;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0a;
}

void wyslij_komenda_bajty (void)
{
	index_odebrany = 0; //Zeby odpowiedz byla pakowana na poczatek buforu
	for (index_tekstu = 0; index_tekstu < strlen_P(komenda_bajty); index_tekstu++)
	{
		while (!(UCSRA & (1<<UDRE)));
		UDR = komenda_bajty[index_tekstu];
	}
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0d;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0a;
}

char wyslij_na_serwer (void)
{
	
	//wyzeruj dane do wysyllki
	ilosc_znakow = 0;
	for (index_tekstu = 0; index_tekstu < strlen(dane); index_tekstu++)
	{
		dane[index_tekstu] = 0;
	}
	for (index_tekstu = 0; index_tekstu < strlen(dane); index_tekstu++)
	{
		dane2[index_tekstu] = 0;
	}	
	do 
	{
		/*
		index_odebrany = 0;
		for (index_tekstu = 0; index_tekstu < strlen_P(komenda_wlacz_robocza); index_tekstu++)
		{
			while (!(UCSRA & (1<<UDRE)));
			UDR = komenda_wlacz_robocza[index_tekstu];
		}
		czekaj_ms(2000);
		if(znajdz_ok())*/
		//{
			liczba_errorow = 0;		//wyzeruj bledy wynikle z proby wlaczenia stacji roboczej
			do 
			{
				index_odebrany = 0;
				for (index_tekstu = 0; index_tekstu < strlen_P(komenda_polacz_do_serwer); index_tekstu++)
				{
					while (!(UCSRA & (1<<UDRE)));
					UDR = komenda_polacz_do_serwer[index_tekstu];
				}
				
				

				czekaj_ms(2000);_delay_ms(5000);
				if(potwierdz_polaczenie_z_serwerem())
				{
					liczba_errorow = 0;
					do 
					{
						index_odebrany = 0;
						for (index_tekstu = 0; index_tekstu < strlen_P(komdena_wyslij_ilosc_do_serwer); index_tekstu++)
						{
							while (!(UCSRA & (1<<UDRE)));
							UDR = komdena_wyslij_ilosc_do_serwer[index_tekstu];
						}
					
						ultoa(dana_do_wysylki, dane, 10);
					
						ilosc_znakow = strlen_P(komenda_wyslij_adresI) + strlen_P(komenda_wyslij_adresII) + strlen(dane);
					
						ultoa(ilosc_znakow, dane2, 10);
						index_odebrany = 0;
						for (index_tekstu = 0; index_tekstu < strlen(dane2); index_tekstu++)
						{
							while (!(UCSRA & (1<<UDRE)));
							UDR = dane2[index_tekstu];
						}
						while (!(UCSRA & (1<<UDRE)));
						UDR = '\r';
						while (!(UCSRA & (1<<UDRE)));
						UDR = '\n';
					
						czekaj_ms(2000);
					
						if (potwierdz_przeslane_bajty())
						{
							liczba_errorow = 0;
							do 
							{
								index_odebrany = 0;
								for (index_tekstu = 0; index_tekstu < strlen_P(komenda_wyslij_adresI); index_tekstu++)
								{
									while (!(UCSRA & (1<<UDRE)));
									UDR = komenda_wyslij_adresI[index_tekstu];
								}
								index_odebrany = 0;
								for (index_tekstu = 0; index_tekstu < strlen(dane); index_tekstu++)
								{
									while (!(UCSRA & (1<<UDRE)));
									UDR = dane[index_tekstu];
								}
								index_odebrany = 0;
								for (index_tekstu = 0; index_tekstu < strlen_P(komenda_wyslij_adresII); index_tekstu++)
								{
									while (!(UCSRA & (1<<UDRE)));
									UDR = komenda_wyslij_adresII[index_tekstu];
								}

								czekaj_ms(2000);
								if (potwierdz_przesyl_adresu())
								{
												
									liczba_errorow = 0;
									do 
									{
											
										_delay_ms(2000);
										if (potwierdz_zapis_na_serwerze())					//czy zapisano np serwer moze byc pelny
										{


											liczba_errorow = 0;
											while (!(UCSRA & (1<<UDRE)));
											UDR = '+';
											while (!(UCSRA & (1<<UDRE)));
											UDR = '+';
											while (!(UCSRA & (1<<UDRE)));
											UDR = '+';
											return 1;									//zapisano na serwerze
										}
										else{liczba_errorow ++;}									
									} while (liczba_errorow < 3);
									return 0;								//nie zapisano na serwerze/serwer jest pelny

								}
								else{liczba_errorow ++;}							
							} while (liczba_errorow < 3);
							return 0;									// nie przeslano adresu
						
						}
						else{liczba_errorow ++;}						
					} while (liczba_errorow < 3);
					return 0;											// nie przeslano bajtow 

				}
				else
				{
					liczba_errorow ++;
				}			
			} while (liczba_errorow < 3);
			return 0;							//trzeba dopisac rozlaczanie z serwera przed ponowna proba laczenia z serwerem		// nie polaczono z serwerem 
/*
		}
		else{liczba_errorow ++;}	*/
			
	} while (liczba_errorow < 3);
	return 0;					//  nie przelaczaczono na stacje robocza

}

void esp8866_resetuj (void)
{
	PORTD &=~ (1<<PD6);
	_delay_ms(100);
	PORTD |= (1<<PD6);
	_delay_ms(8000);
	
}
void pobierz_czas (void)
{
	czas_odebrany = 0;
	czas_odebrany = czas_odebrany +  16777216ul*(* wynik); //16e6;
	wynik ++;
	czas_odebrany = czas_odebrany + 65536ul*(* wynik) ;//16e4;
	wynik ++;
	czas_odebrany = czas_odebrany + 256ul*(* wynik);//16e2;
	wynik ++;
	czas_odebrany = czas_odebrany + (* wynik);
	
	czas_UNIX = czas_odebrany - 2208988800;
	
	
	
}
void wyslij_czas (void)
{
	index_odebrany = 0; //Zeby odpowiedz byla pakowana na poczatek buforu
	while (!(UCSRA & (1<<UDRE)));
	UDR = czas_UNIX;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0d;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0a;
	czas_UNIX = czas_UNIX >> 8;
	while (!(UCSRA & (1<<UDRE)));
	UDR = czas_UNIX;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0d;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0a;
	czas_UNIX = czas_UNIX >> 8;
	while (!(UCSRA & (1<<UDRE)));
	UDR = czas_UNIX;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0d;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0a;
	czas_UNIX = czas_UNIX >> 8;
	while (!(UCSRA & (1<<UDRE)));
	UDR = czas_UNIX;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0d;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0a;
	
}

void wyzeruj_dane_po_wyslaniu_na_serwer (void)
{
		_delay_ms(2000);
		rozlacz();
		dana_do_wysylki = 0;
		czas_UNIX = 0;
		_delay_ms(10000);

}
void wyslij_bajty (void)
{
	index_odebrany = 0;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x1b;
	for (index_tekstu = 0; index_tekstu < 47; index_tekstu++)
	{
		while (!(UCSRA & (1<<UDRE)));
		UDR = 0;
	}
	
	/*  przenios³em do maun()
	_delay_ms(1000);
	while (!(UCSRA & (1<<UDRE)));
	UDR = '+';
	while (!(UCSRA & (1<<UDRE)));
	UDR = '+';	
	while (!(UCSRA & (1<<UDRE)));
	UDR = '+';	
	_delay_ms(1000);
	*/
}

void polacz_z_NTP (void)
{
	index_odebrany = 0; //Zeby odpowiedz byla pakowana na poczatek buforu
	for (index_tekstu = 0; index_tekstu < strlen_P(komenda_polacz); index_tekstu++)
	{
		while (!(UCSRA & (1<<UDRE)));
		UDR = komenda_polacz[index_tekstu];
	}
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0d;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0a;
}
void rozlacz (void)
{
	index_odebrany = 0; //Zeby odpowiedz byla pakowana na poczatek buforu
	for (index_tekstu = 0; index_tekstu < strlen_P(komenda_rozlacz); index_tekstu++)
	{
		while (!(UCSRA & (1<<UDRE)));
		UDR = komenda_rozlacz[index_tekstu];
	}
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0d;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0a;
	
}

unsigned long polacz_i_pobierz_czas_UNIX (void)
{
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
			
			
			if (wynik != NULL)
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
				
				wynik = 0;
				return czas_UNIX;
				
			}
				
			else
			{
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
				wynik = 0;
				return 66;
			}



		}
		else return 66;	
				
	}
	else return 66;			
}



void ustaw_w_tryb_at(void)
{
	UCSRA |= (1<<U2X);
	UBRRL = 7; // predkosc 115200 bps
	UCSRB |= (1<<RXEN) | (1<<TXEN);// wlacz transmisje i odbior
	//transmisja asynchroniczna nie trzeba
	//parzystosci nie trzeba
	//bitow stop nie trzeba
	UCSRC |= (1<<URSEL) | (1 << UCSZ1) | (1 <<UCSZ0); //wysylaj 8 bitow
	UCSRB |= (1<<RXCIE);//wlacz przerwania odbioru
}
void ustaw_w_tryb_normalny(void)
{
	UCSRA |= (1<<U2X);
	UBRRL = 51; // predkosc 9600 bps
	UCSRB |= (1<<RXEN) | (1<<TXEN);// wlacz transmisje i odbior
	//transmisja asynchroniczna nie trzeba
	//parzystosci nie trzeba
	//bitow stop nie trzeba
	UCSRC |= (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0); //wysylaj 8 bitow
	UCSRB |= (1<<RXCIE);//wlacz przerwania odbioru
}

