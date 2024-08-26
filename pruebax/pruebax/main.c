// LIBRERIAS
#include <avr/io.h>
#define F_CPU 16000000
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>

#include "ADC/ADC.h"
#include "I2C/I2C.h"
#define  slave2 0x04

// VARIABLES
uint8_t valorADC = 0;
uint8_t Abierto = 0;
uint8_t day, adentro;
volatile uint8_t dato, estado = 0; // Variable global para almacenar datos recibidos
volatile uint8_t debounce_flag = 0;

void daySensor(void);
void initPCint0(void);
void initTimer0(void);

int main(void) {
	cli();
	DDRB |= (1 << DDB5); // COLOCAR LED DEL PB5 COMO SALIDA
	DDRD |= (1 << DDD4) | (1 << DDD3); // COLOCAR LED COMO SALIDA
	// BOTONES
	DDRB &= ~(1 << DDB4); // PINES B4 COMO ENTRADA
	PORTB |= (1 << PORTB4); // PULLUPS
	initADC();
	initPCint0();
	initTimer0();
	I2C_Slave_Init(slave2);
	sei();
	
	while (1) {
		if (dato == 'x') {
			estado = !estado;
			dato = 0;
		}
		if (estado == 1 && dato == 'n') {
			// Cambiar el estado de Abierto
			day = !day;  // Cambia entre 0 y 1
			if (day == 1) {
				PORTD |= (1 << PORTD4);
				} else {
				PORTD &= ~(1 << PORTD4);
			}
			// Limpiar la variable 'dato' para evitar cambios no deseados
			dato = 0;
			} else if (estado == 0) {
			daySensor();
		}
		if (dato == 'p') {
			adentro = !adentro;
			if (adentro == 1) {
				PORTD |= (1 << PORTD3);
				} else if (adentro == 0) {
				PORTD &= ~(1 << PORTD3);
			}
		}
		dato = 0;
	}
}

void daySensor(void) {
	//_delay_ms(50);
	valorADC = ADC_CHANEL_SELECT(0);
	
	if (valorADC <= 70) {
		PORTD |= (1 << PORTD4);
		day = 0;
		} else if (valorADC > 70) {
		PORTD &= ~(1 << PORTD4);
		day = 1;
	}
}

void initPCint0(void) {
	PCMSK0 |= (1 << PCINT4); // INTERRUPCIONES EN EL BOTON PB4
	PCICR |= (1 << PCIE0); // HABILITANDO LA ISR PCINT[7:0]
}

void initTimer0(void) {
	TCCR0A = 0; // Modo normal
	TCCR0B |= (1 << CS02) | (1 << CS00); // Prescaler de 1024
	TIMSK0 |= (1 << TOIE0); // Habilitar interrupción por desbordamiento
}

ISR(PCINT0_vect) {
	if (debounce_flag == 0) {
		TCNT0 = 0; // Reinicia el contador
		debounce_flag = 1; // Activa el flag de debounce
	}
}

ISR(TIMER0_OVF_vect) {
	if (debounce_flag == 1) {
		debounce_flag = 0; // Desactiva el flag de debounce
		if ((PINB & (1 << PINB4)) == 0) { // Si el botón sigue presionado
			adentro = !adentro;
			if (adentro == 1) {
				PORTD |= (1 << PORTD3);
				} else {
				PORTD &= ~(1 << PORTD3);
			}
		}
	}
}

/*******COMUNICACION I2C************/
// Rutina de interrupción del TWI (I2C)
ISR(TWI_vect) {
	uint8_t estado = TWSR & 0xF8;  // Lee los 3 bits superiores del registro de estado

	switch (estado) {
		case 0x60: // SLA+W recibido, ACK enviado
		case 0x70: // SLA+W recibido en modo general, ACK enviado
		TWCR |= (1 << TWINT); // Borra el flag TWINT para continuar
		break;

		case 0x80: // Datos recibidos, ACK enviado
		case 0x90: // Datos recibidos en llamada general, ACK enviado
		dato = TWDR;  // Lee el dato recibido del registro de datos
		TWCR |= (1 << TWINT); // Borra el flag TWINT para continuar
		break;

		case 0xA8: // SLA+R recibido, ACK enviado
		case 0xB8: // Dato transmitido y ACK recibido
		TWDR = day;  // Carga el valor en el registro de datos
		TWCR |= (1 << TWINT) | (1 << TWEA); // Borra el flag TWINT y habilita ACK para el próximo byte
		break;

		case 0xC0: // Dato transmitido y NACK recibido
		case 0xC8: // Último dato transmitido y ACK recibido
		TWCR |= (1 << TWINT) | (1 << TWEA); // Borra el flag TWINT y habilita ACK para el próximo byte
		break;

		default: // Manejo de errores
		TWCR |= (1 << TWINT) | (1 << TWSTO); // Borra el flag TWINT y envía una condición de STOP
		break;
	}
}