/*******************************************************************************
* Name: MIRP2.cpp
* Description: MAGE IR Protocol Library
* Author: Jonathan Kropfinger & CJ Vaughter
*******************************************************************************/

#include "MIRP2.h"
#include <avr/interrupt.h>
#include <Arduino.h>

MIRP2Class MIRP2;

void MIRP2Class::init()
{	
	DDRK &= ~(0x0F);		//Set pins to input
	PORTK |= 0x0F;			//Enable pull-up resistors
	
	//PCMSK2 |= 0x0F;			//Enable PCINT 16-19
	TCCR1B |= (1<<WGM12)|(1<<CS10); //set up prescaler of 1 and CTC mode	

	msgReady = false;
	direction = NoDirection;
	color = NoColor;
	reset();
}

void MIRP2Class::reset()
{
	step = 0;
	byteCount = 0;
	bitPos = 7;
	startNibble = 0;
	reading = false;	
	//setPinChange();
}

void MIRP2Class::setPinChange()
{
	byte sreg = SREG;
	cli();
	TIMSK1 &= ~(1<<OCIE1A);  //Disable CTC interrupt
	PCICR |= (1<<PCIE2);	//Enable PCINT2
	SREG = sreg;
}

void MIRP2Class::setTimer(boolean half)
{
	byte sreg = SREG;
	cli();
	PCICR &= ~(1<<PCIE2);	//Disable PCINT2
	
	if(half)
		OCR1A = OCR1A_halfBit;
	else
		OCR1A = OCR1A_2533Hz;
	
	TCNT1 = 0;
	TIMSK1 |= (1<<OCIE1A); //Enable CTC interrupt
	SREG = sreg;
}

void MIRP2Class::decode()
{
	uint8_t pinValue = 0;
	switch(direction)
	{
		case Front:
			pinValue = ((PINK & 0x08) >> 3) ^ 0x01;
			break;
		case Back:
			pinValue = ((PINK & 0x04) >> 2) ^ 0x01;
			break;
		case Right:
			pinValue = ((PINK & 0x02) >> 1) ^ 0x01;
			break;
		case Left:
			pinValue = (PINK & 0x01) ^ 0x01;
			break;
	}
	
	switch(step)
	{
		case 0:
			setTimer(); //wait for whole bit now
			startNibble = pinValue;
			step++;
			break;
		case 1: case 2: case 3:
			startNibble |= (pinValue << step);
			if(step == 3)
			{
				if(startNibble != 0x0C)
				{
					reset();
					//MIRP2.color = Cyan;
					break;
				}
				else
				{
					msgReady = false;
				}
			}
			step++;
			break;
		default:
			if(step % 2 == 0)
			{
				GPIO_even = pinValue;
			}
			else
			{
				GPIO_odd = pinValue;
				if(GPIO_even == GPIO_odd)
				{
					reset();
					MIRP2.color = Purple;
					break;
				}
				else
				{
					data[byteCount] |= (GPIO_even << bitPos--);
					if(bitPos == 0)
					{
						bitPos = 7;
						byteCount++;
					}
					if(byteCount == 6)
					{
						validate();
						reset();
						break;
					}
				}
			}
			step++;
			break;
	}
}

void MIRP2Class::validate()
{
	uint8_t sum = 0;
	for(uint8_t i = 0; i < PACKET_SIZE - 1; i++)
	{
		sum += data[i];
	}
	if(0xFF - sum == data[PACKET_SIZE-1]) msgReady = true;
	else
	{
		msgReady = false;
		MIRP2.color = Red;
	}
}

void MIRP2Class::read()
{
	if(reading) return;
	
	uint8_t pins = ~(PINK & 0x0F);
	
	if(pins & 0x08)
	{
		direction = Front;
		color = Orange;
	}
	/*else if(pins & 0x04)
	{
		direction = Back;
		color = Yellow;
	}
	else if(pins & 0x02)
	{
		direction = Right;
		color = Cyan;
	}
	else if(pins & 0x01)
	{
		direction = Left;
		color = Blue;
	}*/
	
	if(direction != NoDirection)
	{
		MIRP2.setTimer(true); //wait for half of a bit
		reading = true;
	}
}
/*
ISR(PCINT2_vect)
{
	uint8_t pins = ~(PINK & 0x0F);
	uint8_t direction = NoDirection;
	
	if(pins & 0x08)
	{
		direction = Front;
		MIRP2.color = Orange;
	}
	else if(pins & 0x04)
	{
		direction = Back;
		MIRP2.color = Yellow;
	}
	else if(pins & 0x02)
	{
		direction = Right;
		MIRP2.color = Cyan;
	}
	else if(pins & 0x01)
	{
		direction = Left;
		MIRP2.color = Blue;
	}
	
	if(direction != NoDirection)
	{
		MIRP2.direction = direction;
		MIRP2.setTimer(true); //wait for half of a bit
	}
}
*/
ISR(TIMER1_COMPA_vect)
{
	MIRP2.decode();
}
