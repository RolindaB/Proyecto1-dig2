/*
 * LCD.h
 *
 * Created: 18/08/2024 23:33:32
 *  Author: asrol
 */ 


#ifndef LCD_H_
#define LCD_H_

#include <util/delay.h>
#include <stdint.h> // Para uint8_t

//funcion para inicializar LCD en  8 bits
void initLCD8bits(void);
// funcion para colocar en el puerto un valor
void LCD_port(char a);
//función pata enviar un comando
void LCD_CMD(char a);
// Función para enviar una caracter
void LCD_WriteCaracter(char c);
//función para enviar una cadena
void LCD_Write_String(char * a);
//desplazamiento hacia la derecha
void Lcd_Shift_Right(void);
//desplazamiento hacia la izquierda
void Lcd_Shift_Left(void);
//Establecer cursor
void LCD_Set_Cursor(char c,char f );
// Mover texto a la izquierda
void LCD_Move_Text_Left(char *text, int delay_msx);
// Mover texto a la derecha
void LCD_Move_Text_Right(char *text, int delay_msx);
void delay_ms(unsigned int ms);



#endif /* LCD_H_ */