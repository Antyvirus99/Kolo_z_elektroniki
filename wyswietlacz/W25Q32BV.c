/*
 * W25Q32BV.c
 *
 * Created: 04.05.2017 18:31:03
 *  Author: Donat
 */ 
#include <avr/io.h>
#include <stdlib.h>
#include <util/delay.h>


#include "W25Q32BV.h"
#include "SSD1306.h"
#include "main.h"
#include "utilities.h"

char napis[10];

inline void W25Q32BV_CE_low (void)
{
	PORTB &=~ (1<<W25Q32BV_PIN_CE);
}

inline void W25Q32BV_CE_high (void)
{
	PORTB |= (1<<W25Q32BV_PIN_CE);
}

//podejcie 2
void W25Q32BV_init(W25Q32BV* memory, unsigned record_size) {
	
	//ustawienie mikrokontrolera na tryb master
	set_master();
	
	//ustawienie pinow pod prace z pamiecia w25q32bv
	DDRD |= (1<<W25Q32BV_PIN_WP);
	DDRB |= (1<<W25Q32BV_PIN_CE);
	DDRB |= (1<<W25Q32BV_PIN_HOLD);
	
	
	PORTB |= (1<<W25Q32BV_PIN_CE);
	PORTD |= (1<<W25Q32BV_PIN_WP);
	PORTB |= (1<<W25Q32BV_PIN_HOLD);
	
	memory->record_size = record_size;
	record_read = 0;
	
}

void W25Q32BV_start_reading(unsigned long address) {
	
	W25Q32BV_CE_low();
	SPDR = 0x03;
	while(!(SPSR & (1<<SPIF)));
	SPDR = (address>>16);
	while(!(SPSR & (1<<SPIF)));
	SPDR = (address>>8);
	while(!(SPSR & (1<<SPIF)));
	SPDR = address;
	while(!(SPSR & (1<<SPIF)));
	
}

char W25Q32BV_read_next(void) {
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	return SPDR;
}

void W25Q32BV_check_records(W25Q32BV* memory) {
	
	W25Q32BV_start_reading(FIRST_RECORD_ADDRESS);
	
	char data;
	char last_record = -1;
	
	char searching = 1;
	char save_flag = 0;
	char send_flag = 0;
	unsigned long bytes_checked = 0;
	char block_index = 0;
	
	data = W25Q32BV_read_next();
	//bytes_checked++;
	
	while(searching) {
		//komentarz
		//block index ma odpowiednio przeskakiwac dla data == 0, data != dfasdf, 
		//sprawdzanie kolejnych blokow
		//jesli znajdzie ssve i ssend to koniec
		//z savem jeszcze jakby blok byl caly pusty do zrobienia
		if(data == 255) {
			if(!save_flag || (save_flag && (last_record == 1 || last_record == 2))) {
				memory->save = bytes_checked;
				save_flag = 1;
			}
			
			//pomin caly blok
			while(bytes_checked < (block_index + 1)*BLOCK_SIZE) {
				data = W25Q32BV_read_next();
				bytes_checked++;
			}
			last_record = 0;
		}
		else if(data == 0)
		{
			int i = 0;
			while(i < memory->record_size) {
				data = W25Q32BV_read_next();
				bytes_checked++;
				i++;
			}
			last_record = 1;
		}
		else
		{
			if(!send_flag || (send_flag && (last_record == 0 || last_record == 255))) {
				memory->send = bytes_checked;
				send_flag = 1;
			}
			int i = 0;
			while(i < memory->record_size) {
				data = W25Q32BV_read_next();
				bytes_checked++;
				i++;
			}
			last_record = 2;
		}
		
		if(bytes_checked > (block_index*BLOCK_SIZE - memory->record_size)) {
			while(bytes_checked < block_index*BLOCK_SIZE){
				data = W25Q32BV_read_next();
				bytes_checked ++;
			}
			block_index++;
		}		
		//sprawdz czy przeanalizowal wszystkie bloki
		if (block_index >= BLOCKS_COUNT)
		{
			if(save_flag == 0) memory->save = -1;
			if(send_flag == 0) memory->send = -1;
			searching = 0;
		}
	
	}

	W25Q32BV_CE_high();
}

void W25Q32BV_write_eneable (void)
{
	W25Q32BV_CE_low();
	
	SPDR = 0x06;
	while(!(SPSR & (1<<SPIF)));
	
	W25Q32BV_CE_high();
}

unsigned char W25Q32BV_read_status_register1 (void)
{
	
	W25Q32BV_CE_low();
	
	SPDR = 0x05;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	char dana1 = SPDR;
	
	W25Q32BV_CE_high();
	return dana1;
	//wyslij na wyswietlacz	
	/*
	itoa(dana1,napis,10);
	tekst_z_sram(napis,0,0);
	*/
}

