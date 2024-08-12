#include "UART.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdint.h>

#define F_CPU 16000000UL
#define BAUD_PRESCALE(baud) ((F_CPU / (baud * 16UL)) - 1)

volatile char uart_buffer[128]; // Buffer circular para almacenar datos recibidos por UART
volatile uint8_t uart_head = 0; // Índice de la cabeza del buffer
volatile uint8_t uart_tail = 0; // Índice de la cola del buffer

/* Inicializa el UART con el baud rate especificado. */
void UART_init(unsigned int baud) {
	unsigned int ubrr = BAUD_PRESCALE(baud);
	UBRR0H = (unsigned char)(ubrr >> 8);
	UBRR0L = (unsigned char)ubrr;
	UCSR0B = (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); // Habilitar RX, TX y la interrupción RX
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00); // Configurar para 8 bits de datos, 1 bit de stop
}

/* Envía un solo carácter a través de UART. */
void UART_send(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0))); // Esperar hasta que el buffer de transmisión esté vacío
	UDR0 = data; // Enviar el carácter
}

/* Envía una cadena de caracteres a través de UART. */
void UART_send_string(const char* str) {
	while (*str) {
		UART_send(*str++); // Enviar cada carácter de la cadena
	}
}

/* Envía un número a través de UART. */
void UART_send_number(uint8_t number) {
	char buffer[4]; // Para almacenar hasta 3 dígitos y un terminador de cadena
	snprintf(buffer, sizeof(buffer), "%u", number); // Convierte el número a cadena
	UART_send_string(buffer); // Envía la cadena
}

/* Lee un solo carácter del buffer UART. */
unsigned char UART_receive(void) {
	if (uart_head != uart_tail) { // Verificar si hay datos disponibles
		unsigned char data = uart_buffer[uart_tail]; // Leer el carácter del buffer
		uart_tail = (uart_tail + 1) % sizeof(uart_buffer); // Actualizar el índice de la cola
		return data; // Devolver el carácter recibido
		} else {
		return 0; // No hay datos disponibles
	}
}

/* Lee una cadena del buffer UART hasta que se encuentra un delimitador. */
void UART_receive_string(char* buffer, uint8_t max_length) {
	uint8_t index = 0;
	char received_char;

	while (index < (max_length - 1)) { // Deja espacio para el terminador de cadena
		if (UART_available()) {
			received_char = UART_receive();
			if (received_char == '\n' || received_char == '\r') {
				// Fin de la cadena
				break;
			}
			buffer[index++] = received_char;
		}
	}
	buffer[index] = '\0'; // Null-terminar el string
}

/* Verifica si hay datos disponibles en el buffer UART. */
int UART_available(void) {
	return (uart_head != uart_tail); // Retorna 1 si hay datos en el buffer, 0 si no
}

// Rutina de interrupción del UART
ISR(USART_RX_vect) {
	uart_buffer[uart_head] = UDR0; // Leer el carácter recibido del registro UDR0
	uart_head = (uart_head + 1) % sizeof(uart_buffer); // Actualizar el índice de la cabeza del buffer
	uart_buffer[uart_head] = '\0'; // Terminador de cadena
}
