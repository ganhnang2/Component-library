/* 
	Hàm để khởi tạo, truyền (transmit), nhận (receive) ký tự qua USART
	initUSART() yêu cầu BAUD được định nghĩa để tính toán hệ số nhân tốc
	độ truyền bit (bit-rate multiplier)
 */
 
#ifndef USART_RS232_H
#define USART_RS232_H

#ifndef BAUD                          /* Nếu không được định nghĩa trong Makefile... */
#define BAUD  9600                    /* Đặt một baud rate default... */
#endif

// Nếu file <util/delay.h> tồn tại thì undefine F_CPU
#ifdef F_CPU
#undef F_CPU
#endif

// redefine clock CPU: 12MHz
#define F_CPU 12000000UL

/* Định nghĩa để thuận lợi cho việc sử dụng về sau	*/
#define   USART_HAS_DATA   (UCSR0A & (1 << RXC0))
#define   USART_READY      (UCSR0A & (1 << UDRE0))

/* Nhận F_CPU và BAUD đã được định nghĩa, tính toán hệ số
	nhân bit-rate theo công thức trong datasheet - cái này tự
	được tính toán trong <hàm setbaud.h>			*/
void initUSART(void);

/* 	Khối hàm truyền (transmit) và nhận (receive) dữ liệu từ thanh ghi UDR0
	Khi gọi hàm receiveByte() thì chương trình của bạn sẽ bị treo
	đển khi có dữ liệu đến.		*/
void transmitByte(uint8_t data);
uint8_t receiveByte(void);

/* 	Hàm tiện ích để truyền toàn bộ chuỗi ký tự từ RAM	*/
void printString(const char myString[]);

/* 	Định nghĩa một biến char[] truyền vào hàm, biến này sẽ lưu trữ
	tất cả những gì bạn đã truyền qua cổng Serial	*/
void readString(char myString[], uint8_t maxLength);

/*	In byte thành một số ASCII 3-digits tương ứng 
	Ví dụ: byte = 56 -> Tương đương "0", "5", "6" trong mã ASCII và truyền
	bằng USART		*/
void printByte(uint8_t byte);

/*	In ra một word (16-bits) thành một số ASCII 5-digit tương ứng 	*/
void printWord(uint16_t word);

/* 	In ra một byte dưới dạng hệ nhị phân 0s và 1s	*/
void printBinaryByte(uint8_t byte);
                                     
/* 	In ra một byte dưới dạng hệ thập phân	*/								 
char nibbleToHexCharacter(uint8_t nibble);
void printHexByte(uint8_t byte);
                               
/*	Nhận 3 chữ cái ASCII và chuyển chúng thành một byte khi nhấn Enter ('\r')	*/
uint8_t getNumber(void);


#endif