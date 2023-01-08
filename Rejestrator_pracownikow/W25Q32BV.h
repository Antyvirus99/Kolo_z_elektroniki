/*
 * W25Q32BV.h
 *
 * Created: 04.05.2017 18:30:50
 *  Author: Donat
 */ 


#ifndef W25Q32BV_H_
#define W25Q32BV_H_

#define W25Q32BV_PIN_CE PD2
#define W25Q32BV_PIN_WP PD5
#define W25Q32BV_PIN_HOLD PB7

#define BLOCK_SIZE 4096 //rozmiar bloku
#define BLOCKS_COUNT 3	//ilosc blokow uzywanych przez bufor

#define FIRST_RECORD_ADDRESS 0	//adres pierwszego rekordu

/*
*	Obiekt przechowujacy adresy miejsc w pamieci
*	w ktorych sa dane do zapisu i do wyslania oraz
*	przechowuje rozmiar jednego rekordu w buforze cyklicznym
*/
typedef struct {
	
	long save;
	long send;
	
	unsigned int record_size;
	
} W25Q32BV;

/*
*	Struktura reprezentujaca pojedynczy rekord
*/
typedef struct {
	
	unsigned long datetime;	//data i czas unix
	unsigned long id;		//id karty
	
} record;

extern char record_read;	//flaga informujaca o tym, ze z pamieci zostal pobrany rekord

/*
*	Inicjalizacja pamieci do pracy jako bufor cykliczny
*	record_size - rozmiar rekordu ktory bedzie przechowywany w buforze
*/
extern void W25Q32BV_init(W25Q32BV* memory, unsigned int record_size);
/*
*	Analizowanie buora cyklicznego w celu ustawienia
*	adresow danej do wyslania i wolnego miejsca do zapisu
*/
extern void W25Q32BV_check_records(W25Q32BV* memory);
/*
*	Zapisanie rekordu 'data' do pamieci pod adres przechowwyany w obiekcie 'memory'
*/
extern char W25Q32BV_save_record(W25Q32BV* memory, record* data);
/*
*	Czyszczenie bloku numer 'block_index'+1 (numeracja od 0)
*/
extern void W25Q32BV_clear_block(char block_index);
/*
*	Odczytanie rekordu z pod adresu przechowywanego w polu 'send' obiektu 'memory'
*/
extern record W25Q32BV_read_record(W25Q32BV* memory);
/*
*	Jesli flaga 'record_read' jest ustawiona na 1, to jest przesuwany wskaznik danej
*	do wyslania przechowywany w strukturze 'memory'
*/
extern char W25Q32BV_move_send(W25Q32BV* memory);


extern void W25Q32BV_read_jedec (void);
extern void W25Q32BV_ustaw_piny (void);
extern void W25Q32BV_read_id (void);
extern unsigned char W25Q32BV_read_status_register1 (void);
extern void W25Q32BV_read_status_register2 (void);
extern void W25Q32BV_read_status_register_both (void);
extern void W25Q32BV_page_program (unsigned long adres, char dana);
extern void W25Q32BV_read_data (unsigned long adres, long liczba_danych);
extern void wykasuj_flasha_32kB (unsigned long adres);
extern void dd_W25Q32BV_read_data (unsigned long *adres, long liczba_danych);
extern unsigned long search_for_free_space (unsigned long adres);


#endif /* W25Q32BV_H_ */