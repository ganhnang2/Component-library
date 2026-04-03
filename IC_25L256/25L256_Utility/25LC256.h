/*
	_25LC256.h
	
	## Pin Connection
	-------------------------------------------------
	|	ATmega328 pin		|		25LC256 pin		|
	-------------------------------------------------
	|		SS#				|			CS#			|
	-------------------------------------------------
	|		MOSI			|			DI			|
	-------------------------------------------------
	|		MISO			|			DO			|
	-------------------------------------------------
	|		SCK				|			SCK			|
	-------------------------------------------------
	|		VCC				|		WP#, HOLD#		|
	-------------------------------------------------
	
	## Firmware
	In this sketch, i will create utility functions to interface with IC 25LC256.
 */ 

#include <avr/io.h>

#ifndef _25LC256_H_
#define _25LC256_H_

/*-------------Instruction Set------------------*/
#define EEPROM_READ				0b00000011				// Read data from memory array beginning at selected address
#define EEPROM_WRITE			0b00000010				// Write data to memory array beginning at selected address
#define EEPROM_WRDI				0b00000100				// Reset the write enable latch (disable write operations)
#define EEPROM_WREN				0b00000110				// Set the write enable latch (enable write operations)
#define EEPROM_RDRS				0b00000101				// Read Status register
#define EEPROM_WRSR				0b00000001				// Write Status register 

/*------------EEPROM Status Register-----------*/
#define EEPROM_WPEN				7						
#define EEPROM_BP1				3						// BP1:0: blocks are currently write-protected -> look up the table in datasheet
#define EEPROM_BP0				2						
#define EEPROM_WEL				1						// Write Enable Latch: 1 -> enable write, 0 -> disable write
#define EEPROM_WIP				0						// Write-In-Process: 1 -> a write is in progress, 0 -> no write is in progress.

#define EEPROM_BYTES_PER_PAGE			64				// 64 bytes per page
#define EEPROM_ADDRESS_BYTES_MAX		0x7FFF			// Maximum address: 0x7FFF

/*-------------------SPI Protocol---------------*/
#define SPI_DDR			DDRB
#define SPI_PORT		PORTB
#define SPI_PIN			PINB
#define SS				PORTB2			// SS (Slave Select)
#define MOSI			PORTB3			// MOSI (Master In Slave Out)
#define MISO			PORTB4			// MISO (Master Out Slave In)
#define SCK				PORTB5			// SCK (Serial Clock)

/*----------Utility Marco---------------*/
#define SS_SLECT		(SPI_PORT &= ~(1 << SS));
#define SS_DESELECT		(SPI_PORT |= (1 << SS));

/*-----------Prototype Function-------------*/
void SPI_Init();									// Initialize SPI protocol
uint8_t SPI_Transfer_Data(uint8_t data);			// Transfer (transmit/receive) data with IC
void EEPROM_Send_16Bit_Address(uint16_t address);	// Send selected address
uint8_t EEPROM_Read_Status(void);					// Read status register
void EEPROM_Write_Enable(void);						// Enable write operation
uint8_t EEPROM_Read_Byte(uint16_t address);			// Read one byte at selected address
uint16_t EEPROM_Read_Word(uint16_t address);		// Read one word at selected address
void EEPROM_Write_Byte(uint16_t address, uint8_t data);			// Write one byte at selected address
void EEPROM_Write_Word(uint16_t address, uint16_t word);		// Write one word at selected address
void EEPROM_Clear_All(void);						// Clear all data in 25LC256

#endif /* 25LC256_H_ */