void W25Q32BV_start_writing(unsigned long address)
{
	
	W25Q32BV_write_eneable();
	W25Q32BV_CE_low();
	SPDR = 0x02;
	while(!(SPSR & (1<<SPIF)));
	SPDR = (address>>16);
	while(!(SPSR & (1<<SPIF)));
	SPDR = (address>>8);
	while(!(SPSR & (1<<SPIF)));
	SPDR = (address);
	while(!(SPSR & (1<<SPIF)));
	
}

char W25Q32BV_save_record(W25Q32BV* memory, record* data)
{
	
	//sprawdzenie czy jest miejsce
	if(memory->save == -1) return 0; 
	
	//dane z rekordu
	unsigned long datetime = data->datetime;
	unsigned long id = data->id;
	
	//zapis rekordu
	for(char i = sizeof(datetime)-1; i >= 0; i--)
	{
		while(W25Q32BV_read_status_register1() & 0x01);
		W25Q32BV_start_writing(memory->save + (sizeof(datetime)-1 - i));
		SPDR = datetime>>i*8;
		while(!(SPSR & (1<<SPIF)));
		
	}
	
	for(char i = sizeof(id)-1; i >= 0; i--)
	{
		while(W25Q32BV_read_status_register1() & 0x01);	
		W25Q32BV_start_writing(memory->save + (sizeof(id)-1 - i));
		SPDR = id>>i*8;
		while(!(SPSR & (1<<SPIF)));
	}
	//koniec zapisu rekordu
	
	//jesli nie bylo wczesniej zadnych danych do wyslania to aktualnie zapisana dana
	//staje sie dana do wyslania do bazy danych
	if(memory->send == -1) memory->send = memory->save;
	
	//przesuniecie wkaznika wolnego miejsca na nastepne miejsce
	memory->save += memory->record_size;
	
	//sprawdzenie czy wskaznik nie wskoczyl na koniec bloku i czy nie przekroczyl rozmiaru bufora
	for(char i = 0; i < BLOCKS_COUNT; i++)
	{
		if(memory->save > (i*BLOCK_SIZE - memory->record_size))
		{
			memory->save += (i+1)*BLOCK_SIZE;
			
			//sprawdzenie czy po przesuniecie wskaznika nowy blok jest wolny, jesli nie to memory save na -1
			if(memory->send >= (i+1)*BLOCK_SIZE && memory->send < (i+2)*BLOCK_SIZE)
			{
				memory->save = -1;
			}
			
		}
	}
	
	//sprawdzanie czy nie przekroczylismy rozmiaru bufora
	if(memory->save >= BLOCKS_COUNT*BLOCK_SIZE)
	{
		memory->save = 0;
		if(memory->send >= 0*BLOCK_SIZE && memory->send < 2*BLOCK_SIZE)
		{
			memory->save = -1;
		}
	}
	
	//koniec transmisji danych
	W25Q32BV_CE_high();
	return 1;
}

record W25Q32BV_read_record(W25Q32BV* memory)
{
	
	record data;
	data.datetime = 0;
	data.id = 0;
	
	//sprawdz czy mozna cos wyslac
	if(memory->save == -1) return data;
		
	//poczatek czytania
	W25Q32BV_start_reading(memory->send);
	for(int i = sizeof(data.datetime)-1; i >= 0; i--) {
		char flash_data = W25Q32BV_read_next(); 
		data.datetime += ((unsigned long)flash_data)<<i*8;
	}
	
	for(int i = sizeof(data.id)-1; i >= 0; i--) {
		char flash_data = W25Q32BV_read_next(); 
		data.id += ((unsigned long)flash_data)<<i*8;
	}
	//koniec czytania
	
	W25Q32BV_CE_high();
	//rekord odczytany
	record_read = 1;
	return data;
}

void W25Q32BV_clear_block(char block_index) {
	
	W25Q32BV_write_eneable();
	
	W25Q32BV_CE_low();
	
	unsigned long address = block_index*BLOCK_SIZE;
	
	//kasowanie bloku
	SPDR = 0x20;
	while(!(SPSR & (1<<SPIF)));
	SPDR = (address>>16);
	while(!(SPSR & (1<<SPIF)));
	SPDR = (address>>8);
	while(!(SPSR & (1<<SPIF)));
	SPDR = address;
	while(!(SPSR & (1<<SPIF)));
	
	W25Q32BV_CE_high();
	while(W25Q32BV_read_status_register1() & 0x01);			//czekaj na zakonczenie kasowania
}

