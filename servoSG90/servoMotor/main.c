/*
	servoMotor.c
	Using Timer1 to control Servo SG90 rotate to an arbitrary angle.
	
	In the sketch, i create a signal pulse 20 ms with PWM method. Time for logic HIGH in this pulse is between 
	0.544 ms and 2.4 ms.
	(In the datasheet, the pulse length is specified as ranging from 1 ms to 2 ms, but in practice the pulse length
	is approximately from 0.544 ms to 2.4 ms.)
	+) 0.544 ms (Duty cycle = 5%)	-> 0 degree	
	+) 1.472 ms (Duty cycle = 7.36%)-> 90 degree
	+) 2.4 ms	(Duty cycle = 12%)	-> 180 degree 
	
	Step to control servo:
	1. First, choose Fast PWM mode and set TOP value so that Timer1 counts from 0 to TOP in 20ms. In the sketch,
	assuming that F_CPU = 12MHz, fPWM = 50Hz and prescaler 8, i can calculate TOP value as 30000
	2. Second, select non-inverting mode (COM1A/B1:0 = 2) to generate control signal. From there, servo can rotate
	a desired angle with this signal. 
	+) With duty cycle = 2.72%, OCR1x = 2.72% * 30000 = 816
	+) With duty cycle = 7.36%, OCR1x = 2208
	+) With duty cycle = 12%, OCR1x = 3600
	
	Mapping angle to OCR1x with:
	+) Pulse_length_min = 0.544 ms ,Pulse_length_max = 2.4 ms
	+) Signal_pulse = 20ms (f = 50Hz)
	+) TOP = 30000
	
	Formulation:
	OCR1x = 816 + (degree * (3600 - 816) / 180)
	
	Source: Nguyen Hai Nam
 */ 

#define F_CPU	12000000UL					// Clock CPU: 12MHz
#include <avr/io.h>
#include <util/delay.h>
#include "USART_RS232.h"

/*	Define time (us) for pulse lengths	*/
#define TIME_PULSE_LENGTH_MAX			2400	// 2.4 ms -> 180 degree
#define TIME_PULSE_LENGTH_MIN			544		// 0.544 ms -> 0 degree
#define TIME_SIGNAL_PULSE				20000	// 20ms (f = 50Hz)

/*	Define TOP_VALUE (ICR1)	for Timer1	
	fPWM = 50Hz -> TOP + 1 = 12MHz / (50 * 8) = 30000 -> Approximate TOP value = 30000
	or TOP + 1 = 12MHz * 20000us / 8 = (12MHz / 10^6) * 20000 / 8 = 30000				*/
#define TOP_VALUE			((F_CPU / 1000000UL) * TIME_SIGNAL_PULSE / 8)

/*	Define pulse to control servo (Set OCR1 value)	*/
#define	PULSE_MAX			((TIME_PULSE_LENGTH_MAX * TOP_VALUE) / TIME_SIGNAL_PULSE)				// 180 degree
#define PULSE_MIN			((TIME_PULSE_LENGTH_MIN * TOP_VALUE) / TIME_SIGNAL_PULSE)				// 0 degree

/*	Define device pins */
#define SERVO_DDR			DDRB
#define SERVO_PORT			PORTB
#define SERVO				PORTB1				// OC1A pin to control servo

/*	Initiate Timer1	*/
static inline void initTimer1(void)
{
	/*	Fast PWM mode with ICR1 as TOP value, non-inverting and prescaler 8	*/
	TCCR1A |= (1 << COM1A1) | (1 << WGM11);			
	TCCR1B |= (1 << WGM12) | (1 << WGM13) | (1 << CS11);
	
	// Set ICR1 as TOP value
	ICR1 = TOP_VALUE;
	
	// set output mode
	SERVO_DDR |= (1 << SERVO);
}


/*	Enter any angles to rotate	*/
static inline void setServoRotation(uint8_t servoAngle)
{					
	uint16_t servoPulseLength;				// The length of pulse (Ton) to control
	
	if (servoAngle > 180)	
		servoAngle = 180;
	
	/*	Calculate servoPulseLength to assign it to OCR1A	*/
	servoPulseLength = PULSE_MIN + ((uint32_t)servoAngle * (PULSE_MAX - PULSE_MIN) / 180);
	OCR1A = servoPulseLength;
}

int main(void)
{
    /*-----------Inits-----------*/
	uint8_t servoAngle;					// angle (0 ... 180 degree)
	initTimer1();
	initUSART();
	printString("\r\nWelcome to the Servo Demo\r\n");
	OCR1A = PULSE_MIN;
	
    while (1) 
    {
		printString("\r\nEnter a rotation angle:\r\n");			// enter a degree (0 ... 180)
		servoAngle = getNumber();
		
		printString("On my way....\r\n");						// start rotating!
		setServoRotation(servoAngle);
		while (TCNT1 < TOP_VALUE);								// loop until the end of a signal pulse
		printString("DONE!\r\n");								// finish rotating!
    }
}


