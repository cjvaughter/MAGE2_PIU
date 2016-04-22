/*******************************************************************************
* Name: MIRP2.cpp
* Description: MAGE IR Protocol Library
* Author: Jonathan Kropfinger & CJ Vaughter
*******************************************************************************/

#include "MIRP2.h"

MIRP2Class MIRP2;

void MIRP2Class::init()
{
	LOG("Initializing...");

	DDRK &= ~(0x0F);		//Set pins to input
	PORTK |= 0x0F;			//Enable pull-up resistors

	msgReady = false;
	direction = NoDirection;
	reset();

	LOG("Success!");
}

void MIRP2Class::reset()
{
	step = 0;
	byteCount = 0;
	bitPos = 7;
	sample = 0;
	sampleCount = 1;
	sampleBias = 15;
	frontDrift = 1;
	backDrift = 0;
	badPacket = false;
	badStart = false;
	zeroBiasCounter = 0;
	#ifdef DEBUG
	badEncodingCount = 0;
	#endif
	setPinChange();
}

void MIRP2Class::setPinChange()
{
	byte sreg = SREG;
	cli();
	
	TCCR3B = 0;				  //Disable Timer
	TIMSK3 &= ~(1 << OCIE3A); //Disable CTC interrupt
	
	PCMSK2 = 0x0F;			  //Enable PCINT 16-19
	PCICR |= (1 << PCIE2);	  //Enable PCINT2
	
	SREG = sreg;
}

void MIRP2Class::setTimer()
{
	byte sreg = SREG;
	cli();

	PCICR &= ~(1 << PCIE2);		//Disable PCINT2
	PCMSK2 = 0x00;				//Disable PCINT 12-19
	
	OCR3A = SamplePeriod;
	TIFR3 = 0;							 //Clear any pending timer3 interrupts
	TCNT3 = 0;							 //Reset counter
	TCCR3A = 0;							 //No compare output pins & CTC mode
	TCCR3B = (1 << WGM12) | (1 << CS10); //set up prescaler of 1 and CTC mode
	TIMSK3 |= (1 << OCIE3A);			 //Enable CTC interrupt

	SREG = sreg;
}

void MIRP2Class::decode()
{
	sampleCount++;
	if(sampleCount <= 0) return;

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

	if(pinValue)
	{
		(sampleCount < 16) ? sampleBias += (16 - sampleCount) : sampleBias -= (sampleCount - 15);
		if(sampleCount < 4) frontDrift++;
		if(sampleCount > 27) backDrift++;
	}
	if(sampleCount < 30) return;

	sample = (sampleBias >= 0) ? 1 : 0;
	
	if(step < 2)
	{
		sample = (frontDrift + backDrift > 4) ? 1 : 0;
		sampleCount = 0;
		sampleBias = 0;
	}
	else if(sampleBias < 100 && sampleBias > -100)
	{
		#ifdef DEBUG
		if(sampleBias < 60 && sampleBias > -60)
		{
			badEncoding[badEncodingCount++] = (byteCount << 8) | bitPos;
		}
		#endif

		if(sampleBias == 0) zeroBiasCounter++;
		if(zeroBiasCounter > 10)
		{
			reset(); //zero bias indicates no signal
			return;
		}
		
		sampleCount = 0;
		sampleBias = 0;

		if(frontDrift < 2 && backDrift < 2)
		{
			(sample) ? sampleCount = -2 : sampleCount = 2;
		}
		else if(frontDrift > 1 && backDrift > 1)
		{
			(sample) ? (sampleCount = 2, sampleBias = 28) : sampleCount = -2;
		}
	}
	else
	{
		sampleCount = 0;
		sampleBias = 0;
	}

	frontDrift = 0;
	backDrift = 0;

	switch (step)
	{
		case 0:
			startNibble = sample << 3;
			startNibble |= sample << 2;
			step++;
			break;
		case 1:
			startNibble |= sample << 1;
			startNibble |= sample;
			step++;
			if(startNibble == 0x0C)
				msgReady = false; //if you haven't read the previous message by now, you missed it
			else if(startNibble == 0x00)
				reset(); //ignore noise
			else
				badStart = true;				
			break;
		default:
			if(!badStart)
			{
				if (bitPos == 7)
				{
					data[byteCount] = sample << 7;
				}
				else
				{
					data[byteCount] |= sample << bitPos;
				}
			}
			bitPos--;

			if (bitPos == 0xFF)
			{
				bitPos = 7;
				byteCount++;
			}

			if (byteCount == PACKET_SIZE)
			{
				#ifdef DEBUG
				printWave();
				#endif
				validate();
				reset();
				break;
			}
			break;
	}
}

void MIRP2Class::validate()
{
	//validate start
	if (badStart)
	{
		LOG_F("BAD START INDICATOR: 0x%02X - Expected 0x0C", startNibble);
		return;
	}

	//validate checksum
	uint8_t sum = 0;
	for (uint8_t i = 0; i < byteCount - 1; i++)
	{
		sum += data[i];
	}
	if (0xFF - sum != data[byteCount - 1])
	{
		LOG_F("BAD PACKET CHECKSUM: 0x%02X - Expected 0x%02X", data[byteCount - 1], 0xFF-sum);
		badPacket = true;
	}

	msgReady = !badPacket;
}

#ifdef DEBUG
void MIRP2Class::printWave()
{
	if(badStart) return;

	int k = 0;
	inBfr[k++] = '\r';
	inBfr[k++] = '\n';
	inBfr[k++] = '\r';
	inBfr[k++] = '\n';
	for(int j = 4; j < 8; j++)
	{
		boolean bit = ((startNibble & (0x80 >> j)) != 0);
		if(bit)
		{
			inBfr[k++] = '-';
		}
		else
		{
			inBfr[k++] = '_';
		}
	}
	for(int i = 0; i < byteCount; i++)
	{
		inBfr[k++] = '|';
		for(int j = 0; j < 8; j++)
		{
			boolean bit = ((data[i] & (0x80 >> j)) != 0);
			if(bit)
			{
				inBfr[k++] = '-';
				inBfr[k++] = '_';
			}
			else
			{
				inBfr[k++] = '_';
				inBfr[k++] = '-';
			}
		}
	}
	inBfr[k++] = '\r';
	inBfr[k++] = '\n';
	inBfr[k] = '\0';
	LOG(inBfr);
	k = 0;

	if(badEncodingCount > 0)
	{
		LOG("POSSIBLE BAD ENCODING:");
	}
	for(int i = 0; i < badEncodingCount; i++)
	{
		uint8_t bytepos = badEncoding[i]>>8;
		uint8_t bitpos = badEncoding[i]&0x00FF;
		sprintf(&inBfr[k], "%X[%X] ", bytepos, bitpos);
		k += 5;
		if((i+1) % 8 == 0)
		{
			inBfr[k] = '\0';
			LOG(inBfr);
			k = 0;
		}
	}
	if(k > 0)
	{
		inBfr[k] = '\0';
		LOG(inBfr);
	}
}
#endif

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
		MIRP2.setTimer();
	}
}

ISR(TIMER3_COMPA_vect)
{
	MIRP2.decode();
}
