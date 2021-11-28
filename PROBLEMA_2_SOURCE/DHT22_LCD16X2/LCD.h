
//////////////////////////////////////////////////////////////////////////
////  Mombre :      LCD.h                                             ////
////  Creación :    04/11/2018                                        ////
////  Descripción:  Configurable con cualquier PIN                    ////
////                Para familia mejorada LAT                         ////
//////////////////////////////////////////////////////////////////////////

//#ifndef LCD_H
//#define LCD_H


//#include "DEF_ATMEGA328P.h"
#include <stdlib.h>	// mostrar inteiros
#include <stdint.h>	// usar tipo uint


//******** Pines para los datos del LCD *********

#define D4			PORTDbits.PD5_          // pin 11 lcd
#define D5          PORTDbits.PD6_          // pin 12 lcd
#define D6          PORTDbits.PD7_          // pin 13 lcd
#define D7          PORTBbits.PB0_          // pin 14 lcd

#define DDR_D4     DDRDbits.DDRD5			//D4 Out
#define DDR_D5     DDRDbits.DDRD6			//D5 Out
#define DDR_D6     DDRDbits.DDRD7			//D6 Out
#define DDR_D7     DDRBbits.DDRB0			//D7 Out

//******** Pines para el control del LCD **********


#define RS         PORTDbits.PD3_          //Pin 4 LCD
#define EN          PORTDbits.PD4_          //Pin 6 LCD
#define DDR_RS		DDRDbits.DDRD3        //RS Out
#define DDR_EN		DDRDbits.DDRD4       //EN Out

#define ON  1
#define OFF 0

/******** Comandos para LCD *******/

#define LCD_CLEAR       0X01    //Limpar a tela
#define LCD_CURSOR_ON   0X0F    //Cursor ON
#define LCD_CURSOR_OFF  0X0C    //Cursor off
#define LCD_HOME		0X02	//Primeira linha
#define LCD_LINHA2      0XC0    //Segunda linha
#define LCD_LINHA3      0x94    //Terceira linha
#define LCD_LINHA4      0XD4    //Quarta linha
#define LCD_LEFT        0X10    //Move cursor esquerda
#define LCD_RIGHT       0X14    //Move cursor direita

#define ROT_LEFT        0X18    //Rotacionar esquerda
#define ROT_RIGHT       0X1C    //Rotacionar direita

/************ Prototipos de funções *************/

void LCD_init(void);                    	//Inicializa LCD
void LCD_write_nible(unsigned char nible);
void LCD_cmd(unsigned char);                //Envia comando
void LCD_data(char);						//Envia dado
void LCD_write(unsigned char data);         //Rotina para enviar dados
void LCD_printf(char*);						//Visualizar string de caracteres
void LCD_goto(uint8_t, uint8_t);            //Mover o cursor para um ponto XY
void LCD_cursor(unsigned char);             //Ativa ou desativa cursor
void LCD_segunda_linha(void);
void LCD_clear(void);
void LCD_home(void);
void LCD_custom_char(unsigned char loc,unsigned char *msg);


/***********************************************
 ------- Inicializa LCD 4 bits de dados -------
 ***********************************************/

void LCD_init(void)
{   
	 EN = 0;                     //Limpamos pinos de Controle
	 RS = 0;
	 D4 = 0;                     //Limpamos pinos de Dados
	 D5 = 0;
	 D6 = 0;
	 D7 = 0;

	 DDR_EN = 1;                //Pinos de controle como saída
	 DDR_RS = 1;
	 DDR_D4 = 1;                //Pinos de dados como saída
	 DDR_D5 = 1;
	 DDR_D6 = 1;
	 DDR_D7 = 1;

	 _delay_ms(20);             //Espera que as portas se estabilizem

	 LCD_write_nible(0x03);      //por fabricante
	 _delay_ms(5);
	 LCD_write_nible(0x03);      //por fabricante
	 _delay_us(100);
	 LCD_write_nible(0x03);      //por fabricante
	 _delay_us(100);
	 LCD_write_nible(0x02);      //Initial function set to change interface,
	 _delay_us(100);            //Configurar modo 4 bits

	 LCD_cmd(0x28);				//LCD com fonte de 5*7
	 LCD_cmd(LCD_CURSOR_OFF);    //Cursor apagado
	 LCD_cmd(LCD_CLEAR);
	 LCD_cmd(0x06);              //Modo incremental, sem deslocamento
	 LCD_cmd(0x80);              //Endereço DDRam superior esquerda
}

