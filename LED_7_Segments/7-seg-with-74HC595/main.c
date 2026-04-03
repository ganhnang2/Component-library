/*
	7-seg-with-74HC595.c
	This program is proggrammed to display number on LED-7-segment with IC 74HC595
	
	Source: Nguyen Hai Nam
 */ 

#define F_CPU		12000000UL				// Clock CPU: 12MHz
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/*-----------------MARCO-------------------*/
/*	Digit 1...4	*/
#define DIGIT_DDR			DDRB
#define DIGIT_PORT			PORTB
#define DIGIT(i)			i
#define NUMBER_OF_DIGITS	4
#define PRESCALER			1024
#define REFRESH_RATE		100					// Assuming the refresh rate is 100Hz, time on per a LED: t_on = 1 / 400Hz = 2.5ms
#define DIGIT_MASK			((1 << NUMBER_OF_DIGITS) - 1)

/*	IC 74HC595	*/
#define HC595_DDR			DDRC
#define HC595_PORT			PORTC
#define HC595_SHIFT_PIN		PORTC0
#define HC595_DATA_PIN		PORTC1
#define HC595_LATCH_PIN		PORTC2

// Generated number array (COMMON ANODE)
const uint8_t digitTable[] PROGMEM = {
	0xC0, // 0
	0xF9, // 1
	0xA4, // 2
	0xB0, // 3
	0x99, // 4
	0x92, // 5
	0x82, // 6
	0xF8, // 7
	0x80, // 8
	0x90  // 9
};

volatile uint8_t currentDigit = 0;
volatile uint8_t digit1, digit2, digit3, digit4;
volatile uint8_t totalTicks = 1;
volatile uint8_t segmentData;

void SevSeg_Init()
{
	/*	Digit pins	*/
	for (uint8_t i = 0; i < 4; i++)
		DIGIT_DDR |= (1 << DIGIT(i));
		
	/*	74HC595 pins	*/
	HC595_DDR |= (1 << HC595_DATA_PIN) | (1 << HC595_LATCH_PIN) | (1 << HC595_SHIFT_PIN);
	HC595_PORT|= (1 << HC595_DATA_PIN) | (1 << HC595_LATCH_PIN) | (1 << HC595_SHIFT_PIN);
	
	/*	Timer0	*/
	TCCR0A = 0x00;
	TCCR0B |= (1 << CS02) | (1 << CS00);		// internal clock: 12MHz, pre-scaler: 1024 -> clock: 12MHz/ 1024 -> time cycle = 85.33us
	TIMSK0 |= (1 << TOIE0);
}

// Calculate the number of ticks which is necessary to scanning LED
// F_interrupt = Refresh Rate * NUMBER_OF_DIGITS -> t_on = 1 / F_interrupt
// A tick takes (PRESCALER / F_CPU). So, to cover the required t_on duration, we need to run about (t_on / t_one_tick) ticks
// NOTE: Refresh rate should be between 50Hz and 1000Hz to avoid "ghosting" and "flicker"
void setRefreshRate(uint16_t freq)
{
	// Verify that the frequency falls within the permissible range
	// If not, assuming that the refresh rate is 100Hz
	if (freq < 50 || freq > 1000)
		freq = 100;
	totalTicks = 46875 / (4 * freq * NUMBER_OF_DIGITS);
	TCNT0 = 256 - (uint8_t)totalTicks;
}

void SevSeg_Display_Number(uint16_t number)
{
	digit4 = number % 10;
	number /= 10;
	digit3 = number % 10;
	number /= 10;
	digit2 = number % 10;
	digit1 = number / 10;
}

// Shift data into 8-bit shift register in IC74HC595 (MSB is sent first)
// Note: If we send the LSB first, then pin Q7 (IC 74HC595) will correspond to bit 0 of the data. 
// Conversely, when the MSB is transmitted first, Q0 (IC 74HC595) will correspond to the LSB of the data
// -> In this function, i have decided to transmit MSB first
void HC595_Shift_Out(uint8_t data)
{
	uint8_t i;
	for (i = 0; i < 8; i++)
	{
		if (data & 0x80)
			HC595_PORT |= (1 << HC595_DATA_PIN);
		else
			HC595_PORT &= ~(1 << HC595_DATA_PIN);
		data = (data << 1);
		
		// A low-to-high transition on this input causes the data at the Serial Input pin 
		// to be shifted into the 8-bit shift register.
		HC595_PORT &= ~(1 << HC595_SHIFT_PIN);
		HC595_PORT |= (1 << HC595_SHIFT_PIN);
	}
}

ISR(TIMER0_OVF_vect)
{
	// Check whether currentDigit is greater than allowed number
	if (currentDigit > NUMBER_OF_DIGITS)
		currentDigit = 0;
		
	// Step 1: Turn all digits off
	// DIGIT_PORT = DIGIT_MASK;		// Transistor PNP for common anode
	DIGIT_PORT &= ~DIGIT_MASK;		// Transistor NPN for common anode
	HC595_PORT &= ~(1 << HC595_LATCH_PIN);
	
	switch(currentDigit)
	{
		case 0:
			segmentData = pgm_read_byte(&digitTable[digit1]);
			break;
		case 1:
			segmentData = pgm_read_byte(&digitTable[digit2]);
			break;
		case 2:
			segmentData = pgm_read_byte(&digitTable[digit3]);
			break;
		case 3:
			segmentData = pgm_read_byte(&digitTable[digit4]);
		break;
	}
	
	HC595_Shift_Out(segmentData);
	HC595_PORT |= (1 << HC595_LATCH_PIN);
	
	// Step 3: Turn the selected digit on
	// DIGIT_PORT &= ~(1 << DIGIT(currentDigit));	// Transistor PNP for common anode
	DIGIT_PORT |= (1 << DIGIT(currentDigit));		// Transistor NPN for common anode
	
	currentDigit++;
	
	// Step 4: Load value to TCNT0
	TCNT0 = 256 - totalTicks;
}

int main(void)
{
    SevSeg_Init();
	setRefreshRate(150);
	sei();
	uint16_t i = 1000;
    while (1) 
    {
		SevSeg_Display_Number(i);
		i++;
		_delay_ms(500);
    }
}

