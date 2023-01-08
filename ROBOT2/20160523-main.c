/*
 * Grzesiek_bluetooth.c
 *
 * Created: 16.01.2016 10:49:27
 * Author : Donat
 
 -p  atmega48 -B8  -c usbasp -P usb  -U flash:w:$(ProjectDir)Debug\$(ItemFileName).hex:i
 
 */ 
// email - grzegorz.jur99@gmail.com
//nick - antyvirus34
//haslo CBA - grzegorz_jur
//nazwa konta : antyvirus34.cba.pl
//konto ftp: admin@antyvirus34.cba.pl
//haslo FTP: grzegorz_jur
//nazwa bazy danych: antyvirus34_cba_pl
//nazwa uzytkownika: antyvirus34
//nazwa hosta: mysql.cba.pl
//haslo bazy danych: grzegorz_jur

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>

#include "jg_usart.h"
#include "dane.h"

#include "silniki.h"

#define PWM_START 10
#define PWM_LEWO_PRAWO 10
#define SUWAK_SRODEK 25
unsigned char bajt_odebrany;
volatile unsigned long timer1_licznik_przepelnien = 0;
volatile unsigned char wyslij_ramke = 0;

volatile unsigned long wynik;
unsigned int nastawa[2] = {0,0};
volatile unsigned char rozkaz = 0;




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
	
	//rozkaz = UDR0;
	if (index == 0)
	{
		nastawa[1] = UDR0;
		index ++;
	}
	else
	{
		nastawa[2] = UDR0;
		index = 0;
	}
	/*nastawa[index] = UDR0;
	index ++;
	if (index >= 2) index = 0;
	*/
	PORTD ^= (1<<PD7);
	
}


ISR (INT1_vect) //przerwanie na zboczu
{
	PORTD ^= (1<<PD6);	

}
/*
ISR (INT1_vect) //przerwanie na zboczu
{
	static unsigned char licznik_przerwan = 0;
	static unsigned long wynik;
	
	licznik_przerwan ++;
	if (licznik_przerwan == 1 )
	{
		TCCR1B |= (1<<CS10); //wystartuj timer 1 preskaler 1
		PORTB |= (1<<PB0);
	}
	else
	{
		TCCR1B &=~ (1<<CS10); //zatrzymaj timer
		licznik_przerwan = 0; // nastepne przerwanie rozpocznij pomiar
		PORTB &=~ (1<<PB0);
		
		if (wyslij_ramke ==0)
		{
			
		
			wynik = timer1_licznik_przepelnien * 65536 + TCNT1;
		
			ramka_nadawcza[0] =  (unsigned char) wynik; //8 najmlodszych bitow
			ramka_nadawcza[1] = (unsigned char) (wynik>>8); //bity od 8-15
			ramka_nadawcza[2] = (unsigned char) (wynik>>16); //8 najstarszych bitow
		}
		
		//przygotuj timer do ponownego pomiaru
		SFIOR |= (1<<PSR10);// zresetuj preskaler
		TCNT1 =0;// zeruj wartos timera
		timer1_licznik_przepelnien =0;
		
		
		
	}

}
*/

ISR (TIMER1_OVF_vect) 
{
	
	//while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
	//UDR0 = nastawa[1];
	/*while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
	UDR0 = nastawa[2];
	while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
	UDR0 = SUWAK_SRODEK;
	while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
	UDR0 = PRAWY_WSTECZ_OCR;
	*/
	
	timer1_licznik_przepelnien ++;
}



ISR (TIMER1_CAPT_vect) //funkcja przerwania dla zbocza narastajacego ICP1
{
	
	/*	
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
	**/
	
	/*
	icr1_aktualny = TCNT1;
	wartosc_aktualna = timer1_licznik_przepelnien * 256 + (icr1_aktualny > icr1_poprzedni)? icr1_aktualny - icr1_poprzedni : icr1_poprzedni - icr1_aktualny;
	if (wartosc_aktualna > wartosc_poprzednia) wynik = wartosc_aktualna - wartosc_poprzednia;
	else wynik =  wartosc_poprzednia - wartosc_aktualna;
	wartosc_poprzednia = wartosc_aktualna;
	timer1_licznik_przepelnien = 0;
	icr1_poprzedni = icr1_aktualny;	
	*/
}


