/*
 * ESP8266.h
 *
 * Created: 14.03.2017 19:09:39
 *  Author: Donat
 */ 


#ifndef ESP8266_H_
#define ESP8266_H_

extern volatile char odbiorcza[];
extern unsigned long dana_do_wysylki;

extern void wyzeruj_dane_po_wyslaniu_na_serwer (void);
extern void esp8866_resetuj (void);
extern void ustaw_w_tryb_at(void);
extern void ustaw_w_tryb_normalny(void);
extern char wyslij_na_serwer (void);
extern void polacz_z_NTP (void);
extern void rozlacz (void);
extern unsigned long polacz_i_pobierz_czas_UNIX (void);
extern void polacz_z_router (void);
#endif /* ESP8266_H_ */