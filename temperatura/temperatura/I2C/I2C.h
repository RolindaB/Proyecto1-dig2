/*
 * I2C.h
 *
 * Created: 13/08/2024 08:27:33
 *  Author: asrol
 */ 


#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>

// Estados del protocolo TWI
#define TWI_START_SUCCESS     0x08   // Condición de START transmitida
#define TWI_REPEATED_START    0x10   // Condición de REPEATED START transmitida
#define TWI_MT_SLA_ACK        0x18   // Dirección de esclavo con ACK recibido
#define TWI_MT_SLA_NACK       0x20   // Dirección de esclavo con NACK recibido
#define TWI_MT_DATA_ACK       0x28   // Datos transmitidos con ACK recibido
#define TWI_MT_DATA_NACK      0x30   // Datos transmitidos con NACK recibido
#define TWI_MR_SLA_ACK        0x40   // Dirección de esclavo con ACK recibido (modo lectura)
#define TWI_MR_SLA_NACK       0x48   // Dirección de esclavo con NACK recibido (modo lectura)
#define TWI_MR_DATA_ACK       0x50   // Datos recibidos con ACK enviado
#define TWI_MR_DATA_NACK      0x58   // Datos recibidos con NACK enviado

// Inicializa el bus I2C
void I2C_Master_Init(unsigned long SC_Clock, uint8_t Preescaler);
// Envía un inicio de comunicación I2C
uint8_t I2C_Master_Start(void);
// Envía una parada de comunicación I2C
void I2C_Master_Stop(void);
// Escribe un byte en el bus I2C
uint8_t I2C_Master_Write(uint8_t dato);
// Lee un byte del bus I2C con ACK
uint8_t I2C_Master_Read(uint8_t *buffer, uint8_t ack);
// Lee un solo byte del esclavo
uint8_t read_from_slave(uint8_t direccion_esclavo, uint8_t *dato);


#endif /* I2C_H_ */