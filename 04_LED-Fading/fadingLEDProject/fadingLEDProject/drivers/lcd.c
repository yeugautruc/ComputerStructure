#include "../inc/lm4f120h5qr.h"
#include "lcd.h"

void lcd_delay() {
	volatile unsigned long ulLoop;
	ulLoop = 200;
	while(ulLoop--);
}

void clear_delay() {
	volatile unsigned long ulLoop;
	ulLoop = 50000;
	while(ulLoop--);
}

void lcd_delay_2() {
	volatile unsigned long ulLoop;
	ulLoop = 1000000;
	while(ulLoop--);
}

void shiftOut(int val)
{
    int i;
	  for (i = 0; i < 8; i++)  {
       	int res = (val & (1 << (7 - i)));
				if(0 == res) {
					GPIO_PORTB_DATA_R &= ~(0x80); // EBP: LCD-MOSI -> LOW
				} else {
					GPIO_PORTB_DATA_R |= 0x80;	  // EBP: LCD-MOSI -> HIGH
				}
				// Zeichen liegt an, übenehme bitte
				GPIO_PORTB_DATA_R |= 0x10;   		// EBP: LCD-SCK -> HIGH
				GPIO_PORTB_DATA_R &= ~(0x10);		// EBP: LCD-SCK -> LOW
    }
}

void lcdCMD(int data) {
	// cmd = 0, also auf port LCD-RS 0 setzen
	GPIO_PORTA_DATA_R &= ~(0x4); 		// EBP: LCD-RS -> LOW
	shiftOut(data);
	lcd_delay(); 
}

void nextline() {
  lcdCMD(0xc0); 	
}

void lcdDATA(char text) {
				// data = 1, also auf port LCD-RS 1 setzen
				GPIO_PORTA_DATA_R |= 0x4; 		// EBP: LCD-RS -> HIGH
				shiftOut(text);
        lcd_delay();

}

int lcdPRINTF(char text) {
				if('\n' == text) {
					nextline();
					return 1;
				}
				// data = 1, also auf port LCD-RS 1 setzen
				GPIO_PORTA_DATA_R |= 0x4; 		// EBP: LCD-RS -> HIGH
				shiftOut(text);
        lcd_delay();
				return 1;
}

void lcdClear() {
	lcdCMD(0x01);
	clear_delay();
	lcdCMD(0x02);
	clear_delay();
}

void lcd_init() {
  lcdCMD(0x30); // wake-up
  lcdCMD(0x30); // wake-up
  lcdCMD(0x30); // wake-up
  lcdCMD(0x39); // wake-up
  lcdCMD(0x14); // wake-up
  lcdCMD(0x56); // wake-up
  lcdCMD(0x6D); // wake-up
  lcdCMD(0x70); // wake-up
  lcdCMD(0x0c); // wake-up

  lcdClear();

	lcdDATA('L');
	lcdDATA('C');
	lcdDATA('D');
	lcdDATA('-');
	lcdDATA('T');
	lcdDATA('E');
	lcdDATA('S');
	lcdDATA('T');
	lcd_delay_2();
	lcdClear();
}
