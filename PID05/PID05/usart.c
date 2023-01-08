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
	//definiowanie parametr�w transmisji za pomoc� makr zawartych w pliku
	//nag��wkowym setbaud.h. Je�eli wybierzesz pr�dko��, kt�ra nie b�dzie
	//mo�liwa do realizacji otrzymasz ostrze�enie:
	//#warning "Baud rate achieved is higher than allowed"
	
	#define BAUD 9600        //tutaj podaj ��dan� pr�dko�� transmisji
	#include <util/setbaud.h> //linkowanie tego pliku musi by�
	//po zdefiniowaniu BAUD
	
	//ustaw obliczone przez makro warto�ci
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	#if USE_2X
	UCSRA |=  (1<<U2X);
	#else
	UCSRA &= ~(1<<U2X);
	#endif
	
	
	//Ustawiamy pozosta�e parametry modu� USART
	//U W A G A !!!
	//W ATmega8, aby zapisa� do rejestru UCSRC nale�y ustawia� bit URSEL
	//zobacz tak�e: http://mikrokontrolery.blogspot.com/2011/04/avr-czyhajace-pulapki.html#avr_pulapka_2
	UCSRC = (1<<URSEL) | (1<<UCSZ1) | (1<<UCSZ0);  //bit�w danych: 8
	//bity stopu:  1
	//parzysto��:  brak
	//w��cz nadajnik i odbiornik oraz ich przerwania odbiornika
	//przerwania nadajnika w��czamy w funkcji wyslij_wynik()
	UCSRB = (1<<TXEN);
	DDRD |= (1<<PD1);	//TxD na wyj�cie
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
