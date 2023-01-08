/*
 * silniki.h
 *
 * Created: 2016-03-16 19:30:26
 *  Author: Dondu
 */ 


#ifndef SILNIKI_H_
#define SILNIKI_H_

//silnik 1

//S1K1_D
#define PRAWY_K1_D_PWM_DDRx				DDRD
#define PRAWY_K1_D_PWM_PORTx			PORTD
#define PRAWY_K1_D_PWM_PINx				PD6
#define PRAWY_WSTECZ_OCR				OCR0A
//S1K2_D
#define PRAWY_K2_D_PWM_DDRx				DDRD
#define PRAWY_K2_D_PWM_PORTx			PORTD
#define PRAWY_K2_D_PWM_PINx				PD5
#define PRAWY_NAPRZOD_OCR				OCR0B
//S1K1_G
#define PRAWY_K1_G_DDRx					DDRC
#define PRAWY_K1_G_PORTx				PORTC
#define PRAWY_K1_G_PINx					PC0

//S1K2_G
#define PRAWY_K2_G_DDRx					DDRC
#define PRAWY_K2_G_PORTx				PORTC
#define PRAWY_K2_G_PINx					PC1

	//silnik1_naprzod()				OC0B
	//silnik1_wstecz()				OC0A



//silnik 2
//S2K1_D
#define LEWY_K1_D_PWM_DDRx				DDRB
#define LEWY_K1_D_PWM_PORTx				PORTB
#define LEWY_K1_D_PWM_PINx				PB3
#define LEWY_WSTECZ_OCR					OCR2A	
//S2K2_D
#define LEWY_K2_D_PWM_DDRx				DDRD
#define LEWY_K2_D_PWM_PORTx				PORTD
#define LEWY_K2_D_PWM_PINx				PD3
#define LEWY_NAPRZOD_OCR				OCR2B
//S2K1_G
#define LEWY_K1_G_DDRx					DDRC
#define LEWY_K1_G_PORTx					PORTC
#define LEWY_K1_G_PINx					PC2
//S2K1_G
#define LEWY_K2_G_DDRx					DDRC
#define LEWY_K2_G_PORTx					PORTC
#define LEWY_K2_G_PINx					PC3

	//silnik_lewy_naprzod()				OC2B
	//silnik_lewy_wstecz()				OC2A
	
	
	//enkoder_lewy()					PB0
	//enkoder_prawy()					PD2

extern void silniki_inicjuj (void);
extern void silnik1_naprzod (void);
extern void silnik1_wstecz (void);
extern void silnik2_naprzod (void);
extern void silnik2_wstecz (void);
extern void silnik1_stop (void);
extern void silnik2_stop (void);
#endif /* SILNIKI_H_ */