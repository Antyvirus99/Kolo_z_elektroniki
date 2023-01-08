/*
 * MFRC522_funkcje.c
 *
 * Created: 10.03.2018 08:11:13
 *  Author: Donat
 */ 
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "mfrc522.h"
#include "atm8a_usart.h"

char odczytanie_karty (char* serial)
{
	
	uint8_t str[MAX_LEN];
	uint8_t byte = mfrc522_request(PICC_REQALL,str);
	/*
	sprintf(send_buffer, "%d \r\n", byte);
	usart_send_buffer();
	*/
	if(byte == CARD_FOUND)
	{
		PORTD ^= (1<<PD7);
		byte = mfrc522_get_card_serial(str);
		if(byte == CARD_FOUND)
		{
			sprintf(serial ,"%.5s",str);
			serial[6]=0;
			return 1;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return 0;
	}
}