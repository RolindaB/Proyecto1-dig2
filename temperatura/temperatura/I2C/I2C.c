#include "I2C.h"
#define F_CPU 16000000UL  // Define la frecuencia de tu microcontrolador en Hz
#include <stdint.h>
#include <avr/io.h>

// Configura el bus I2C
void I2C_Master_Init(unsigned long SC_Clock, uint8_t Preescaler) {
	DDRC &= ~((1 << DDC4) | (1 << DDC5)); // Configura los pines I2C como entradas

	switch (Preescaler) {
		case 1:
		TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
		break;
		case 4:
		TWSR &= ~(1 << TWPS1);
		TWSR |= (1 << TWPS0);
		break;
		case 16:
		TWSR &= ~(1 << TWPS0);
		TWSR |= (1 << TWPS1);
		break;
		case 64:
		TWSR |= (1 << TWPS1) | (1 << TWPS0);
		break;
		default:
		// Valor predeterminado para el prescaler
		TWSR &= ~((1 << TWPS1) | (1 << TWPS0));
		break;
	}
	TWBR = ((F_CPU / SC_Clock) - 16) / (2 * (1 << (TWSR & 0x03)));
	TWCR |= (1 << TWEN);
}

// Envía un inicio de comunicación I2C
uint8_t I2C_Master_Start(void) {
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN); // Envía una señal de START
	while (!(TWCR & (1 << TWINT))); // Espera a que el START se complete

	uint8_t estado = TWSR & 0xF8; // Lee el estado del TWI
	if (estado != 0x08 && estado != 0x10) {
		return 1; // Error
	}
	return 0; // Éxito
}

// Envía una parada de comunicación I2C
void I2C_Master_Stop(void) {
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN); // Envía una señal de STOP
	while (TWCR & (1 << TWSTO)); // Espera a que el STOP se complete
}

// Escribe un byte en el bus I2C
uint8_t I2C_Master_Write(uint8_t dato) {
	TWDR = dato; // Carga el dato en el registro de datos
	TWCR = (1 << TWINT) | (1 << TWEN); // Inicia la transmisión
	while (!(TWCR & (1 << TWINT))); // Espera a que la transmisión se complete

	uint8_t estado = TWSR & 0xF8; // Lee el estado del TWI
	if (estado != 0x18 && estado != 0x28 && estado != 0x40) {
		return estado; // Error
	}
	return 0; // Éxito
}

uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack) {
	if (ack) {
		TWCR |= (1 << TWEA); // Configura ACK para la próxima lectura
		} else {
		TWCR &= ~(1 << TWEA); // Desactiva ACK
	}
	TWCR |= (1 << TWINT); // Inicia la lectura
	while (!(TWCR & (1 << TWINT))); // Espera a que el dato sea recibido

	uint8_t estado = TWSR & 0xF8; // Lee el estado del TWI
	if (estado == 0x58 || estado == 0x50) { // Verifica estado de recepción
		*buffer = TWDR; // Lee el dato recibido
		return 0; // Recepción exitosa
		} else {
		return estado; // Retorna el código de error
	}
}

uint8_t read_from_slave(uint8_t direccion_esclavo, uint8_t *dato) {
	// Iniciar condición de START
	TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete

	// Verificar estado de START
	if ((TWSR & 0xF8) != 0x08) return 1; // Error: No se pudo iniciar la condición de START

	// Enviar dirección del esclavo con bit de lectura
	TWDR = (direccion_esclavo << 1) | 1; // SLA + R (bit de lectura)
	TWCR = (1 << TWEN) | (1 << TWINT);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete

	// Verificar estado de la dirección del esclavo
	if ((TWSR & 0xF8) != 0x40) {
		I2C_Master_Stop(); // Detener si se recibe un NACK o estado inesperado
		return 1; // Error: No se pudo recibir ACK del esclavo
	}

	// Leer el dato del esclavo
	TWCR = (1 << TWINT) | (1 << TWEN);
	while (!(TWCR & (1 << TWINT))); // Esperar a que se complete

	// Verificar estado del dato recibido
	if ((TWSR & 0xF8) != 0x58 && (TWSR & 0xF8) != 0x50) {
		I2C_Master_Stop(); // Detener si hay error en la recepción
		return 1; // Error en la recepción
	}

	*dato = TWDR; // Leer el dato recibido
	I2C_Master_Stop(); // Generar condición de STOP
	return 0; // Éxito
}

void send_to_slave(uint8_t slave_address, uint8_t data) {
	// Inicia la comunicación I2C
	I2C_Master_Start();

	// Envía la dirección del esclavo con el bit de escritura
	I2C_Master_Write(slave_address << 1);

	// Envía el dato
	I2C_Master_Write(data);

	// Detiene la comunicación I2C
	I2C_Master_Stop();
}
void send_string_to_slave(uint8_t slave_address, char* str) {
	// Inicia la comunicación I2C
	I2C_Master_Start();

	// Envía la dirección del esclavo con el bit de escritura
	I2C_Master_Write(slave_address << 1);

	// Envía cada carácter de la cadena
	while(*str) {
		I2C_Master_Write(*str++);
	}

	// Detiene la comunicación I2C
	I2C_Master_Stop();
}