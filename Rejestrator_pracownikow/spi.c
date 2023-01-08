#include <avr/io.h>
#include <util/delay.h>
#include "spi.h"
#include "integer.h"




void port_init(void)
{
	// Uwaga! przy zmianach dokonac poprawek w funkcjach: SD_power_OFF() i byc moze SD_power_ON()
	// w razie zmiany poprawic dane w mmcc
	DDRB |= SD_MOSI | SD_SCLK;
	DDRD |= SD_CS;
	DDRB &= ~SD_MISO;
	PORTB |= SD_MOSI | SD_SCLK ;
	PORTD |= SD_CS;
}



/*****************************************************************************************************/

// UWAGA!!!  dalej nic nie zmieniać!

void init_spi(void)
{
SPCR = 0x50; //setup SPI: Master mode, MSB first, SCK phase low, SCK idle low
SPSR = 0x01;
}

BYTE rcv_spi(void)
{
BYTE data;

SPDR = 0xff;
while(!(SPSR & (1<<SPIF)));
data = SPDR;
return data;
}

unsigned char xmit_spi (BYTE data)
{
// Start transmission
SPDR = data;

// Wait for transmission complete
while(!(SPSR & (1<<SPIF)));
data = SPDR;

return(data);
}



void dly_100us (void)
{

_delay_us(100);
}
////////////////////do gory od sd//////////na dole mfrc///////////
uint8_t spi_transmit(uint8_t data)
{
	SPDR = data;
	while(!(SPSR & (1<<SPIF)));
	
	return SPDR;
}

