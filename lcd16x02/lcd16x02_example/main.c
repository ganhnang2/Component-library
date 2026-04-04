/*
	lcd16x02_library.c
	Programming to display on LCD16x02 from the scratch.
	
	## Hardware
	|	AVR pin	|	LCD pin	|
	|-----------|-----------|
	|	PORTC0	|	RS		|
	|	PORTC1	|	R/W		|
	|	PORTC2	|	EN		|
	|	PORTD4	|	D4		|
	|	PORTD5	|	D5		|
	|	PORTD6	|	D6		|
	|	PORTD7	|	D7		|
	
	## Firmware
	First, I will initialize the LCD in 4-bit mode and define certain modes for consistent use 
	across this sketch
	Then, write a helper function lcdCommand(), this function takes an argument (8-bit) as an
	instruction, which MPU (LCD16x02) interprets to carry out the desired functionality.
	Finally, I have implemented basic fuctions as printString(), clearDisplay(), ...
	
	NOTE: When lcd16x02 operates at 4-bus mode, it needs to transfer 4-bit data twice:
	first from DB7 to DB4, then from DB3 to DB0.
	
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


int main(void)
{
    /*--------Inits--------*/
	LCD_Begin();
	LCD_Print_String("Nguyen Hai Nam");
	LCD_Print_String_At_Position(1, 0, "Hello World");
	
    while (1)
	{
		LCD_Scroll_Display();
	}
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

