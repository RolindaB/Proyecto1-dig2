#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "UART/UART.h"
#include "PWM/PWM.h"
#include "I2C/I2C.h"

// Variables
#define TRIG_PIN PORTD2
#define ECHO_PIN PORTD3
#define OUT_PIN PORTD4 // No se usa en el código proporcionado
#define LED_PIN PORTC3 // Asumiendo que el LED está conectado al pin PC3
volatile uint8_t dato = 0; // Variable global para almacenar datos recibidos
volatile uint8_t Abierto, estado = 0; // Estado inicial del servo cerrado
#define SLAVE_ADDR 0x02  // Dirección del esclavo

void init_ultrasonic() {
	DDRD |= (1 << TRIG_PIN);  // Configura TRIG_PIN como salida
	DDRC |= (1 << LED_PIN);   // Configura LED_PIN como salida
	DDRD &= ~(1 << ECHO_PIN); // Configura ECHO_PIN como entrada
	PORTD &= ~(1 << TRIG_PIN);// Asegúrate de que el TRIG_PIN está en estado bajo al inicio
}

void send_pulse() {
	PORTD &= ~(1 << TRIG_PIN);
	_delay_us(2);
	PORTD |= (1 << TRIG_PIN);
	_delay_us(10);
	PORTD &= ~(1 << TRIG_PIN);
}

uint16_t measure_distance() {
	uint16_t duration = 0;
	uint32_t count = 0;
	send_pulse();

	// Esperar a que el pin ECHO esté alto
	while (!(PIND & (1 << ECHO_PIN)) && count < 30000) {
		_delay_us(1);
		count++;
	}
	if (count >= 30000) {
		return 0; // Retorna 0 si el pin ECHO no se volvió alto
	}

	// Medir duración del pulso
	count = 0;
	while (PIND & (1 << ECHO_PIN) && count < 30000) {
		count++;
		_delay_us(1);
	}
	if (count >= 30000) {
		return 0; // Retorna 0 si el pin ECHO no se volvió bajo
	}

	duration = count;
	return duration;
}

void setup() {
	init_ultrasonic();
	UART_init(9600);  // Inicializa UART a 9600 baudios
	resetPWM0();
	initPWM0FastB(no_invertido, 1024);
	sei();  // Habilitar interrupciones
}

/**********************COMUNICACION I2C*********************************/
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
		TWDR = Abierto;  // Carga el valor en el registro de datos
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

void loop() {
	static uint16_t previous_duration = 0;
	uint16_t duration;
	char duration_str[28];

	// Medir la duración del pulso
	duration = measure_distance();
	if (duration == 0) {
		PORTC &= ~(1 << LED_PIN); // Apagar el LED si la medición falla
		updateDutyCycleB0(25); // Mover el servo a una posición de seguridad cerrada
		} else {
		// Filtrado simple: Tomar el promedio de la medición actual y la anterior
		duration = (duration + previous_duration) / 2;
		previous_duration = duration;

		// Convertir la duración a una cadena para mostrar en la UART
		snprintf(duration_str, sizeof(duration_str), "\n\rDuration: %d \n\r", duration);
		UART_send_string(duration_str);

		// Ajustar umbrales según la duración filtrada
		if (duration <= 58) {
			Abierto = 1;
			PORTC |= (1 << LED_PIN); // Enciende el LED
			updateDutyCycleB0(150);  // Mover servo al estado abierto
			_delay_ms(3000);
			UART_send_string("\n\rLED Encendido y Servo Abierto\n\r");
			} else if (duration > 68) {
			Abierto = 0;
			PORTC &= ~(1 << LED_PIN); // Apaga el LED
			updateDutyCycleB0(25);    // Mover servo al estado cerrado
			UART_send_string("\n\rLED Apagado y Servo Cerrado\n\r");
		}
	}

	_delay_ms(100); // Espera antes de la siguiente medición
}

int main(void) {
	setup();
	I2C_Slave_Init(SLAVE_ADDR); // Inicializa el esclavo I2C
	sei(); // Habilitar interrupciones
	//updateDutyCycleB0(0);
	while (1) {
		if (dato == 'x')
		{
			estado = !estado;
			dato =0;
		}
		if (estado == 1 && dato == 'm') {
			// Cambiar el estado de Abierto
			Abierto = !Abierto;  // Cambia entre 0 y 1
			if (Abierto == 1) {
				PORTC |= (1 << LED_PIN); // Enciende el LED
				updateDutyCycleB0(150);  // Mover servo al estado abierto
				UART_send_string("\n\rServo Abierto\n\r");
				} else {
				PORTC &= ~(1 << LED_PIN); // Apaga el LED
				updateDutyCycleB0(40);    // Mover servo al estado cerrado
				UART_send_string("\n\rServo Cerrado\n\r");
			}
			// Limpiar la variable 'dato' para evitar cambios no deseados
			dato = 0;
		 }else if(estado == 0){
			loop();
		}
	}
}