char W25Q32BV_move_send(W25Q32BV* memory)
{
	
	//sprawdz czy zostala uruchomiona funkcja odczytujaca rekord	
	if(record_read == 0) return 0;
	
	while(W25Q32BV_read_status_register1() & 0x01);
	//rozpoczecie zapisu
	W25Q32BV_start_writing(memory->save);
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	W25Q32BV_CE_high();
	
	record_read = 0;
	
	//przesuniecie wkaznika wyslania na nastepne miejsce
	memory->send += memory->record_size;
	
	//sprawdzenie czy wskaznik nie wskoczyl na koniec bloku i czy nie przekroczyl rozmiaru bufora
	for(char i = 0; i < BLOCKS_COUNT; i++)
	{
		if(memory->send > (i*BLOCK_SIZE - memory->record_size))
		{
			W25Q32BV_clear_block(i);
			memory->send += (i+1)*BLOCK_SIZE;			
		}
	}
	
	//sprawdzanie czy nie przekroczylismy rozmiaru bufora
	if(memory->send >= BLOCKS_COUNT*BLOCK_SIZE)
	{
		W25Q32BV_clear_block(BLOCKS_COUNT-1);
		memory->send = 0;
	}
	
	//sprawdzenie czy po przesuniecie wskaznika nowy rekord jest cos do wyslania, jesli nie to memory save na -1
	if(memory->send == memory->save)
	{
		memory->save = -1;
	}
	
	//przesuwanie udane
	return 1;
}







void W25Q32BV_ustaw_piny (void)
{
	DDRD |= (1<<W25Q32BV_PIN_WP);
	DDRB |= (1<<W25Q32BV_PIN_CE);
	DDRB |= (1<<W25Q32BV_PIN_HOLD);
	
	
	PORTB |= (1<<W25Q32BV_PIN_CE);
	PORTD |= (1<<W25Q32BV_PIN_WP);
	PORTB |= (1<<W25Q32BV_PIN_HOLD);
}


void W25Q32BV_read_jedec (void)
{
	//przed uzyciem trzeba inicjowac tryb master
	
	W25Q32BV_CE_low();
	
	
	
	SPDR= 0x9F;		//read
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
	
	
	W25Q32BV_CE_high();
	
	//wyslij na realterm
	UDR = dana1;
	while (!(UCSRA & (1<<UDRE)));
	UDR = dana2;
	while (!(UCSRA & (1<<UDRE)));
	UDR = dana3;
	while (!(UCSRA & (1<<UDRE)));
	
	//_delay_ms(100);
	
	//wyslij na wyswietlacz
	/*
	itoa(dana1,napis,10);
	tekst_z_sram(napis,0,0);
	itoa(dana2,napis,10);
	tekst_z_sram(napis,8,0);
	itoa(dana3,napis,10);
	tekst_z_sram(napis,16,0);	
	*/
}

void W25Q32BV_read_id (void)
{
	
	W25Q32BV_CE_low();
	
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
	
	W25Q32BV_CE_high();
	
	//wyslij na wyswietlacz
	
	itoa(dana1,napis,10);
	tekst_z_sram(napis,0,0);
	itoa(dana2,napis,10);
	tekst_z_sram(napis,8,0);
}


void W25Q32BV_read_status_register2 (void)
{
	
	W25Q32BV_CE_low();
	
	SPDR = 0x35;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 0;
	while(!(SPSR & (1<<SPIF)));
	char dana1 = SPDR;
	
	W25Q32BV_CE_high();
	
	//wyslij na wyswietlacz
	/*
	itoa(dana1,napis,10);
	tekst_z_sram(napis,0,0);
	*/
}

void W25Q32BV_read_status_register_both (void)
{
	
	W25Q32BV_read_status_register1();
	W25Q32BV_read_status_register2();
	
}





void W25Q32BV_page_program (unsigned long adres, char dana)
{


	W25Q32BV_write_eneable();
	
	W25Q32BV_CE_low();
	
		//wymuszone otwieranie adresu 0
	SPDR = 0x02;
	while(!(SPSR & (1<<SPIF)));
	SPDR = (adres>>16);
	while(!(SPSR & (1<<SPIF)));
	SPDR = (adres>>8);
	while(!(SPSR & (1<<SPIF)));
	SPDR = adres;
	while(!(SPSR & (1<<SPIF)));
	

	
	SPDR = dana;
	while(!(SPSR & (1<<SPIF)));
	
	/*
	SPDR = 1;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 2;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 3;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 4;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 5;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 6;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 7;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 8;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 9;
	while(!(SPSR & (1<<SPIF)));
	SPDR = 10;
	while(!(SPSR & (1<<SPIF)));
	*/
	W25Q32BV_CE_high();
	
	while(W25Q32BV_read_status_register1() & 0x01);			//czekaj na zakonczenie zapisu
}

