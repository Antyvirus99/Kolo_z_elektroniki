/*
 * sd_card.c
 *
 * Created: 28.12.2017 19:14:04
 *  Author: Donat
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <avr/eeprom.h>

#include "sd_card.h"
#include "W25Q32BV.h"
#include "atm8a_usart.h"
#include "PetitFAT\pff.h"
#include "PetitFAT\diskio.h"
#include "PetitFAT\spi.h"

volatile char send_buffer[128];

unsigned int przesuniecie=0;
EEMEM unsigned long SD_index[256];

//przerwanie z pinu 

ISR(INT1_vect)
{
	EIMSK &=~ (1<<INT1);
	
	eeprom_write_dword(&SD_index[przesuniecie], 0);
	DDRD |= ((1<<PD4));
	PORTD |= (1<<PD4);
}


//------- PetitFAT - niezbedne deklaracje ---------------------------------
FATFS fs;          // Work area (file system object) for the volume
FRESULT res;	//dd_    , res2;       // Petit FatFs function common result code
UINT br;           // File read count
WORD bw;           // File write count//DIR dir;			/* Directory object */
BYTE buff[16];     // File read buffer
//FILINFO fno, fno2;
/*
odwrocic zapis do pliku
dopisac czyszczenie pamieci eeprom
zrobic testy na inne wartosci niz 8
*/

void delete_eeprom (void)
{
	for (przesuniecie = 0;przesuniecie<256;przesuniecie++)
	{
		eeprom_write_dword(&SD_index[przesuniecie],0);
	}
	przesuniecie = 0;
}

void wyznacz_przesuniecie_na_data (void)
{
	long date_eeprom=0;
	
	date_eeprom = eeprom_read_dword(&SD_index[przesuniecie]);
	while(date_eeprom!=0 && przesuniecie<256)
	{
		przesuniecie++;
		date_eeprom = eeprom_read_dword(&SD_index[przesuniecie]);
	}
	sprintf(send_buffer, "%d \r\n", przesuniecie);
	usart_send_buffer();
	if(przesuniecie == 0) przesuniecie =255;
	else przesuniecie--;
	sprintf(send_buffer, "%d \r\n", przesuniecie);
	usart_send_buffer();
	/*
	if (przesuniecie>=256)
	{
		for (przesuniecie = 0;przesuniecie<256;przesuniecie++)
		{
			eeprom_write_dword(&SD_index[przesuniecie],0);
		}
		
	}*/
}

void write_sdindex_to_eeprom (long data)
{
	//sprawdza gdzie jest przesuniecie i kasuje pamiec
	if (przesuniecie>=255)
	{
		delete_eeprom();
		przesuniecie=0;
	}else przesuniecie++;
		
	eeprom_write_dword(&SD_index[przesuniecie],data);

	/*
	long date_eeprom=0;
	
	date_eeprom = eeprom_read_dword(&SD_index[przesuniecie]);
	while(date_eeprom!=0 && przesuniecie<256)
	{
		date_eeprom = eeprom_read_dword(&SD_index[przesuniecie]);
		przesuniecie++;
	}
	if (przesuniecie==256)
	{
		for (przesuniecie = 0;przesuniecie<256;przesuniecie++)
		{
			eeprom_write_dword(&SD_index[przesuniecie],0);
		}
		przesuniecie = 0;
	}
	else
	{
		eeprom_write_dword(&SD_index[przesuniecie],data);
		przesuniecie++;
	}*/
	/*
	long date_eeprom=eeprom_read_dword(&SD_index);
	long date_eeprom2=eeprom_read_dword(&SD_index+1);
	sprintf(send_buffer, "%ld %ld %ld\r\n", date_eeprom,date_eeprom2,SD_index);
	usart_send_buffer();*/
}


char sprawdzenie (char result, char* answer)
{
	if (result != FR_OK) 
	{
		sprintf(send_buffer,strlen(answer));
		usart_send_buffer();
		return 0;		//zakoncz funkcje jesli nie da sie otworzyc pliku
	}
	return 1;
}

