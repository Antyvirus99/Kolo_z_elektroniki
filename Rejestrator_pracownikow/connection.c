#include <inttypes.h>
#include <avr/pgmspace.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
//#include <time.h>
#include <util/delay.h>

#include "connection.h"
#include "atm8a_usart.h"

volatile char send_buffer[128];

uint8_t at_connect_to_server(uint8_t type, char * server_name, uint8_t port) {
	
	uint8_t connected = 0;
	
	char buffer[128];
	
	if(type) {
		sprintf(buffer, "\"UDP\",\"%s\",%d\r\n", server_name, port);
		} else {
		sprintf(buffer, "\"TCP\",\"%s\",%d\r\n", server_name, port);
	}
	
	usart_send(CIPSTART, 0);
	usart_send_sram(buffer);
	
	receive_index = 0;
	//FLAGA CONNECTING USTAWIONA NA 1 TAK DLUGO JAK DLUGO TRWA LACZENIE Z GOOGLE
	uint8_t connecting = 1;
	while(connecting) {
		if(usart_receive(15000,1)) 
		{
			//JESLI MODUL ODPOWIADA TO SPRAWDZAMY CO
			if(memmem_P((const void*)receive_buffer, receive_index, OK, strlen_P(OK))) 
			{
				//JESLI "OK" TO FLAGA POLACZENIA Z WIFI NA 1 I KONCZYMY CZEKANIE
				connected = 1;
				connecting = 0;
			}
		} 
		
		else 
		{
			//JESLI MODUL PRZESTAL ODPOWIADAC TO KONCZYMY CZEKANIE
			connecting = 0;
		}
	}
	
	if(connected) {
		return 1;
	}
	else {
		return 0;
	}
	
}

uint8_t at_send_data_to_server(char * data, uint8_t time) {
	
	uint8_t done = 0;
	
	usart_send(CIPSEND, 0);
	if(time) {
		usart_send_sram("48");
	}
	else {
		char buffer[4];
		sprintf(buffer, "%d", strlen(data));
		usart_send_sram(buffer); 
	}
	
	usart_send_sram("\r\n");
	
	receive_index = 0;
	uint8_t sending = 1;
	while(sending) {
		if(usart_receive(5000,1)) {
			
			if(memmem_P((const void*)receive_buffer, receive_index, MORE_SIGN, strlen_P(MORE_SIGN))) {
				if(time) {
					usart_send_character(0x1B);
					for(int i = 0; i < 47; i++) {
						usart_send_character(0);
					}
				}
				else {
					usart_send_sram(data);
				}
				done = 1;
				sending = 0;
			}
			
		}
		else {
			sending = 0;
		}
	}
	
	return done;
	
}

uint8_t at_send_flash_data_to_server() {
	
	//FLAGA ZWRACANA PRZEZ FUNKCJE
	uint8_t done = 0;
		
	//WYSYLANIE DLUGOSCI ZAPYTANIA
	uint8_t data_length = strlen((char*)send_buffer);
	usart_send(CIPSEND, 0);
	char buffer[8];
	sprintf(buffer, "%d\r\n", data_length);
	usart_send_sram(buffer);
	
	receive_index = 0;
	uint8_t sending = 1;
	while(sending) {
		//CZEKAMY NA POZWOLENIE SERWERA NA WYSYL DANYCH
		if(usart_receive(5000,1)) {
			
			if(memmem_P((const void*)receive_buffer, receive_index, MORE_SIGN, strlen_P(MORE_SIGN))) {
				//SERWER CZEKA NA DANE WIEC WYSYLAMY DANE!!!!!!!!!!
				receive_index = 0;
				usart_send_buffer();
				done = 1;
				sending = 0;
			}
		}
		else {
			sending = 0;
		}
	}
	
	return done;
	
}

void at_command_mode() {
	_delay_ms(500);
	//PRZECHODZIMY SPOWROTEM NA TRYB KOMEND AT
	usart_send(PLUSES, 0);
	_delay_ms(1500);
}

