/*
 * usart.c
 *
 * Created: 09.05.2017 18:32:36
 *  Author: Donat
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "stdlib.h"


void inicjuj_usart (void)
{
	UCSRB |= (1<<RXEN) | (1<<TXEN);						//wlacz tranmisje i odbior
	UCSRA |= (1<<U2X);
	UBRRH = (767>>8);	
	UBRRL = (unsigned char)767;									//2400 na 14,7456 mHz
	//transmisja asynchroniczna nie trzeba
	//brak parzystosci nie trzeba
	//ilosc bitow stop nie trzeba
	UCSRC |= (1<<UCSZ0) | (1<<UCSZ1) | (1<<URSEL);		//8 bitow danych
	UCSRB |= (1<<RXCIE);								//wlacz przerwania odbioru
		
} 