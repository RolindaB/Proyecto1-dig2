#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include "I2C/I2C.h"   // Incluye librer�a I2C
#include "UART/UART.h" // Incluye librer�a UART
#include "LCD/LCD.h"   // Librer�a LCD

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
volatile uint8_t debounce_counter = 0;
uint8_t estadoVent =0;
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
			strcpy(buffer2, "open");
			} else {
			strcpy(buffer2, "close");
		}
		} else {
		strcpy(buffer2, "error");
	}
}

void diax(){
	uint8_t result = read_from_slave(SLAVE2, &day);
	int day2 = (int)(day);
	if (result == 0) {
		if (day2 == 1) {
			strcpy(buffer1, "on");
			} else {
			strcpy(buffer1, "off");
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

		// Corrige la comparaci�n para verificar si la temperatura es mayor a 20
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

	// Env�a el mensaje por UART a la consola
	//UART_send_string(buffer);
}

uint8_t read_temperature(uint8_t *high_byte, uint8_t *low_byte) {
	// Inicia la comunicaci�n I2C
	if (I2C_Master_Start() != 0) return 1;

	// Env�a la direcci�n del esclavo (LM75) en modo escritura
	if (I2C_Master_Write(LM75_ADDRESS << 1) != 0) return 1;

	// Env�a la direcci�n del registro de temperatura
	if (I2C_Master_Write(0x00) != 0) return 1;

	// Repite la condici�n de START para lectura
	if (I2C_Master_Start() != 0) return 1;

	// Env�a la direcci�n del esclavo (LM75) en modo lectura
	if (I2C_Master_Write((LM75_ADDRESS << 1) | 1) != 0) return 1;

	// Lee el byte alto de la temperatura
	if (I2C_Master_Read(high_byte, 1) != 0) return 1;

	// Lee el byte bajo de la temperatura
	if (I2C_Master_Read(low_byte, 0) != 0) return 1;

	// Detiene la comunicaci�n I2C
	I2C_Master_Stop();

	return 0; // �xito
}

void LCDcasa() {
	// Muestra la temperatura
	snprintf(buffer, sizeof(buffer), "%d C", temp_value / 256);
	LCD_Set_Cursor(1, 2); // Posiciona el cursor en la primera l�nea del LCD
	LCD_Write_String("      "); // Limpia la l�nea
	LCD_Set_Cursor(1, 2); // Posiciona el cursor nuevamente
	LCD_Write_String(buffer); // Muestra el valor
	LCD_Set_Cursor(1, 1); // Posiciona el cursor nuevamente
	LCD_Write_String("temp:"); // Muestra la etiqueta

	// Muestra el estado del port�n
	LCD_Set_Cursor(7, 2); // Posiciona el cursor en la segunda l�nea del LCD
	LCD_Write_String("        "); // Limpia la l�nea
	LCD_Set_Cursor(7, 2); // Posiciona el cursor nuevamente
	LCD_Write_String(buffer2); // Muestra el estado del port�n
	LCD_Set_Cursor(7, 1); // Posiciona el cursor nuevamente
	LCD_Write_String("Gate:"); // Muestra la etiqueta
	
	// Muestra el estado del dia
	LCD_Set_Cursor(13, 2); // Posiciona el cursor en la segunda l�nea del LCD
	LCD_Write_String("        "); // Limpia la l�nea
	LCD_Set_Cursor(13, 2); // Posiciona el cursor nuevamente
	LCD_Write_String(buffer1); // Muestra el estado del port�n
	LCD_Set_Cursor(13, 1); // Posiciona el cursor nuevamente
	LCD_Write_String("Day:"); // Muestra la etiqueta
}

void uartCasa() {
	if (UART_available()) {
		char comando = UART_receive();
		UART_send(comando);
		UART_send_string("\r\n");

		if (estadoVent == 1 && comando == 'm') {
			_delay_ms(100);
			UART_send_string("\nCambiando el estado del port�n...\r\n");
			send_to_slave(SLAVE1, 'm');
		}
		else if (estadoVent == 1 && comando == 'n') {
			UART_send_string("\nCambiando el estado del d�a...\r\n");
			send_to_slave(SLAVE2, 'n');
		}
		else if (estadoVent == 1 && comando == 'o') {
			UART_send_string("\nCambiando el estado del ventilador...\r\n");
			ventilador = !ventilador;
			if (ventilador) {
				PORTC |= (1 << PORTC2); // Enciende el LED si el ventilador est� encendido
				} else {
				PORTC &= ~(1 << PORTC2); // Apaga el LED si el ventilador est� apagado
			}
		}
		else if (comando == 'x') {
			// Cambia al estado autom�tico (0), manual (1);
			estadoVent = !estadoVent;
			send_to_slave(SLAVE1,'x');
			send_to_slave(SLAVE2,'x');
			
		}
		else {
			UART_send_string("\nEl comando ingresado no se reconoce\n");
		}
	}
	else {
		// UART_send_string("No data available.\r\n");
	}
}

/*****************************************************************/
void setup(){
	// Configura PC2 como salida para el ventilador
	DDRC |= (1 << DDC2);
	// Configurar PB5, PC0, PC1, PC3 como entradas
	DDRB &= ~(1 << DDB4);
	DDRC &= ~((1 << DDC0) | (1 << DDC1) | (1 << DDC3));
	// Habilitar pull-ups internos para PB5, PC0, PC1, PC3
	PORTB |= (1 << PORTB4);
	PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC3);
}
void initPCint1(void) {
	// Habilitar interrupciones en los botones PB5, PC0, PC1, y PC3
	
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT11); // Habilitar PCINT en PC0, PC1, y PC3
	PCICR |=  (1 << PCIE1); // Habilitar las interrupciones de los pines de PCINT[7:0] y PCINT[14:8]
}
void initPCint0(void){
	PCMSK0 |= (1 << PCINT4); // Habilitar PCINT en PB5
	PCICR |= (1 << PCIE0) ;
}
ISR(PCINT0_vect) {
	// Verificar cu�l pin caus� la interrupci�n para PB5
	if (!(PINB & (1 << PINB5))) {
			UART_send_string("PB5 presionado.\r\n");
			estadoVent = !estadoVent;
			send_to_slave(SLAVE1, 'x');
			send_to_slave(SLAVE2, 'x');
			//LCD_Write_String("cambiando estado");
	
	}
}