uint8_t at_close_connection() {
	usart_send(CIPCLOSE, 1);
	receive_index = 0;
	uint8_t closing = 1;
	uint8_t done = 0;
	while(closing) {
		if(usart_receive(5000,1)) {
			
			if(memmem_P((const void*)receive_buffer, receive_index, OK, strlen_P(OK))) {
				done = 1;
				closing = 0;
			}
			
		}
		else {
			closing = 0;
		}
	}
	
	if(done) {
		return 1;
	}
	else {
		return 0;
	}
	
}

unsigned long at_get_time() {

	unsigned long time = 0;
	
	if(at_connect_to_server(1, "ntp1.tp.pl", 123)) {
		
		if(at_send_data_to_server(NULL, 1)) {
			
			receive_index = 0;
			usart_receive(5000,1);
			if(usart_receive(1000,1)) {
				unsigned char * pointer = memmem_P((const void*)receive_buffer, receive_index, IPD, strlen_P(IPD));
				if(pointer) {
					//JESLI DOSTALISMY INFO O CZASIE TO PARSUJEMY!!!!!!!!!!!!!!!!
					//PARSOWANKO
					pointer = pointer + 8 + 40;
					//ODCZTYTYWANIE CZASU
					
					for(int i = 0; i < 4; i++){
						time += *pointer;
						if(i < 3) time = time << 8;
						pointer++;
					}

					time -= 2208988800;	//zmiana na czas UNIX
					
					//struct tm *time_struct;
					////time_t timestamp = time;
					//set_zone(2 * ONE_HOUR);
					//time_struct = localtime(&time);
					char time_buffer[64];
					sprintf(time_buffer, "%lu", time);
					
					if(time > 1494660000 && time < 1494690000){
						usart_send_character('Y');			//test
						usart_send_character('-');
						usart_send_sram(time_buffer);
					}
					else usart_send_character('N');			//test
				}
			}
		}
		at_command_mode();
		at_close_connection();
	}
	
	return time;
}

uint8_t at_connect_to_ap() {
	//FLAGI INFORMUJACE O STANIE MODULU
	uint8_t cwmode_set = 0;
	uint8_t wifi_connected = 0;
	
	//USTAWIENIE TRYBU PRACY MODULU NA ...
	usart_send(CWMODE, 1);
	
	//CZEKANIE NA ODPOWIEDZ MODULU
	receive_index = 0;
	usart_receive(500,1);
	usart_receive(500,1);
	if(memmem_P((const void*)receive_buffer, receive_index, OK, strlen_P(OK))) cwmode_set = 1;
	
	_delay_ms(1000);
	
	//JESLI UDALO SIE USTAWIC TRYB PRACY TO MODUL LACZY SIE Z WIFI
	if(cwmode_set) {
		//CZEKANIE NA ODPOWIEDZ MODULU
		usart_send(AP_CONNECT, 1);
		receive_index = 0;
		//FLAGA CONNECTING USTAWIONA NA 1 TAK DLUGO JAK DLUGO TRWA LACZENIE Z WIFI
		uint8_t connecting = 1;
		while(connecting) {
			if(usart_receive(5000,1)) {
				//JESLI MODUL ODPOWIADA TO SPRAWDZAMY CO
				if(memmem((const void*)receive_buffer, receive_index, "FAIL", strlen("FAIL"))) {
					//JESLI "FAIL" TO KONCZYMY CZEKANIE
					connecting = 0;
				}
				if(memmem((const void*)receive_buffer, receive_index, "OK", strlen("OK"))) {
					//JESLI "OK" TO FLAGA POLACZENIA Z WIFI NA 1 I KONCZYMY CZEKANIE
					wifi_connected = 1;
					connecting = 0;
				}
				} else {
				//JESLI MODUL PRZESTAL ODPOWIADAC TO KONCZYMY CZEKANIE
				connecting = 0;
			}
		}
	}
	
	if(wifi_connected) return 1;
	else return 0;
	
}