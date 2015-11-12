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
	timerCounter = 0;
	byteCount = 0;
	msgReady = false;
	startNibble = 0;
	for(int i =0; i< 10; i++){
		bytes[i] = 0x00;
	}
	bytePos = 0;
	
	DDRK &= ~(0x0F);		//Set pins to input
	PORTK |= 0x0F;			//Enable pull-up resistors
	
	PCMSK2 |= 0x0F;			//Enable PCINT 16-19
	
	
	TCCR1B |= (1<<WGM12)|(1<<CS10); //set up prescaler of 1 and CTC mode
	TCNT1 = 0;
	TIMSK1 |= (1<<OCIE1A); //CTC interrupt enable
	OCR1A = OCR1A_halfBit;//(uint16_t) OCR1A_2533Hz/2; //7.5 cycles
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

boolean MIRP2Class::available()
{
	return msgReady;
}

void MIRP2Class::decode()
{
	timerCounter++;
	uint8_t pinValue = 0;
	switch(direction)
	{
		case Front:
			pinValue = PINK & 0x08;
			pinValue = pinValue >> 3;
			break;
		case Back:
			pinValue = PINK & 0x04;
			pinValue = pinValue >> 2;
			break;
		case Right:
			pinValue = PINK & 0x02;
			pinValue = pinValue >> 1;
			break;
		case Left:
			pinValue = PINK & 0x01;
			break;
	}
	
	switch(step)
	{
		case 0:
			setTimer(); //wait for whole bit now
			startNibble = pinValue;
			step++;
			break;
		case 1:
			startNibble = (startNibble << 1) | pinValue;
			break;
	}
	
	if (timerCounter >= 100) //when timerCounter = 100 all of the data has been transmitted
	{
		init_pinChange_interrupts();
		disable_timer_interrupts();
		msgReady = true;
		for(int i =0; i < 6; i++)
		{
			Serial.print("Byte ");
			Serial.print(i);
			Serial.print(" = ");
			Serial.println(bytes[i], HEX);
		}
		init_variables();
	}
	else if(timerCounter <= 4)
	{
		startNibble = (startNibble << 1) + pinValue;
	}
	else if(((timerCounter % 2) != 0) && timerCounter > 4)
	{
		GPIO_odd = pinValue;
	}
	else if(((timerCounter % 2) == 0) && timerCounter > 4)
	{
		bytePos ++;
		GPIO_even = pinValue;
		if(startNibble != 0x03) //Invalid start sequence
		{
			init_variables();
			setPinChange();
		}
		else
		{ 
			bytes[byteCount] = (bytes[byteCount] << 1) + (GPIO_even);
		}
		if(bytePos == 8)
		{
			bytePos = 0;
			byteCount ++;
		}
	}
	msgReady = false;
}

ISR(PCINT2_vect)
{
	uint8_t pins = PINK & 0x0F;
	
	switch(pins) //Front, Back, Right, Left
	{
		case 0x08: //Front
		case 0x09: //Front-Left
		case 0x0A: //Front-Right
			MIRP2.direction = Front;
			break;
		case 0x04: //Back
		case 0x05: //Back-Left
		case 0x06: //Back-Right
			MIRP2.direction = Back;
			break;
		case 0x02: //Right
			MIRP2.direction = Right;
			break;
		case 0x01: //Left
			MIRP2.direction = Left;
			break;
		default:
			MIRP2.direction = NoDirection;
			break;
	}
	
	if(MIRP2.direction != NoDirection)
		MIRP2.setTimer(true); //wait for half of a bit
}

ISR(TIMER1_COMPA_vect)
{
	MIRP2.decode();
}
