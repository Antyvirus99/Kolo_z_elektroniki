/*
 * Grzesiek_bluetooth.c
 *
 * Created: 16.01.2016 10:49:27
 * Author : Donat
 
 -p  atmega48 -B8  -c usbasp -P usb  -U flash:w:$(ProjectDir)Debug\$(ItemFileName).hex:i
 
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "jg_usart.h"
#include "dane.h"

#include "silniki.h"

#define PWM_START 0
#define PWM_LEWO_PRAWO 0
#define SUWAK_SRODEK 50
unsigned char bajt_odebrany;
volatile unsigned long timer1_licznik_przepelnien_prawy_enkoder = 0;
volatile unsigned long timer1_licznik_przepelnien_lewy_enkoder = 0;
volatile unsigned long wynik_lewy_enkoder = 0;
volatile unsigned long wynik_prawy_enkoder = 0;
volatile unsigned char wyslij_ramke = 0;
volatile unsigned long predkosc_prawa = 0;
volatile unsigned long predkosc_lewa = 0;

volatile unsigned long wynik;
unsigned int nastawa[2] = {50,50};
volatile unsigned char rozkaz = 0;

volatile unsigned char ignoruj_dane;


ISR (TIMER0_OVF_vect)
{
	
	static unsigned char x = 0;
	
	x ++;
	if (x >= (F_CPU / 1024 /256))
	{
		//funkcja przerwania timera0 KILKA razy na s
	
		wyslij_ramke = 1;
		x=0;
		//ramka_nadawcza[1] ^= (1<<3);
		//ramka_nadawcza[1] ++;
	
	}
}

ISR (USART_RX_vect) //funkcja przerwania odbioru danych (jednego bajt) usart 
{
	static unsigned int index = 0;
	
	
	
	if (ignoruj_dane)
	{
		TCNT0 = UDR0;
		
	} 
	else
	{
		//rozkaz = UDR0;
		if (index == 0)
		{
			nastawa[0] = UDR0;
			index ++;
		}
		else
		{
			nastawa[1] = UDR0;
			index = 0;
		}
	}
	
	/*nastawa[index] = UDR0;
	index ++;
	if (index >= 2) index = 0;
	*/
	//PORTD ^= (1<<PD7);
	
}

ISR (PCINT2_vect) //przerwanie na zboczu prawe
{
	static unsigned char licznik_przerwan = 0;
	static unsigned long wynik_poprzedni;
	static unsigned long wynik_nastepny;
	
	
	if (PIND & (1<<PD2))
	{
		
		if (licznik_przerwan == 0)
		{
			wynik_poprzedni = TCNT1;
			licznik_przerwan =1;
			timer1_licznik_przepelnien_prawy_enkoder = 0;
			
		}
		else
		{
			wynik_nastepny = TCNT1;

		
			wynik_prawy_enkoder = timer1_licznik_przepelnien_prawy_enkoder * 65536 + wynik_nastepny - wynik_poprzedni;
			//wynik_prawy_enkoder =54321;
			//przygotuj timer do ponownego pomiaru
		
			timer1_licznik_przepelnien_prawy_enkoder =0;
			licznik_przerwan = 0;
			
		
		}

	}
}

ISR (PCINT0_vect) //przerwanie na zboczu lewe
{
	static unsigned char licznik_przerwan = 0;
	static unsigned long wynik_poprzedni;
	static unsigned long wynik_nastepny;
	
	
	if (PINB & (1<<PB0))
	{
		
		if (licznik_przerwan == 0)
		{
			wynik_poprzedni = TCNT1;
			licznik_przerwan =1;
			timer1_licznik_przepelnien_lewy_enkoder = 0;

		}
		else
		{
			wynik_nastepny = TCNT1;

			
			wynik_lewy_enkoder = timer1_licznik_przepelnien_lewy_enkoder * 65536 + wynik_nastepny - wynik_poprzedni;
			
			//przygotuj timer do ponownego pomiaru
			
			timer1_licznik_przepelnien_lewy_enkoder =0;
			licznik_przerwan = 0;
			PORTD ^= (1<<PD7);
			
		}

	}
}


