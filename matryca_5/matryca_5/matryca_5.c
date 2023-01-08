/*
 * matryca_led.c
 *
 * Created: 15.12.2016 18:09:13
 * Author : jur
 */ 

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h>  

unsigned char wyswietlacz[8];
unsigned char snake_x[64];
unsigned char snake_y[64];
//wspolrzedne snake
volatile unsigned char x=3;
volatile unsigned char y=0b00001000;
//wspolrzedne jedzenia
volatile unsigned char a;
volatile unsigned char b;
volatile unsigned char posilek = 0;
volatile unsigned char z = 0;
volatile unsigned char modul_snake;

unsigned int index_glowa = 0;
unsigned int index_ogon = 0;
unsigned char kierunek = 3; //kierunek ruchu weza 1 - gora 2 - lewo, 3 - dol 4- prawo
unsigned int poziom = 10;
unsigned int difficult = 1000; //obecna trudnosc 1 - hard, 10 - easy;
volatile unsigned char przerwa = 0;

int dd_rnd(int min, int max) {
	
	//return rakiety_wsp_y[RAKIETY_ILOSC]>>6;	//dwa najmniej znacz¹ce bity generatora liczb losowych
	
	//poprzednia funkcja
	if (max>=min) {
		max -= min;
		} else {
		int tmp = min - max;
		min = max;
		max = tmp;
	}
	return max ? rand() % max + min : min;
}


void send (unsigned char address, unsigned char data)
{
		//wyslij rozkaz do wyswietlacza
		PORTB &=~ (1<<PB2); //ustaw bit na 0 zeby wyslac
		SPDR = address;
		while(!(SPSR & (1<<SPIF)));// czekaj na zakonczenie transmisji
		SPDR = data;
		while(!(SPSR & (1<<SPIF)));// czekaj na zakonczenie transmisji
		PORTB |= (1<<PB2); //ustaw bit na 1 zeby przerwac wysylanie

}



ISR (TIMER0_OVF_vect)
{
	static unsigned char licznik = 0;
	licznik ++;
	if (licznik > 1)
	{
		licznik = 0;
		wyswietlacz[a] ^= b;
	}
	
	for (unsigned int a=1; a<9; a++) send(a, wyswietlacz[a-1]);
	
	przerwa ++;
}

void koniec_gry (void)
{
	
	while (PINB & (1<<PB7));
	PORTB |= (1<<PB1);
	index_glowa = 0;
	index_ogon = 0;								
	x = 3;
	y = 0b00001000;
	for (unsigned char a=0; a<sizeof (snake_x); a++)
	{
		snake_x[a] = 0;
		snake_y[a] = 0;
	}
	//Pokaz tylko jeden posilek

	a = dd_rnd(0,7);
	do
	{
		b = 1<<dd_rnd(0,7);
	} while (b==y);
	posilek = 1;
	wyswietlacz[a] |= b;

}



void czysc (void)
{
	for (unsigned char a=0; a<8; a++) wyswietlacz[a] = 0;
}

void wybuch (void)
{
	for (unsigned char b=0; b<6; b++)
	{
		for (unsigned char a=0; a<8; a++) wyswietlacz[a] = ~wyswietlacz[a];
		_delay_ms(100);
	}
	czysc();
}



void rusz_weza(void)
{
	//Poruszanie sie snake
	wyswietlacz[snake_x[index_ogon]] &=~ snake_y[index_ogon];//zgas ogon		
	wyswietlacz[x] |= y;//zapal glowe
	index_glowa ++;
	index_ogon ++;
	if (index_glowa >= sizeof (snake_x)) index_glowa = 0;
	if (index_ogon >= sizeof (snake_x)) index_ogon = 0;	
	snake_x[index_glowa] = x;
	snake_y[index_glowa] = y;

}

void lewo (void)
{
	if ((!(PIND & (1<<PD5)) || kierunek == 2) && (kierunek != 4))
	{
		if (przerwa >= poziom)
		{
			if(x<7)		
			{
				x++;
				//wyswietlacz[x-1] &=~ y;	
				rusz_weza();				

				
			}
			else
			{
				x = 0;
				rusz_weza();
				
			}
			przerwa = 0;
		}
		kierunek = 2;
		
	}
}

void prawo (void)
{
	if (  (!(PIND & (1<<PD6)) || kierunek == 4)   &&   (kierunek != 2)   )
	{

		if (przerwa >= poziom)
		{
			if(x>0)	
			{
				x--;
				//wyswietlacz[x+1] &=~ y;
				
				rusz_weza();
				
				
			}
			else
			{
				x = 7;
				rusz_weza();
			}
			
			przerwa = 0;
		}
		kierunek = 4;
		
	}
}

