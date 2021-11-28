// Definições dos pinos e dos registradores

#define DHT_DDR DDRD
#define DHT_PORT PORTD
#define DHT_PIN	PIND
#define PIN 2


// Função que realiza a inicialização do sensor DHT22
void DHT22_init(void)
{
	DHT_DDR |= (1<<PIN);		// Configuração do pino como saída
	DHT_PORT |= (1<<PIN);		// Configuração do pino para nível alto
}				

// Função que realiza a leitura do sensor DHT22
uint8_t DHT22_read(float *dht_temperatura, float *dht_umidade)
{
	uint8_t bits[5];			// Array para armazenar os 5 bytes correspondentes aos dados
	uint8_t i,j=0;
	uint8_t contador = 0;
	
	//Envio de um pulso em nível baixo durante 18ms 
	
	DHT_PORT &= ~(1<<PIN);		// Configuração para nível baixo
	_delay_ms(18);
	DHT_PORT |= (1<<PIN);		// Configuração para nível alto
	DHT_DDR &= ~(1<<PIN);		// Configuração do pino como entrada
	
	//Espera de 20 a 40us até que o sensor DHT22 envie o valor 0
	contador = 0;					// Contador para verificar o funcionamento do sensor DHT22
	
	// Se o sensor DHT22 estiver com defeito, esse tempo de espera será superior a 60us
	while(DHT_PIN & (1<<PIN))		// Enquanto o pino estiver em alto
	{
		_delay_us(2);				// Ocorre um tempo de espera de 2us
		contador += 2;				// O contador é incrementado conforme o tempo de espera
		
		// Verifica se o sensor DHT22 está funcionando
		if (contador > 60)
		{
			DHT_DDR |= (1<<PIN);	// Configuração do pino como saída
			DHT_PORT |= (1<<PIN);	// Configuração para nível alto
			return 0;				// Retorna 0 caso o sensor DHT22 não esteja funcionando
		}	
	}	
	
	// Espera de 80us até que o sensor DHT22 envie 1
	contador = 0;					// O contador que verifica o funcionamento do sensor DHT22
	while(!(DHT_PIN & (1<<PIN)))	//Enquanto o sensor não enviar o 1
	{
		_delay_us(2);				// Ocorre um tempo de espera de 2us
		contador += 2;				// O contador é incrementado conforme o tempo de espera
		
		// Verifica se o sensor DHT22 está funcionando
		if (contador > 100) 
		{
			DHT_DDR |= (1<<PIN);	// Configuração do pino como saída
			DHT_PORT |= (1<<PIN);	// Configuração para nível alto
			return 0;				// Retorna 0 caso o sensor DHT22 não esteja funcionando
		}	
	}
	
	// Espera de 80us até que o sensor DHT22 envie 0
	contador = 0;					// O contador que verifica o funcionamento do sensor DHT22
	while(DHT_PIN & (1<<PIN))		//Enquanto o sensor não enviar o 0
	{
		_delay_us(2);				// Ocorre um tempo de espera de 2us
		contador += 2;				// O contador é incrementado conforme o tempo de espera
		
		// Verifica se o sensor DHT22 está funcionando
		if (contador > 100)
		{
			DHT_DDR |= (1<<PIN);	// Configuração do pino como saída
			DHT_PORT |= (1<<PIN);	// Configuração para nível alto
			return 0;				// Retorna 0 caso o sensor DHT22 não esteja funcionando
		}
	}
	
	// Ocorre a leitura dos 5 bytes
	for (j = 0; j < 5; j++)
	{
	uint8_t result = 0;						// Variável que armazena os valores resultantes
		for (i = 0; i < 8; i++)
		{
			while (!(DHT_PIN & (1<<PIN)));	// Enquanto o PIND2 não estiver em 1
				_delay_us(35);				// Tempo de espera de 35us 

			if (DHT_PIN & (1<<PIN))			// Se o PIND2 estiver em 1
				result |= (1<<(7-i));		// O pino 7-i é colocado em 1  
					
			while(DHT_PIN & (1<<PIN));		// Enquanto PIND2 estiver em 1
		}
		bits[j] = result;					// O vetor bits recebe a variável com os valores resultantes
	}

	DHT_DDR |= (1<<PIN);	// Configuração do pino como saída
	DHT_PORT |= (1<<PIN);	// Configuração para nível alto
	
	// Conversão da temperatura e da umidade
	if ((uint8_t) (bits[0] + bits[1] + bits[2] +bits[3]) == bits[4])	// Verificação da paridade
	{
		uint16_t rawumidade = bits[0]<<8 | bits[1];			// Definição dos valores brutos de temperatura
		uint16_t rawtemperatura = bits[2]<<8 | bits[3];		// Definição dos valores brutos de umidade
	
		
		if (rawtemperatura & 0x8000)	// Verifica se os valores de temperatura são negativos
		{
			*dht_temperatura = (float)((rawtemperatura & 0x7fff) / 10.0)* -1.0;	// Para valores negativos de temperatura
		}
		else{
			*dht_temperatura = (float)(rawtemperatura)/10.0;	// Para valores positivos de temperatura
		}

		*dht_umidade = (float)(rawumidade)/10.0;				// Cálculo do valor de umidade

		return 1;
	}
	return 1;
}