void W25Q32BV_read_data (unsigned long adres, long liczba_danych)
{

		
	char dana1[10];
	int index1 = 0;
	
	
	W25Q32BV_CE_low();
	SPDR = 0x03;
	while(!(SPSR & (1<<SPIF)));
	SPDR = (adres>>16);
	while(!(SPSR & (1<<SPIF)));
	SPDR = (adres>>8);
	while(!(SPSR & (1<<SPIF)));
	SPDR = adres;
	while(!(SPSR & (1<<SPIF)));
	
	
	for (long n=0; n<liczba_danych; n++)
	{
		SPDR = 0;
		while(!(SPSR & (1<<SPIF)));
		//dana1[index1] = SPDR;
		dzwiek_play = SPDR;
		
		//index1 ++;
		//do poprawy zrobic tablive zapamietujaca odebrane dane
		
		
	}
	
	W25Q32BV_CE_high();
	
	//wyslij na wyswietlacz
	/*
	for (index1 = 0; index1 > 10; index1++)
	{
		UDR = dana1[index1];
		while(!(UCSRA & (1<<TXC)));
		
		itoa(dana1[index1],napis,10);
		tekst_z_sram(napis,0,0);
		_delay_ms(1000);
		
	}
	*/
	
}

void dd_W25Q32BV_read_data (unsigned long *adres, long liczba_danych)
{
	
	char dana1[10];
	int index1 = 0;
	
	W25Q32BV_CE_low();
	
	SPDR = 0x03;
	while(!(SPSR & (1<<SPIF)));
	SPDR = *(adres+2);
	while(!(SPSR & (1<<SPIF)));
	SPDR = *(adres+1);
	while(!(SPSR & (1<<SPIF)));
	SPDR = *adres;
	while(!(SPSR & (1<<SPIF)));
	
	
	for (long n=0; n<liczba_danych; n++)
	{
		SPDR = 0;
		while(!(SPSR & (1<<SPIF)));
		dana1[index1] = SPDR;
		dzwiek_play = SPDR;
		index1 ++;
		//do poprawy zrobic tablive zapamietujaca odebrane dane
		
		
	}
	
	W25Q32BV_CE_high();
	
	
	//wyslij na wyswietlacz
	for (index1 = 0; index1 > 10; index1++)
	{
		UDR = dana1[index1];
		while(!(UCSRA & (1<<TXC)));
		/*
		itoa(dana1[index1],napis,10);
		tekst_z_sram(napis,0,0);
		_delay_ms(1000);
		*/
	}
	
	
}
void wykasuj_flasha_32kB (unsigned long adres)
{
	

	
	
	W25Q32BV_write_eneable();
	
	W25Q32BV_CE_low();
	
	//wymuszone kasowanie od adresu 0
	SPDR = 0x52;
	while(!(SPSR & (1<<SPIF)));
	SPDR = (adres>>16);
	while(!(SPSR & (1<<SPIF)));
	SPDR = (adres>>8);
	while(!(SPSR & (1<<SPIF)));
	SPDR = adres;
	while(!(SPSR & (1<<SPIF)));
	
	W25Q32BV_CE_high();
	
	
	while(W25Q32BV_read_status_register1() & 0x01);			//czekaj na zakonczenie kasowania

}

//void zapisz_dane_do_wyslania (unsigned long adres, long liczba_danych)
//{
	//adres = adres + czytaj_z_pamieci();
	//
	//W25Q32BV_write_eneable();
	//
	//
	//
	////wymuszone otwieranie adresu 0
	//SPDR = 0x02;
	//while(!(SPSR & (1<<SPIF)));
	//SPDR (adres>>16);
	//while(!(SPSR & (1<<SPIF)));
	//SPDR = (adres>>8);
	//while(!(SPSR & (1<<SPIF)));
	//SPDR = adres;
	//while(!(SPSR & (1<<SPIF)));
	//
	//for (long n=0; n>=liczba_danych; n++)
	//{
		//SPDR = dana;
		//while(!(SPSR & (1<<SPIF)));
		//
		//while(W25Q32BV_read_status_register1() & 0x01);			//czekaj na zakonczenie zapisu
		//W25Q32BV_CE_low();
	//}
	//W25Q32BV_CE_high();
	//
	//
		//
//}

