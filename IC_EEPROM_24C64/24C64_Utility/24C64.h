/*
	_24C64.h	
	
	## Write Operations  
	1. Enable WRITE operation by clear I2C_WRITE_CONTROL.
	2. Send START condition.
	3. Transmit SLA + R/W# and wait for the ACK response.
	4. After acknowledgement, send 2-byte selected address with MSB first transmitted.
	5. Generate a data byte on SDA and wait for the ACK response.
	6. Finally, send STOP condition to finish the transmission.
	
	NOTE: 
	+) The Page Write mode allows up to 32 bytes to be written in a single Write cycle
	+) After the Stop condition, the delay tW, and the successful completion of a Write operation
	With fOscl = 400kHz, tW = 5 - 10ms
	
	## Read Operations
	1. A dummy Write is first performed to load the address into this address counter
	2. Send REPEATED START condition and a byte including SLA + R/W# (bit R/W# = 1) 
	3. Master returns ACK for byte SLA + R/W#
	4. Receive a data byte from IC24C64
	5. Transmit NACK to finish the tranmission
	6. Send STOP condition
	NOTE: 
	+) Read operations are performed independently of the state of the Write Control (WC) signal.
	
	Source: Nguyen Hai Nam
 */


#ifndef _24C64_H_
#define _24C64_H_

#include <avr./io.h>
#include "TWI_Master.h"
#include <util/delay.h>

#define IC24C64_ADDRESS			0b01010000			// address: 0x50
#define IC24C64_BYTES_PER_PAGE	32					// 32 bytes/ page

/*-------------Prototype function----------------*/
void IC24C64_Init();																					// Initialize I2C protocol to transfer data
void IC24C64_Write_One_Byte(uint8_t data, uint16_t address);											// Write a byte at selected address
void IC24C64_Write_Sequence_Bytes(uint8_t data[], uint16_t address, uint8_t numberOfDatas);				// Write sequence bytes 
uint8_t IC24C64_Read_One_Byte(uint16_t address);														// Read a byte at selected address
void IC24C64_Read_Sequence_Byte(uint8_t receivedData[], uint16_t address, uint8_t numberOfDatas);		// Read sequence bytes into receivedData[]

#endif /* 24C64_H_ */