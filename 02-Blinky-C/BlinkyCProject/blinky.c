//matrikel1 70466827
//matrikel2 70465018

#include "lm4f120h5qr.h"
#include "lcd.h"
#include <stdio.h>
#include <stdlib.h> 

#define PA2 0x4  //LCD-RS
#define PB4 0x10 //LCD-SCK
#define PB7 0x80 //LCD-MOSI

#define PF0 0x01   //BTN Right
#define PF4 0x10  //BTN Left

#define PA1 0x8   // blue led
#define PA0 0x40	// red led
#define PA3 0xC0	// yellow led
#define PA4 0x80	// green led
#define PA5 0x48	// violet led
#define PA6 0xC8	// white led

#define GPIO_PORTF_BASE 	0x40025000 // GPIO Port F
#define GPIO_LOCK_KEY_DD 	0x4C4F434B // Unlocks the GPIO_CR register on
#define GPIO_O_LOCK 		0x00000520 // GPIO Lock
#define GPIO_O_CR 			0x00000524 // GPIO Commi


//left button
int button() 
{	
while(GPIO_PORTF_DATA_R & PF4); //wait till the left button is hit
	lcd_delay_2();									//delay
	lcdClear();											//clear LCD 
	return 1;
}

//right button
int button2() 
{
	while(GPIO_PORTF_DATA_R & PF0);//wait till the right button is hit
	lcd_delay_2();									//delay
	lcdClear();										 //clear LCD 
	return 1;
}


void init(void) 
{
	// Enable Clock auf Port A + B +F
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOA + SYSCTL_RCGC2_GPIOB + SYSCTL_RCGC2_GPIOF;

	// Right Button-Unlock PF0 (0x01)
	*((volatile unsigned long *) (GPIO_PORTF_BASE + GPIO_O_LOCK)) = GPIO_LOCK_KEY_DD;
	*((volatile unsigned long *) (GPIO_PORTF_BASE + GPIO_O_CR)) |= 0x01;
	*((volatile unsigned long *) (GPIO_PORTF_BASE + GPIO_O_LOCK)) = 0;
	// todo: Left+Right Button-Init

	//Set Button-Pin as Input-Pin 
	GPIO_PORTF_DIR_R &=~(PF0);	// make PF0 input 
  GPIO_PORTF_DIR_R &=~(PF4);	// make PF4 input 
	//Set Button-Pin as Digital-Pin 
	GPIO_PORTF_DEN_R |= PF0;
	GPIO_PORTF_DEN_R |= PF4;
	//Set Pull-Up for Button-Pin 
  GPIO_PORTF_PUR_R |= PF0; // enable pull up resistors on PF0(right button) and PF4 (left button)
	GPIO_PORTF_PUR_R |= PF4;
	// 

	// todo: LCD-Init: Port A: PA2 + LED init(LCD-RS)  

	GPIO_PORTA_DIR_R |= PA2;
	GPIO_PORTA_DEN_R |= PA2;
	//set all bitmuster from LED-Color to output from port A
	GPIO_PORTA_DIR_R |= PA6;
	GPIO_PORTA_DEN_R |= PA6;

	// LCD INIT Port B: PB4 (SCK) und PB7 (MOSI)
	GPIO_PORTB_DIR_R |= PB4+PB7;
	GPIO_PORTB_DEN_R |= PB4+PB7;
	
}

int main(void)
{
	//todo: check following in debug mode
	int a, b, i, j=10;
	float fa, pi=3.1415;
	char ca='1', cb[]={'a','b','\n', 0}, cc[]="Hallo";
	int random = 0;
	
	init();
	lcd_init();    //init the LCD

	lcdClear();
  printf("Click \nLeft Button"); //print into LCD 
	button();												//wait the left button is hit
	
	
	i = ( 1+5 )/2 * 3;	//value of (int)/2 = int *3 = int
	printf (" 1. = %i \n",i);//print i value into LCD with %i(print integer typ)
	button();	//wait the left button is hit

	
	i = 10*(1/0.2); //1/0.2 = 5 (int) * 10 = 50(int)
	printf (" 1. = %i \n",i);//print i value into LCD with %i(print integer typ)
	button();	//wait the left button is hit

	i = 10*(1.0/5);						// 1.0/5 = 0.2 * 10 get 2.0 
	printf (" 2. = %i \n",i);//print i value into LCD with %i(print integer typ)
	button();	//wait the left button is hit
	
	i = 10*(7.0/9);	// here the value of i ist 7,777
	i = (7.0/9);// here the value of i ist 0,777
	printf (" 3. = %i \n",i);//print i value into LCD, result ist 0 because of %i(print integer typ)
	button();	//wait the left button is hit

	a = 6;
	b = a++; //b take the value of a (b = 6), increase a after b take a(a++), so the value of a now is 7
	printf (" 4. = %i , %i  \n",a,b);////print a and b value into LCD  with %i(print integer typ)
	button();	//wait the left button is hit
	
	a = 6;
	b = ++a;//increase a before b take a(++a), so the value of b now is 7, and a now is 7
	printf (" 5. = %i , %i  \n",a,b);//print into LCD with %i(print integer typ)
	button();	//wait the left button is hit

	i = 5;
	j = 7;
	if (i=j) printf (" 6.  i = %i und\nj = %i \n",i,j);// set i = j(=7), print i and j in integer
	button();
	
	while (i<10) //till i <10, run the while loop
	{
		printf (" %i . = %i \n", i ,i); //print into LCD value of i(from 5 till 9) with %i(print integer typ)
		i++; //increase the value of i
		button();	//wait the left button is hit
	}	 

	fa = pi*(1/3.0);
	printf (" 10.  fa = %f\n\n",fa); //print into LCD value of fa with float typ(%f) so resule in LCD is: 1.047
	button();	//wait the left button is hit

	printf ("%c. %s \n",ca,cc);//print into LCD character '1'(%c) and string cc(%s)
	button();	//wait the left button is hit
	lcdClear();
	printf("DICE PLAY...\nHit right button!");//print into LCD
	button2();	//wait the right button is hit
	lcdClear();
		//Wuerfel Endlosschleife
	while (1) 
	{
		random = rand()%6+1;
		printf("Result= , = %i \n", random);
		switch(random) 
      {
        case 1: 
            GPIO_PORTA_DATA_R = 0x8; // turn on the blue led
            break;
        case 2: 
            GPIO_PORTA_DATA_R = 0x40; // turn on the red led
            break;
        case 3: 
            GPIO_PORTA_DATA_R = 0xC0; // turn on the yellow led
            break;
				case 4:
            GPIO_PORTA_DATA_R = 0x80; // turn on the green led
            break;
				case 5: 
            GPIO_PORTA_DATA_R = 0x48; // turn on the violet led
            break;
				case 6: 
            GPIO_PORTA_DATA_R = 0xC8; // turn on the white led
						break;
        default: 
            break;
      }
			button2(); //wait to hit the right button
			lcdClear(); // clear LCD to the next print
	}	 
}
