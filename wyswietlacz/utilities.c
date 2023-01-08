#include "utilities.h"
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

void set_master (void)
{
	DDRB |= (1<<PIN_DC);
	DDRB |= (1<<PIN_CS);	//ustaw pin SS na wyjscie
	PORTB |= (1<<PIN_CS);	//ustaw stan wysoki SS
	DDRB |= (1<<PIN_MOSI);	//ustaw pin MOSI
	DDRB |= (1<<PIN_SCK);	//ustaw pin SCK
	DDRB &=~ (1<<PIN_MISO);	//ustaw miso
	SPCR |= (1<<MSTR);	//ustaw tryb MASTER
	SPCR |= (1<<CPHA) | (1<<CPOL);	//ustaw przesyl  domyslnie 1 zbocze narastajace
	//wysyl MSB nie trzeba ustawiac
	//Preskaler 4 nie trzeba ustawaic
	//SPCR |= (1<<SPR1) | (1<<SPR0);	//preskaler 128
	SPSR |= (1<<SPI2X);	//2 razy szybciej
	SPCR |= (1<<SPE);	//wlacz SPI
	SPSR;
	SPDR;
}
