/*
	DS1307.c	
*/

#define F_CPU	12000000UL				// Clock CPU: 12MHz
#include <avr/io.h>
#include <stdio.h>
#include <string.h>
#include "RTC_DS1307.h"
#include "OLED_128x64.h"

enum {SUNDAY = 1, MONDAY, TUESDAY , THURSDAY, WEDNESDAY , FRIDAY, SATURDAY};

int main(void)
{
	DS1307_Init();
	OLED_Init();
	OLED_Display_Mode(1);
	
	char buffer[100];
	uint8_t sec, minute, hour, day, date, month, year;
	
	/* Replace with your application code */
	while (1)
	{
		OLED_Clear_Buffer();
		
		// Display the time (Time: second:minute:hour)
		sec = DS1307_Read_Data(0x00);
		minute = DS1307_Read_Data(0x01);
		hour = DS1307_Read_Data(0x02);
		sprintf(buffer, "Time: %02d:%02d:%02d", hour, minute, sec);
		OLED_Print_String(buffer, 0, 0);
		
		// Display the date (Date: day-month-year)
		date = DS1307_Read_Data(0x04);
		month = DS1307_Read_Data(0x05);
		year = DS1307_Read_Data(0x06);
		sprintf(buffer, "Date: %02d - %02d - %02d", date, month, 2000 + year);
		OLED_Print_String(buffer, 0, 10);
		
		day = DS1307_Read_Data(0x03);
		switch(day)
		{
			case SUNDAY:
				OLED_Print_String("Day: Sunday", 0, 30);
				break;
			case MONDAY:
				OLED_Print_String("Day: Monday", 0, 30);
				break;
			case TUESDAY:
				OLED_Print_String("Day:Tuesday", 0, 30);
				break;
			case THURSDAY:
				OLED_Print_String("Day: Thursday", 0, 30);
				break;
			case WEDNESDAY:
				OLED_Print_String("Day: Wednesday", 0, 30);
				break;
			case FRIDAY:
				OLED_Print_String("Day: Friday", 0, 30);
				break;
			case SATURDAY:
				OLED_Print_String("Day: Saturday", 0, 30);
				break;	
		}	
		OLED_Update_Display();
	}
}

