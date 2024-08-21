#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "I2C/I2C.h"   // Incluye librería I2C
#include "UART/UART.h" // Incluye librería UART
#include "LCD/LCD.h"   // Librería LCD

// Direcciones
#define LM75_ADDRESS 0x48
#define SLAVE1 0x02
#define SLAVE2 0x04

// Buffer para mensajes UART
char buffer[128];
char buffer1[16];
char buffer2[16];
int16_t temp_value;
uint8_t abierto, day, ventilador,dato;

// Prototipos de funciones
uint8_t read_temperature(uint8_t *high_byte, uint8_t *low_byte);
void uartCasa();
void LCDcasa();
void sensorTemp();
void portonUltra();

void portonUltra() {
	uint8_t result = read_from_slave(SLAVE1, &abierto);
	int abierto2 = (int)(abierto);
	if (result == 0) {
		if (abierto2 == 1) {
			strcpy(buffer1, "open");
			} else {
			strcpy(buffer1, "close");
		}
		} else {
		strcpy(buffer1, "error");
	}
}

void diax(){
	uint8_t result = read_from_slave(SLAVE2, &day);
	int day2 = (int)(day);
	if (result == 0) {
		if (day2 == 1) {
			strcpy(buffer1, "Día");
			} else {
			strcpy(buffer1, "noche");
		}
		} else {
		strcpy(buffer1, "error");
	}
}
	
void sensorTemp() {
	uint8_t temp_high = 0;
	uint8_t temp_low = 0;

	// Lee la temperatura del sensor LM75
	if (read_temperature(&temp_high, &temp_low) == 0) {
		// Convierte los datos del sensor a una cadena de texto
		temp_value = (temp_high << 8) | temp_low;
		snprintf(buffer, sizeof(buffer), "Temperature: %d C \n\r", temp_value / 256);

		// Convierte la temperatura a grados Celsius
		int temp2 = (int)(temp_value / 256);

		// Corrige la comparación para verificar si la temperatura es mayor a 20
		if (temp2 > 20 ) {
			PORTC |= (1 << PORTC2); // Enciende el LED si la temperatura es mayor a 20
			ventilador = 1;
			} else {
			PORTC &= ~(1 << PORTC2); // Apaga el LED si la temperatura es 20 o menor
			ventilador =0;
		}
		} else {
		// Si hubo un error en la lectura, formatea un mensaje de error
		snprintf(buffer, sizeof(buffer), "Error reading temperature");
	}

	// Envía el mensaje por UART a la consola
	//UART_send_string(buffer);
}

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

void LCDcasa() {
	// Muestra la temperatura
	snprintf(buffer, sizeof(buffer), "%d C", temp_value / 256);
	LCD_Set_Cursor(1, 2); // Posiciona el cursor en la primera línea del LCD
	LCD_Write_String("      "); // Limpia la línea
	LCD_Set_Cursor(1, 2); // Posiciona el cursor nuevamente
	LCD_Write_String(buffer); // Muestra el valor
	LCD_Set_Cursor(1, 1); // Posiciona el cursor nuevamente
	LCD_Write_String("temp:"); // Muestra la etiqueta

	// Muestra el estado del portón
	LCD_Set_Cursor(7, 2); // Posiciona el cursor en la segunda línea del LCD
	LCD_Write_String("        "); // Limpia la línea
	LCD_Set_Cursor(7, 2); // Posiciona el cursor nuevamente
	LCD_Write_String(buffer1); // Muestra el estado del portón
	LCD_Set_Cursor(7, 1); // Posiciona el cursor nuevamente
	LCD_Write_String("Gate:"); // Muestra la etiqueta
	
	// Muestra el estado del dia
	LCD_Set_Cursor(10, 2); // Posiciona el cursor en la segunda línea del LCD
	LCD_Write_String("        "); // Limpia la línea
	LCD_Set_Cursor(10, 2); // Posiciona el cursor nuevamente
	LCD_Write_String(buffer2); // Muestra el estado del portón
	LCD_Set_Cursor(10, 1); // Posiciona el cursor nuevamente
	LCD_Write_String("Day:"); // Muestra la etiqueta
}

void uartCasa() {
	if (UART_available()) {
		char comando = UART_receive();
		UART_send(comando);
		UART_send_string("\r\n");

		switch (comando) {
			case 'm':
			_delay_ms(100);
			UART_send_string("\nCambiando el estado del portón...\r\n");
			send_to_slave(SLAVE1, 'm');
			break;
			case 'n':
			UART_send_string("\nCambiando el estado del dia...\r\n");
			send_to_slave(SLAVE2, 'n');
			break;
			case 'o':
			UART_send_string("\nCambiando el estado del ventilador...\r\n");
			ventilador = !ventilador;
			if (ventilador) {
				PORTC |= (1 << PORTC2); // Enciende el LED si el ventilador está encendido
				} else {
				PORTC &= ~(1 << PORTC2); // Apaga el LED si el ventilador está apagado
			}
			break;
			default:
			UART_send_string("\nEl comando ingresado no se reconoce\n");
			break;
		}
		} else {
		//UART_send_string("No data available.\r\n");
	}
}



int main(void) {
	// Inicializa UART con baud rate 9600
	UART_init(9600);
	// Inicialización del LCD
	initLCD8bits();
	// Inicializa I2C a 100 kHz con prescaler 1
	I2C_Master_Init(100000, 1);
	// Configura LED_PIN como salida
	DDRC |= (1 << DDC2);
	sei();

	while (1) {
		//UART_send_string("Hello from ATmega328P!\r\n");
		_delay_ms(500);
		uartCasa();
		sensorTemp();
		portonUltra();
		LCDcasa();
		
		_delay_ms(300); // Espera antes de la siguiente iteración
	}
}