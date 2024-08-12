#include <avr/io.h>
#include <util/delay.h>
#include "UART/UART.h"

#define TRIG_PIN PD2
#define ECHO_PIN PD3
#define OUT_PIN PD4 // No se usa en el código proporcionado
#define LED_PIN PC4 // Asumiendo que el LED está conectado al pin PC4

void init_ultrasonic() {
	// Configura TRIG_PIN como salida
	DDRD |= (1 << TRIG_PIN);
	// Configura LED_PIN como salida
	DDRC |= (1 << LED_PIN);
	// Configura ECHO_PIN y OUT_PIN como entrada
	DDRD &= ~(1 << ECHO_PIN);
	DDRD &= ~(1 << OUT_PIN);
	// Asegúrate de que el TRIG_PIN está en estado bajo al inicio
	PORTD &= ~(1 << TRIG_PIN);
}

void send_pulse() {
	// Enviar un pulso de 10 microsegundos
	PORTD &= ~(1 << TRIG_PIN);
	_delay_us(2);
	PORTD |= (1 << TRIG_PIN);
	_delay_us(10);
	PORTD &= ~(1 << TRIG_PIN);
}

uint16_t measure_distance() {
	uint16_t duration = 0;
	uint32_t count = 0;
	// Enviar pulso
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
	// Inicializar puertos
	init_ultrasonic();
	UART_init(9600); // Inicializa UART a 9600 baudios
}

void loop() {
	uint16_t duration;
	int distance;
	char distance_str[28]; // Asegúrate de que el buffer es lo suficientemente grande

	// Medir distancia
	duration = measure_distance();
	if (duration == 0) {
		//UART_send_string("Medición fallida\n\r");
		PORTC &= ~(1 << LED_PIN); // Apagar el LED si la medición falla
		} else {
		distance = (((duration * 0.034) / 2.0) * 100); // Convertir a centímetros

		// Convertir la distancia a una cadena con dos decimales
		snprintf(distance_str, sizeof(distance_str), "\n\rDistancia: %d.%02d cm \n\r", distance / 100, distance % 100);
		// Enviar el valor de la distancia a través de UART
		UART_send_string(distance_str);

		// Usar la duración para encender o apagar el LED
		if (duration <= 355) {
			PORTC |= (1 << LED_PIN); // Enciende el LED
			UART_send_string("\n\rLED Encendido\n\r");
			_delay_ms(1000); // Mantener el LED encendido por 1 segundo
			} else {
			PORTC &= ~(1 << LED_PIN); // Apaga el LED
		}
	}
	_delay_ms(1000); // Espera antes de la siguiente medición
}

int main(void) {
	setup();
	while (1) {
		loop();
	}
}
