/*
 * PID_Grzesiek.c
 *
 * Created: 22.10.2016 14:19:34
 * Author : Donat
 */ 

/*
kod poprawi³em - szukaæ znaków: //DD
najpierw jednak pokazaæ jak wywo³ywane s¹ przerwania przepe³nienia timer 1 - w tym celu u¿ywaæ przycisku (na PD5) i oscyloskopu z przebiegów:
1. z enkodera
2. z PD6 - OVF TIMER1
3. z PD7 - pocz¹tek i koniec pomiaru

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
	unsigned int czas_poczatkowy;
	unsigned int czas_koncowy;
	unsigned int ovf_counter;
	
}dane_t;

volatile dane_t dane;
		 dane_t dane_temp;




// PID
volatile signed long		pid;
volatile signed int			kp_x100 = 250;		//150
volatile signed int			ki_x100 = 1;		//10
volatile signed int			kd_x100 = 100;		//70

signed	 long				suma_uchybow = 0;
signed	 long					uchyb = 0;


volatile unsigned int		stan = 0;
volatile unsigned long		licznik_OVF_dla_czasu;
volatile unsigned long		licznik_OVF_dla_usartu;
volatile unsigned long		proba = 999999;
volatile unsigned int		ovf_counter = 0;
volatile unsigned long		pomiar_czasu;
volatile unsigned char		pomiar_gotowy;
volatile unsigned long		czas_obrotu_kola_w_impulsach_timer1;
volatile unsigned long		zadana_predkosc_w_impulsach = 36000;	//by³o 4500
//volatile signed int			szerokosc_impulsu = 1023;
volatile signed long			uchyb_poprzedni = 0;

//-----------------------------------------------------------------------------------------------

ISR (TIMER1_OVF_vect)
{
	ovf_counter ++;
	PORTD ^= (1<<PD6);
	
	//pomocnicze
	licznik_OVF_dla_czasu ++;
	licznik_OVF_dla_usartu ++;
}

//-----------------------------------------------------------------------------------------------

ISR (TIMER1_CAPT_vect)
{
	static unsigned int czas_poczatkowy = 0;

	//paczka danych - czêœæ (bo w poni¿ej u¿ywam dane.czas_koncowy buforuj¹c ICR1
	dane.czas_koncowy		= ICR1;
	
	czas_obrotu_kola_w_impulsach_timer1 = (ovf_counter * 1024UL + dane.czas_koncowy) - czas_poczatkowy;	//DD  1024 a nie 65536, bo timer1 ustawiony na PWM 10 bit
		
	uchyb = czas_obrotu_kola_w_impulsach_timer1 - zadana_predkosc_w_impulsach;  //aktualna minus zadana poniewa¿ czas obrotu roœnie wraz ze spadkiem prêdkoœci, 
																				//co oznacza, ¿e powinniœmy zwiêkszaæ OCR1
																				//a to oznacza, ¿e uchyb powinien byæ dodatni
		
	suma_uchybow	=  suma_uchybow + uchyb;
		
	pid = (kp_x100 * uchyb	+	ki_x100 * suma_uchybow	+	kd_x100 * (uchyb - uchyb_poprzedni)) / 50000; // dzielimyu przez 100 bo wspó³czynniki s¹ pomno¿one przez 100
		
	uchyb_poprzedni = uchyb;
	
	//pid = 600;
		
	//zabezpieczenie zakresu OCR1A
	if (pid < 0)		pid = 0;
	if (pid > 1023)		pid = 1023;
		
	OCR1A = pid;			//szerokosc impulsu dla predkosci silnika

	//paczka danych	- pozosta³e
	dane.czas_poczatkowy	= czas_poczatkowy;
	dane.ovf_counter		= ovf_counter;

	//przygotuj do nastêpnego pomiaru
	ovf_counter = 0;
	czas_poczatkowy = dane.czas_koncowy;
	
	
	
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

	ultoa (czas_obrotu_kola_w_impulsach_timer1, usart_bufor, 10);
	usart_send();
	
	utoa (OCR1A, usart_bufor, 10);
	usart_send();
	
	itoa (suma_uchybow, usart_bufor, 10);
	usart_send();
	
	itoa (uchyb - uchyb_poprzedni, usart_bufor, 10);
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

	
	//_delay_ms(3000);

	

	// USART	
	usart_inicjuj();

	//wlacz timer1
	TCCR1B	|= (1<<CS10);					//prescaler 1 - by³ wczeœnie 8
	
	//wystartuj silnik
	OCR1A = 1023;							//maksymalne PWM
	_delay_ms(2000);

	

	
	sei();									//wlacz przerwania globalne
	
	
	//pêtla g³ówna
    while (1) 
    {
		if(ovf_counter > 3000) OCR1A = 1023;
		
		//zapamietaj wynik
		cli();
		dane_temp = dane;
		sei();
		/*
		czas_obrotu_kola_w_impulsach_timer1 = (dane_temp.ovf_counter * 1024UL + dane_temp.czas_koncowy) - dane_temp.czas_poczatkowy;	//DD  1024 a nie 65536, bo timer1 ustawiony na PWM 10 bit
			
			
		//if(licznik_OVF_dla_czasu >= 5)
		//{
				
			uchyb = czas_obrotu_kola_w_impulsach_timer1 - zadana_predkosc_w_impulsach;
			
			suma_uchybow	=  suma_uchybow + uchyb;
			
			pid = (kp_x100 * uchyb	+	ki_x100 * suma_uchybow	+	kd_x100 * (uchyb - uchyb_poprzedni)) / 100; // dzielimyu przez 100 bo wspó³czynniki s¹ pomno¿one przez 100
			
			uchyb_poprzedni = uchyb;
			
			//zabezpieczenie zakresu OCR1A
			if (pid < 0)		pid = 0;
			if (pid > 1023)		pid = 1023;
			
			OCR1A = pid;			//szerokosc impulsu dla predkosci silnika
			
			licznik_OVF_dla_czasu = 0;
		
		//}
		
		
		*/
			
		#define USART_WYKRES_CZAS_OBROTU		1
		//#define USART_WYKRES_OCR1A	1
		//#define USART_PELNE			1
		
		#ifdef USART_WYKRES_CZAS_OBROTU
			//Wysy³aj dane co konkretny zadany czas
			if((licznik_OVF_dla_usartu >= 800) && (UCSRA & (1<<UDRE)))
			{
				UDR = czas_obrotu_kola_w_impulsach_timer1 / 1000;	// dzielimy przez 10 by dostosowaæ do aplikacji rysuj¹cej wykres 0-255
				licznik_OVF_dla_usartu = 0;
				//if(zadana_predkosc_w_impulsach < 65000) zadana_predkosc_w_impulsach += 5;
			}
		#endif
		
		#ifdef USART_WYKRES_OCR1A
			//Wysy³aj dane co konkretny zadany czas
			if((licznik_OVF_dla_usartu >= 800) && (UCSRA & (1<<UDRE)))
			{
				cli();
				UDR = (unsigned char) (OCR1A >> 2);	// dzielimy przez 4 by dostosowaæ do aplikacji rysuj¹cej wykres 0-255
				sei();
				licznik_OVF_dla_usartu = 0;
			}
		#endif
		
		#ifdef USART_PELNE		
			//Wysy³aj dane co konkretny zadany czas
			if((licznik_OVF_dla_usartu >= 8000) && (UCSRA & (1<<UDRE)))
			{
				wyslij_dane_szczegolowe();
				licznik_OVF_dla_usartu = 0;
			}
		#endif
		
	}
}