/***********************************************
 ---- Rotina para enviar dados e comandos ------
 ***********************************************/

void LCD_write_nible(unsigned char nible)
{  
    if (nible & (1<<0)) D4 = 1; else D4 = 0;             
    if (nible & (1<<1)) D5 = 1; else D5 = 0;
    if (nible & (1<<2)) D6 = 1; else D6 = 0;
    if (nible & (1<<3)) D7 = 1; else D7 = 0;
    
    EN = 1;     
    _delay_us(1);	//Enable pulse width PWeh min 230ns
    EN = 0; 
}
/***********************************************
 -------- Envio de comandos para o LCD ---------
 ***********************************************/

void LCD_cmd(uint8_t data)
{
    RS = 0; 
	asm("nop");
    LCD_write_nible(data>>4);
    LCD_write_nible(data & 0x0F);
	if (data == LCD_CLEAR || data == LCD_HOME)
		_delay_ms(2);		//tempo mínimo 1.7ms
	else
		_delay_us(50);		//tempo min 40 us
}

/***********************************************
 --------- Exibir dados no visor LCD -----------
 **********************************************/

void LCD_data(char data)
{
	RS = 1;
	asm("nop");	
    LCD_write_nible(data>>4);
    LCD_write_nible(data & 0x0F);
	_delay_us(50);			//tempo mínimo 40us
}


/***********************************************
------------ Exibir string no LCD -------------
***********************************************/

void LCD_printf(char *data) {
    while (*data){
        LCD_data(*data);    // Envio os dados para o LCD
        data++;             // Incrementa o buffer de dados
    }
}

/***********************************************
 ----- Move o cursor para uma posição x y ------
***********************************************/

void LCD_goto(uint8_t x, uint8_t y){
    uint8_t posicao;
    switch (y) {
        case 1: posicao = 0x00 + x - 1; break;
        case 2: posicao = 0x40 + x - 1; break;
        case 3: posicao = 0x14 + x - 1; break;
        case 4: posicao = 0x54 + x - 1; break;
        default: posicao = 0x00 + x - 1; break;
    }
    LCD_cmd(0x80 | posicao);
}

/***********************************************
 --------- Ativa ou desativa cursor ----------
 ***********************************************/

void LCD_cursor(unsigned char cursor){
    if(cursor) 
        LCD_cmd(0x0F);
    else 
        LCD_cmd(0x0C);
}

/***********************************************
 --------- Limpa toda a tela LCD ---------
 ***********************************************/

void  LCD_segunda_linha(){
    LCD_cmd(0xC0);
}

/***********************************************
 --------- Limpa toda a tela LCD ---------
 ***********************************************/

void LCD_clear(void) {
    LCD_cmd(0x01);
    _delay_ms(3);
}

/***********************************************
 ---------- Cursor retorna ao inicio ----------
 ***********************************************/

void LCD_home(void) {
   LCD_cmd(0x02);
   _delay_ms(2);
}

/***********************************************
 --------- Para personalizar caracter ----------
 ***********************************************/

void LCD_custom_char(unsigned char loc,unsigned char *msg){
    
    unsigned char i;
    if(loc<8){
        LCD_cmd(0x40+(loc*8));  /* Command 0x40 and onwards forces the device to point CGRAM address */
        for(i=0;i<8;i++)  /* Write 8 byte for generation of 1 character */
            LCD_data(msg[i]);
    }   
}









