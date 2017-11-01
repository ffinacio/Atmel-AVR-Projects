/*
 * interrupts.cpp
 *
 * Created: 11/1/2017 10:11:23 AM
 * Author : Fernando
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

void (*isrCallBack)();

volatile uint8_t update = 0;
volatile uint8_t change = 0;

uint8_t debounce(volatile uint8_t *port,uint8_t pin)
{
	uint8_t value = (*port & (1<<pin));
	_delay_ms(50);
	if((value == (*port & (1<<pin))) && ((value & (1<<pin)) == 0)) return 1;
	else return 0;
}

void pisca()
{
	if(debounce(&PINB,PINB0)) update ^= 1;
	if(debounce(&PINB,PINB1)) change ^= 1;
}


void attachPCINT(uint8_t pcint)
{
	cli();
	PCICR |= (1<<PCIE0);
	PCMSK0 |= (1<<pcint);
	isrCallBack = pisca;
	sei();
}


void initPCINT(uint8_t pcint)
{
	cli();
	PCICR |= (1<<PCIE2);
	PCMSK2 |= (1<<pcint);
	sei();
}

ISR(PCINT0_vect)
{
	cli();
	isrCallBack();
	sei();
}

ISR(PCINT2_vect)
{
	cli();
	change ^= 1;
	sei();
}


int main(void)
{
	attachPCINT(PCINT0);						// PB0
	attachPCINT(PCINT1);						// PB1
	DDRD |= (1<<PORTD2)|(1<<PORTD7);
	PORTD |= (1<<PORTD2);
	initPCINT(PCINT19);							// PD3
    while (1) 
    {
		if(update) PORTD |= (1<<PORTD2);
		else PORTD &= ~(1<<PORTD2);
		if(change) PORTD |= (1<<PORTD7);
		else PORTD &= ~(1<<PORTD7);
    }
}

