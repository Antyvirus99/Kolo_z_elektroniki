/*
 * W25Q32BV.h
 *
 * Created: 04.05.2017 18:30:50
 *  Author: Donat
 */ 


#ifndef W25Q32BV_H_
#define W25Q32BV_H_

#define W25Q32BV_PIN_CE PB0
#define W25Q32BV_PIN_WP PD5
#define W25Q32BV_PIN_HOLD PB7

#define BLOCK_SIZE 4096
#define BLOCKS_COUNT 3

#define BLOCK_FREE 0
#define BLOCK_NOT_SENT 1
#define BLOCK_TO_DELETE 2

#define FIRST_RECORD_ADDRESS 0


typedef struct {
	
	long save;
	long send;
	
	unsigned int record_size;
	
} W25Q32BV;

typedef struct {
	
	unsigned long datetime;
	unsigned long id;
	
} record;

extern char record_read;

extern void W25Q32BV_init(W25Q32BV* memory, unsigned int record_size);
extern void W25Q32BV_check_records(W25Q32BV* memory);
//extern char W25Q32BV_save_record(W25Q32BV* memory, unsigned long timestamp, unsigned long id);
extern char W25Q32BV_send_to_database(W25Q32BV* memory);
extern char W25Q32BV_save_record(W25Q32BV* memory, record* data);



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