void gora (void)
{
	if ((!(PIND & (1<<PD7)) || kierunek == 1) && (kierunek != 3))
	{
		if (przerwa >= poziom)
		{
			if(y<128)
			{
				y = y<<1;
				//wyswietlacz[x] &=~ (y>>1);
				rusz_weza();
				
			}
			else
			{
				y = 1;
				rusz_weza();
				
			}
			przerwa = 0;			
		}
		kierunek = 1;
	}	

}
void dol (void)
{
	if ((!(PINB & (1<<PB0)) || kierunek == 3) && (kierunek != 1))
	{
		if (przerwa >= poziom)
		{
			if(y>1)
			{
				y = y>>1;
				//wyswietlacz[x] &=~ (y<<1);
				rusz_weza();
				
			}
			else
			{
				y = 128;
				rusz_weza();
			}
			przerwa = 0;
		}
		kierunek = 3;
	}

}
unsigned char losuj_ponownie_jedzenie (void)
{
		
	if (index_glowa > index_ogon)
	{

		for(modul_snake = index_ogon; modul_snake<=index_glowa; modul_snake++)
		{
			if (b == snake_y[modul_snake]) return 1;
			
		}
	}
	else 	if (index_glowa < index_ogon)
	{
		
		for (modul_snake = 0; modul_snake<=index_glowa; modul_snake++)
		{
			if (b == snake_y[modul_snake]) return 1;
		}
		
		
		for (modul_snake = index_ogon; modul_snake< sizeof (snake_x); modul_snake++)
		{
			if (b == snake_y[modul_snake]) return 1;
		}
	}
	
	return 0;
}


unsigned char  kolizja_z_wezem (void)
{
	if (index_glowa > index_ogon)
	{
		
		modul_snake = index_ogon;
		for(modul_snake; modul_snake<index_glowa; modul_snake++) 
		{
			if ((snake_x[index_glowa] == snake_x[modul_snake]) && (snake_y[index_glowa] == snake_y[modul_snake]))
			{
				_delay_ms(1000);
				wybuch();
				return 1;
				
			}
		}
	}
	else 	if (index_glowa < index_ogon)
	{
		
		for (modul_snake = 0; modul_snake<index_glowa; modul_snake++)
		{
			if ((snake_x[index_glowa] == snake_x[modul_snake]) && (snake_y[index_glowa] == snake_y[modul_snake]))
			{
				_delay_ms(1000);
				wybuch();
				return 1;
				
			}
		}
		
		
		for (modul_snake = index_ogon; modul_snake< sizeof (snake_x); modul_snake++)
		{
			if ((snake_x[index_glowa] == snake_x[modul_snake]) && (snake_y[index_glowa] == snake_y[modul_snake]))
			{
				_delay_ms(1000);
				wybuch();
				return 1;
			}
		}
	}
	
	return 0;
}

int main(void)
{
	//testowe diody
	DDRB |= (1<<PB1);
	DDRD |= (1<<PD0);
	
	
	//wlacz pull up na strzalkach
	PORTD |= (1<<PD5);
	PORTD |= (1<<PD6);
	PORTD |= (1<<PD7);
	PORTB |= (1<<PB0);
	
	//wlacz pull up na start
	PORTB |= (1<<PB7);
	
	//ustaw SS na wyjscie i ustaw stan wysoki
    DDRB	|= (1<<PB2);
	PORTB	|= (1<<PB2);
	
	//wlacz wyjscie na MOSI i SCK
	DDRB	|= (1<<PB3) | (1<<PB5);
	
	//wlaczyc tryb master
	SPCR	|= (1<<MSTR);
	
	//tryb
	//nie trzeba ustawiac
	
	//wysylaj MSB
	//nie trzeba ustawiac
	
	//preskaler
	SPSR	|= (1<<SPI2X);
	
	//wlacz SPI
	SPCR	|= (1<<SPE);
	
	//-----------------------------------
	//wlacz timer 0
	
	
	//preskaler 1024
	TCCR0	|= (1<<CS02) | (1<<CS00);
	
	//wlacz przerwania timera 0
	TIMSK	|= (1<<TOIE0);
	
	
//dodac zmienne przechowujace poprzednie wspolrzedne weza by gasic jego poprzednie miejsce pobytu	
	
	//------------
	//ustaw wyswietlacz do pracy
	//normal operation
	send(0x0c,0x01);
	send(0x0f,0x00);
	
	//brak kodu BCD
	send(0x09,0x00);
	
	//ustaw jasnosc
	send(0x0a,0x00);
	
	//wyswietlaj wszystkie diody
	send(0x0B,0x07);
	
	czysc();
	
	wyswietlacz[0] = 0b00000000;
	wyswietlacz[1] = 0b00000000;
	wyswietlacz[2] = 0b00000000;
	wyswietlacz[3] = 0b00000000;
	wyswietlacz[4] = 0b00000000;
	wyswietlacz[5] = 0b00000000;
	wyswietlacz[6] = 0b00000000;
	wyswietlacz[7] = 0b00000000;
	
	//poczatkowe wartosci
	snake_x[index_glowa] = x;
	snake_y[index_glowa] = y;
	wyswietlacz[x] = y;
	
	sei();
    while (1) 
	{
	


		lewo();
		prawo();
		gora();
		dol();


		
			
		
			
		
		
		
			if (kolizja_z_wezem())
			{
				koniec_gry();

			}
			else
			{
			
				//zjedzenie
				if((x == a) & (y==b) )
				{
			
					index_ogon--;
			
			
					posilek = 0;
				}
		
		
		
		
		
				//Pokaz tylko jeden posilek
				if (posilek == 0)
				{
					a = dd_rnd(0,7);
					do
					{
						b = 1<<dd_rnd(0,7);
					} while (b==y);												
					posilek = 1;
					wyswietlacz[a] |= b;
				}
		
		
				/*
				if (x==a)
				{
					wyswietlacz[a] |= b;	
				
				}
				else if ((x != a) & (y != b))
				{
					wyswietlacz[a] &=~ b;
			
				}
				*/

		

			
			}
		}
}

