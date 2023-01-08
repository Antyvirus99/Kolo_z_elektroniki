/*
 * wyswietlacz.c
 *
 * Created: 21.03.2017 18:52:11
 * Author : Donat
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "stdlib.h"

#include "czcionki.h"
#include "czcionka+8pts.h"
#include "SSD1306.h"
#include "SST25PF040B.h"
#include "dzwieki.h"
#include "dzwiek_tablica.h"
#include "W25Q32BV.h"
#include "usart.h"
#include "MFRC522.h"
#include "MFRC522_CMD.h"
#include "MFRC522_REG.h"

const __flash char grzegorz[] = "grzegorz";
volatile unsigned char dzwiek_play = 127;


//zapisany dzwiek hello od 0x0 do 0x000035D7 (13783 lub do 0x00004B7C (19324)

volatile unsigned long W25Q32BV_adres = 0x0;
volatile unsigned long dlugosc_dzwieku =  0x00004B7C;

////////////////////////////////////////////////////////

void Wlacz_glosnik (void)
{
	TCCR1B |= ((1<<CS10));
}

void wylacz_glosnik (void)
{
	TCCR1B &=~ ((1<<CS10));
}



ISR (USART_RXC_vect)
{

	char dana = UDR;
	UDR = dana;
	W25Q32BV_page_program(W25Q32BV_adres, dana);
	W25Q32BV_adres ++;
	//nie trzeba czekac na zapis - program zdazy zapisac
	PORTD ^= (1<<PD7);
}



ISR (TIMER1_OVF_vect)
{
	
	/*
	static unsigned int index_dzwiek = 0;
	static unsigned int licznik = 0;
	licznik ++;
	if (licznik > 5)
	{
		licznik = 0;
		OCR1A = dzwiek[index_dzwiek];
		index_dzwiek ++;
		if (index_dzwiek > 2421)
		{
			index_dzwiek = 0;
			TIMSK &=~ (1<<TOIE1);						//wylacz przepelnienia
			TCCR1B &=~ (1<<CS10);						//zeruj preskaler 1
		}
	}	
	*/
	
	
	PORTD |= (1<<PD2);
	static unsigned int licznik = 0;
	
	
	
	if (licznik > 3)		//pozwala ustawic preskaler 4. Dzwiek jest odtwarzany co 4 przepelnienie timera
	{
		PORTD |= (1<<PD7);
		
		
		licznik = 0;	
		
		W25Q32BV_read_data(W25Q32BV_adres,1);
		W25Q32BV_adres++;

		
		OCR1A = dzwiek_play;
		/*
		while (!(UCSRA & (1<<UDRE)));
		UDR = dzwiek_play;
		*/
		
		
		if (W25Q32BV_adres > dlugosc_dzwieku)
		{
			//zatrzymaj timer po wygenerowaniu dzwieku
			TIMSK &=~ (1<<TOIE1);						//wylacz przepelnienia
			//TCCR1B &=~ ((1<<CS11));						//zeruj preskaler 8, wylacz odtwarzanie dzwieku
			wylacz_glosnik();
			W25Q32BV_adres = 0;
		}
		PORTD &=~ (1<<PD7);
	}

	
	licznik ++;
PORTD &=~ (1<<PD2);
	
	
}




void EEPROM_write (unsigned int adres, unsigned char dana)
{
	while (EECR & (1<<EEWE));
	
	EEAR = adres;
	EEDR = dana;
	EECR |= (1<<EEMWE);
	EECR |= (1<<EEWE);
}
char EEPROM_read (unsigned int adres)
{
	while (EECR & (1<<EEWE));
	
	EEAR = adres;
	EECR |= (1<<EERE);
	return EEDR;
}
unsigned long zmienna ;

inline void SST25_CE_high (void)
{
	PORTD |= (1<<SST25_PIN_CE);
}