ISR (TIMER1_OVF_vect) 
{
	
	//while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
	//UDR0 = nastawa[0];
	/*while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
	UDR0 = nastawa[1];
	while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
	UDR0 = SUWAK_SRODEK;
	while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
	UDR0 = PRAWY_WSTECZ_OCR;
	*/
	
	timer1_licznik_przepelnien_prawy_enkoder ++;
	timer1_licznik_przepelnien_lewy_enkoder ++;
}


/*
ISR (TIMER1_CAPT_vect) //funkcja przerwania dla zbocza narastajacego ICP1
{
	
	
	static unsigned long wartosc_poprzednia = 0, wartosc_aktualna = 0;
	static unsigned int icr1_poprzedni = 0, icr1_aktualny = 0;
	
	
	
	//icr1_aktualny = ICR1;
	//wynik = icr1_aktualny;

	if(	icr1_aktualny > icr1_poprzedni)
	{
		wartosc_aktualna = timer1_licznik_przepelnien * 256 + (icr1_aktualny - icr1_poprzedni);
	}
	else
	{
		wartosc_aktualna = (timer1_licznik_przepelnien - 1) * 256 + (256 - icr1_poprzedni) + icr1_aktualny;
	}
	
	wartosc_aktualna = timer1_licznik_przepelnien * 256 + (icr1_aktualny > icr1_poprzedni)? icr1_aktualny - icr1_poprzedni : icr1_poprzedni - icr1_aktualny;
	//if (wartosc_aktualna > wartosc_poprzednia) wynik = wartosc_aktualna - wartosc_poprzednia;
	//else wynik =  wartosc_poprzednia - wartosc_aktualna;
	//wynik =  wartosc_poprzednia - wartosc_aktualna;
	wynik = icr1_aktualny;
	wartosc_poprzednia = wartosc_aktualna;
	timer1_licznik_przepelnien = 0;
	icr1_poprzedni = icr1_aktualny;	
	
	
	
	icr1_aktualny = TCNT1;
	wartosc_aktualna = timer1_licznik_przepelnien * 256 + (icr1_aktualny > icr1_poprzedni)? icr1_aktualny - icr1_poprzedni : icr1_poprzedni - icr1_aktualny;
	if (wartosc_aktualna > wartosc_poprzednia) wynik = wartosc_aktualna - wartosc_poprzednia;
	else wynik =  wartosc_poprzednia - wartosc_aktualna;
	wartosc_poprzednia = wartosc_aktualna;
	timer1_licznik_przepelnien = 0;
	icr1_poprzedni = icr1_aktualny;	
	
}
*/

