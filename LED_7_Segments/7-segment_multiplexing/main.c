/*
	7-segment_multiplexing.c
	In this sketch, i will implement multiplexing method to control many LED-7-segment Common Anode.
	For each a digit, transistor PNP is used to protect MCU from over-current
	
	+) Multiplexing method:
	In this method, at a time one display is driven by the controller and the rest are OFF. 
	It keeps switching the displays using transistors. Due to the persistence of vision, 
	it appears as a normal display. 
	
	The two terms we need to consider are Refresh Rate and Interrupt Frequency
	+) Refresh Rate: The scanning frequency for a single LED.
	+) Interrupt Frequency: The frequency at which MCU generates periodic interrupts to perform LED scanning.
	
	The relationship between Refresh Rate and Interrupt Frequency:
	F_interrupt = F_refresh * N
	where:
	+) F_interrupt: Interrupt Frequency
	+) F_refresh: The scanning frequency for a single LED.
	+) N: Number of LEDs 
	Example: 
	Assume that we have scanned 4 LEDs with the frequency 100Hz per a LED, F_interrupt = 100 * 4 = 400Hz
	-> Time on per a LED: t_on = 1 / 400Hz = 2.5ms -> After the interval 2.5ms, the ISR will be created
	
	NOTE: The Interrupt Frequency have always scanned faster than the Refresh Rate, which ensures that 
	MCU can scan each LED 100 times per second
 */ 

#define F_CPU		12000000UL				// Clock CPU: 12MHz
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

/*-------------------Marco-----------------*/
/*	SEG0...6	*/
#define SEG_DDR				DDRD
#define SEG_PORT			PORTD
#define SEG(i)				i

/*	Digit 1...4	*/
#define DIGIT_DDR			DDRB
#define DIGIT_PORT			PORTB
#define DIGIT(i)			i
#define NUMBER_OF_DIGITS	4
#define PRESCALER			1024
#define REFRESH_RATE		100					// Assuming the refresh rate is 100Hz, time on per a LED: t_on = 1 / 400Hz = 2.5ms

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
volatile uint8_t totalTicks;

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

void displayNumber(uint16_t number)
{
	digit4 = number % 10;
	number /= 10;
	digit3 = number % 10;
	number /= 10;
	digit2 = number % 10;
	digit1 = number / 10;
}

// Assuming the refresh rate = 100Hz -> Interrupt frequency = 100 * 4 = 400Hz -> Time on per a LED = 1 / 400 = 2.5ms
// Load value to TCNT0: 2.5ms / 85.33 = 30
ISR(TIMER0_OVF_vect)
{
	// Check whether currentDigit is greater than allowed number
	if (currentDigit >= NUMBER_OF_DIGITS)
		currentDigit = 0;
	
	// Step 1: Turn all digits off
	// DIGIT_PORT = 0xFF;	// Transistor PNP for common anode
	DIGIT_PORT = 0x00;		// Transistor NPN for common anode
	
	// Step 2: The value of digit is displayed on LED-7-Segment
	switch(currentDigit)
	{
		case 0:
			SEG_PORT = pgm_read_byte(&digitTable[digit1]);
			break;
		case 1:
			SEG_PORT = pgm_read_byte(&digitTable[digit2]);
			break;
		case 2:
			SEG_PORT = pgm_read_byte(&digitTable[digit3]);
			break;
		case 3: 
			SEG_PORT = pgm_read_byte(&digitTable[digit4]);
			break;
	}
	
	// Step 3: Turn the selected digit on
	// DIGIT_PORT &= ~(1 << DIGIT(currentDigit));	// Transistor PNP for common anode
	DIGIT_PORT |= (1 << DIGIT(currentDigit));		// Transistor NPN for common anode
	
	currentDigit++;
	
	// Step 4: Load value to TCNT0 
	TCNT0 = 256 - totalTicks;
}

int main()
{
	/*	Digit, Segment Pin	*/
	for (uint8_t i = 0; i < NUMBER_OF_DIGITS; i++)
	{
		DIGIT_DDR |= (1 << DIGIT(i));
		DIGIT_PORT|= (1 << DIGIT(i));
	}
	
	for (uint8_t i = 0; i < 8; i++)
	{
		SEG_DDR |= (1 << SEG(i));
		SEG_PORT|= (1 << SEG(i));
	}
	
	/*	Timer0	*/
	TCCR0A = 0x00;
	TCCR0B |= (1 << CS02) | (1 << CS00);		// internal clock: 12MHz, pre-scaler: 1024 -> clock: 12MHz/ 1024 -> time cycle = 85.33us
	TIMSK0 |= (1 << TOIE0);
	sei();
	
	setRefreshRate(200);			// modify refresh rate
	uint16_t i = 1000;
	while(1)
	{
		displayNumber(i);
		i++;
		_delay_ms(500);
	}
}