ISR(PCINT1_vect) {
	// Verificar cu�l pin caus� la interrupci�n para PC0, PC1, y PC3
	if (!(PINC & (1 << PINC0))) {
		if (estadoVent == 1) {
			send_to_slave(SLAVE1, 'm');
			//LCD_Write_String("cambiando estado");
			UART_send_string("PC0 presionado.\r\n");
		}
	}
	if (!(PINC & (1 << PINC1))) {
		if (estadoVent == 1) {
			send_to_slave(SLAVE2, 'n');
			//LCD_Write_String("cambiando estado");
			UART_send_string("PC1 presionado.\r\n");
		}
	}
	if (!(PINC & (1 << PINC3))) {
		ventilador = !ventilador;
		if (ventilador && estadoVent == 1) {
			PORTC |= (1 << PORTC2); // Enciende el ventilador si est� activado
			} else {
			PORTC &= ~(1 << PORTC2); // Apaga el ventilador si est� desactivado
		}
		UART_send_string("PC3 presionado.\r\n");
	}
}


int main(void) {
	UART_init(9600);
	initLCD8bits();
	I2C_Master_Init(100000, 1);
	setup();
	initPCint0();
	initPCint1();

	sei();

	while (1) {
		uartCasa();
		sensorTemp();
		portonUltra();
		diax();
		LCDcasa();
		_delay_ms(300);
	}
}