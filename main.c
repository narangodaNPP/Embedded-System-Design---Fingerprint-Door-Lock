/*
 * door_lock.c
 *
 * Created: 16/10/2022 01:52:07 PM
 * Author : Pasan
 */ 
#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE ((F_CPU/8/BAUD)-1)

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile unsigned char rxData;

void initHardware();
void initUSART();
void Lcd_DataWrite(char data);
void Lcd_CmdWrite(char cmd);
void Lcd_init(void);

int main(void)
{
	initHardware();
	initUSART();
	Lcd_init();
	
    while (1) {
		if (rxData == '1') { 
			PORTC = 0B00000010;
			Lcd_DataWrite('W');
			Lcd_DataWrite('E');
			Lcd_DataWrite('L');
			Lcd_DataWrite('L');
			Lcd_DataWrite('C');
			Lcd_DataWrite('O');
			Lcd_DataWrite('M');
			Lcd_DataWrite('E');			
			_delay_ms(1000);
			rxData = '0';
			PORTC &= 0B00000000;
			 
		}
		if (rxData == '2') {
			PORTC &= 0B00000000;
			Lcd_CmdWrite(0x01);
			Lcd_CmdWrite(0x01);
			Lcd_DataWrite('T');
			Lcd_DataWrite('R');
			Lcd_DataWrite('Y');
			Lcd_DataWrite(' ');
			Lcd_DataWrite('A');
			Lcd_DataWrite('G');
			Lcd_DataWrite('A');
			Lcd_DataWrite('I');
			Lcd_DataWrite('N');
			Lcd_CmdWrite(0x01);
			rxData = '0';
		}	
    }
}

/*input & output pins*/
void initHardware() {
	DDRC |= 0B00000010;
	DDRA |= 0B11111111; //outputs for data pins(8,7,6,5) and control pins(3,2,1)
}

void Lcd_CmdWrite(char cmd){
	PORTA = (cmd & 0xF0); //Set upper 4 bits of the cmd
	PORTA &= ~(1<<0); //Set RS pin to LOW
	PORTA &= ~(1<<1); //Set RW pin to LOW
	PORTA |= (1<<2); //Generate High-to-Low pulse
	_delay_ms(10);
	PORTA &= ~(1<<2); //clear
	_delay_ms(10);
	PORTA = ((cmd<<4) & 0xF0);	//Set lower 4 bits of the cmd
	PORTA &= ~(1<<0); //Set RS pin to LOW
	PORTA &= ~(1<<1); //Set RW pin to LOW
	PORTA |= (1<<2); //Generate High-to-Low pulse
	_delay_ms(10);
	PORTA &= ~(1<<2); //clear
	_delay_ms(10);
}

void Lcd_DataWrite(char data){
	PORTA = (data & 0xF0);	//Set upper 4 bits of the data
	PORTA |= (1<<0); //Set RS pin to High
	PORTA &= ~(1<<1); //Set RW pin to LOW
	PORTA |= (1<<2); //Generate High-to-Low pulse
	_delay_ms(10);
	PORTA &= ~(1<<2); //clear
	_delay_ms(10);
	PORTA = ((data<<4) & 0xF0); //Set lower 4 bits of the data
	PORTA |= (1<<0); //Set RS pin to HIGH
	PORTA &= ~(1<<1); //Set RW pin to LOW
	PORTA |= (1<<2); //Generate High-to-Low pulse
	_delay_ms(10);
	PORTA &= ~(1<<2); //clear
	_delay_ms(10);
}

void Lcd_init(void){
	Lcd_CmdWrite(0x02); //Initialize LCD in 4-bit
	Lcd_CmdWrite(0x28); //Enable 5x7 mode for char
	Lcd_CmdWrite(0x0E); //Display on cursor blinking
	Lcd_CmdWrite(0x01); //Clear Display
	Lcd_CmdWrite(0x80); //Force Cursor to beginning
}

/*initialize uart*/
void initUSART(){
	UCSR0B |= (1<<TXEN0)|(1<<RXEN0)|(1<<RXCIE0); // tx enable, rx enable and rx complete interrupt enable
	UCSR0C |= (1<<UCSZ00)|(1<<UCSZ01); // 8 bit 
	UCSR0A |= (1<<U2X0); // 
	UBRR0H = (unsigned char)(UBRR_VALUE>>8); // UBRR0H = 0
	UBRR0L = (unsigned char)(UBRR_VALUE);	// UBRR0L = 207
	
	sei(); //globally enable interrupt
}

ISR (USART0_RX_vect) {
	rxData = UDR0;
} 

//
// /*interrupt initialization*/
// void initInterrupt() {
// 	EICRA |= 0B00000011; // rising edge
// 	EIMSK |= 0B00000001; // interrupt 0
// 	sei();
// }
//
//
// /*interrupt service routine*/
// ISR(INT0_vect){
// 	if(flag) {
// 		light = 1;
// 	}
// 	else {
// 		light = 0;
// 	}
// }
//

// void transmitUSART(unsigned char ch) {
// 	while (!(UCSR0A & (1<<UDRE0))); // check if USART data register is empty
// 	UDR0 = ch;
// }

// void strSend(char *str) {
// 	unsigned char i = 0;
//
// 	while(str[i] != '\0') {
// 		txChar(str[i]);
// 		i++;
// 	}
// }

// void Lcd_string(char *str, char size) {
// 	char i = 0;
// 	for (i=0; i<size; i++) {
// 		Lcd_DataWrite(str[i]);
// 	}
// }
//