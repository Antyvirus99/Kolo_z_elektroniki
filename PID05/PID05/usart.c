/*
 * CFile1.c
 *
 * Created: 2016/12/07 01:11:44
 *  Author: Dondu
 */ 


#include <avr/io.h>

#include "usart.h"


void usart_inicjuj(void)
{
	//definiowanie parametrów transmisji za pomoc¹ makr zawartych w pliku
	//nag³ówkowym setbaud.h. Je¿eli wybierzesz prêdkoœæ, która nie bêdzie
	//mo¿liwa do realizacji otrzymasz ostrze¿enie:
	//#warning "Baud rate achieved is higher than allowed"
	
	#define BAUD 9600        //tutaj podaj ¿¹dan¹ prêdkoœæ transmisji
	#include <util/setbaud.h> //linkowanie tego pliku musi byæ
	//po zdefiniowaniu BAUD
	
	//ustaw obliczone przez makro wartoœci
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	#if USE_2X
	UCSRA |=  (1<<U2X);
	#else
	UCSRA &= ~(1<<U2X);
	#endif
	
	
	//Ustawiamy pozosta³e parametry modu³ USART
	//U W A G A !!!
	//W ATmega8, aby zapisaæ do rejestru UCSRC nale¿y ustawiaæ bit URSEL
	//zobacz tak¿e: http://mikrokontrolery.blogspot.com/2011/04/avr-czyhajace-pulapki.html#avr_pulapka_2
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);  //bitów danych: 8
	//bity stopu:  1
	//parzystoœæ:  brak
	//w³¹cz nadajnik i odbiornik oraz ich przerwania odbiornika
	//przerwania nadajnika w³¹czamy w funkcji wyslij_wynik()
	UCSRB = (1<<TXEN);
	DDRD |= (1<<PD1);	//TxD na wyjœcie
}

//-------------------------------------------------------------------------------------

void usart_send(void)
{
	for (int z=0; z<sizeof (usart_bufor); z++)
	{
		while (!(UCSRA & (1<<UDRE)));
		if( usart_bufor[z] != 0 ) UDR = usart_bufor[z];
		else
		{
			UDR = ' ';
			break;
		}
	}

}

//-------------------------------------------------------------------------------------


void usart_send_space(void)
{
	while (!(UCSRA & (1<<UDRE)));
	UDR = ' ';
}

//-------------------------------------------------------------------------------------


void usart_send_newline(void)
{
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0d;
	while (!(UCSRA & (1<<UDRE)));
	UDR = 0x0a;
}


//-------------------------------------------------------------------------------------
