/*
 * S1_naprzod.c
 *
 * Created: 2016-03-16 19:05:21
 *  Author: Dondu
 */ 
#include <avr/io.h>

#include <util/delay.h>


#include "silniki.h"
//silnik 1



//Zmniejszyæ fusebitami czas startu do minimum  by chroniæ tranzystory


void silniki_inicjuj (void)
{
//ustawienie pinow na wyjscie do sterowania mostkami
	PRAWY_K1_D_PWM_DDRx		|=  (1 << PRAWY_K1_D_PWM_PINx);
	PRAWY_K1_D_PWM_PORTx	&=~ (1 << PRAWY_K1_D_PWM_PINx);
	
	PRAWY_K2_D_PWM_DDRx		|=  (1 << PRAWY_K2_D_PWM_PINx);
	PRAWY_K2_D_PWM_PORTx	&=~ (1 << PRAWY_K2_D_PWM_PINx);

	LEWY_K1_D_PWM_DDRx		|=  (1 << LEWY_K1_D_PWM_PINx);
	LEWY_K1_D_PWM_PORTx		&=~ (1 << LEWY_K1_D_PWM_PINx);
	
	LEWY_K2_D_PWM_DDRx		|=  (1 << LEWY_K2_D_PWM_PINx);
	LEWY_K2_D_PWM_PORTx		&=~ (1 << LEWY_K2_D_PWM_PINx);
	
	PRAWY_K1_G_DDRx			|= (1 << PRAWY_K1_G_PINx);
	PRAWY_K1_G_PORTx		|= (1 << PRAWY_K1_G_PINx);
	
	PRAWY_K2_G_DDRx			|= (1 << PRAWY_K2_G_PINx);
	PRAWY_K2_G_PORTx		|= (1 << PRAWY_K2_G_PINx);
	
	LEWY_K1_G_DDRx			|= (1 << LEWY_K1_G_PINx);
	LEWY_K1_G_PORTx			|= (1 << LEWY_K1_G_PINx);
	
	LEWY_K2_G_DDRx			|= (1 << LEWY_K2_G_PINx);
	LEWY_K2_G_PORTx			|= (1 << LEWY_K2_G_PINx);
	
	//timer 0 obsluguje PWM silnika 1
	TCCR0A |= (1<<WGM01) | (1<< WGM00); //Fast PWM TOP= 0xFF
	TCCR0B |= (1<<CS00); //preskaler 1
	
	//timer 2 obsluguje PWM silnika 2
	TCCR2A |= (1<<WGM21) | (1<< WGM20); //Fast PWM TOP= 0xFF
	TCCR2B |= (1<<CS20); //preskaler 8
	

	
	
}




void silnik1_naprzod (void)
{
	//wylacz pwm OC0A
	PRAWY_K1_D_PWM_PORTx &=~ (1 << PRAWY_K1_D_PWM_PINx);//na wszelki wypadek wyzerowanie pinu
	TCCR0A &=~ ( 1 << COM0A1); //Odlacz pin OC0A od timera 0
	
	//Wylacz tranzystor gorny
	PRAWY_K2_G_PORTx	 |= (1 << PRAWY_K2_G_PINx);
	
	_delay_ms(5);
	
	TCCR0A			 |= ( 1 << COM0B1); //Wlacz PWM na pinie OC0B
	PRAWY_K1_G_PORTx	 &=~ (1 << PRAWY_K1_G_PINx);//Wlacz tranzystor gorny
}



void silnik1_wstecz (void)
{
	PRAWY_K2_D_PWM_PORTx &=~ (1 << PRAWY_K2_D_PWM_PINx);
	TCCR0A			&=~ (1<<COM0B1); //Odlacz pin OC0B od timera 0
	
	//Wylacz tranzystor gorny
	PRAWY_K1_G_PORTx |= (1<<PRAWY_K1_G_PINx);
	
	_delay_ms(5);
	
	TCCR0A |= (1<<COM0A1); //Wlacz PWM na pinie OC0A
	PRAWY_K2_G_PORTx	 &=~ (1 << PRAWY_K2_G_PINx);//wlacz tranzystor gorny
}


void silnik2_naprzod (void)//zmieniany z silnik2 naprzod
{
	LEWY_K1_D_PWM_PORTx &=~ (1 << LEWY_K1_D_PWM_PINx);
	TCCR2A			 &=~ (1<<COM2A1); //odlacz pin OC2A PB3 od timera 2
	
	//wylacz tranzystor gorny
	LEWY_K2_G_PORTx	 |= (1 << LEWY_K2_G_PINx);
	
	_delay_ms(5);
	
	TCCR2A			 |= (1 << COM2B1); //wlacz PWM na pinie PD3 OC2B
	LEWY_K1_G_PORTx	 &=~ (1 << LEWY_K1_G_PINx); //wlacz tranzystor gorny
}


void silnik2_wstecz (void)//zmieniany z silnik2 wstecz
{
	LEWY_K2_D_PWM_PORTx &=~ (1 << LEWY_K2_D_PWM_PINx);
	TCCR2A			 &=~ (1 << COM2B1); //odlacz pin OC2B  pin PD3 od timera 2
	
	//wylacz tranzystor gorny
	LEWY_K1_G_PORTx	 |= (1 << LEWY_K1_G_PINx);
	
	_delay_ms(5);
	
	TCCR2A			 |= (1<< COM2A1); //wlacz PWN na pinie OC2A PB3
	LEWY_K2_G_PORTx	 &=~ (1 << LEWY_K2_G_PINx); //wlacz tranzystor gorny
}

void silnik1_stop (void)
{
	PRAWY_K2_D_PWM_PORTx &=~ (1 << PRAWY_K2_D_PWM_PINx);
	TCCR0A			&=~ (1<<COM0B1); //Odlacz pin OC0B od timera 0
	//wylacz tranzystor
	PRAWY_K1_G_PORTx	 |= (1 << PRAWY_K1_G_PINx);
	PRAWY_K1_D_PWM_PORTx &=~ (1 << PRAWY_K1_D_PWM_PINx);
	TCCR0A &=~ ( 1 << COM0A1); //Odlacz pin OC0A od timera 0
	//wylacz tranzystor
	PRAWY_K2_G_PORTx	 |= (1 << PRAWY_K2_G_PINx);
}


void silnik2_stop (void)
{
	LEWY_K2_D_PWM_PORTx &=~ (1 << LEWY_K2_D_PWM_PINx);
	TCCR2A			 &=~ (1<<COM2A1); //odlacz pin OC2A od timera 2
	//wylacz tranzystor
	LEWY_K1_G_PORTx	 |= (1 << LEWY_K1_G_PINx);
	LEWY_K1_D_PWM_PORTx &=~ (1 << LEWY_K1_D_PWM_PINx);
	TCCR2A			 &=~ (1 << COM2B1); //odlacz pin OC2B od timera 2
	//wylacz tranzystor
	LEWY_K2_G_PORTx	 |= (1 << LEWY_K2_G_PINx);
}
