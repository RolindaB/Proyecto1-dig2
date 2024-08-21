#include <avr/io.h>
#include <stdint.h>
#include "PWM.h"

void resetPWM0(void){
	TCCR0A = 0;								//RESET
	TCCR0B = 0;								//RESET
}

float map0(float duty, float Imin, float Imax, float Omin, float Omax){
	return ((duty - Imin)*(Omax - Omin)/(Imax - Imin)) + Omin;
}

void initPWM0FastA(uint8_t inverted, uint16_t prescaler){
	DDRD |= (1<<DDD6);						//PIN D6 COMO SALIDA
	if(inverted){
		TCCR0A |= (1<<COM0A1)|(1<<COM0A0);	//OC0A INVERTIDO
		}else{
		TCCR0A |= (1<<COM0A1);				//OC0A NO INVERTIDO
	}
	
	TCCR0A |= ((1<<WGM00)|(1<<WGM01));		//MODO PWM FAST, 8 bits
	if(prescaler==1024){
		TCCR0B |= (1<<CS02)|(1<<CS00);//PRESCALER DE 1024
	}
}

void initPWM0FastB(uint8_t inverted, uint16_t prescaler){
	DDRD |= (1<<DDD5);						//PIN D3 COMO SALIDA
	if(inverted){
		TCCR0A |= (1<<COM0B1)|(1<<COM0B0);	//OC0B INVERTIDO
		}else{
		TCCR0A |= (1<<COM0B1);				//OC0B NO INVERTIDO
		
	}
	
	TCCR0A |= ((1<<WGM00)|(1<<WGM01));		//MODO PWM FAST, 8 bits
	if(prescaler==1024){
		TCCR0B |= (1<<CS02)|(1<<CS00);			//PRESCALER DE 1024
	}
}

void updateDutyCycleA0(uint8_t duty){
	duty = map0(duty, 0, 255, 6, 23);
	OCR0A = duty;
	
}
void updateDutyCycleB0(uint8_t duty){
	duty = map0(duty, 0, 255, 6, 36);
	OCR0B = duty;
	
}