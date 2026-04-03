/*
	lcd16x02.c
 */ 

#include "lcd16x02.h"

/*	Helper fuction lcdCommand()	*/
void LCD_Command(uint8_t data)
{
	COMMAND_PORT &= ~((1 << RS) | (1 << RW));			// RS = 0, RW = 0
	
	/* High nibble of uint8_t data */
	DATA_PORT = (DATA_PORT & 0x0F) | (data & 0xF0);		// high nibble of uint8_t data
	COMMAND_PORT |= (1 << EN);							// set EN HIGH
	_delay_us(450);
	COMMAND_PORT &= ~(1 << EN);							// falling edge on EN: HIGH -> LOW to enable signal
	_delay_us(10);
	
	/* Low nibble of uint8_t data */
	DATA_PORT = (DATA_PORT & 0x0F) | (data << 4);		// low nibble of uint8_t data
	COMMAND_PORT |= (1 << EN);							// set EN HIGH
	_delay_us(450);
	COMMAND_PORT &= ~(1 << EN);							// falling edge on EN: HIGH -> LOW to enable signal
	_delay_us(10);
}

/*	Initialize LCD16x02	*/
void LCD_Begin(void)
{
	DATA_DDR |= (1 << D4) | (1 << D5) | (1 << D6) | (1 << D7);		// set as output
	COMMAND_DDR |= (1 << EN) | (1 << RS) | (1 << RW);
	
	// After power on, setup 8-bit mode to synchronize with lcd16x02
	// -> to be stable at the beginning of initialization.
	LCD_Command(0x33);
	_delay_us(50);
	
	// set 4-bit mode
	LCD_Command(0x32);
	_delay_us(50);
	
	// DB7:0 = 0b00101000 -> 4 bit mode, 2-line display and 5x8 dots format display
	LCD_Command(0x28);
	_delay_us(50);
	
	// DB7:0 = 0b00001100 -> turn on entire display and cursor; don't blink cursor
	LCD_Command(0x0E);
	_delay_us(50);
	
	// DB7:0 = 0b00000001 -> clear display
	LCD_Command(0x01);
	_delay_ms(2);
	
	// Entry mode set -> cursor/ blink moves to right automatically
	LCD_Command(0x06);
}

/*	Clear LCD and set the cursor original position	*/
void LCD_Clear_Display()
{
	LCD_Command(0x01);		// clear display
	_delay_ms(2);
	LCD_Command(0x80);		// return home
}

/*	Move cursor to specific position	*/
void LCD_Set_Cursor(uint8_t row, uint8_t col)
{
	if (row == 0 && col < 16)
	LCD_Command((0x0F & col) | 0x80);					// first row
	else if (row == 1 && col < 16)
	LCD_Command((0x0F & col) | (0x80 | 0x40));			// second row
}

void LCD_Print_Character(uint8_t data)
{
	
	/*	Write data to RAM	*/
	COMMAND_PORT &= ~(1 << RW);							// RS = 1, RW = 0
	COMMAND_PORT |= (1 << RS);
	
	DATA_PORT = (DATA_PORT & 0x0F) | (data & 0xF0);		// high nibble of uint8_t data
	COMMAND_PORT |= (1 << EN);							// set EN HIGH
	_delay_us(450);
	COMMAND_PORT &= ~(1 << EN);							// falling edge on EN: HIGH -> LOW to enable signal
	_delay_us(10);
	DATA_PORT = (DATA_PORT & 0x0F) | (data << 4);		// low nibble of uint8_t data
	COMMAND_PORT |= (1 << EN);							// set EN HIGH
	_delay_us(450);
	COMMAND_PORT &= ~(1 << EN);							// falling edge on EN: HIGH -> LOW to enable signal
	_delay_us(10);
}

/*	Display string on LCD16x02	*/
void LCD_Print_String(const char* str)
{
	uint8_t i = 0;
	while (str[i] != '\0')			// loop until character is NULL
	{
		LCD_Print_Character(str[i]);
		++i;
	}
}

/*	Display string at any positions on LCD16x02	*/
void LCD_Print_String_At_Position(uint8_t row, uint8_t col, const char* str)
{
	LCD_Set_Cursor(row, col);
	LCD_Print_String(str);
}

/*	Shift all the display to the right	*/
void LCD_Scroll_Display()
{
	// Entry mode set -> shift of entire display right
	LCD_Command(0x1C);
	_delay_ms(100);
}

/*	Create a custom character */
void LCD_Create_Character(uint8_t characterCode, uint8_t customChar[])
{
	LCD_Command(0x40 | (characterCode << 3));				// Set CGRAM address -> DB5:3 = characterCode << 3, DB2:0 = 0 (Start at line 0 in 5x8 matrix)
	for (uint8_t i = 0; i < 8; ++i)							// Write data into each row in 5x8 matrix
	{
		LCD_Print_Character(customChar[i]);
		_delay_us(40);
	}
	
	LCD_Set_Cursor(0, 0);										// NOTE: Move the cursor back to the DDRAM memory area
}
