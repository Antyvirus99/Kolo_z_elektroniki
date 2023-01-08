#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <string.h>
#include "atm8a_usart.h"

volatile char receive_buffer[128];
volatile uint16_t receive_index;

ISR(USART_RX_vect) {
	if(receive_index == 128) receive_index = 0;
	receive_buffer[receive_index] = UDR0;
	receive_index++;
}

void usart_init(){
	sei();
	
	// WLACZENIE NADAJNIKA I ODBIORNIKA
	UCSR0B |= (1 << RXCIE0) | (1 << RXEN0) | (1 << TXEN0);
	
	// USTAWIANIE OSMIOBITOWEJ TRANSMISJI
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
	
	UCSR0A |= (1<<U2X0);
	
	// USTAWIANIE PREDKOSCI TRANSMISJI DANYCH
	UBRR0 = 15;
	
	// USTAWIENIE TXD NA WYJSCIE
	DDRD |=	(1 << PD1);
	
	DDRB |= (1 << PB0);
	PORTB |= (1 << PB0);
}
void usart_send(const __flash char data[], uint8_t clear){
	
	for(uint16_t i = 0; data[i] != '\0'; i++){
		/* Wait for empty transmit buffer */
		while ( !( UCSR0A & (1<<UDRE0)) );
		/* Put data into buffer, sends the data */
		UDR0 = data[i];
	}
	
	if(clear) {
		while ( !( UCSR0A & (1<<UDRE0)) );
		UDR0 = '\r';
		while ( !( UCSR0A & (1<<UDRE0)) );
		UDR0 = '\n';	
	}
}

void usart_send_character(char character){
	
	while ( !( UCSR0A & (1<<UDRE0)) );
	UDR0 = character;
	
}

void usart_send_sram(char data[]){
	
	for(uint16_t i = 0; data[i] != '\0'; i++){
		/* Wait for empty transmit buffer */
		while ( !( UCSR0A & (1<<UDRE0)) );
		/* Put data into buffer, sends the data */
		UDR0 = data[i];
	}
	
}

void usart_send_buffer() {
	
	for(uint8_t i = 0; i < strlen(send_buffer); i++) {
		/* Wait for empty transmit buffer */
		while ( !( UCSR0A & (1<<UDRE0)) );
		/* Put data into buffer, sends the data */
		UDR0 = send_buffer[i];
	}
	
	send_buffer[0] = '\0';
	
}

uint8_t usart_receive(uint16_t timeout, uint16_t dt) {	
	uint16_t time = 0;
	cli();
	uint16_t previous_index = receive_index;
	sei();
	
	//CZEKANNIE NA POCZATEK ODBIORU
	while(time < timeout) {
		
		cli();
			if(receive_index != previous_index) {
				sei();
				break;
			}
		sei();
		
		_delay_ms(1);
		time++;
	}
	
	//KONIEC CZASU OCZEKIWANIA NA POCZATEK TRANSMISJI
	if(time == timeout) return 0;
	
	//ODBIERANIE PACZKI BAJTOW
	cli();
	while(previous_index < receive_index) {
		previous_index = receive_index;
		sei();
		for(int i = 0; i < dt; i++)	_delay_ms(1);
		cli();
	}
	sei();
	return 1;
}