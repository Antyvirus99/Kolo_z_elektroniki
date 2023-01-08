/*
 * MFRC522.h
 *
 * Created: 08.06.2017 18:28:16
 *  Author: Donat
 */ 


#ifndef MFRC522_H_
#define MFRC522_H_

#define MFRC522_SS PC5

#define MFRC522_READ 0b10000000
#define MFRC522_WRITE 0b0

extern void MFRC522_Ustaw_piny (void);
extern void MFRC522_High (void);
extern void MFRC522_low (void);
extern unsigned char MFRC522_Read_data (char adres);
extern void MFRC522_write_data (char adres,char data);			
extern void MFRC522_inicjuj (void);
extern void MFRC522_timer (void);
extern void MFRC522_interrupt (void);
#endif /* MFRC522_H_ */