int main(void)
{
	silniki_inicjuj();
	usart_inicjuj();
	_delay_ms(2000);
	
	//w³acz zasilanie HC05
	DDRD |= (1<<PD4);
	PORTD &=~ (1<<PD4);

	
	
	
	//Program zadeklarowany jest dla ATmega8, a pracujesz na ATmega48  :-)
	//Po zmianie zapewne bedziesz musia³ poprawiæ funkcje USART itp bo rejestry s¹ inne.
	
	
	DDRD	|= (1<<PD7);//LED
	
	sei();
	
	
	DDRC |= (1<< PC1);
	PORTC &=~ (1<<PC1);
	

	
	
	
	
	

	

	
	
	
	
	
	
	
	
	
 
	while(1)
	{
		switch (nastawa[1])//przod-tyl
		{
			case (SUWAK_SRODEK + 5) ... 100 :	//naprzod
				switch (nastawa[2])
				{
					case (SUWAK_SRODEK + 5) ... 100:	//prawo			
			
						LEWY_WSTECZ_OCR	 = 0;
						PRAWY_WSTECZ_OCR = 0;
						
						PRAWY_NAPRZOD_OCR = nastawa[1] - SUWAK_SRODEK + (nastawa[2] - SUWAK_SRODEK);
						LEWY_NAPRZOD_OCR = nastawa[1] - SUWAK_SRODEK - (nastawa[2] - SUWAK_SRODEK);
						silnik1_naprzod();
						silnik2_naprzod();
						break;
					case 0 ... (SUWAK_SRODEK - 5):	//lewo	
						
						LEWY_WSTECZ_OCR	 = 0;
						PRAWY_WSTECZ_OCR = 0;	
										
						PRAWY_NAPRZOD_OCR = nastawa[1] - SUWAK_SRODEK - (SUWAK_SRODEK - nastawa[2]);
						LEWY_NAPRZOD_OCR = nastawa[1] - SUWAK_SRODEK + (SUWAK_SRODEK - nastawa[2]);
						silnik1_naprzod();
						silnik2_naprzod();
						break;
					default:
						
						LEWY_WSTECZ_OCR	 = 0;
						PRAWY_WSTECZ_OCR = 0;
						
						PRAWY_NAPRZOD_OCR = nastawa[1] - SUWAK_SRODEK;
						LEWY_NAPRZOD_OCR = nastawa[1] - SUWAK_SRODEK;
						silnik1_naprzod();
						silnik2_naprzod();
						break;
	
				}
				
				
			case 0 ... (SUWAK_SRODEK - 5) :	//wstecz
				switch (nastawa[2])
				{
					case (SUWAK_SRODEK + 5) ... 100:	//prawo
					
						LEWY_NAPRZOD_OCR = 0;
						PRAWY_NAPRZOD_OCR = 0;
					
						PRAWY_WSTECZ_OCR = SUWAK_SRODEK - nastawa[1] + (nastawa[2] - SUWAK_SRODEK);
						LEWY_WSTECZ_OCR	 = SUWAK_SRODEK - nastawa[1] - (nastawa[2] - SUWAK_SRODEK);
						silnik1_wstecz();
						silnik2_wstecz();
						break;
					case 0 ... (SUWAK_SRODEK - 5):	//lewo
					
						LEWY_NAPRZOD_OCR = 0;
						PRAWY_NAPRZOD_OCR = 0;
					
						PRAWY_WSTECZ_OCR = SUWAK_SRODEK - nastawa[1] - (SUWAK_SRODEK - nastawa[2]);
						LEWY_WSTECZ_OCR	 = SUWAK_SRODEK - nastawa[1] + (SUWAK_SRODEK - nastawa[2]);
						silnik1_wstecz();
						silnik2_wstecz();
						break;
					default:
					
						LEWY_NAPRZOD_OCR = 0;
						PRAWY_NAPRZOD_OCR = 0;
						
						PRAWY_WSTECZ_OCR = SUWAK_SRODEK - nastawa[1];
						LEWY_WSTECZ_OCR	 = SUWAK_SRODEK - nastawa[1];
						silnik1_wstecz();
						silnik2_wstecz();
						break;
						
				}
			default:
				switch (nastawa[2])
				{
					case (SUWAK_SRODEK + 5) ... 100:	//prawo
					
						LEWY_NAPRZOD_OCR = 0;
						PRAWY_WSTECZ_OCR = 0;
						
						PRAWY_NAPRZOD_OCR = nastawa[2] - SUWAK_SRODEK;
						LEWY_WSTECZ_OCR	 = nastawa[2] - SUWAK_SRODEK;
						silnik1_naprzod();
						silnik2_wstecz();
						break;
					case 0 ... (SUWAK_SRODEK - 5):	//lewo
					
						LEWY_WSTECZ_OCR	 = 0;
						PRAWY_NAPRZOD_OCR = 0;
					
						PRAWY_WSTECZ_OCR = SUWAK_SRODEK - nastawa[2];
						LEWY_NAPRZOD_OCR = SUWAK_SRODEK - nastawa[2];
						silnik1_wstecz();
						silnik2_naprzod();
						break;
					default:
						PRAWY_NAPRZOD_OCR = 0;
						LEWY_WSTECZ_OCR	 = 0;
						LEWY_NAPRZOD_OCR = 0;
						PRAWY_WSTECZ_OCR = 0;
						//silnik1_naprzod();
						//silnik2_naprzod();
					
				}
		}
		
		TCCR1B |= (1<<CS12) | (1<<CS10); //preskaler timer 1
		
		
		_delay_ms(100);
		
		UDR0	= PRAWY_NAPRZOD_OCR;		
		while (!(UCSR0A & (1<<UDRE0)));//zaczekaj na wolny bufor nadawania
		UDR0	= LEWY_WSTECZ_OCR	;	
		
		
		
		/*
		if (nastawa[1]>60) //naprzod
		{			
			
			PRAWY_NAPRZOD_OCR = nastawa[1] - 50 + (abs (nastawa[2] - 50) > 10)?  abs (nastawa[2] - 50) : 0 ; 
			LEWY_NAPRZOD_OCR = nastawa[1] - 50 + (abs (nastawa[2] - 50) > 10)?  abs (nastawa[2] - 50) : 0 ; 
		}
		else if (nastawa[1]<40)//wstecz
		{
			PRAWY_WSTECZ_OCR = 50 - nastawa[1] + (abs (nastawa[2] - 50) > 10)?  abs (nastawa[2] - 50) : 0 ; 
			LEWY_WSTECZ_OCR	 = 50 - nastawa[1] + (abs (nastawa[2] - 50) > 10)?  abs (nastawa[2] - 50) : 0 ; 
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
	
	
	
	_delay_ms (5000);
			
	//jg_usart_inicjuj_atmega8_hc05();
	usart_inicjuj();

	
	TIMSK0 |= (1<<TOIE0);//timer 0 przepelnienie przerwanie wlacz	dla wysylania danych do aplikacji 

	DDRD |= (1<<PD7); // dioda led
	
	
	//ramka_nadawcza [1] = 64;
	
	
	
	PORTB |= (1<<PB0); //w³¹czenie pull up dla czujnika
	
	
	
	//rozpoczêliœmy pomiar prêdkoœci od przypadku pomiaru czasu timerem i przerwaniem na pinie int1
	
	EICRA |= (1<<ISC11) | (1<<ISC10); // Przerwanie dla zbocza narastajacego dla pin INT1
	EIMSK |= (1<<INT1); //wlacz przerwanie
	
	TIMSK1 |= (1<<TOIE1); //wlacz przerwwania przepelnienia timera 1
	
	TCCR1A |= (1<<WGM10);//ustawianie trybu PWM 8bit
	TCCR1B |= (1<<WGM12);//ustawianie PWM 8bit
	TCCR1B |= (1<<CS11);//preskaler 8 dla timer1
	TCCR1A |= (1<<COM1A1); // impuls dodatni na wyjsci PWM
	OCR1A = 150;
	DDRB |= (1<<PB1); //OC1A jako wyjscie
	
	TCCR1B |= (1<<ICES1); // zbocze narastajace na pinie ICP1
	TCCR1B |= (1<<ICNC1);	//eliminacja szumów na ICP1
	TIMSK1  |= (1<<ICIE1); //wlacz przerwania Capture uniut dla ICP1
	
	
	

	sei (); //wlacz przerwania globalne
	
	
	
	
    while (1) 
    {	
		//SUWAK_SRODEK
		if (ramka_odbiorcza [2]>60)	PORTB |= (1<<PB1);
		else PORTB &=~ (1<<PB1);
		
		
		if (wyslij_ramke)
		{
			
			cli ();
			ramka_nadawcza[0] =  (unsigned char) wynik; //8 najmlodszych bitow
			ramka_nadawcza[1] = (unsigned char) (wynik>>8); //bity od 8-15
			ramka_nadawcza[2] = (unsigned char) (wynik>>16); //8 najstarszych bitow
			sei();
			
			
			while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
			UDR0 = ramka_nadawcza [0];
			while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
			UDR0 = ramka_nadawcza [1];
			while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
			UDR0 = ramka_nadawcza [2];
			while (!(UCSR0A & (1<<UDRE0))); //zaczekaj jesli cos jest nadawane
			UDR0 = 0;	
			wyslij_ramke = 0;	
			PORTD ^= (1<<PD7);	
		}
		//checkbox 2
		/*if (ramka_odbiorcza[3] & (1<<6)) PORTB |= (1<<PB0);
		else							 PORTB &=~ (1<<PB0);
		*/
		
		/*
		if(PIND & (1<<PD3))
		{
			
		}
		else
		{
			
		}
		*/
		
		
		
		
		
		
		
		
		
		
    }
}