unsigned char sprawdz_czy_dane_mozna_usunac (unsigned long adres)
{
	W25Q32BV_CE_low();
	
	SPDR = 0x03;
	while(!(SPSR & (1<<SPIF)));
	SPDR = (adres>>16);
	while(!(SPSR & (1<<SPIF)));
	SPDR = (adres>>8);
	while(!(SPSR & (1<<SPIF)));
	SPDR = adres;
	while(!(SPSR & (1<<SPIF)));
	
	char block_index = 1;
	unsigned long space_index = 0;	//zmienna odliczjaca ilosc pobranych bajtow od pierwszego adresu
	unsigned int bytes = 7;	//7 bajtow /datetime (4 bajty)/id (2 bajty)/flaga inf czy zapis do bazy danych(1 bajt)
	char status = 0;		//flaga stanu danych status=adres-dane_do_usuniecia od adres, status=0-nic nie usuwaj
	char searching = 1;	//
	unsigned long block_size = 32768;
	
	while(searching == 1) 
	{
		SPDR = 0;
		while(!(SPSR & (1<<SPIF)));
		char data = SPDR;
		
		if(data != 0) 
		{
			//cos jest nadal zapisane
			if (block_index > 3)								//korzystamy z 3 blokow po 32kB na zapisywanie danych
			{
				W25Q32BV_CE_high();
				status = 0;
				searching = 0;
			}
			else
			{
				while (space_index != block_index*block_size)				//przejdz do drugiego bloku
				{
					SPDR = 0;
					while(!(SPSR & (1<<SPIF)));
					space_index++;
				}
				block_index ++;
			}	
		
		}
		else 
		{
			//bajt jest do usunieca
			for(int i = 0; i < bytes - sizeof(char); i++)		//przejdz do kolejnego bytes
			{
				SPDR = 0;
				while(!(SPSR & (1<<SPIF)));		
				space_index++;		
			}
			if(space_index > block_index*block_size - bytes && adres < block_index*block_size-1)
			{
				W25Q32BV_CE_high();
				adres = (block_index-1)*block_size;
				status = adres;
				searching = 0;
			}
			
		}
	}
	return status;
	
}

//unsigned long search_for_free_space (unsigned long adres)
//{
	//W25Q32BV_CE_low();
	//
	//SPDR = 0x03;
	//while(!(SPSR & (1<<SPIF)));
	//SPDR = (adres>>16);
	//while(!(SPSR & (1<<SPIF)));
	//SPDR = (adres>>8);
	//while(!(SPSR & (1<<SPIF)));
	//SPDR = adres;
	//while(!(SPSR & (1<<SPIF)));
	//
	//char searching = 1;	
	//char block_searching =1;			//flaga informujaca czy kontynuowane jest poszukiwanie bloku
	//unsigned long freespace_index = 0;	//zmienna odliczjaca ilosc pobranych bajtow od pierwszego adresu
	//unsigned long datetime;	//data i czas przylozenia karty
	//unsigned long id;	//id studenta odwiedzajacego zajecia
	//unsigned int bytes = sizeof(datetime) + sizeof(id);	// record uix + id
	//unsigned long block_size = 32768;
	//char block_index = 1;
	//
	//while(searching == 1) 
	//{
		//SPDR = 0;
		//while(!(SPSR & (1<<SPIF)));
		//char data = SPDR;
		//
		//if(data == 255) 
		//{
			////wolne miejsce
			//adres += freespace_index;
			//searching = 0;
			//W25Q32BV_CE_high();
		//}
		//else 
		//{
			////pomijamy zajete miejsce 
			//for(int i = 0; i < bytes - 1; i++) 
			//{
				//SPDR = 0;
				//while(!(SPSR & (1<<SPIF)));
				//freespace_index++;
			//}
			//if (freespace_index > block_index*block_size - bytes && freespace_index < block_index*block_size-1)
			//{
				//if (block_index > 3)
				//{
					//
					////error wszystkie 3 bloki sa zajete	dorobic bufor cykliczny oraz 8 bajtowe rekordy	str 5 mozna uzyc sektorow 4kB mozna je kasowac osobno
				//}
				//while(freespace_index != block_index*block_size)	
				//{
					////przejscie do nastepnego bloku, zeby dana nie byla zapiasna pomiedzy blokami
					//SPDR = 0;
					//while(!(SPSR & (1<<SPIF)));
					//freespace_index ++;
				//}
				//block_index ++;
			//}
		//}
	//}
	//
	//
	////wyslij na wyswietlacz i na usarcie do realterma
	//UDR = adres;
	//while(!(UCSRA & (1<<TXC)));
		//
	//itoa(adres,napis,10);
	//tekst_z_sram(napis,0,0);
	//
	//return adres;
	//
//}