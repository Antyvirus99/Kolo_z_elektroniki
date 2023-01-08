/* DD dorobilem spi.h bo nie otrzymalem od Drzaska */

#include <avr/io.h>
#include "integer.h"


// dd_ tutaj definicje pinów mikrokontrolera vs karta SD
#define SD_MISO (1<<PB4)
#define SD_MOSI (1<<PB3)
#define SD_SCLK (1<<PB5)
#define SD_CS 	(1<<PD4)

void port_init(void);		
void init_spi(void);
BYTE rcv_spi(void);
unsigned char xmit_spi (BYTE data);

/*
 * Config SPI pin diagram
 */
#define SPI_DDR		DDRB
#define SPI_PORT	PORTB
#define SPI_PIN		PINB
#define SPI_MOSI	PB3
#define SPI_MISO	PB4
#define CARD_SPI_SS		PD6
#define SPI_SCK		PB5

#define ENABLE_CHIP() (PORTD &= (~(1<<CARD_SPI_SS)))
#define DISABLE_CHIP() (PORTD |= (1<<CARD_SPI_SS))

void spi_init();
uint8_t spi_transmit(uint8_t data);

void dly_100us (void);
