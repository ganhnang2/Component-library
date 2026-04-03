/*
	_25LC526.c
 */ 

#include "25LC256.h"

// Initialize SPI protocol
void SPI_Init()
{
	// set pin mode
	SPI_DDR |= (1 << MOSI) | (1 << SS) | (1 << SCK);			
	SPI_DDR &= ~(1 << MISO);							
	
	SPI_PORT |= (1 << SS);				// SS# = HIGH -> SPI is inactive 	
	SPI_PORT |= (1 << MISO);			// enable resistor pull up
	
	// enable SPI, configure Master SPI and fSPI = fosc/16
	// CPOL = 0, CPHA = 0 -> look up the datasheet
	SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}	

// Transfer (transmit/receive) data with IC		
uint8_t SPI_Transfer_Data(uint8_t data)
{
	SPDR = data;
	
	while (!(SPSR & (1 << SPIF)));			// wait until trading data is complete
	return (SPDR);							// access SPDR to clear SPIF
}				

// Enable write operation
void EEPROM_Write_Enable(void)
{
	SS_SLECT								// SS# = LOW
	SPI_Transfer_Data(EEPROM_WREN);			// Send instruction WREN
	SS_DESELECT								// SS# = HIGH
}						

// Send selected address
void EEPROM_Send_16Bit_Address(uint16_t address)
{
	SPI_Transfer_Data(address >> 8);		// send high nibble -> low nibble
	SPI_Transfer_Data(address);
}

// Read status register
uint8_t EEPROM_Read_Status(void)
{
	uint8_t data;
	SS_SLECT										// SS# = LOW
	SPI_Transfer_Data(EEPROM_RDRS);					// Send instruction RDRS
	data = SPI_Transfer_Data(0);					// Write dummy data to generate SCK for receiving data
	SS_DESELECT										// SS# = HIGH
	return data;
}			

// Read one byte at selected address
uint8_t EEPROM_Read_Byte(uint16_t address)
{
	uint8_t data;
	SS_SLECT								// SS# = LOW
	SPI_Transfer_Data(EEPROM_READ);			// Send instruction READ
	EEPROM_Send_16Bit_Address(address);		// Send selected address
	data = SPI_Transfer_Data(0);			// Write dummy data to generate SCK for receiving data
	SS_DESELECT								// SS# = HIGH	
	return data;		// data is read from IC
}				

// Read one word at selected address
uint16_t EEPROM_Read_Word(uint16_t address)
{
	uint16_t eepromWorld;
	SS_SLECT
	SPI_Transfer_Data(EEPROM_READ);
	EEPROM_Send_16Bit_Address(address);
	eepromWorld = SPI_Transfer_Data(0);
	eepromWorld = (eepromWorld << 8);				// MSB
	eepromWorld |= SPI_Transfer_Data(0);			// LSB
	SS_DESELECT
	return eepromWorld;
}	

// Write one byte at selected address
void EEPROM_Write_Byte(uint16_t address, uint8_t data)
{
	EEPROM_Write_Enable();
	SS_SLECT
	SPI_Transfer_Data(EEPROM_WRITE);
	EEPROM_Send_16Bit_Address(address);
	SPI_Transfer_Data(data);
	SS_DESELECT;
	while ((EEPROM_Read_Status() & (1 << EEPROM_WIP)));		// wait until write operation in IC is complete
}

// Write one word at selected address
void EEPROM_Write_Word(uint16_t address, uint16_t word)
{
	EEPROM_Write_Enable();
	SS_SLECT
	SPI_Transfer_Data(EEPROM_WRITE);
	EEPROM_Send_16Bit_Address(address);
	SPI_Transfer_Data((uint8_t)(word >> 8));
	SPI_Transfer_Data((uint8_t) word);
	SS_DESELECT;
	while ((EEPROM_Read_Status() & (1 << EEPROM_WIP)));		// wait until write operation in IC is complete
}

// Clear all data in 25LC256
// NOTE: Test failure -> Need to modify
void EEPROM_Clear_All(void)
{
	uint8_t i;
	uint16_t pageAddress = 0;
	while (pageAddress < EEPROM_ADDRESS_BYTES_MAX)
	{
		EEPROM_Write_Enable();
		SS_SLECT
		SPI_Transfer_Data(EEPROM_WRITE);
		EEPROM_Send_16Bit_Address(pageAddress);
		for (i = 0; i < EEPROM_BYTES_PER_PAGE; i++)
		{
			SPI_Transfer_Data(0xFF);
		}		
		SS_DESELECT
		pageAddress += EEPROM_BYTES_PER_PAGE;
		while ((EEPROM_Read_Status() & (1 << EEPROM_WIP)));		// wait until write operation in IC is complete
	}
}