char sd_init(void)
{
	
	//delete_eeprom();
	
	//eeprom_write_dword(&SD_index[0],0);
	wyznacz_przesuniecie_na_data();
	
	//SD - inicjalizacja portow
	port_init();	//funkcja z PetitFAT ustawiaj¹ca piny karty Sd
	sprintf(send_buffer, "inicjalizacja udana \r\n");//"Nie udalo sie odczytac karty!\r\n");
	usart_send_buffer();
	return 1;
}


char sd_open(char* file)
{
	sprintf(send_buffer, "przed \r\n");//"Nie udalo sie odczytac karty!\r\n");
	usart_send_buffer();
	res = pf_mount(&fs);		//otwieramy karte sd
	//if(sprawdzenie(res,"Nie udalo sie odczytac karty!") != 1) return 0;
	//sprintf(send_buffer, "po \r\n");//"Nie udalo sie odczytac karty!\r\n");
	//usart_send_buffer();
	
	if (res != FR_OK) {
		sprintf(send_buffer, "blad pf_mount %d",res);//"Nie udalo sie odczytac karty!\r\n");
		usart_send_buffer();
		return 0;		//zakoncz funkcje gdy nie da sie odczytac karty
	}
	else 
	{
		sprintf(send_buffer, "udalo sie odzczytac karte\r\n");//"Nie udalo sie odczytac karty!\r\n");
		usart_send_buffer();
	}
	
	res = pf_open(file);
	//if(sprawdzenie(res,"Nie udalo sie otworzyc pliku!") != 1) return 0;
	
	if (res != FR_OK) {
		sprintf(send_buffer, "Nie udalo sie otworzyc pliku!\r\n");
		usart_send_buffer();
		return 0;		//zakoncz funkcje jesli nie da sie otworzyc pliku
	}
	else
	{
		sprintf(send_buffer, "udalo sie otworzyc plik\r\n");//"Nie udalo sie odczytac karty!\r\n");
		usart_send_buffer();
	}
	
	return 1;	
}
char sd_write(record* data)
{ 
	const int SIZE = 512;
	unsigned char buffor[SIZE];
	char record_buffor[32];
	sprintf(record_buffor, "%ld,%ld\n", data->id, data->datetime);
	
	unsigned long index = eeprom_read_dword(&SD_index[przesuniecie]);
	
	
	//wyznaczenie sektora do ktorego zapisywana jest data
	unsigned int sector = (index/SIZE);
	/*
	sprintf(send_buffer, "%ld %d %ld\r\n", index,sector,SD_index);
	usart_send_buffer();
	*/
	
	sprintf(send_buffer, "sector %d \r\n", sector);
	usart_send_buffer();
	
	//================pf_plseek wymagany bo jest stara biblioteka========================
	
	res = pf_lseek(sector*SIZE);							//znalezeinie adresu wolnego miejsca w pamieci
	//if(sprawdzenie(res,"Nie udalo sie przesunac wskaznika do odczytu!") != 1) return 0;
	if (res != FR_OK) {
		sprintf(send_buffer, "Nie udalo sie przesunac wskaznika do odczytu!\r\n");
		usart_send_buffer();
		return 0;		//zakoncz funkcje jesli nie da sie otworzyc pliku
	}
	
	
	
	//odczytanie danych zapisanych w pliku

	res = pf_read((void*)buffor, sector*SIZE, SIZE);
	//if(sprawdzenie(res,"Nie udalo sie odczytac danych z sectora!") != 1) return 0;
	//sprintf(send_buffer, "%d", res);
	//usart_send_buffer();
	if (res != FR_OK) {
		sprintf(send_buffer, "Nie udalo sie odczytac danych z sectora! %ld %d\r\n",index,res);
		usart_send_buffer();
		return 0;		//zakoncz funkcje jesli nie da sie otworzyc pliku
	}
	
	else
	{
	
		sprintf(send_buffer, "%ld %d \r\n", index,sector);
		usart_send_buffer();
		
		
		
	}
	
	unsigned int sector_index = index % SIZE;			//wyznaczenie liczby danych juz zapisanych w sectorze
	
	unsigned int free_space = SIZE - sector_index;		
	if(free_space >= strlen(record_buffor)) free_space = strlen(record_buffor);
	
	//long data2 = data->datetime;
	/*
	buffor[sector_index] = 'a'; 
	buffor[sector_index+1] = 'b';
	buffor[sector_index+2] = 'c';
	*/
	
	//for(signed char i = free_space - 1; i >= 0; i--) {
		//buffor[sector_index + free_space - 1 - i] = *(record_buffor + i);
	//}
	
	//for(char a = 1 ; a < sizeof(data->datetime)+1; a++)
	//{
		////record* wsk = &data;
		//
		//
		//buffor[sector_index+a-1] = data2 >> ((sizeof(data->datetime)-a)*8);
	//}
	
	
	memmove((void*)buffor+sector_index, (const void*)record_buffor, free_space);	
	res = pf_lseek(sector*SIZE);							//znalezeinie adresu wolnego miejsca w pamieci
	//if(sprawdzenie(res,"Nie udalo sie przesunac wskaznika do odczytu!") != 1) return 0;
	
	if (res != FR_OK) {
		sprintf(send_buffer, "Nie udalo sie przesunac wskaznika do odczytu!\r\n");
		usart_send_buffer();
		return 0;		//zakoncz funkcje jesli nie da sie otworzyc pliku
	}
	
	res = pf_write((const void*)buffor, SIZE, &bw);				//zapisanie danych do pamieci
	pf_write(0, 0, &bw);
	
	//if(sprawdzenie(res,"Nie udalo sie zapisac danych!") != 1) return 0;
	
	if (res != FR_OK) {
		sprintf(send_buffer, "Nie udalo sie zapisac danych!\r\n");
		usart_send_buffer();
		return 0;		//zakoncz funkcje jesli nie da sie otworzyc pliku
	}
	
	else
	{
		//sprintf(send_buffer, "%d \r\n", bw);
		//usart_send_buffer();
	}
	
	if(free_space < strlen(record_buffor))								//zapisanie reszty "data" do nast sektora w przypadku gdy w pierwszym sektorze zabraklo miejsca
	{
		unsigned int rest_bytes = strlen(record_buffor) - free_space;
		memset((void*)buffor, 0, SIZE);
		//
		//for(char i = rest_bytes - 1; i >= 0; i--) 
		//{
			//buffor[rest_bytes - 1 - i] = *(record_buffor + i);
		//}
		
		memmove((void*)buffor, (const void*)record_buffor+free_space, rest_bytes);
		
		sector++;
		res = pf_lseek(sector*SIZE);
		//if(sprawdzenie(res,"Nie udalo sie przesunac wskaznika do odczytu!") != 1) return 0;
		
		if (res != FR_OK) {
			sprintf(send_buffer, "Nie udalo sie przesunac wskaznika do odczytu!\r\n");
			usart_send_buffer();
			return 0;		//zakoncz funkcje jesli nie da sie otworzyc pliku
		}
		
		res = pf_write((const void*)buffor, SIZE, &bw);
		pf_write(0, 0, &bw);
		//if(sprawdzenie(res,"Nie udalo sie zapisac danych!") != 1) return 0;
		
		if (res != FR_OK) {
			sprintf(send_buffer, "Nie udalo sie zapisac danych!\r\n");
			usart_send_buffer();
			return 0;		//zakoncz funkcje jesli nie da sie otworzyc pliku
		}
	}
	sprintf(send_buffer, "%ld %d %ld %d \r\n", index,sector,SD_index[przesuniecie],przesuniecie);
	usart_send_buffer();
	index += strlen(record_buffor);
	//eeprom_write_dword(&SD_index,index);
	write_sdindex_to_eeprom(index);
		
	sprintf(send_buffer, "%ld %d %ld %d \r\n", index,sector,SD_index[przesuniecie],przesuniecie);
	usart_send_buffer();
	
	return 1;
}

void sd_close (void)
{
	pf_mount(NULL);
}


