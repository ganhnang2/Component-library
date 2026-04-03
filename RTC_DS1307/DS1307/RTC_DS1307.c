/*
	RTC_DS1307.c

 */ 
#include "RTC_DS1307.h"

// Initialize the device
void DS1307_Init()
{
	I2C_Master_Init();
}

// Read the value at the selected address
uint8_t DS1307_Read_Data(uint8_t address)
{
	uint8_t data;
	I2C_Start();
	I2C_Master_SLA_W(DS1307_ADDRESS);
	I2C_Write(address);
	I2C_Repeated_Start();
	I2C_Master_SLA_R(DS1307_ADDRESS);
	data = I2C_Read_NACK();
	
	// Special case: 12-hour mode
	if (data & (1 << DS1307_AMPM_BIT) && address == 0x02)
	{
		data = data & 0x1F;		// clear bit 6-5
	}
		
	data = BCD2DEC(data);
	I2C_Stop();
	return data;
}

// Function for updating the time
void DS1307_Update_Time(uint8_t second, uint8_t minute, uint8_t hour)
{
	I2C_Start();
	I2C_Master_SLA_W(DS1307_ADDRESS);
	I2C_Write(0x00);						// the starting address: 0x00
	
	// Send data
	DS1307_Transmit_Data(second);
	DS1307_Transmit_Data(minute);
	DS1307_Transmit_Data(hour);				// Default: 24h mode
	
	I2C_Stop();
}

// Function for updating the date
void DS1307_Update_Date(uint8_t day ,uint8_t date, uint8_t month, uint8_t year)
{
	I2C_Start();
	I2C_Master_SLA_W(DS1307_ADDRESS);
	I2C_Write(0x03);					// the starting address: 0x03
	
	// Send data
	DS1307_Transmit_Data(day);
	DS1307_Transmit_Data(date);
	DS1307_Transmit_Data(month);
	DS1307_Transmit_Data(year);
	
	I2C_Stop();
}

// Choose the 12-hour mode
void DS1307_12_Hour_Mode()
{
	I2C_Start();
	I2C_Master_SLA_W(DS1307_ADDRESS);
	I2C_Write(0x02);							// access the hour register
	
	DS1307_Transmit_Data(DS1307_AMPM_MODE);		// choose 12-hour mode
	I2C_Stop();
	/*	NOTE: The hours value must be re-entered whenever the 12/24-hour mode bit is changed	*/
}			

// Return time indicators: 1 - PM, 0 - AM	
uint8_t isAMPMbit()
{
	I2C_Start();
	I2C_Master_SLA_W(DS1307_ADDRESS);
	I2C_Write(0x02);				// the hour-register
	I2C_Repeated_Start();
	I2C_Master_SLA_R(DS1307_ADDRESS);
	uint8_t data = I2C_Read_NACK();
	I2C_Stop();
	return data & (1 << DS1307_AMPM_BIT);
}										
