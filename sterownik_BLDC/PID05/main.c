/*
 * PID_Grzesiek.c
 *
 * Created: 22.10.2016 14:19:34
 * Author : Grzegorz
 */ 

//adres wysylania 98:D3:31:40:0E:32
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>

#include "usart.h"

char usart_bufor[30];


// paczka danych
typedef struct 
{
	unsigned int		czas_poczatkowy;
	unsigned int		czas_koncowy;
	unsigned int		ovf_counter;
	unsigned long		czas_obrotu_kola_w_impulsach_timer1;
	signed	 long		suma_uchybow;
	signed	 long		uchyb;	
	signed long			pid;
	signed long			uchyb_poprzedni;
}dane_t;

volatile dane_t dane;
		 dane_t dane_temp;


// PID nastawy
volatile unsigned long		zadana_predkosc_w_impulsach = 20000;	//by�o 4500
volatile signed int			kp_x100 = 250;		//250
volatile signed int			ki_x100 = 10;		//10         <--- pokaza� wp�yw zmiany na reakcj� po zwi�kszeniu i gwa�townym zmniejszeniu obci��enia
volatile signed int			kd_x100 = 150;		//150


//volatile unsigned int		stan = 0;
volatile unsigned long		licznik_OVF_dla_usartu;


//-----------------------------------------------------------------------------------------------

ISR (TIMER1_OVF_vect)
{
	dane.ovf_counter ++;
	
	//pomocnicze
	licznik_OVF_dla_usartu ++;
}

//-----------------------------------------------------------------------------------------------

ISR (TIMER1_CAPT_vect)
{
	//przerwanie obliczaj�ce PID

	static unsigned int		czas_poczatkowy = 0;
	
	dane.czas_koncowy		= ICR1;	//z�apany stan licznika
	
	dane.czas_obrotu_kola_w_impulsach_timer1 = (dane.ovf_counter * 1024UL + dane.czas_koncowy) - dane.czas_poczatkowy;	//DD  1024 a nie 65536, bo timer1 ustawiony na PWM 10 bit
		
	//aktualna minus zadana poniewa� czas obrotu ro�nie wraz ze spadkiem pr�dko�ci, co oznacza, �e powinni�my zwi�ksza� OCR1,
	//a to oznacza, �e uchyb powinien by� dodatni
	dane.uchyb = dane.czas_obrotu_kola_w_impulsach_timer1 - zadana_predkosc_w_impulsach;	
		
	dane.suma_uchybow	=  dane.suma_uchybow + dane.uchyb;
		
	dane.pid = (kp_x100 * dane.uchyb	+	ki_x100 * dane.suma_uchybow	+	kd_x100 * (dane.uchyb - dane.uchyb_poprzedni)) / 50000; // dzielimyu przez 100 bo wsp�czynniki s� pomno�one przez 100
																																	//zastanowi� si�, dlaczego 50000 jest optymalnym dzielnikiem !!!!!
																																	//obserwuj�c zmiany pid czyli OCR1A
	dane.uchyb_poprzedni = dane.uchyb;
	
	//zabezpieczenie zakresu OCR1A
	if (dane.pid < 0)		dane.pid = 0;
	if (dane.pid > 1023)	dane.pid = 1023;
	
	//ustaw PID
	OCR1A = dane.pid;			//szerokosc impulsu PWM reguluje pr�dem silnika

	//paczka danych	- pozosta�e
	dane.czas_poczatkowy	= czas_poczatkowy;
	dane.ovf_counter		= dane.ovf_counter;
	
	

	//przygotuj do nast�pnego pomiaru
	dane.ovf_counter = 0;
	//czas_poczatkowy			= dane.czas_koncowy ;
	
	//Doda�em zerowanie - om�wi� i pokaza� wp�yw na problem peak�w na wykresie czas_obrotu_kola_w_impulsach_timer1 
	czas_poczatkowy =0;//dane.czas_koncowy;
	TCNT1 = 0;
	TIFR |= TOV1;
	
	//czas_poczatkowy = dane.czas_koncowy ;
	
	
}

//-----------------------------------------------------------------------------------------------