int main(void)
{
	//dioda testowa
	DDRB |= (1<<PB0);	
	PORTB |= (1<<PB0);
	DDRD |= (1<<PD7);
	PORTD |= (1<<PD7);
	DDRD |= (1<<PD2);
	PORTD |= (1<<PD2);
	
	//jest kontakt z czytnikiem, mozna odczytywac i zapisywac rejestry
	
	
	

	
	ustaw_master();	
	inicjuj_usart();
	
	W25Q32BV_ustaw_piny();
	MFRC522_inicjuj();
	
	
	
	/*
	//reset wyswietlacza
	DDRD |= (1<<PD7);	
	PORTD &=~ (1<<PD7);
	_delay_ms(100);
	PORTD |= (1<<PD7);
	_delay_ms(1000);
	
	*/
	
	/*
	ssd1306_wyslij_2_bajty(CHARGE_PUMP_SETTING, 0x14);
	ssd1306_wyslij_1_bajt(SET_DISPLAY_ON);
	ssd1306_wyslij_2_bajty(SET_MEMORY_ADDRESSING_MODE, HORIZONTAL_ADDRESSING_MODE);	//horizontal addressing mode

	wyczysc_ekran();
*/
	



/////////////////////wartosci testowe/////////////////////////
	
	while (!(UCSRA & (1<<UDRE)));
	UDR = dzwiek_play;
  
	sei();
	UCSRB &=~ (1<<RXEN);




///////////////////////////////////////////////////
/*
	//tryb fast pwm 8 bit
	
	TCCR1A |= (1<<COM1A1);					//non invertig mode
	DDRB |= (1<<PB1);						//ustaw wyjscie dla buzzera
	TCCR1A |= (1<<WGM10);
	TCCR1B |= (1<<WGM12);
	TIMSK |= (1<<TOIE1);					//uruchom przepelnienia
	Wlacz_glosnik();
	

	
	_delay_ms(1000);
	
	MFRC522_timer();
	MFRC522_interrupt();
	
	
	*/
////////////mfrc522/////////////////////
	
	
	_delay_ms(1000);
	while(!(UCSRA & (1<<UDRE)));
	UDR = MFRC522_Read_data(ComIEnReg);
	while(!(UCSRA & (1<<UDRE)));
	UDR = MFRC522_Read_data(DivIEnReg);
	
	
	
	
///////////////////////////////////////////
    while (1)
    {
		
		
		
		
		
		
		
		///////////////////////////////////////////////// odczytaj wersje mfrc522
		/*
		_delay_ms(1000);
		while(!(UCSRA & (1<<UDRE)));
		UDR = MFRC522_Read_data(0x37);
		*/
		
		/////////////////////////////////////////////////
		/*
		_delay_ms(5000);
		ssd1306_wyslij_3_bajty(SET_COLUMN_ADDRESS, 0, 127);		//ustaw kolumny
		
		ssd1306_wyslij_3_bajty(SET_PAGE_ADDRESS, 0, 7);			//ustaw adresy
		miedzy delay i while wysyla cos na wyswietlacz
		*/
		
		
		/*
		DC_high();
		CS_low();
		for (int a=1; a<1024; a++)
		{
			
			 = a;
			while(!(SPSR & (1<<SPIF)));
			_delay_ms(500);
		}
		CS_high();	
		*/

/*
		W25Q32BV_read_jedec();
		//read_id();
		_delay_ms(2);
		W25Q32BV_read_status_register_both();
		_delay_us(100);
*/	/*
	
		while(1)
{
		while (!(UCSRA & (1<<UDRE)));
		UDR = 143;
}
*/
/*
		for (long z=0; z < 20000000; z ++)
		{
			//W25Q32BV_read_data((pusty_adres.bajt)+z*32768,10);
			
			zmienna = (pusty_adres.adres)+z;
			dd_W25Q32BV_read_data(& zmienna,10);
			_delay_ms(10);

		}
		_delay_ms(50);
*/

		
		//while(1);
		
/*
		while(1);
		
		EEPROM_write(45,'r');
		EEPROM_write(105,'p');
		napis[0] = EEPROM_read(45);
		napis[5] = EEPROM_read(105);
		tekst_z_sram(&napis[0],20,50);
		PORTB ^= (1<<PB0);
		_delay_ms(1000);
		tekst_duzy_z_sram(napis,3,10);
		PORTB ^= (1<<PB0);
		_delay_ms(1000);

		while(1);
		
*/	



////////////		wlaczanie buzzera		//////////////////////
	
	
	//TCCR1B |= (1<<CS10);
	TIMSK |= (1<<TOIE1);			//wlacz przepelnienia
	Wlacz_glosnik();
	_delay_ms(5000);
	

	
    }
}

