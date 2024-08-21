/*
 * PWM.h
 *
 * Created: 16/08/2024 11:14:32
 *  Author: asrol
 */ 


#ifndef PWM_H_
#define PWM_H_

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

#define invertido 1
#define no_invertido 0

void resetPWM0(void);

float map0(float duty, float Imin, float Imax, float Omin, float Omax);

//Función para configurar PWM1 Modo Fast, Canal A
void initPWM0FastA(uint8_t inverted, uint16_t prescaler);

//Función para configurar PWM1 Modo Fast, Canal B
void initPWM0FastB(uint8_t inverted, uint16_t prescaler);

void updateDutyCycleA0(uint8_t duty);

void updateDutyCycleB0(uint8_t duty);



#endif /* PWM_H_ */