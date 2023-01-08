/*
 * dzwieki.c
 *
 * Created: 02.05.2017 18:29:35
 *  Author: Donat
 */ 
#include <avr/io.h>
#include <string.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include <stdlib.h>

void ustaw_FAST_PWM_TIMER2 (void)
{
	DDRB |= (1<<PB3);
	TCCR2 |= (1<<WGM20) | (1<<WGM21);
	TCCR2 |= (1<<COM21);					//non invertig
	
	

}
void oddtworz_dzwiek (void)
{
	
	
	
	
}



