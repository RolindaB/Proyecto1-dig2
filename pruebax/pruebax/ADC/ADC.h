/*
 * ADC.h
 *
 * Created: 20/08/2024 18:26:07
 *  Author: asrol
 */ 


#ifndef ADC_H_
#define ADC_H_

#include <avr/io.h>
#include <stdint.h>
void initADC(void);
uint8_t ADC_CHANEL_SELECT(uint8_t chanel);
#endif /* ADC_H_ */