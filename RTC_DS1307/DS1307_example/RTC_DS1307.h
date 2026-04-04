/*
	RTC_DS1307.h
	1. IC DS1307 has used BCD format to store the contents of the time and calendar registers.
	Example: Value = 47 = 0b00101111 = 0x2F -> In the BCD format, the register has stored value 47 as 0x47F
	-> In the register, upper nibble = 4 = 0100 and higher nibble = 7 = 0111
	-> Register: 0b01000111
	2. The DS1307 can be run in either 12-hour or 24-hour mode (The register at the address 0x02)
	Hour register (0x02): 
	Bit 6 = mode select (1 = 12h, 0 = 24h) 
	In 12h mode: Bit 5 = AM/PM (1 = PM) and Bit 4 = tens digit
	In 24h mode: Bit 5, 4 = tens digit for 20–23 hours	
	3. The address of Timekeeper Registers:
	- 0x00 -> Second (0 - 60)
	- 0x01 -> Minute (0 - 60)
	- 0x02 -> Hour (1 - 12 with 12-hour mode, 0 - 23 with 24-hour mode)
	- 0x03 -> Day (1 - 7 is respectively Sunday - Saturday)
	- 0x04 -> Date (The total days in a month)
	- 0x05 -> Month (1 - 12)
	- 0x06 -> Year (00 - 99)
	- 0x07 -> Control the operation of the SQW/OUT pin -> Not important, no need to care
	
	NOTE:
	The hours value must be re-entered whenever the 12/24-hour mode bit is changed.
	By default, this library uses the 24-hour mode
	
	Source: Nguyen Hai Nam
 */ 


#ifndef RTC_DS1307_H_
#define RTC_DS1307_H_

#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "TWI_Master.h"

/***********MARCO***************/
#define DS1307_ADDRESS				0b01101000
#define DEC2BCD(value)				((value % 10) | ((value / 10) << 4))
#define BCD2DEC(value)				((value & 0x0F) + (value >> 4) * 10)
#define DS1307_AMPM_MODE			0x40			// 12-hour mode
#define DS1307_AMPM_BIT				6
#define DS1307_Transmit_Data(value)	I2C_Write(DEC2BCD(value));
/**********END FOR MARCO*********/

/*************UTILITY FUNCTION************/
void DS1307_Init();																		// Initialize the device
uint8_t DS1307_Read_Data(uint8_t address);												// Read the value at the selected address
void DS1307_Update_Time(uint8_t second, uint8_t minute, uint8_t hour);					// Function for updating the time	
void DS1307_Update_Date(uint8_t day ,uint8_t date, uint8_t month, uint8_t year);		// Function for updating the date
void DS1307_12_Hour_Mode();																// Choose the 12-hour mode
uint8_t isAMPMbit();																	// Return time indicators: 1 - PM, 0 - AM	
/*************END FOR UTILITY FUNCTION************/

#endif /* RTC_DS1307_H_ */