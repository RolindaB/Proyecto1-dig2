#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include "I2C/I2C.h"   // Incluye tu librería I2C
#include "UART/UART.h" // Incluye tu librería UART

// Dirección del sensor LM75
#define LM75_ADDRESS 0x49

// Buffer para mensajes UART
char buffer[128];

// Prototipos de funciones
uint8_t read_temperature(uint8_t *high_byte, uint8_t *low_byte);

int main(void) {
	// Inicializa UART con baud rate 9600
	UART_init(9600);

	// Inicializa I2C a 100 kHz con prescaler 1
	I2C_Master_Init(100000, 1);

	while (1) {
		uint8_t temp_high = 0;
		uint8_t temp_low = 0;

		// Lee la temperatura del sensor LM75
		if (read_temperature(&temp_high, &temp_low) == 0) {
			// Convierte los datos del sensor a una cadena de texto
			int16_t temp_value = (temp_high << 8) | temp_low;
			snprintf(buffer, sizeof(buffer), "Temperature: %d C \n\r", temp_value / 256);
			} else {
			// Si hubo un error en la lectura, formatea un mensaje de error
			snprintf(buffer, sizeof(buffer), "Error reading temperature");
		}

		// Envía el mensaje por UART a la consola
		UART_send_string(buffer);

		// Espera 300 ms antes de la siguiente lectura
		_delay_ms(3000);
	}
}

// Lee la temperatura del sensor LM75
uint8_t read_temperature(uint8_t *high_byte, uint8_t *low_byte) {
	// Inicia la comunicación I2C
	if (I2C_Master_Start() != 0) return 1;

	// Envía la dirección del esclavo (LM75) en modo escritura
	if (I2C_Master_Write(LM75_ADDRESS << 1) != 0) return 1;

	// Envía la dirección del registro de temperatura
	if (I2C_Master_Write(0x00) != 0) return 1;

	// Repite la condición de START para lectura
	if (I2C_Master_Start() != 0) return 1;

	// Envía la dirección del esclavo (LM75) en modo lectura
	if (I2C_Master_Write((LM75_ADDRESS << 1) | 1) != 0) return 1;

	// Lee el byte alto de la temperatura
	if (I2C_Master_Read(high_byte, 1) != 0) return 1;

	// Lee el byte bajo de la temperatura
	if (I2C_Master_Read(low_byte, 0) != 0) return 1;

	// Detiene la comunicación I2C
	I2C_Master_Stop();

	return 0; // Éxito
}