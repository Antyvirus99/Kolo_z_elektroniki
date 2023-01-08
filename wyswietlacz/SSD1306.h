/*
 * SSD1306.h
 *
 * Created: 21.03.2017 19:38:02
 *  Author: Donat
 */ 
#include <avr/pgmspace.h>						//nag³ówek do funkcji obs³ugi danych w pamiêci flash (pamiêæ programu)

#ifndef SSD1306_H_
#define SSD1306_H_
	
	#define SSD1306_PIN_CS PB2
	#define SSD1306_PIN_DC PB1
	
	
	#define SET_CONTRAST_CONTROL	0x81
	#define SET_MEMORY_ADDRESSING_MODE	0x20
	#define SET_COLUMN_ADDRESS	0x21
	#define SET_PAGE_ADDRESS 0x22
	#define CHARGE_PUMP_SETTING 0x8D
	#define SET_MEMORY_ADDRESSING_MODE 0x20
		#define HORIZONTAL_ADDRESSING_MODE 0b00
		#define VERTICAL_ADDRESSING_MODE 0b01
		#define PAGE_ADDRESSING_MODE 0b10
	#define SET_NORMAL_DISPLAY 0xA6
	#define SET_INVERSE_DISPLAY 0xA7
	#define SET_DISPLAY_ON 0xAF
	#define SET_DISPLAY_OFF 0xAE
	
	

extern char napis[];
extern void ustaw_master (void);
extern void ustaw_slave (void);
extern void ssd1306_inicjuj (void);
extern void ssd1306_wyslij_3_bajty (unsigned char komenda, unsigned char bajt1, unsigned char bajt2);
extern void ssd1306_wyslij_2_bajty (unsigned char komenda, unsigned char bajt1);
extern void ssd1306_wyslij_1_bajt (unsigned char komenda);
extern void wyczysc_ekran(void);
extern void tekst_z_flash (const __flash char* napis, char linia, char kolumna);
extern void tekst_z_sram (char* napis, char linia, char kolumna);
extern void tekst_duzy_z_flash (const __flash char* napis, char page, char kolumna);
extern void tekst_duzy_z_sram (char* napis, char page, char kolumna);

#endif /* SSD1306_H_ */