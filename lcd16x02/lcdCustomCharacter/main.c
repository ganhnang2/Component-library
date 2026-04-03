/*
	lcdCustomCharacter.c
	In the sketch, a custom character is displayed on LCD16x2
	
	+) CGRAM Address
	---------------------------------------------------------------------------------
	|	RS	|	R/W	|	DB7	|	DB6	|	DB5	|	DB4	|	DB3	|	DB2	|	DB1	|	DB0	|
	---------------------------------------------------------------------------------
	|	0	|	0	|	0	|	1	|	AC5	|	AC4	|	AC3	|	AC2	|	AC1	|	AC0	|
	---------------------------------------------------------------------------------
	DB5:3: Character code
	DB2:0: Designate the character pattern position (Row in 5x8 matrix)
	NOTE: Once a character is defined, it is assigned to a corresponding character code and stored in CGROM Pattern 
	-> In essence, we have created a character to be stored in CGROM pattern with a specific character code 
	(similar to how the character 'A' has the code '65' in the ASCII table).
	
	+) Step to create a custom character 
	1. First, choose character code assigning to a custom character (DB5:3) and specific row to write in 5x8 matrix (DB2:0)
	2. Second, select the 5 bits (DB4:0 in Write Data to RAM) to write the appropriate data into a fixed row of the 5x8 pixel matrix
	Example: Write 01010 into row 0 of 5x8 pixel matrix
	3. Finally, use commands such as Set CGRAM address (with DB5:0 in step 1) and Write Data to RAM (with DB4:0 in step 2)
	4. From the subsequent operations, the cursor will automatically move to the next row of the 5x8 pixel character. 
	At this stage, data simply needs to be written into RAM.
	DONE!
	
	NOTE: Move the cursor back to the DDRAM memory area after creating a custom character
	
	Source: Nguyen Hai Nam
 */ 

#define F_CPU	12000000UL			// Clock CPU: 12MHz
#include <avr/io.h>
#include <util/delay.h>

/*	Define device pins	*/
#define DATA_PORT		PORTD
#define DATA_DDR		DDRD
#define D4				PORTD4			// DB4:DB7 = high nibble
#define D5				PORTD5
#define D6				PORTD6
#define D7				PORTD7

#define COMMAND_DDR		DDRC
#define COMMAND_PORT	PORTC
#define RS				PORTC0
#define RW				PORTC1
#define EN				PORTC2

/*	Prototype function	*/
void LCD_Begin(void);
void LCD_Command(uint8_t);
void LCD_Clear_Display();
void LCD_Set_Cursor(uint8_t, uint8_t);
void LCD_Print_Character(uint8_t);
void LCD_Print_String(const char*);
void LCD_Print_String_At_Position(uint8_t, uint8_t, const char*);
void LCD_Scroll_Display();
void LCD_Create_Character(uint8_t characterCode, uint8_t customChar[]);


int main(void)
{
	/*--------Inits--------*/
	LCD_Begin();

	/* Custom char set for alphanumeric LCD Module */
	unsigned char Character1[8] = { 0x00, 0x0A, 0x15, 0x11, 0x0A, 0x04, 0x00, 0x00 };
	unsigned char Character2[8] = { 0x04, 0x1F, 0x11, 0x11, 0x1F, 0x1F, 0x1F, 0x1F };
	unsigned char Character3[8] = { 0x04, 0x0E, 0x0E, 0x0E, 0x1F, 0x00, 0x04, 0x00 };
	unsigned char Character4[8] = { 0x01, 0x03, 0x07, 0x1F, 0x1F, 0x07, 0x03, 0x01 };
	unsigned char Character5[8] = { 0x01, 0x03, 0x05, 0x09, 0x09, 0x0B, 0x1B, 0x18 };
	unsigned char Character6[8] = { 0x0A, 0x0A, 0x1F, 0x11, 0x11, 0x0E, 0x04, 0x04 };
	unsigned char Character7[8] = { 0x00, 0x00, 0x0A, 0x00, 0x04, 0x11, 0x0E, 0x00 };
	unsigned char Character8[8] = { 0x00, 0x0A, 0x1F, 0x1F, 0x0E, 0x04, 0x00, 0x00 };
	
	LCD_Create_Character(0, Character1);		// Create a character with character code 0
	LCD_Create_Character(1, Character2);		// Create a character with character code 1
	LCD_Create_Character(2, Character3);		// Create a character with character code 2
	LCD_Create_Character(3, Character4);		// Create a character with character code 3
	LCD_Create_Character(4, Character5);		// Create a character with character code 4
	LCD_Create_Character(5, Character6);		// Create a character with character code 5
	LCD_Create_Character(6, Character7);		// Create a character with character code 6
	LCD_Create_Character(7, Character8);		// Create a character with character code 7
	
	LCD_Set_Cursor(0, 0);					// Line 0
	LCD_Print_String("Custom Character");
	LCD_Set_Cursor(1, 0);					// Line 1
	for (uint8_t i = 0; i < 8; ++i)		// Print all custom characters
	{
		LCD_Print_Character(i);
		LCD_Print_Character(' ');
	}

	
	/*-------------Loop events----------*/
	while (1);
}

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