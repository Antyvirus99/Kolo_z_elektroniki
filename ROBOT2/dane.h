/*
 * dane.h
 *
 * Created: 11.02.2016 18:10:24
 *  Author: Donat
 */ 


#ifndef DANE_H_
#define DANE_H_
	
	
	extern volatile unsigned char ramka_odbiorcza [3];
	extern volatile unsigned char ramka_nadawcza [3];
	
	
	/*
	bajt 0 - liczba
	
	
	bajt 1
	-bit 2 przycisk PD7
	-bit 3 przycisk PD3
	-bit 6 dioda PB0
	
	bajt 2
	-bit 5 dioda PD6
	-bit 7 dioda PC5
	-bit 1 przycisk PC2
	
	*/




#endif /* DANE_H_ */