/*
 * UART.h
 *
 * Created: 13/08/2024 08:24:27
 *  Author: asrol
 */ 


#ifndef UART_H_
#define UART_H_
#include <stdint.h> // Para uint8_t
#define F_CPU 16000000UL // Frecuencia del reloj del microcontrolador
#define BAUD_PRESCALE(baud) ((F_CPU / (baud * 16UL)) - 1) // Macro para calcular el prescaler del baud rate



volatile char uart_buffer[128]; // Buffer circular para almacenar datos recibidos por UART
volatile uint8_t uart_head; // Índice de la cabeza del buffer
volatile uint8_t uart_tail; // Índice de la cola del buffer

/* Inicializa el UART con el baud rate especificado.*/
void UART_init(unsigned int baud);
/* Envía un solo carácter a través de UART. */
void UART_send(unsigned char data);
/* Envía una cadena de caracteres a través de UART. */
void UART_send_string(const char* str);
/* Envía un número a través de UART. */
void UART_send_number(uint8_t number);
/* Recibe un solo carácter del buffer UART. */
unsigned char UART_receive(void);
/* Verifica si hay datos disponibles en el buffer UART. */
int UART_available(void);
/* Lee una cadena del buffer UART hasta que se encuentra un delimitador. */
void UART_receive_string(char* buffer, uint8_t max_length);
void writeTextUART(char* Texto);


#endif /* UART_H_ */