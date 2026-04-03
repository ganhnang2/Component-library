/*
	_24C64.c
 */ 

#include "24C64.h"
#include "TWI_Master.h"

// Initialize I2C protocol to transfer data
void IC24C64_Init()
{
	I2C_Master_Init();
}																			

// Write a byte at selected address															
void IC24C64_Write_One_Byte(uint8_t data, uint16_t address)
{
	I2C_Start();
	I2C_Master_SLA_W(IC24C64_ADDRESS);
	I2C_Write(address >> 8);
	I2C_Write(address);
	I2C_Write(data);
	I2C_Stop();
	_delay_ms(10);							// with focs = 100kHz, delay
}	

// Write sequence bytes 
void IC24C64_Write_Sequence_Bytes(uint8_t data[], uint16_t address, uint8_t numberOfDatas)	
{
	uint8_t i = 0;
	I2C_Start();
	I2C_Master_SLA_W(IC24C64_ADDRESS);
	I2C_Write(address >> 8);
	I2C_Write(address);
	while (i < numberOfDatas)
	{
		I2C_Write(data[i]);
		
		i++;
		
		// roll-over occurs -> Stop Write operation
		if ((address + i) % IC24C64_BYTES_PER_PAGE == 0)
			 break;
	}
	
	I2C_Stop();
	_delay_ms(10);
}														

// Read a byte at selected address
uint8_t IC24C64_Read_One_Byte(uint16_t address)
{
	uint8_t data;
	I2C_Start();
	I2C_Master_SLA_W(IC24C64_ADDRESS);
	I2C_Write(address >> 8);
	I2C_Write(address);
	I2C_Repeated_Start();
	I2C_Master_SLA_R(IC24C64_ADDRESS);	
	data = I2C_Read_NACK();
	I2C_Stop();
	return data;
}	

// Read sequence bytes into receivedData[]
void IC24C64_Read_Sequence_Byte(uint8_t receivedData[], uint16_t address, uint8_t numberOfDatas)
{
	I2C_Start();
	I2C_Master_SLA_W(IC24C64_ADDRESS);
	I2C_Write(address >> 8);
	I2C_Write(address);
	I2C_Repeated_Start();
	I2C_Master_SLA_R(IC24C64_ADDRESS);
	for (uint8_t i = 0; i < numberOfDatas; i++)
	{
		if (i != numberOfDatas - 1)
			receivedData[i] = I2C_Read_ACK();
		else
			receivedData[i] = I2C_Read_NACK();
	}
	I2C_Stop();
}
									