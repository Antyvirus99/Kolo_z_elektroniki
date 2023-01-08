/*
 * SST25PF040B.c
 *
 * Created: 20.04.2017 19:06:54
 *  Author: Donat
 */ 
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>


#include "SST25PF040B.h"
#include "SSD1306.h"

char napis[10];

inline void SST25_CE_low (void)
{
	PORTD &=~ (1<<SST25_PIN_CE);
}

inline void SST25_CE_high (void)
{
	PORTD |= (1<<SST25_PIN_CE);
}

void SST25_ustaw_piny (void)
{
	DDRD |= (1<<SST25_PIN_WP);
	DDRD |= (1<<SST25_PIN_CE);
	DDRB |= (1<<SST25_PIN_HOLD);
	
	
	PORTD |= (1<<SST25_PIN_CE);
	PORTD |= (1<<SST25_PIN_WP);
	PORTB |= (1<<SST25_PIN_HOLD);
}

void SST25PF040B_read_status (void)
{
	
	SST25_CE_low();
	SPDR = 0x05;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 0;						//wyslij cokolwiek, aby odebrac status
	while(!(SPSR & (1<<SPIF)));
	SST25_CE_high();
	
	DDRC |= (1<<PC5);
	PORTC |= (1<<PC5);
	
	//wyslij na wyswietlacz
	
	itoa(SPDR,napis,10);
	tekst_z_sram(napis,0,0);
	
}

void SST25PF040B_read_jedec (void)
{
	//przed uzyciem trzeba inicjowac tryb master
	//usunac zmiane trybu wysylania
	
	SST25_CE_low();
	
	
	SPDR = 0x9F;		//read
	while(!(SPSR & (1<<SPIF)));
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	char dana1 = SPDR;
	SPDR = 0;						//wyslij cokolwiek, aby odebrac status
	while(!(SPSR & (1<<SPIF)));
	char dana2 = SPDR;
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	char dana3 = SPDR;
	
	
	SST25_CE_high();
	_delay_ms(100);
	
	//wyslij na wyswietlacz
	
	itoa(dana1,napis,10);
	tekst_z_sram(napis,0,0);
	itoa(dana2,napis,10);
	tekst_z_sram(napis,8,0);
	itoa(dana3,napis,10);
	tekst_z_sram(napis,16,0);	
	
}

void read_id (void)
{
	
	
	SST25_CE_low();
	SPDR = 0x90;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 00;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	char dana1 = SPDR;
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	char dana2 = SPDR;
	
	SST25_CE_high();
	
	//wyslij na wyswietlacz
	
	itoa(dana1,napis,10);
	tekst_z_sram(napis,0,0);
	itoa(dana2,napis,10);
	tekst_z_sram(napis,8,0);
}
void read_status_register (void)
{
	
	SST25_CE_low();
	
	SPDR = 0x05;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	char dana1 = SPDR;
	
	SST25_CE_high();
	
	//wyslij na wyswietlacz
	
	itoa(dana1,napis,10);
	tekst_z_sram(napis,0,0);
	
	
}