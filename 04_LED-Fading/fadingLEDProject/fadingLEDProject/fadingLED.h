#ifndef __FADING_LED_H__
#define __FADING_LED_H__

#include "inc/hw_types.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/lm4f120h5qr.h"

#include "drivers/adc.h"
#include "drivers/lcd.h"
#include "drivers/gpio.h"
#include "drivers/timer.h"
#include "drivers/sysctl.h"
#include "drivers/interrupt.h"


#define LED_GRN	GPIO_PIN_7	//0x80	//PORT A
#define LED_RED	GPIO_PIN_6	//0x40	//PORT A
#define LED_BLU   GPIO_PIN_3	//0x08	//PORT A

#define LED_WHT   (LED_RED|LED_GRN|LED_BLU) //0xC8
#define LED_YEL   (LED_RED|LED_GRN)         //0xC0
#define LED_CYA   (LED_GRN|LED_BLU)         //0x88
#define LED_MAG   (LED_BLU|LED_RED)         //0x48 

#define LCD_D_C   GPIO_PIN_2	//0x04	//PORT A
#define LCD_SCK	GPIO_PIN_4	//0x10	//PORT B
#define LCD_MOS	GPIO_PIN_7	//0x80	//PORT B

#define POTI		GPIO_PIN_4	//0x10	//PORT E

#define BTN_ONE	GPIO_PIN_4	//0x10	//PORT F
#define BTN_TWO	GPIO_PIN_0	//0x01	//PORT F

//SysCtlClock isn't set here. Initial value is PLL (400 MHz)
#define PWM_PERIOD (SysCtlClockGet()/20000) //PWM_PERIOD = 20kHz

//ADC Register Bitmask 
typedef struct
{
int ADCACTSS;
int ADCRIS;
int ADCIM;
int ADCISC;
int ADCOSTAT;
int ADCDEMUX;
int ADCNIX[12];
int ADCSSFIFO0;
int ADCNIX1[7];
int ADCSSFIFO1;
int ADCNIX2[7];
int ADCSSFIFO2;
int ADCNIX3[7];
int ADCSSFIFO3;
int ADCNIX4[7];
} tADC; 


typedef struct
{
	int value;
} tPoti;

int Debounce(void);
void setup(void);
void getValueADC(tADC* adc, tPoti* poti);
void Timer0AIntHandler(void);
void GPIOPORTFIntHandler(void);

#endif //__FADING_LED_H__
