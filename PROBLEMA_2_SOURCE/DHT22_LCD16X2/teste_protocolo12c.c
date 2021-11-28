/*
 * teste_protocolo12c.c
 *
 * Created: 26/09/2021 15:08:54
 *  Author: rafae
 */ 

#include <avr/io.h>
#include "LCD_I2C.h"
#include "i2c.h"
#include <util/delay.h>

int main(void) {
	i2c_init();
	i2c_start();
	i2c_write(0x70);
	lcd_init();
	
	while(1) {
		lcd_cmd(0x80);
		lcd_msg("Teste");
		lcd_cmd(0xC3);
		lcd_msg("I2C");
	}
		 
}