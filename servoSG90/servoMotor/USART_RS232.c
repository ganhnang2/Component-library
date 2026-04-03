
/*
	Các hàm đơn giản giúp truyền thông nối tiếp hoạt động.

	Lưu ý rằng receiveByte() là hàm dạng blocking — nó sẽ ngồi chờ mãi mãi cho đến khi có 
	một byte dữ liệu đến. Nếu bạn đang làm điều gì đó thú vị hơn, bạn sẽ muốn triển khai 
	chức năng này bằng ngắt (interrupts).

	initUSART yêu cầu BAUDRATE phải được định nghĩa để tính toán hệ số nhân tốc độ truyền (bit-rate multiplier). 
	Giá trị 9600 là một mặc định hợp lý.

	NOTE: Có thể không hoạt động với vài chip AVR cũ như: Mega8, Mega16, Mega32, Tiny2313, ...
*/

#include <avr/io.h>
#include "USART_RS232.h"
#include <util/setbaud.h>

// Khởi tạo USART
void initUSART(void)
{
	// UBRR0H và UBRR0L được tính toán trong file <setbaud.h>
	UBRR0H = UBRRH_VALUE;
	UBRR0L = UBRRL_VALUE;
	
	// Chỉ thị tiền xử lý về việc có sử dụng chế độ double-speed hay không?
	#if USE_2X
	UCSR0A |= (1 << U2X0);
	#else
	UCSR0A &= ~(1 << U2X0);
	#endif

	// Khởi tạo 2 thanh ghi UCSR0B và UCSR0C
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);				// Receive, Transmit mode enable
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);				// 8 data bits, 1 stop bit, no parity bit
}

// Hàm truyền dữ liệu qua USART
void transmitByte(uint8_t data)
{
	// Đợi đến khi UDRE = 1 (hay là Data Register sẵn sàng nhận dữ liệu)
	while ((UCSR0A & (1 << UDRE0)) == 0x00){;}
	
	// USART Data Register store data
	UDR0 = data;
}

// Hàm nhận dữ liệu qua USART
uint8_t receiveByte(void)
{
	/* Đợi dữ liệu đến */
	// RXC: Bit 1 neu co du lieu chua duoc
	while(!(UCSR0A & (1 << RXC0)));
	
	// Nhận dữ liệu từ USART Data Register
	return UDR0;
}

/* 	Hàm tiện ích để truyền toàn bộ chuỗi ký tự từ RAM	*/
void printString(const char myString[])
{
	uint8_t i = 0;
	while (myString[i] != '\0')
	{
		transmitByte(myString[i]);
		++i;
	}
	
}

/* 	Định nghĩa một biến char[] truyền vào hàm, biến này sẽ lưu trữ
	tất cả những gì bạn đã truyền qua cổng Serial	*/
void readString(char myString[], uint8_t maxLength)
{
	char response;
	uint8_t i = 0;
	// i < maxLength - 1: Để ký tự cuối là ký tự NULL 
	while (i < (maxLength - 1)) 
	{                   
		response = receiveByte();		// Nhận một ký tự 
		transmitByte(response);         // Phản hồi lại lên terminal để xác nhận việc đã đọc thành công?
		if (response == '\r') 			// Nếu nhập ký tự return ('\r') thì dừng việc đọc                  
			break;
		else 
		{
		myString[i] = response;         // Thêm một ký tự vào chuỗi
		i++;
		}
	}
	myString[i] = 0;                    // Ký tự NULL
}

/*	In byte thành một số ASCII 3-digits tương ứng */
void printByte(uint8_t byte)
{
	transmitByte('0' + (byte / 100));					// Hàng trăm
	transmitByte('0' + ((byte / 10) % 10));				// Hàng chục
	transmitByte('0' + byte % 10);						// Hàng đơn vị
}

/*	In ra một word (16-bits) thành một số ASCII 5-digit tương ứng 	*/
void printWord(uint16_t word)
{
	transmitByte('0' + (word / 10000));                 // Hàng chục nghìn
	transmitByte('0' + ((word / 1000) % 10));           // Hàng nghìn
	transmitByte('0' + ((word / 100) % 10));            // Hàng trăm
	transmitByte('0' + ((word / 10) % 10));             // Hàng chục
	transmitByte('0' + (word % 10));                    // Hàng đơn vị
}

/* 	In ra một byte dưới dạng hệ nhị phân 0s và 1s	*/
void printBinaryByte(uint8_t byte)
{
	// MSB -> LSB
	for (uint8_t i = 8; i > 0; i--)
		transmitByte((byte & (1 << (i - 1)) ? '1' : '0'));
}

/* 	In ra một byte dưới dạng hệ thập phân	*/		
char nibbleToHexCharacter(uint8_t nibble)
{
	return (nibble < 10 ? '0' + nibble : 'A' + nibble - 10);
}

void printHexByte(uint8_t byte)
{
	uint8_t upperNibble = byte >> 4;
	uint8_t lowerNibble = byte & 0x0F;
	transmitByte(upperNibble >= 10 ? 'A' + (upperNibble - 10) : '0' + upperNibble);
	transmitByte(lowerNibble >= 10 ? 'A' + (lowerNibble - 10) : '0' + lowerNibble);
}
	
/*	Nhận 3 chữ cái ASCII và chuyển chúng thành một byte khi nhấn Enter ('\r')	*/
uint8_t getNumber(void)
{
	// Nhận 1 số 0 - 255 từ cổng Serial và chuyển chuỗi ký tự thành số tương ứng
	char hundreds = '0';
	char tens = '0';
	char ones = '0';
	char thisChar = '0';
	do 
	{                    
		// Dịch các hàng sang bên trái <-
		hundreds = tens;
		tens = ones;
		ones = thisChar;
		thisChar = receiveByte();                   // Nhận một ký tự mới
		transmitByte(thisChar);                     // Truyền lại ký tự vừa nhận được để xác nhận
	} while (thisChar != '\r');                     // Lặp đến khi gặp ký tự return 
	return (100 * (hundreds - '0') + 10 * (tens - '0') + ones - '0');
}

