/*******************************************************************************
* Name: MIRP2.cpp
* Description: MAGE IR Protocol Library
* Author: Jonathan Kropfinger & CJ Vaughter
*******************************************************************************/

#include "MIRP2.h"
#include <RGB.h>

MIRP2Class MIRP2;

void MIRP2Class::init()
{
	DDRB |= 0x04;
	PORTB &= ~(0x04);
	DDRK &= ~(0x0F);		//Set pins to input
	PORTK |= 0x0F;			//Enable pull-up resistors

	msgReady = false;
	direction = NoDirection;
	reset();
}

void MIRP2Class::reset()
{
	step = 0;
	byteCount = 0;
	bitPos = 7;
	setPinChange();
}

void MIRP2Class::setPinChange()
{
	byte sreg = SREG;
	cli();
	
	TIMSK3 &= ~(1 << OCIE3A); //Disable CTC interrupt
	TCCR3B = 0;				  //Disable Timer
	
	PCMSK2 = 0x0F;			  //Enable PCINT 16-19
	PCICR |= (1 << PCIE2);	  //Enable PCINT2
	
	SREG = sreg;
}

void MIRP2Class::setTimer(boolean half)
{
	byte sreg = SREG;
	cli();
	PCICR &= ~(1 << PCIE2);				 //Disable PCINT2

	if (half)
		OCR3A = HalfBitPeriod;
	else
		OCR3A = BitPeriod;

	TCNT3 = 0;							 //Reset counter
	TCCR3A = 0;							 //No compare output pins & CTC mode
	TCCR3B = (1 << WGM12) | (1 << CS10); //set up prescaler of 1 and CTC mode
	TIMSK3 |= (1 << OCIE3A);			 //Enable CTC interrupt
	TIFR3 = 0;							 //Clear any pending timer3 interrupts
	
	SREG = sreg;
}

void MIRP2Class::decode()
{
	PORTB |= 0x04;
	uint8_t pinValue = 0;
	uint8_t pins = ~PINK;
	switch (direction)
	{
		case Front:
			pinValue = ((pins & 0x08) >> 3);
			break;
		case Back:
			pinValue = ((pins & 0x04) >> 2);
			break;
		case Right:
			pinValue = ((pins & 0x02) >> 1);
			break;
		case Left:
			pinValue = (pins & 0x01);
			break;
	}

	switch (step)
	{
		case 0:
			setTimer(); //wait for whole bit now
			startNibble = pinValue << 3;
			step++;
			break;
		case 1: case 2: case 3:
			startNibble |= (pinValue << (3 - step));
			if (step == 3)
			{
				if (startNibble != 0x0C)
				{
					Serial.println("BAD START");
					reset();
					break;
				}
				else
				{
					msgReady = false; //if you haven't read the message by now, you missed it
				}
			}
			step++;
			break;
		default:
			if (step % 2 == 0)
			{
				GPIO_even = pinValue;
			}
			else
			{
				GPIO_odd = pinValue;
				if (GPIO_even == GPIO_odd)
				{
					Serial.println("BAD ENCODING");
					reset();
					break;
				}
				else
				{
					if (bitPos == 7)
					{
						data[byteCount] = (GPIO_even << bitPos);
					}
					else
					{
						data[byteCount] |= (GPIO_even << bitPos);
					}
					bitPos--;

					if (bitPos == 0xFF)
					{
						bitPos = 7;
						byteCount++;
					}

					if (byteCount == PACKET_SIZE)
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
	PORTB &= ~(0x04);
}

void MIRP2Class::validate()
{
	uint8_t sum = 0;
	for (uint8_t i = 0; i < PACKET_SIZE - 1; i++)
	{
		sum += data[i];
	}
	if (0xFF - sum == data[PACKET_SIZE - 1])
		msgReady = true;
	else
	{
		Serial.println("BAD CHECKSUM");
		msgReady = false;
	}
}

ISR(PCINT2_vect)
{
	uint8_t pins = ~(PINK & 0x0F);
	uint8_t direction = NoDirection;

	if (pins & 0x08)
		direction = Front;
	else if (pins & 0x04)
		direction = Back;
	else if (pins & 0x02)
		direction = Right;
	else if (pins & 0x01)
		direction = Left;

	if (direction != NoDirection)
	{
		MIRP2.direction = direction;
		MIRP2.setTimer(true); //wait for half of a bit
	}
}

ISR(TIMER3_COMPA_vect)
{
	MIRP2.decode();
}
