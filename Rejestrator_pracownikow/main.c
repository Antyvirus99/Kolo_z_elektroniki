#include <avr/io.h>
#include <inttypes.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "atm8a_usart.h"
#include "connection.h"
#include "W25Q32BV.h"
#include "sd_card.h"
#include "utilities.h"
#include "pff.h"
#include "connection.h"
#include "mfrc522.h"
#include "MFRC522_funkcje.h"

volatile char receive_buffer[128];
volatile char send_buffer[128];
char variables[64];
volatile uint16_t receive_index;

//?test=5 HTTP/1.1\r\nHost: jurpad.cba.pl\r\n\r\n
const __flash char INDEX_SCRIPT[] = "/skrypt.php";
const __flash char WEBSITE_URL[]	= "jurpad2.cba.pl";//"antyvirus.000webhostapp.com";
const __flash char TIME_VAR_NAME[] = "t";
const __flash char SUCCESS[] = "zapisano_dane";
const __flash char FAIL[] = "nie_zapisano_dane";


void send_pointers(W25Q32BV memory) 
{
	sprintf(send_buffer, "Send pointer: %ld, save pointer: %ld\n\r", memory.send, memory.save);
	_delay_ms(1000);
	usart_send_buffer();
}
//
//void read() {
//
//}
//
//void send() {
//
//}

//!!!!!!!!!!!!!!!!!!!!!!przed pierwszym zaprogramowaniem odkomementuj eeprom_write_dword w sd_init!!!!!!!!!!!!

int main(void)
{
				//TESTOWY FRAGMENT DO SPRAWDZENIA MIFARE CS
	/*
	DDRD |=(1<<PD7);
	PORTD |= (1<<PD7);
	DDRD |=(1<<PD6);
	while(1)
	{
		PORTD ^= (1<<PD6);		//cs od carty mifare
		
		
		
		PORTD ^= (1<<PD7);
		_delay_ms(100);
	}
	*/
	if (MCUSR & (1<<PORF))
	{
		MCUSR &=~ (1<<PORF);
		DDRD |= (1<<PD7);
		_delay_ms(100);
		PORTD &= ~(1<<PD7);
		_delay_ms(100);
		PORTD |= (1<<PD7);
		_delay_ms(2000);
		//PORTD &=~ (1<<PD7);
	}
	
	set_master();
	usart_init();
	//pin ss musi byc na wyjscie lub pull iup!!!!!
	_delay_ms(2000);
	DDRD |= (1<<PD4);
	PORTD |= (1<<PD4);
	
	//stan wysoki na ekranie
	DDRC |= (1<<PC0);
	PORTC |= (1<<PC0);
	
	sprintf(send_buffer, "1234!\r\n");
	usart_send_buffer();
	
	
	//test czy eeprom dziala
	
	/*int adres = 100;
	
	eeprom_write_byte((uint8_t *)adres,255);
	int test = eeprom_read_byte((uint8_t *)adres);
	sprintf(send_buffer, "%d %ld \r\n", test,adres2);
	usart_send_buffer();
	
	while(1);
	*/
	EICRA |= (1<<ISC10);
	EIMSK |= (1<<INT1);
	
	
	
	
	
	W25Q32BV memory;
	
	W25Q32BV_init(&memory, sizeof(unsigned long) + sizeof(unsigned long));	
	W25Q32BV_check_records(&memory);
	sd_init();
	
	mfrc522_init();
	
	//wykasuj_flasha_32kB(&memory);	
	
	_delay_ms(500);
	record data;
	data.datetime = 1511893278;
	data.id = 1;
	sprintf(send_buffer, "1234!\r\n");
	usart_send_buffer();
	
	_delay_ms(100);
	sd_open("p2.txt");
	for(int i = 0; i <10; i++)/* 
	{
		W25Q32BV_save_record(&memory, &data);
		
		sd_write(&data);
		usart_send_buffer(&data);
		data.id += 1;
		data.datetime += 327;
		_delay_ms(100);
		
	}*/
	
	sd_close();
	
	
	sprintf(send_buffer, "koniec!\r\n");
	usart_send_buffer();
	/*
	while(1)
	{
		DDRD |=(1<<PD7);
		PORTD ^= (1<<PD7);
		_delay_ms(100);
	};*/
	
	
	
	_delay_ms(5000);
	uint8_t connected_to_ap = 0;
	
	char dane_z_karty[16];
	uint8_t byte = mfrc522_read(VersionReg);
	
	sprintf(send_buffer, "wersja %d \r\n",byte);
	usart_send_buffer();
	/////////test czy oddczytuje karte mifare/////////////
	
	if(byte == 0x92)
	{
		sprintf(send_buffer, "elo\r\n");
		usart_send_buffer();
	}else if(byte == 0x91 || byte==0x90)
	{
		sprintf(send_buffer, "witam \r\n");
		usart_send_buffer();
	}else
	{
		sprintf(send_buffer, "sztos\r\n");
		usart_send_buffer();
	}
	DDRD |=(1<<PD7);
	PORTD |= (1<<PD7);
	DDRD |=(1<<PD6);
	
	while (1)
	{

		
		if(odczytanie_karty(data.id))
		{
			sprintf(send_buffer, "dane %s\r\n", dane_z_karty);
			usart_send_buffer();
			
		}
		PORTD ^= (1<<PD7);
		
		
		

		
		
		_delay_ms(500);
		
		/*
		//data.id++;
		//FLAGA INFORMUJACA CZY UZYSKANO POLACZENIE Z ROUTEREM
		if(!connected_to_ap) connected_to_ap = at_connect_to_ap();
		_delay_ms(5000);
		if(connected_to_ap) 
		{
			data.datetime = at_get_time();
			sprintf(send_buffer, "czas: %ld\r\n",data.datetime);
			usart_send_buffer();
			char connected_to_server = at_connect_to_server(0, "jurpad2.cba.pl", 80);
	
			char sending = 1;
			while (sending && connected_to_server && data.datetime!=0)
			{
				//data = W25Q32BV_read_record(&memory);
				if(data.datetime == 0) break;
	
	
				W25Q32BV_save_record(&memory, &data);
				sd_open("p2.txt");
				sd_write(&data);
				usart_send_buffer(&data);
				//data.id += 1;
				//data.datetime += 327;
				_delay_ms(100);
				sd_close();
	
				sprintf_P(variables, STUDENT_INFO, data.datetime, data.id);
	
				sprintf_P(send_buffer, REQUEST, GET, INDEX_SCRIPT, variables, WEBSITE_URL);
				at_send_flash_data_to_server();
	
				variables[0]='\0';
				receive_index=0;
	
				if(usart_receive(5000, 500)) 
				{
					if(memmem_P((const void*)receive_buffer, receive_index, SUCCESS, strlen_P(SUCCESS))) 
					{
						sending = W25Q32BV_move_send(&memory);
					}
					else 
					{
						sending = 0;
					}
				}
				else 
				{
					sending = 0;	
				}
				_delay_ms(5000);
		}
	
			at_close_connection();
		
		}
		else 
		{
			usart_send(FAILED, 1);
			_delay_ms(15000);
		}
		*/
	}
}

