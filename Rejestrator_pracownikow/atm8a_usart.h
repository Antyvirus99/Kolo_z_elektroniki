/*
 * wifi_usart.h
 *
 * Created: 01.04.2017 14:13:45
 *  Author: Donat
 */ 


#ifndef ATM8A_USART_H_
#define ATM8A_USART_H_

#include <stdint.h>

extern volatile char receive_buffer[128];
extern volatile char send_buffer[128];
extern volatile uint16_t receive_index;

extern void usart_init();
extern void usart_send(const __flash char data[], uint8_t clear);
extern uint8_t usart_receive(uint16_t timeout, uint16_t dt);
extern void usart_send_sram(char data[]);
extern void usart_send_character(char character);
extern void usart_send_buffer();


#endif /* ATM8A_USART_H_ */