// Definicao da frequencia de operacao
#define F_CPU 16000000UL
// Definicao do baudrate para a comunicacao serial
#define BAUD 9600
// Define sinal logico alto
#define set_bit(reg,bit) (reg |= (1<<bit))
//Define sinal logico baixo
#define reset_bit(reg,bit) (reg &= ~(1<<bit))

// Inclusao das bibliotecas
#include <avr/io.h>
#include <util/setbaud.h>
#include <util/delay.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <string.h>
#include <stdint.h>

// Inclusao das bibliotecas importadas
#include "DEF_ATMEGA328P.h"
#include "UART.h"
#include "LCD.h"
#include "LCD_I2C.h"
#include "DHT22.h"
#include "i2c.h"
#include "def_principais.h"
#include "USART.h"

#include "uart_hal.h"
#include "adc_hal.h"
#include "timer0_hal.h"
#include "twi_hal.h"
#include "config.h"

//Habilita o ADC na porta PC1
void ADC_init(void){
	ADMUX |= (1<<REFS0) | (1<<MUX0);
	ADCSRA |= (1<<ADEN) | (1<<ADSC) | (1<<ADATE) | (1<<ADIE) | (1<<ADPS2) | (1<<ADPS1);
	sei();
}

// Atualiza o ADC
/*ISR(ADC_vect){

}
*/

#define endereco 0x05 //define o endereco de acesso para o modulo RF RX

void USART_Inic(unsigned int ubbr0);
void USART_Transmite(unsigned char dado);

float filtro_media_movel(float medidas[3]){
float somatorio = 0;
for(int i=0;i<3;i++){
somatorio += medidas[i];
}
return somatorio/3;
}

static char print_buffer[64] = {0};

#define RTC_ADDR (0x68)

int main(void)
{
	// Inicializacao do ADC
	ADC_init();

	unsigned char i, dado;
	USART_Inic(MYUBRR); //UCSR0C = (1<<UPM01)|(1<<UCSZ01)|(1<<UCSZ00);
	// Inicializacao da comunicação UART
	UART_init();
  
	const char start[] = "\n\rProgram Start\n\r";
	uint8_t run = 0;
	uint16_t adc = 0;
	uint32_t holder = 0;
	uint32_t rtc_interval = 0;
	uint8_t err = 0;
	
	DDRD &= 0xF0;
	DDRB &= 0x0F;
//	uart_init(9600,0);
//	adc_init();
	//adc_pin_enable(ADC3_PIN);
	//adc_pin_select(ADC3_PIN);
	timer0_init();
	
	twi_init(100000); //100khz
	
	uint8_t rtc_data[7] = {0x50,0x59,0x23,0x07,0x31,0x10,0x20};
	
	//sei();

	printf(start);
	
	adc = adc_convert();
	holder = millis();
	
	
/*	err = twi_wire(RTC_ADDR,0x00,rtc_data,sizeof(rtc_data));
	if(err != TWI_OK){
		memset(print_buffer,0,sizeof(print_buffer));
		sprintf(print_buffer,"%d error %d\r\n\n",__LINE__,err);
		uart_send_string((uint8_t*)print_buffer);
		while(1);
	}*/
	
	
	rtc_interval = millis();
	//////////////////////////////////////////////////////// fim
	// Buffer de caracteres
	char printbuff[10];
	// Variavel referente a temperatura
	float temperatura,temperatura_VEC[3],temperatura_media;
	// Variavel referente a umidade
	float umidade,umidade_VEC[3],umidade_media;
	// Variaveis referentes a luminosidade
	float Rldr = 0;
	float lux = 0, lux_VEC[3],lux_media;
	int leitura = 0;

	// Contador associado a leitura
	uint8_t contador = 200;
	
	// Inicializacao do I2C
	i2c_init();
	i2c_start();
	i2c_write(0x70);
	// Inicializacao do display LCD
	LCD_init();
	// Inicializacao do sensor DHT22
	DHT22_init();

	
	while (1)
	{
	if(millis_end(rtc_interval,500)){
		
		err = twi_read(RTC_ADDR,0x00,rtc_data,sizeof(rtc_data));
	/*	if(err != TWI_OK){
			memset(print_buffer,0,sizeof(print_buffer));
			sprintf(print_buffer,"%d error %d\r\n\n",__LINE__,err);
			//uart_send_string((uint8_t*)print_buffer);
			while(1);
		}*/

	    /*for(i=0; i<16;i++) //envia 16 dados diferentes (são 4 MSB)
		{
			dado = (i << 4 ) | endereco; //4 LSB são o endereço do módulo RF RX
			USART_Transmite(dado); 

			_delay_ms(500);//envia um novo dado a cada 0,5 s
		}*/
		
		memset(print_buffer,0,sizeof(print_buffer));
		sprintf(print_buffer,"20%02x/%02x/%02x %02x:%02x:%02x :",
		rtc_data[6],
		rtc_data[5],
		rtc_data[4],
		rtc_data[2],
		rtc_data[1],
		rtc_data[0]
		);
//		printf(print_buffer);
		
		
		rtc_interval = millis();
	}

	/*if(millis_end(holder,adc)){
		run++;
		PORTD &= 0x0F;
		PORTB &= 0xF0;
		PORTD |= ((run & 0x0F) << 4);
		PORTB |= ((run & 0xF0) >> 4);
		adc = adc_convert();
		holder = millis();
	}*/
	//////////////////////////////////////////////

		contador++;
		//Leitura dos sensores a cada 200x10ms = 2000ms
		if(contador >= 200){
			contador = 0;
			
			// A variavel recebe o status atual do sensor DHT22
			uint8_t status = DHT22_read(&temperatura, &umidade);
			// Variaveis referentes ao LDR
			Rldr = ((ADC*5.0/1023.0)*10000.0)/5.0-(ADC*5.0/1023.0); //Calcula a resistencia do LDR com base no valor do ADC
			lux = 400000.0*0.73/Rldr; //Uma aproximacao para o valor em lux

			temperatura_VEC[leitura] = temperatura;
			umidade_VEC[leitura] = umidade;
			lux_VEC[leitura] = lux;
			
			// Verifica o status de funcionamento do sensor DHT22 e atualiza os valores das variaveis associadas aos sensores
			if (status)
			{				                
				
			//	LCD_cmd(0x80);
				LCD_printf("Temp | Umd | LDR");
				LCD_segunda_linha();

				dtostrf(temperatura, 2, 1, printbuff);
				LCD_printf(printbuff);
				LCD_printf("C ");

				dtostrf(umidade, 2, 1, printbuff);
				LCD_printf(printbuff);
				LCD_printf("%  ");

				dtostrf(lux, 2, 1, printbuff);
				LCD_printf(printbuff);
			}
			
			// Indica que houve erro na leitura do sensor
			else
			{
				LCD_clear();
				LCD_printf("Error");
				printf("ERROR\n\r");
			}

			leitura++;
			if(leitura==3){
			temperatura_media = filtro_media_movel(temperatura_VEC);
			umidade_media = filtro_media_movel(umidade_VEC);
			lux_media = filtro_media_movel(lux_VEC);
			    printf("Media movel das medidas na data ");
			    printf(print_buffer);
			    printf("\n\r");
			    printf("Temperatura = %2.3f C\n\r", temperatura_media);
			    printf("Umidade = %2.2f%% \n\r", umidade_media);
			    printf("LDR = %2.2f lux\n\r", lux_media);
			    printf("\n\r");
				leitura=0;
			}

		}
		
		// Tempo de espera
		else{
			_delay_ms(10);
		}
	}
	}