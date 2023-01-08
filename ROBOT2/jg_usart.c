/*
 * jg_usart.c
 *
 * Created: 19.01.2016 18:39:41
 *  Author: Donat
 */ 

#include <avr/io.h>
/*
	void jg_usart_inicjuj_atmega8_hc05 (void)
	{
		 UCSR0A |= (1<<U2X0);
		 UBRR0L = 12;
		 UCSR0C |= (1<<UCSZ00) | (1<<UCSZ01);
		 //UCSRC |= (1<<URSEL) | (0<<UPM0) | (0<<UPM1);
		 //UCSRC |= (1<<URSEL) | (1<<USBS);
		 //UCSRC |= (0<<UMSEL);
		 UCSR0B |= (1<<RXCIE0); //przerwania od odbioru danych
		 UCSR0B |= (1<<TXEN0) | (1<<RXEN0);
		 DDRD |= (1<<PD1); //PIN txd jako wyjscie
	}
*/	
	
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
		UBRR0H = UBRRH_VALUE;
		UBRR0L = UBRRL_VALUE;
		#if USE_2X
		UCSR0A |=  (1<<U2X0);
		#else
		UCSR0A &= ~(1<<U2X0);
		#endif
		
		
		//Ustawiamy pozosta³e parametry modu³ USART
		//U W A G A !!!
		//W ATmega8, aby zapisaæ do rejestru UCSRC nale¿y ustawiaæ bit URSEL
		//zobacz tak¿e: http://mikrokontrolery.blogspot.com/2011/04/avr-czyhajace-pulapki.html#avr_pulapka_2
		UCSR0C = (1<<UCSZ01) | (1<<UCSZ00);  //bitów danych: 8
		//bity stopu:  1
		//parzystoœæ:  brak
		//w³¹cz nadajnik i odbiornik oraz ich przerwania odbiornika
		//przerwania nadajnika w³¹czamy w funkcji wyslij_wynik()
		UCSR0B = (1<<TXEN0) | (1<<RXEN0) | (1<<RXCIE0);
	}