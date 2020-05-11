// (c)Ostfalia - Hochschule für angewandte Wissenschaften
//	FakultätInformatik - Rechnerstrukturen Labor 
//	Aufgabe: Fading LED
//	matrikel1 70466827
//	matrikel2 70465018

#include "fadingLED.h"
#include <stdio.h>
#include <stdlib.h>


unsigned int ledChain[] = {LED_WHT,LED_RED,LED_YEL,LED_GRN,LED_CYA,LED_BLU,LED_MAG};
 int uChainCtr = 0;
unsigned int uBtn1Flag = 0;
unsigned int uBtn2Flag = 0;

//
// Main
//
int main(void)
{
	// init ADC Pointer to BaseRegister from ADC
	tADC *pADC =(tADC*) ADC0_BASE;
	// init Potimeter variable
	tPoti poti;
	// counter to update LCD 
	int counter = 0;
	// initializierung
  setup();	
	while(1)
	{	
		// set button flag again to 0
		
		
		getValueADC(pADC,&poti);
		//set Match Value depend on the value from Potimeter
		if(poti.value<100)
		{
			TIMER0_TAMATCHR_R = 25;
		}
		else if(poti.value >=4000) 
			{
			TIMER0_TAMATCHR_R = 999;
			} 
		else  
			{
			TIMER0_TAMATCHR_R = poti.value/4;
			//TIMER0_TAMATCHR_R <-Timer Match Register (Set Match Value)
			}
			// when the button 1, set the index of color chain down
			if(uBtn1Flag)
			{
				uChainCtr--;
				// when the index smaller than 0, set index to end of color chain
				if(uChainCtr<0)
				{
				uChainCtr=6;
				}
				// make delay to debounce
				uBtn1Flag=0;
				lcd_delay_2();
				
			}
			// when the button 2, set the index of color chain up
			if(uBtn2Flag)
			{
				uChainCtr++;
				// when the index greater than 6, set index to begin of color chain
				if(uChainCtr>6)
				{
				uChainCtr=0;
				}
				// make delay to debounce
				uBtn2Flag=0;
				lcd_delay_2();
		}
			// if 100k cycle come print the value of POTI and Timer Match Register / 10 = prozent value
		if(counter++ >= 100000)
		{
			lcdClear();
			printf("POTI: %d\n" ,poti.value);
			printf("DUTY: %03ld%%\n",TIMER0_TAMATCHR_R/10); 
			counter = 0;
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Get ADC Values from Register
//
void getValueADC(tADC* adc, tPoti* poti)
{
	ADCProcessorTrigger(ADC0_BASE, 0);
	poti->value = adc->ADCSSFIFO0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Software PWM Timer
//
void Timer0AIntHandler(void)
{  
	//when time out
	if((TIMER0_RIS_R) == TIMER_RIS_TATORIS)
	{
		GPIO_PORTA_DATA_R |=ledChain[uChainCtr];
		TIMER0_ICR_R |= TIMER_ICR_TATOCINT;
	} 
	//when match bitmash
	else if ((TIMER0_RIS_R)== TIMER_RIS_TAMRIS)
	{
		// all bitmuster  from LED off
		GPIO_PORTA_DATA_R &= ~(LED_RED|LED_GRN|LED_BLU);
		TIMER0_ICR_R |=TIMER_ICR_TAMCINT;
	}
	else {
		TIMER0_ICR_R |=TIMER_ICR_TAMCINT;//Clear MATCH-Interrupt-Flag
		TIMER0_ICR_R |=TIMER_ICR_TATOCINT;//Clear TIMEOUT-Interrupt-Flag
	}
}


//
// Button Interrupt
void GPIOPORTFIntHandler(void)
{
	// check if which button 
	uBtn1Flag = !(GPIO_PORTF_DATA_R & BTN_TWO);
	uBtn2Flag = !(GPIO_PORTF_DATA_R & BTN_ONE);
	GPIO_PORTF_ICR_R |= (BTN_ONE|BTN_TWO);//Clear BUTTON-Interrupt-Flag
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// Setup
//
void setup(void)
{
	//Set System Clock
	//Run System at 20MHz by using an external source on 16MHz and internal PLL on 400MHz 
	SysCtlClockSet(SYSCTL_SYSDIV_10|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	
	/**********************************************************************************************/
	//Enable GIPOs, Timers, ADC
   
   SYSCTL_RCGC0_R |= SYSCTL_RCGC0_ADC0;
	SYSCTL_RCGC1_R |= SYSCTL_RCGC1_TIMER0;
	SYSCTL_RCGC2_R |= (SYSCTL_RCGC2_GPIOF|SYSCTL_RCGC2_GPIOE|SYSCTL_RCGC2_GPIOB|SYSCTL_RCGC2_GPIOA);
	
	/**********************************************************************************************/
	//Set LEDs
	
	GPIO_PORTA_DIR_R	|= (LED_RED|LED_GRN|LED_BLU);
	GPIO_PORTA_DEN_R	|= (LED_RED|LED_GRN|LED_BLU);
	GPIO_PORTA_DATA_R	&= ~(LED_RED|LED_GRN|LED_BLU); 
	
	/**********************************************************************************************/
	//Set Switches
		
	//Unlock PPORTF-NMI-Pins (PF0)
	GPIO_PORTF_LOCK_R	= GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R	|= 0x01; 
	GPIO_PORTF_LOCK_R = GPIO_LOCK_UNLOCKED;
	
	GPIO_PORTF_DIR_R &= ~(BTN_ONE|BTN_TWO); 
	GPIO_PORTF_DEN_R |= (BTN_ONE|BTN_TWO); 
	GPIO_PORTF_PUR_R |= (BTN_ONE|BTN_TWO); 
	
   GPIOIntTypeSet(GPIO_PORTF_BASE, BTN_ONE|BTN_TWO, GPIO_FALLING_EDGE);
	GPIOPinIntEnable(GPIO_PORTF_BASE, BTN_ONE|BTN_TWO);
	IntEnable(INT_GPIOF);
	
	/**********************************************************************************************/
	//Set Display
	
	GPIO_PORTA_DIR_R	|= LCD_D_C;
	GPIO_PORTA_DEN_R	|= LCD_D_C;
	
	GPIO_PORTB_DIR_R	|= (LCD_SCK|LCD_MOS);
	GPIO_PORTB_DEN_R	|= (LCD_SCK|LCD_MOS);
		
	lcd_init();
	
	/**********************************************************************************************/
	//Set Timer

	TIMER0_CTL_R       = 0; //Disable Timer
	TIMER0_CFG_R      |= TIMER_CFG_16_BIT; //Set Count Width
	TIMER0_TAMR_R     |= (TIMER_TAMR_TAMRSU | 
                         TIMER_TAMR_TAMIE  |
                         TIMER_TAMR_TACDIR |
                         TIMER_TAMR_TAMR_PERIOD); //Set Timer Mode(s)
	TIMER0_CTL_R      |= TIMER_CTL_TAEVENT_POS; //Set Triggered Edges
	TIMER0_IMR_R      |= (TIMER_IMR_TAMIM+TIMER_IMR_TATOIM); //Set Interrupt Modes
	TIMER0_TAILR_R     = PWM_PERIOD-1; //Set Count Range Value
	TIMER0_TAMATCHR_R  = 25; //Set Match Value	
	TIMER0_CTL_R      |= TIMER_CTL_TAEN;//Enable Timer

	IntEnable(INT_TIMER0A);
	
	/**********************************************************************************************/
	//Set ADC
	GPIO_PORTE_DIR_R &= ~(POTI);
   GPIO_PORTE_DEN_R &= ~(POTI);
   
   SysCtlADCSpeedSet(SYSCTL_ADCSPEED_125KSPS); //Set sample rate of ADC
   ADCSequenceDisable(ADC0_BASE, 0); //Disable ADC
	ADCSequenceConfigure(ADC0_BASE, 0, ADC_TRIGGER_PROCESSOR, 0); //Set trigger source & priority
	ADCSequenceStepConfigure(ADC0_BASE, 0, 0, ADC_CTL_CH9|ADC_CTL_END); //Set steps & ADC ch 
                                                                       //(ch9 = PE4->Poti)
   ADCSequenceEnable(ADC0_BASE, 0); //Enable ADC after configuring
	
	/**********************************************************************************************/
	//Set General Interrupts
		
	IntMasterEnable();
}
