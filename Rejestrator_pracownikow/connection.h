/*
 * connection.h
 *
 * Created: 2017/05/13 13:42:44
 *  Author: Dondu
 */ 


#ifndef CONNECTION_H_
#define CONNECTION_H_

#define CONNECTION_POST 2
#define CONNECTION_GET 3

static const __flash char CIPSTATUS[] = "AT+CIPSTATUS";
static const __flash char AP_CONNECT[] = "AT+CWJAP_CUR=\"huawei_plus\",\"oscylator_harmoniczny\"";
static const __flash char CWMODE[] = "AT+CWMODE_CUR=1";
static const __flash char CIPSTART[] = "AT+CIPSTART=";
static const __flash char CIPSEND[] =	"AT+CIPSEND=";
static const __flash char CIPCLOSE[] = "AT+CIPCLOSE";
static const __flash char PLUSES[] = "+++";
static const __flash char RESTART[] = "AT+RST";
static const __flash char OK[] = "OK";
static const __flash char MORE_SIGN[] = ">";
static const __flash char IPD[] = "+IPD,48:";
static const __flash char GET[] = "GET";
static const __flash char POST[] = "POST";
static const __flash char SPACE[] = " ";
static const __flash char QUESTION[] = "?";
static const __flash char AMPERSAND[] = "&";
static const __flash char EQUAL[] = "=";
//static const __flash char HTTP[] = "HTTP/1.1\r\nHost:";
static const __flash char NEW_LINE[] = "\r\n";
static const __flash char REQUEST[] = "%S %S?%s HTTP/1.1\r\nHost: %S\r\n\r\n";
static const __flash char VAR_5[] = "%S=%d&%S=%d&%S=%d&%S=%d&%S=%d";
static const __flash char VAR_ID[] = "id=%d";
static const __flash char STUDENT_INFO[] = "dt=%ld&id=%ld";
//GET/POST \nazwa_sryptu?\zmienne_do_wypelnienia HTTP/1.1\r\nHost: nazwa_serwera\r\n\r\n
//INFO
static const __flash char FAILED[] = "CONNECTION FAILED!";

extern char variables[64];

extern unsigned long at_get_time();
extern uint8_t at_send_flash_data_to_server();
extern uint8_t at_send_data_to_server(char * data, uint8_t time);
extern uint8_t at_connect_to_server(uint8_t type, char * server_name, uint8_t port);
extern uint8_t at_close_connection();
extern uint8_t at_connect_to_ap();



#endif /* CONNECTION_H_ */