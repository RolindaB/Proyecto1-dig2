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
volatile uint8_t debounce_counter = 0;
uint8_t estadoVent =0;
// Prototipos de funciones
uint8_t read_temperature(uint8_t *high_byte, uint8_t *low_byte);
void uartCasa();
void LCDcasa();
void sensorTemp();
void portonUltra();
#define DEBOUNCE_TIME 20 // Tiempo de antirrebote en ms
volatile uint8_t debounce_flags = 0x00;

void init_timer1() {
	// Configura el Timer1 en modo CTC con un prescaler de 64
	TCCR1B |= (1 << WGM12) | (1 << CS11) | (1 << CS10);

	// Configura el valor de comparación para generar interrupciones cada ~1 ms (16 MHz / 64 / 250)
	OCR1A = 250;

	// Habilita la interrupción por comparación de Timer1
	TIMSK1 |= (1 << OCIE1A);
}



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
	LCD_Set_Cursor(13, 2); // Posiciona el cursor en la segunda línea del LCD
	LCD_Write_String("        "); // Limpia la línea
	LCD_Set_Cursor(12, 2); // Posiciona el cursor nuevamente
	LCD_Write_String(buffer2); // Muestra el estado del portón
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
			UART_send_string("\nCambiando el estado del portón...\r\n");
			send_to_slave(SLAVE1, 'm');
		}
		else if (estadoVent == 1 && comando == 'n') {
			UART_send_string("\nCambiando el estado del día...\r\n");
			send_to_slave(SLAVE2, 'n');
		}
		else if (estadoVent == 1 && comando == 'o') {
			UART_send_string("\nCambiando el estado del ventilador...\r\n");
			ventilador = !ventilador;
			if (ventilador) {
				PORTC |= (1 << PORTC2); // Enciende el LED si el ventilador está encendido
				} else {
				PORTC &= ~(1 << PORTC2); // Apaga el LED si el ventilador está apagado
			}
		}
		else if (comando == 'x') {
			// Cambia al estado automático (0), manual (1);
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
	DDRB &= ~(1 << DDB5);
	DDRC &= ~((1 << DDC0) | (1 << DDC1) | (1 << DDC3));

	// Habilitar pull-ups internos para PB5, PC0, PC1, PC3
	PORTB |= (1 << PORTB5);
	PORTC |= (1 << PORTC0) | (1 << PORTC1) | (1 << PORTC3);
}

void initPCint2(void) {
	// Habilitar interrupciones en los botones PB5, PC0, PC1, y PC3
	PCMSK0 |= (1 << PCINT5); // Habilitando PCINT en PB5
	PCMSK1 |= (1 << PCINT8) | (1 << PCINT9) | (1 << PCINT11); // Habilitando PCINT en PC0, PC1, y PC3
	PCICR |= (1 << PCIE0) | (1 << PCIE1); // Habilitando las interrupciones de los pines de PCINT[7:0] y PCINT[14:8]
}

ISR(PCINT0_vect) {
	if (debounce_counter == 0) {
		// Verificar cuál pin causó la interrupción para PB5
		if (!(PINB & (1 << PB5))) {
			debounce_flags |= (1 << 0); // Marca el botón PB5
		}
	}
}

ISR(PCINT1_vect) {
	if (debounce_counter == 0) {
		// Verificar cuál pin causó la interrupción para PC0, PC1, y PC3
		if (!(PINC & (1 << PC0))) {
			debounce_flags |= (1 << 1); // Marca el botón PC0
		}
		if (!(PINC & (1 << PC1))) {
			debounce_flags |= (1 << 2); // Marca el botón PC1
		}
		if (!(PINC & (1 << PC3))) {
			debounce_flags |= (1 << 3); // Marca el botón PC3
		}
	}
}

// Temporizador ISR para manejar el tiempo de antirrebote
ISR(TIMER1_COMPA_vect) {
	if (debounce_counter > 0) {
		debounce_counter--;
		if (debounce_counter == 0) {
			// Ejecutar las acciones asociadas a los botones después de que se haya estabilizado
			if (debounce_flags & (1 << 0)) {
				estadoVent = !estadoVent;
				send_to_slave(SLAVE1,'x');
				send_to_slave(SLAVE2,'x');
				LCD_Write_String("cambiando estado");
				debounce_flags &= ~(1 << 0);
			}
			if (debounce_flags & (1 << 1)) {
				if (estadoVent == 1){
					send_to_slave(SLAVE1, 'm');
					LCD_Write_String("cambiando estado");

				}
				debounce_flags &= ~(1 << 1);
			}
			if (debounce_flags & (1 << 2)) {
				if (estadoVent == 1){
				send_to_slave(SLAVE2, 'n');
				LCD_Write_String("cambiando estado");

				}
				debounce_flags &= ~(1 << 2);
			}
			if (debounce_flags & (1 << 3)) {
				ventilador = !ventilador;
				if (ventilador && estadoVent == 1) {
					PORTC |= (1 << PORTC2); // Enciende el LED si el ventilador está encendido
					} else {
					PORTC &= ~(1 << PORTC2); // Apaga el LED si el ventilador está apagado
				}
				debounce_flags &= ~(1 << 3);
			}
		}
	}
}

int main(void) {
	UART_init(9600);
	initLCD8bits();
	I2C_Master_Init(100000, 1);
	setup();
	initPCint2();
	init_timer1();

	sei();

	while (1) {
		uartCasa();
		sensorTemp();
		portonUltra();
		LCDcasa();
		_delay_ms(300);
	}
}