void wyslij_dane_szczegolowe(void)
{
	utoa (dane_temp.czas_poczatkowy, usart_bufor, 10);
	usart_send();

	utoa (dane_temp.czas_koncowy, usart_bufor, 10);
	usart_send();

	utoa (dane_temp.ovf_counter, usart_bufor, 10);
	usart_send();

	ultoa (dane_temp.czas_obrotu_kola_w_impulsach_timer1, usart_bufor, 10);
	usart_send();
	
	utoa (dane_temp.pid, usart_bufor, 10);
	usart_send();
	
	itoa (dane_temp.suma_uchybow, usart_bufor, 10);
	usart_send();
	
	itoa (dane_temp.uchyb - dane_temp.uchyb_poprzedni, usart_bufor, 10);
	usart_send();
	
	usart_send_newline();
}


//-----------------------------------------------------------------------------------------------

int main(void)
{
	

	
	// Timer1 - PWM
    DDRB	|= (1<<PB1); 					//oc1a wyjscie silnik
	TCCR1A	|= (1<<COM1A1);					//non invertic mode pwm
	TCCR1A	|= (1<<WGM11) | (1<<WGM10);		// fast PWM 10bit liczy do 1024
	TCCR1B	|= (1<<WGM12);
	
	
	// Timer1- ICP pomiar czasu trwania impulsu
	PORTB |= (1<<PB0);						//pull-up na pinie ICP1
	TCCR1B |= (1<<ICNC1);					// redukcja szumow
	TCCR1B |= (1<<ICES1);					//1 na zboczu narastajacym
	TIMSK |= (1<<TICIE1);					//przerwania ICP
	TIMSK |= (1<<TOIE1);					//wlacz przerwania przepelnienia
	
	
	//Wyjscia pomocnicze
	DDRD	|=(1<<PD7);
	DDRD	|=(1<<PD6);
	DDRD	|=(1<<PD0);
	
	//pull-upy
	PORTD	|=(1<<PD5);						//pullup przycisku

	
	_delay_ms(1000);

	

	// USART	
	usart_inicjuj();

	//wlacz timer1
	TCCR1B	|= (1<<CS10);					//prescaler 1 - by� wcze�nie 8
	
	//wystartuj silnik
	OCR1A = 1023;							//maksymalne PWM
	_delay_ms(50);

	

	
	sei();									//wlacz przerwania globalne
	
	
	//p�tla g��wna
    while (1) 
    {
		
		//zabezpieczenie przed stawaniem silnika
		if(dane_temp.ovf_counter > 300) OCR1A = 1023;
		
		
		
		//zapamietaj wynik
		cli();
		dane_temp = dane;
		sei();

			
		#define USART_WYKRES_CZAS_OBROTU		1
		//#define USART_WYKRES_OCR1A	1
		//#define USART_PELNE			1
		
		#ifdef USART_WYKRES_CZAS_OBROTU
			//Wysy�aj dane co konkretny zadany czas
			if((licznik_OVF_dla_usartu >= 800) && (UCSRA & (1<<UDRE)))
			{
				cli();
				UDR = dane_temp.czas_obrotu_kola_w_impulsach_timer1 / 500;	// dzielimy by dostosowa� do aplikacji rysuj�cej wykres 0-255
				sei();
				licznik_OVF_dla_usartu = 0;
				//if(zadana_predkosc_w_impulsach < 65000) zadana_predkosc_w_impulsach += 5;
			}
		#endif
		
		#ifdef USART_WYKRES_OCR1A
			//Wysy�aj dane co konkretny zadany czas
			if((licznik_OVF_dla_usartu >= 800) && (UCSRA & (1<<UDRE)))
			{
				cli();
				UDR = (unsigned char) (dane_temp.pid /4);	// dzielimy przez 4 by dostosowa� do aplikacji rysuj�cej wykres 0-255
				sei();
				licznik_OVF_dla_usartu = 0;
			}
		#endif
		
		#ifdef USART_PELNE		
			//Wysy�aj dane co konkretny zadany czas
			if((licznik_OVF_dla_usartu >= 2000) && (UCSRA & (1<<UDRE)))
			{
				wyslij_dane_szczegolowe();
				licznik_OVF_dla_usartu = 0;
			}
		#endif
		
	}
}



