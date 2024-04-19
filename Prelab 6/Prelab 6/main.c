/**************************
Universidad del Valle de Guatemala
Programación de Microcrontroladores
Proyecto: Prelab 4
Hardware: ATMEGA328p
Created: 18/04/2024 19:29:40
Author : James Ramírez
***************************/

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>
#include <avr/interrupt.h>

void initUART9600(void);
void writeUART(char Caracter);
void writeTextUART (char* Texto);
void readUART(uint8_t receivedChar);
void initADC(void);
void ADC_start();
volatile uint8_t adcValueHigh = 0;
volatile uint8_t adcValueLow = 0;
volatile bufferTX;


int main(void)
{
    initUART9600();
	writeTextUART("Hola mundo :)");
	writeUART(10);				//ENTER
	writeUART(13);				//REGRESA A LA POSICIÓN 0 DE LA LÍNEA
	sei();
    while (1) 
    {
		//INICIO DE LA LECTURA ADC
		ADCSRA |= (1<<ADSC);	
		
		if (bufferTX != 0) {
			readUART(bufferTX);
			bufferTX = 0;
		}
    
	}
}


void initUART9600(void){
	
	cli ();
	//PORTB COMO SALIDAS
	DDRB = 0xFF;   // Todo como salidas
	PORTB = 0x00;
	
	//PASO 1: RX SE TOMA COMO ENTRADA Y TX COMO SALIDA
	DDRD &= ~(1<<DDD0);
	DDRD |= (1<<DDD1);
	
	//PASO 2: CONFIGURACION DEL UCSR0A
	UCSR0A = 0;
	
	//PASO 3: CONFIGURACION DEL UCSR0B, SE HABILITA ISR DE RECEPCION Y SE HABILITAN RX Y TX
	UCSR0B = 0;
	UCSR0B |= (1<<RXCIE0)|(1<<RXEN0)|(1<<TXEN0);
	
	//PASO 4: CONFIGURACION DE UCSR0C COMO ASINCRONO, SIN PARIDAD, 1 BIT DE STOP, 8 BITS DE DATA
	UCSR0C = 0;
	UCSR0C |= (1<<UCSZ01)|(1<<UCSZ00);
	
	//PASO 5: CONFIGURAR VELOCIDAD DE BAUDRATE: 9600
	UBRR0 = 103;
}

void writeUART(char	Caracter){
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = Caracter;
}

void writeTextUART(char* Texto){
	
	uint8_t i;
	for (i=0; Texto[i] !='\0'; i++){			//INICIA EN 0 Y VA HASTA EL VALOR QUE TENGA LA VARIABLE TEXTO
		while (!(UCSR0A & (1<<UDRE0)));			//VERIFICA QUE EL BUFFER ESTÉ VACÍO PARA PODER INICIAR 
		UDR0 = Texto[i];						
	}
}

void readUART(uint8_t receivedChar) {
	switch (receivedChar) {
		case '0':
		PORTB = 0;
		PORTB |= (1 << PORTB0);
		break;
		case '1':
		PORTB = 0;
		PORTB |= (1 << PORTB1);
		break;
		case '2':
		PORTB = 0;
		PORTB |= (1 << PORTB2);
		break;
		case '3':
		PORTB = 0;
		PORTB |= (1 << PORTB3);
		break;
		case '4':
		PORTB = 0;
		PORTB |= (1 << PORTB4);
		break;
		case '5':
		PORTB = 0;
		PORTB |= (1 << PORTB5);
		break;
		default:
		PORTB = 0xFF;
		_delay_ms(100);
		PORTB = 0x00;
		_delay_ms(100);
		break;
	}
}



void initADC(void){
	ADMUX = 6;
	//REFERENCIA AVCC = 5V
	ADMUX |= (1<<REFS0);
	ADMUX &= ~(1<<REFS1);
	
	//JUSTIFICACION A LA IZQUIERDA
	ADMUX |= (1<<ADLAR);
	
	ADCSRA = 0;
	
	//HABILITA INTERRUPCION
	ADCSRA	|= (1<<ADIE);
	
	//HABILITA PREESCALER A 128 - 125kHz
	ADCSRA |= (1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	
	//HABILITA EL ADC
	ADCSRA |= (1<<ADEN);
	
}

ISR(ADC_vect){
	//SE ALMACENA EL VALOR DE ADC Y SE BORRA EL VALOR DE LA BANDERA
	uint16_t adc_val16 = ADC;
	uint8_t adcValue = (adc_val16 >> 8);

	ADCSRA |= (1<<ADIF);
	
	//SE SEPARA EL VALOR DEL ADC EN 4 BITS CADA UNO
	adcValueHigh = adcValue >> 4;
	adcValueLow = adcValue & 0x0F;
	ADCSRA |= (1<<ADSC);
}


ISR(USART_RX_vect){
	
	bufferTX = UDR0;
	while(!(UCSR0A & (1<<UDRE0)));
	UDR0 = bufferTX;
}