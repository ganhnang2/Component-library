/*
	Spi_interface.c
 */ 

#define F_CPU		12000000UL		// Clock CPU: 12MHz
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <util/twi.h>
#include "25LC256.h"
#include "lcd16x02.h"
#include "USART_RS232.h"

/*	Display data on LCD 
	ch: character indicate selected mode ('r': Read, 'w': Write)	*/
void displayData(uint8_t address, uint8_t data, char ch)
{
	char buffer[20];
	sprintf(buffer, "Address: %d", address);
	LCD_Print_String_At_Position(0 ,0, buffer);
	if (ch == 'r')
	{
		sprintf(buffer, "Receive: %d", data);
		LCD_Print_String_At_Position(1, 0, buffer);
	}
	else
	{
		sprintf(buffer, "Transmit: %d", data);
		LCD_Print_String_At_Position(1, 0, buffer);
	}
}

int main(void)
{
	uint8_t address;
	uint8_t startAddress, endAddress;
	uint8_t receivedChar;
	uint8_t data;
    /*----------------Inits-----------------*/
	SPI_Init();
	LCD_Begin();
	initUSART();
	
	/*--------------Loop event-------------*/
    while (1) 
    {	
		printString(" [r] to read from memory\r\n");
		printString(" [w] to write byte to memory\r\n\r\n");
		printString("Enter an character: ");
		receivedChar = receiveByte();
		printString("\r\n");
		
		switch(receivedChar)
		{
			// receive data from selected addresses
			case 'r':	
				LCD_Set_Cursor(0, 0);
				printString("Enter an starting address: ");				// start address
				startAddress = getNumber();
				printString("\r\n");
				printString("Enter an ending address: ");				// end address
				endAddress = getNumber();
				printString("\r\n");
				
				// display value on LCD
				for (address = startAddress; address <= endAddress; address++)			
				{
					data = EEPROM_Read_Byte(address);
					displayData(address, data, receivedChar);
					_delay_ms(500);
					LCD_Clear_Display();
				}
				break;
			// transmit data into selected address
			case 'w':
				printString("Enter an selected address: ");
				address = getNumber();
				printString("\r\n");
				printString("Enter value on selected address: ");
				data = getNumber();
				EEPROM_Write_Byte(address, data);
				displayData(address, data, receivedChar);
				_delay_ms(500);
				break;
			default:
				address = 0;
				startAddress = 0;
				endAddress = 0;
				printString("Error, enter again!\r\n");
				LCD_Clear_Display();
				break;
		}
		
		receivedChar = '\0';		// reset
		LCD_Clear_Display();
    }
}