int main(void)
{
	unsigned long temp_prawy;
	unsigned long temp_lewy;
	
	signed int wartosc_OCR_prawy;
	signed int wartosc_OCR_lewy;
	silniki_inicjuj();
	
	ignoruj_dane = 1;
	usart_inicjuj();
	
	sei();
	_delay_ms(2000);
	ignoruj_dane = 0;
	
	
	//w�acz zasilanie HC05
	DDRD |= (1<<PD4);
	PORTD &=~ (1<<PD4);

	
	
	
	//Program zadeklarowany jest dla ATmega8, a pracujesz na ATmega48  :-)
	//Po zmianie zapewne bedziesz musia� poprawi� funkcje USART itp bo rejestry s� inne.
	
	
	DDRD	|=	(1<<PD7);	//LED
	DDRB	|= (1<<PB2) | (1<<PB1); 
	
	//Preskaler timer  1
	TCCR1B |= (1<<CS12);
	
	//Przerwanie na PCINT0
	PCMSK0 |= (1<<PCINT0);
	PCICR |= (1<<PCIE0);
	
	
	//Przerwanie na PCINT18
	PCMSK2 |= (1<<PCINT18);
	PCICR |= (1<<PCIE2);
	
	
	DDRC |= (1<< PC1);
	PORTC &=~ (1<<PC1);
	
	/*
	PRAWY_NAPRZOD_OCR = 50;
	LEWY_NAPRZOD_OCR =50;
	silnik1_naprzod();
	silnik2_naprzod();
	*/
	while(1)
	{
		switch (nastawa[0])//przod-tyl
		{
			case (SUWAK_SRODEK + 5) ... 100 :	//naprzod
			
			
				switch (nastawa[1])
				{
					case (SUWAK_SRODEK + 5) ... 100:	//prawo			
						
						LEWY_WSTECZ_OCR	 = 0;
						PRAWY_WSTECZ_OCR = 0;
						
						wartosc_OCR_prawy = (nastawa[0] - SUWAK_SRODEK) - (nastawa[1] - SUWAK_SRODEK);
						wartosc_OCR_lewy  =  (nastawa[0] - SUWAK_SRODEK) + (nastawa[1] - SUWAK_SRODEK);
						
						if(wartosc_OCR_prawy < 0) wartosc_OCR_prawy = 0;
						if(wartosc_OCR_lewy < 0) wartosc_OCR_lewy = 0;
						
						if(wartosc_OCR_prawy > 100) wartosc_OCR_prawy = 100;
						if(wartosc_OCR_lewy > 100) wartosc_OCR_lewy = 100;
						
						
						PRAWY_NAPRZOD_OCR = wartosc_OCR_prawy;
						LEWY_NAPRZOD_OCR  = wartosc_OCR_lewy*2;
						silnik1_naprzod();
						silnik2_naprzod();
						break;
						
					case 0 ... (SUWAK_SRODEK - 5):	//lewo	
						
						LEWY_WSTECZ_OCR	 = 0;
						PRAWY_WSTECZ_OCR = 0;	
										
						wartosc_OCR_prawy = (nastawa[0] - SUWAK_SRODEK) + (SUWAK_SRODEK - nastawa[1]);
						wartosc_OCR_lewy = (nastawa[0] - SUWAK_SRODEK) - (SUWAK_SRODEK - nastawa[1]);
						
						if(wartosc_OCR_prawy < 0) wartosc_OCR_prawy = 0;
						if(wartosc_OCR_lewy < 0) wartosc_OCR_lewy = 0;
						
						if(wartosc_OCR_prawy > 100) wartosc_OCR_prawy = 100;
						if(wartosc_OCR_lewy > 100) wartosc_OCR_lewy = 100;
						
						PRAWY_NAPRZOD_OCR = wartosc_OCR_prawy;
						LEWY_NAPRZOD_OCR = wartosc_OCR_lewy*2;
						
						
						silnik1_naprzod();
						silnik2_naprzod();
						
						break;
						
					default://do przodu
						
						LEWY_WSTECZ_OCR	 = 0;
						PRAWY_WSTECZ_OCR = 0;
						
						wartosc_OCR_prawy = nastawa[0] - SUWAK_SRODEK;
						wartosc_OCR_lewy = nastawa[0] - SUWAK_SRODEK;
						
						if(wartosc_OCR_prawy < 0) wartosc_OCR_prawy = 0;
						if(wartosc_OCR_lewy < 0) wartosc_OCR_lewy = 0;
						
						if(wartosc_OCR_prawy > 100) wartosc_OCR_prawy = 100;
						if(wartosc_OCR_lewy > 100) wartosc_OCR_lewy = 100;						
						
						//wartosc_OCR_prawy = 20;
						//wartosc_OCR_lewy = 20;
						
						PRAWY_NAPRZOD_OCR = wartosc_OCR_prawy;
						LEWY_NAPRZOD_OCR = wartosc_OCR_lewy*2;
						silnik1_naprzod();
						silnik2_naprzod();
						
						break;
	
				}
				break;	
				
			case 0 ... (SUWAK_SRODEK - 5) :	//wstecz
			
				switch (nastawa[1])
				{
					case (SUWAK_SRODEK + 5) ... 100:	//prawo
						
						LEWY_NAPRZOD_OCR = 0;
						PRAWY_NAPRZOD_OCR = 0;
					
						wartosc_OCR_prawy = (SUWAK_SRODEK - nastawa[0]) - (nastawa[1] - SUWAK_SRODEK);
						wartosc_OCR_lewy = (SUWAK_SRODEK - nastawa[0]) + (nastawa[1] - SUWAK_SRODEK);				
					
						if(wartosc_OCR_prawy < 0) wartosc_OCR_prawy = 0;
						if(wartosc_OCR_lewy < 0) wartosc_OCR_lewy = 0;
						
						if(wartosc_OCR_prawy > 100) wartosc_OCR_prawy = 100;
						if(wartosc_OCR_lewy > 100) wartosc_OCR_lewy = 100;
					
						PRAWY_WSTECZ_OCR = wartosc_OCR_prawy;
						LEWY_WSTECZ_OCR	 = wartosc_OCR_lewy*2;
						silnik1_wstecz();
						silnik2_wstecz();
						break;
						
					case 0 ... (SUWAK_SRODEK - 5):	//lewo
						
						LEWY_NAPRZOD_OCR = 0;
						PRAWY_NAPRZOD_OCR = 0;
					
						wartosc_OCR_prawy = (SUWAK_SRODEK - nastawa[0]) + (SUWAK_SRODEK - nastawa[1]);
						wartosc_OCR_lewy = (SUWAK_SRODEK - nastawa[0]) - (SUWAK_SRODEK - nastawa[1]);
					
						if(wartosc_OCR_prawy < 0) wartosc_OCR_prawy = 0;
						if(wartosc_OCR_lewy < 0) wartosc_OCR_lewy = 0;
						
						if(wartosc_OCR_prawy > 100) wartosc_OCR_prawy = 100;
						if(wartosc_OCR_lewy > 100) wartosc_OCR_lewy = 100;
					
						PRAWY_WSTECZ_OCR = wartosc_OCR_prawy;
						LEWY_WSTECZ_OCR	 = wartosc_OCR_lewy*2;
						silnik1_wstecz();
						silnik2_wstecz();
						break;
						
					default://prosto do tylu
						
						LEWY_NAPRZOD_OCR = 0;
						PRAWY_NAPRZOD_OCR = 0;
						
						wartosc_OCR_prawy = SUWAK_SRODEK - nastawa[0];
						wartosc_OCR_lewy = SUWAK_SRODEK - nastawa[0];

						if(wartosc_OCR_prawy < 0) wartosc_OCR_prawy = 0;
						if(wartosc_OCR_lewy < 0) wartosc_OCR_lewy = 0;
						
						if(wartosc_OCR_prawy > 100) wartosc_OCR_prawy = 100;
						if(wartosc_OCR_lewy > 100) wartosc_OCR_lewy = 100;						
						
						PRAWY_WSTECZ_OCR = wartosc_OCR_prawy;
						LEWY_WSTECZ_OCR	 = wartosc_OCR_lewy*2;
						silnik1_wstecz();
						silnik2_wstecz();
						break;
						
				}
				break;
				
				
			default://obrot w miejscu
			
				switch (nastawa[1])
				{
					case (SUWAK_SRODEK + 5) ... 100:	//prawo
						
						LEWY_WSTECZ_OCR	 = 0;
						PRAWY_NAPRZOD_OCR = 0;
						
						wartosc_OCR_prawy = nastawa[1] - SUWAK_SRODEK;
						wartosc_OCR_lewy = nastawa[1] - SUWAK_SRODEK;
						
						if(wartosc_OCR_prawy < 0) wartosc_OCR_prawy = 0;
						if(wartosc_OCR_lewy < 0) wartosc_OCR_lewy = 0;
						
						if(wartosc_OCR_prawy > 100) wartosc_OCR_prawy = 100;
						if(wartosc_OCR_lewy > 100) wartosc_OCR_lewy = 100;						
						
						PRAWY_WSTECZ_OCR = wartosc_OCR_prawy;
						LEWY_NAPRZOD_OCR = wartosc_OCR_lewy*2; 
						silnik1_wstecz();
						silnik2_naprzod();
						
						break;
						
					case 0 ... (SUWAK_SRODEK - 5):	//lewo
						
						LEWY_NAPRZOD_OCR = 0;
						PRAWY_WSTECZ_OCR = 0;
						
						wartosc_OCR_prawy = SUWAK_SRODEK - nastawa[1];
						wartosc_OCR_lewy = SUWAK_SRODEK - nastawa[1];
						
						if(wartosc_OCR_prawy < 0) wartosc_OCR_prawy = 0;
						if(wartosc_OCR_lewy < 0) wartosc_OCR_lewy = 0;
						
						if(wartosc_OCR_prawy > 100) wartosc_OCR_prawy = 100;
						if(wartosc_OCR_lewy > 100) wartosc_OCR_lewy = 100;												
						
						PRAWY_NAPRZOD_OCR = wartosc_OCR_prawy;
						LEWY_WSTECZ_OCR	 = wartosc_OCR_lewy*2;
						silnik1_naprzod();
						silnik2_wstecz();						
						break;
						
					default:
						
						PRAWY_NAPRZOD_OCR = 0;
						LEWY_WSTECZ_OCR	 = 0;
						LEWY_NAPRZOD_OCR = 0;
						PRAWY_WSTECZ_OCR = 0;
						silnik1_stop();
						silnik2_stop();
						
						
						
					
				}
		}
		
		
		cli();
		temp_prawy = wynik_prawy_enkoder;
		temp_lewy = wynik_lewy_enkoder;
		sei();
		
		
		/* POCZATEK ALGORYTMU PID
		
		if (temp_prawy>900)
		{
			if (PRAWY_NAPRZOD_OCR <= 254) PRAWY_NAPRZOD_OCR = PRAWY_NAPRZOD_OCR + 1;			
		}
		if (temp_prawy<900)
		{
			if (PRAWY_NAPRZOD_OCR >= 1)PRAWY_NAPRZOD_OCR = PRAWY_NAPRZOD_OCR - 1;
		}
		if (temp_lewy>900)
		{
			if (LEWY_NAPRZOD_OCR <= 254)LEWY_NAPRZOD_OCR = LEWY_NAPRZOD_OCR + 1;
		}
		if (temp_lewy<900)
		{
			if (LEWY_NAPRZOD_OCR >= 1)LEWY_NAPRZOD_OCR = LEWY_NAPRZOD_OCR - 1;
		}
		*/
		_delay_ms(100);
		
		/*
		while (!(UCSR0A & (1<<UDRE0)));//zaczekaj na wolny bufor nadawania
		UDR0	=  (PRAWY_NAPRZOD_OCR)? PRAWY_NAPRZOD_OCR : PRAWY_WSTECZ_OCR;		
		while (!(UCSR0A & (1<<UDRE0)));//zaczekaj na wolny bufor nadawania
		UDR0	= (LEWY_WSTECZ_OCR)? LEWY_WSTECZ_OCR : LEWY_NAPRZOD_OCR;	
		*/
		
		
		/*
		while (!(UCSR0A & (1<<UDRE0)));//zaczekaj na wolny bufor nadawania
		UDR0	= (unsigned char) nastawa[0];
		while (!(UCSR0A & (1<<UDRE0)));//zaczekaj na wolny bufor nadawania
		UDR0	=  (unsigned char) nastawa[1];
		*/
		
		
		
		//przeysyl prawego enkodera
		//temp_prawy = 54321;
		while (!(UCSR0A & (1<<UDRE0)));//zaczekaj na wolny bufor nadawania
		UDR0	=  (unsigned char) temp_prawy;
		temp_prawy >>= 8;
		
		while (!(UCSR0A & (1<<UDRE0)));//zaczekaj na wolny bufor nadawania
		UDR0	=  (unsigned char) temp_prawy;
		
		
		//przesyl lewego enkodera
		//temp_lewy = 12345;
		while (!(UCSR0A & (1<<UDRE0)));//zaczekaj na wolny bufor nadawania
		UDR0	=  (unsigned char) temp_lewy;
		temp_lewy >>= 8;		 
			
		while (!(UCSR0A & (1<<UDRE0)));//zaczekaj na wolny bufor nadawania
		UDR0	=  (unsigned char) temp_lewy;
		 
		/*
		if (nastawa[0]>60) //naprzod
		{			
			
			PRAWY_NAPRZOD_OCR = nastawa[0] - 50 + (abs (nastawa[1] - 50) > 10)?  abs (nastawa[1] - 50) : 0 ; 
			LEWY_NAPRZOD_OCR = nastawa[0] - 50 + (abs (nastawa[1] - 50) > 10)?  abs (nastawa[1] - 50) : 0 ; 
		}
		else if (nastawa[0]<40)//wstecz
		{
			PRAWY_WSTECZ_OCR = 50 - nastawa[0] + (abs (nastawa[1] - 50) > 10)?  abs (nastawa[1] - 50) : 0 ; 
			LEWY_WSTECZ_OCR	 = 50 - nastawa[0] + (abs (nastawa[1] - 50) > 10)?  abs (nastawa[1] - 50) : 0 ; 
		}
		else
		{
			
		}*/
	/*
		if (rozkaz == 1)
		{
			//testowe wartosci pwm
			PRAWY_WSTECZ_OCR = PWM_START;
			PRAWY_NAPRZOD_OCR = PWM_START;
			LEWY_WSTECZ_OCR	 = PWM_START;
			LEWY_NAPRZOD_OCR = PWM_START;
			
			silnik1_naprzod();
			silnik2_naprzod();
		}
		//_delay_ms(1000);
		if (rozkaz == 2)
		{
			//testowe wartosci pwm
			PRAWY_WSTECZ_OCR = PWM_START;
			PRAWY_NAPRZOD_OCR = PWM_START;
			LEWY_WSTECZ_OCR	 = PWM_START;
			LEWY_NAPRZOD_OCR = PWM_START;
			
			silnik1_wstecz();
			silnik2_wstecz();
		}
		//_delay_ms(1000);
		if (rozkaz == 3)//lewo
		{
			_delay_ms(500);
			//testowe wartosci pwm
			PRAWY_WSTECZ_OCR = PWM_LEWO_PRAWO;
			PRAWY_NAPRZOD_OCR = PWM_LEWO_PRAWO;
			LEWY_WSTECZ_OCR	 = PWM_LEWO_PRAWO;
			LEWY_NAPRZOD_OCR = PWM_LEWO_PRAWO;
			
			silnik1_wstecz();
			silnik2_naprzod();
			
		}
		//_delay_ms(1000);
		if (rozkaz == 4)//prawo
		{
			_delay_ms(500);
			//testowe wartosci pwm
			PRAWY_WSTECZ_OCR = PWM_LEWO_PRAWO;
			PRAWY_NAPRZOD_OCR = PWM_LEWO_PRAWO;
			LEWY_WSTECZ_OCR	 = PWM_LEWO_PRAWO;
			LEWY_NAPRZOD_OCR = PWM_LEWO_PRAWO;
			
			silnik1_naprzod();
			silnik2_wstecz();
		}
		//_delay_ms(1000);
		if (rozkaz == 5)
		{
			silnik1_stop();
			silnik2_stop();
		}
		*/
		/*UDR0 = 0xaa;
		_delay_ms(1000);*/
		
	}
	

}

