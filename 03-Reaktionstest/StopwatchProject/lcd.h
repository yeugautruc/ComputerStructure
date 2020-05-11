// the #ifndef prevents the file from being included more than once
#ifndef __LCD_H__   
#define __LCD_H__
 
void lcd_delay(void);
void clear_delay(void);
void lcd_delay_2(void);
void shiftOut(int );
void lcdCMD(int );
void lcdDATA(char );
int lcdPRINTF(char );
void lcdClear(void);
void nextline(void);
void lcd_init(void);
 
#endif // __LCD_H__
