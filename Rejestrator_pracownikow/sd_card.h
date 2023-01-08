/*
 * sd_card.h
 *
 * Created: 28.12.2017 19:14:38
 *  Author: Donat
 */ 


#ifndef SD_CARD_H_
#define SD_CARD_H_
#include "W25Q32BV.h"

extern char sprawdzenie (char result, char answer[]);
extern char sd_init(void);
extern char sd_open(char* file);
extern char sd_write(record* data);
extern void sd_close (void);
extern void write_sdindex_to_eeprom (long data);
extern void wyznacz_przesuniecie_na_data (void);
extern void delete_eeprom (void);




#endif /* SD_CARD_H_ */