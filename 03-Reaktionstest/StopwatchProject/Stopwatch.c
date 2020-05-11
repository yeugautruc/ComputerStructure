//matrikel1 70466827
//matrikel2 70465018


/*INCLUDE FILES*/
#include "lm4f120h5qr.h"
#include "inc\hw_memmap.h"
#include "inc\hw_types.h"
#include "inc\hw_ints.h"
#include "driverlib\systick.h"
#include "driverlib\interrupt.h"
#include "driverlib\sysctl.h"
#include "driverlib\gpio.h"
#include "lcd.h"
#include <stdio.h>


#include <time.h>
#include <stdlib.h> 		//rnd Function

/*DEFINES*/
#define GPIO_PORTF_BASE 0x40025000 // GPIO Port F
#define GPIO_LOCK_KEY_DD        0x4C4F434B  // Unlocks the GPIO_CR register on
                                            // DustDevil-class devices and
                                            // later
#define GPIO_O_LOCK 0x00000520 // GPIO Lock
#define GPIO_O_CR 0x00000524 // GPIO Commi

#define PA2 0x4  /* LCD-RS   */
#define LED_GREEN 0x80
#define LED_RED 0x40

#define PB4 0x10 /* LCD-SCK  */
#define PB7 0x80 /* LCD-MOSI */

#define BTN_RIGHT 0x1  /* BTN Right */
#define BTN_LEFT 0x10 /* BTN Left */

/*GLOBAL VARIABLES*/
int randomTime = 0;
int randomColor = 0;
int gameRunning_flag = 0;
int startGame_flag = 0;
int endGame_flag = 0;
int correct, incorrect= 0;
int s,t,m, ms;
int random;
int randomLED;

void init(void) 
{
	
	SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	// Enable Clock auf Port A + B +F
	SYSCTL_RCGC2_R = SYSCTL_RCGC2_GPIOA + SYSCTL_RCGC2_GPIOB + SYSCTL_RCGC2_GPIOF;

	// Right Button-Unlock BTN_RIGHT (0x01)
	*((volatile unsigned long *) (GPIO_PORTF_BASE + GPIO_O_LOCK)) = GPIO_LOCK_KEY_DD;
	*((volatile unsigned long *) (GPIO_PORTF_BASE + GPIO_O_CR)) |= 0x01;
	*((volatile unsigned long *) (GPIO_PORTF_BASE + GPIO_O_LOCK)) = 0;

	// Left+Right Button-Init BTN_LEFT (0x10)
	GPIO_PORTF_DIR_R &= ~(BTN_RIGHT+BTN_LEFT);  /* Set Button-Pin as Input-Pin */
	GPIO_PORTF_DEN_R |= BTN_RIGHT+BTN_LEFT;  	 /* Set Button-Pin as Digital-Pin */
	GPIO_PORTF_PUR_R |= BTN_RIGHT+BTN_LEFT;		 /* Set Pull-Up for Button-Pin */ 

	GPIOIntTypeSet(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4, GPIO_FALLING_EDGE);
	GPIOPinIntEnable(GPIO_PORTF_BASE, GPIO_PIN_0|GPIO_PIN_4);	

	// LCD-Init: Port A: PA2 (LCD-RS) , Port B: PB4 (SCK) und PB7 (MOSI)
	GPIO_PORTA_DIR_R |= PA2+LED_GREEN+LED_RED | 0xc8;
	GPIO_PORTA_DEN_R |= PA2+LED_GREEN+LED_RED | 0xc8;
	GPIO_PORTB_DIR_R |= PB4+PB7;
	GPIO_PORTB_DEN_R |= PB4+PB7;

	IntMasterEnable();
	IntEnable(INT_GPIOF);

}


