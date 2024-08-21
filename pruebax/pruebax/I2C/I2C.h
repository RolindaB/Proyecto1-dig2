/*
 * I2C.h
 *
 * Created: 20/08/2024 18:25:46
 *  Author: asrol
 */ 


#ifndef I2C_H_
#define I2C_H_
#include <stdint.h>

#define TW_SR_SLA_ACK     0x60  // Dirección del esclavo recibida con ACK
#define TW_SR_DATA_ACK    0x80  // Datos recibidos con ACK
#define TW_SR_STOP        0xA0  // Condición de STOP recibida
#define TW_ST_SLA_ACK     0xA8  // Dirección del esclavo para transmisión recibida con ACK
#define TW_ST_DATA_ACK    0xB8  // Datos enviados con ACK
#define TW_ST_LAST_DATA   0xC0  // Último dato enviado con ACK
// Inicializa el módulo I2C en modo esclavo
void I2C_Slave_Init(uint8_t adress);
#endif /* I2C_H_ */