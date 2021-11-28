

void UART_init(void);							// Fun��o para iniciar o USART AVR ass�ncrono, 8 bits, 9600 baud
unsigned char UART_read(void);					// Fun��o para recebimento de caracteres
void UART_write(unsigned char);					// Fun��o para a transmiss�o de caracteres
void UART_msg(char*);							// Fun��o para a transmiss�o de string de caracteres 
int USART_printCHAR(char character, FILE *stream);

static FILE USART_0_stream = FDEV_SETUP_STREAM(USART_printCHAR, NULL, _FDEV_SETUP_WRITE);

int USART_printCHAR(char character, FILE *stream)
{
	UART_write(character);	
	return 0;
}

// Fun��o que inicializa a comunica��o UART
void UART_init(void)
{
	DDRD |= (1<<1);							//PD1 COMO SA�DA TXa
	DDRD &= ~(1<<0);						//PD0 COMO ENTRADA RX
	
	// Habilita��o das op��es TX e RX	
	UCSR0A = (0<<TXC0)|(0<<U2X0)|(0<<MPCM0);
	UCSR0B = (1<<RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0)|(1<<RXEN0)|(1<<TXEN0)|(0<<UCSZ02)|(0<<TXB80);
	UCSR0C = (0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00)|(0<<UCPOL0);
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	
	stdout = &USART_0_stream;
}

// Fun��o que realiza a leitura dos dados que chegam � porta serial
unsigned char UART_read(){
	if(UCSR0A&(1<<7)){			// Se o bit7 do registrador UCSR0A assume valor 1
		return UDR0;			// Retorna dado armazenado no registrador UDR0
	}
	else
	return 0;
}

// Fun��o que envia os caracteres da string
void UART_write(unsigned char caracter){
	while(!(UCSR0A&(1<<5)));    // Espera enquanto o registrador UDR0 est� cheio
	UDR0 = caracter;            // Envia caracteres quando o registrador UDR0 est� vazio
}

// Fun��o que permite a impress�o de uma sequ�ncia de caracteres
void UART_write_txt(char* cadeia){		// String de caracteres tipo char
	while(*cadeia !=0x00){				// Enquanto que o �ltimo valor da string � diferente do caractere nulo
		UART_write(*cadeia);			// Transmite os caracteres da string
		cadeia++;						// Incrementa a localiza��o dos caracteres da string
										// Para o envio do pr�ximo caractere da string
	}
}