void SystickInit()
{
	// Set up the period for the SysTick timer.  The SysTick timer period will
	// be equal to the system clock, resulting in a period of 1 second.
	SysTickPeriodSet(SysCtlClockGet()/100);

	// Enable SysTick.
	SysTickEnable();

	// Enable the SysTick Interrupt.
	SysTickIntEnable();  
}

/* Interrupt-Service-Routine */
void SysTickIntHandler(void)
{
	// Der Code der hier steht wird automatisch ausgeführt.
	ms++;
	/*  100tel Sekunde vorbei */
  if(ms  >= 100) { ms = 0; s++; }  
  /*  1 Sekunde vorbei */  
  if(s >= 60) {s = 0; m++;}
}

/* Interrupt-Service-Routine */
void GPIOIntHandler(void) { // interupt called when input output handler catch the change of button
	// if both button is hit
	if((GPIO_PORTF_DATA_R &(BTN_LEFT+BTN_RIGHT)) == 0){
		gameRunning_flag = 1; // set game running
		startGame_flag = 1;  // set start game
		endGame_flag =0; // set end game false
	}
		if(GPIO_PORTF_DATA_R &= BTN_RIGHT) // if right button is hit
		  {
			endGame_flag = 1; // set end game
				if(GPIO_PORTA_DATA_R &= LED_RED)
					incorrect = 0; // compare right corespond to red led, if true set incorrect = 0
				else incorrect = 1;
			}
			
		if(GPIO_PORTF_DATA_R &=BTN_LEFT) // if left button is hit
			{
			endGame_flag = 1;
				if(GPIO_PORTA_DATA_R &= LED_GREEN)
					incorrect = 0; // compare right corespond to red led, if true set incorrect = 0
				else incorrect = 1;
			}
			
	// Clear Interrupt
	GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_0);
	GPIOPinIntClear(GPIO_PORTF_BASE, GPIO_PIN_4);
}

void printDecimalAsBinary(decimal)
{
	int temp, bin = 0,ten=1;
	while(decimal!=0){
	temp = decimal%2;
	bin = bin + ten*temp;
	ten*=10;
  decimal/=2;		
	}
	printf("%07i",bin);//print decimal als binary in 7 digit
}

int main(void)
{
	init();			/* Initalisierung von Board-Pins */
	
	SystickInit();	/* SystemTick Initialization*/

	lcd_init(); 	/* Initalisierung des LCD */

	lcdClear();

	while(1) /* Endlosschleife */
	{ 	
		// Spiel läuft
		if(gameRunning_flag)
		{
			//Spielstart Sequenz
			if(startGame_flag)
			{
				
				lcdClear();
				
				printf("Spiel gestartet");

				random = 0;
				random = rand();  //ALTERNATIVE: abs(rand()*ms / 100000);
				
				randomColor = random % 2;
				randomTime = (random % 5)+1;
				
				correct = 0;
				s=-randomTime;
				m=0;
				ms = 0;
				startGame_flag = 0;
			}//End Speilstart Sequenz
			
			//Spielende Sequenz
			else if(endGame_flag)
			{
				lcdClear();
			
	if(incorrect == 0) // if incorrect = 1 will not come the result, if incorrect = 0; run the above code
		{correct =1;}
		
				if(correct)
				{
					int tempM = m;
					int tempS = s;
					int tempMS = ms;
					
					printf("%02i:%02i:%01i\n", tempM, tempS, tempMS);
					printDecimalAsBinary(tempS);
					printf(":");
					printDecimalAsBinary(tempMS);
				}
				else
				{
					printf("Falsche Eingabe");
				}
				gameRunning_flag = 0;
				endGame_flag = 0;
			}//End Spielende Sequenz
			
			//aktiviere LED
			else if(s >= 0 )
			{
				if(randomColor == 0)
					GPIO_PORTA_DATA_R = LED_GREEN;
				else
					GPIO_PORTA_DATA_R = LED_RED;
						}//End aktiviere LED
			
		}// End GameRunning
		
	}// End While(1)
	
}// End Main
