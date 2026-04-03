/*
	lcd_utility.c
 */ 

#define F_CPU	12000000UL
#include <avr/io.h>
#include "lcd16x02.h"
#include <util/delay.h>

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
