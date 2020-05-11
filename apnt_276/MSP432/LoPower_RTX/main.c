/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP432 CODE EXAMPLE DISCLAIMER
 *
 * MSP432 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see http://www.ti.com/tool/mspdriverlib for an API functional
 * library & https://dev.ti.com/pinmux/ for a GUI approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP432P401 Demo - Enter LPM3 with ACLK = REFO
//
//  Description: MSP432 device is configured to enter LPM3 mode with GPIOs properly 
//  terminated. P1.1 is configured as an input. Pressing the button connect to P1.1
//  results in device waking up and servicing the Port1 ISR. LPM3 current can be 
//  measured when P1.0 is output low (e.g. LED off). 
// 
//  ACLK = 32kHz, MCLK = SMCLK = default DCO
//
//
//               MSP432p401rpz
//            -----------------
//        /|\|                 |
//         | |                 |
//         --|RST              |
//     /|\   |                 |
//      --o--|P1.1         P1.0|-->LED
//     \|/
//
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************

#include "msp.h"
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX

int main(void)
{   
	//void SystemInit (void);
	SystemCoreClockUpdate();
	  /* Hold the watchdog */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;         // Halt the WDT

	  //P1->DIR = ~(uint8_t) ~(BIT1);
	  P1->DIR = ~(uint8_t) BIT1;								// puts 0xFD into P1->DIR
    P1->OUT = BIT1;
    P1->REN = BIT1;                           // Enable pull-up resistor (P1.1 output high)
    P1->SEL0 = 0;
    P1->SEL1 = 0;
    P1->IFG = 0;                              // Clear all P1 interrupt flags
    P1->IE = BIT1;                            // Enable interrupt for P1.1
    P1->IES = BIT1;                           // Interrupt on high-to-low transition

	  // Enable Port 1 interrupt on the NVIC
   NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);

    /* Configure Port J */
    PJ->DIR |= (BIT2 | BIT3); PJ->OUT &= ~(BIT2 | BIT3);

    /* PJ.0 & PJ.1 configured for XT1 */
    PJ->SEL0 |= BIT0 | BIT1;
    PJ->SEL1 &= ~(BIT0 | BIT1);
	
		// Terminate all remaining pins on the device
    P2->DIR |= 0xFF; P2->OUT = 0;
    P3->DIR |= 0xFF; P3->OUT = 0;
    P4->DIR |= 0xFF; P4->OUT = 0;
    P5->DIR |= 0xFF; P5->OUT = 0;
    P6->DIR |= 0xFF; P6->OUT = 0;
    P7->DIR |= 0xFF; P7->OUT = 0;
    P8->DIR |= 0xFF; P8->OUT = 0;
    P9->DIR |= 0xFF; P9->OUT = 0;
    P10->DIR |= 0xFF; P10->OUT = 0;
		
   	/* Starting LFXT in non-bypass mode without a timeout. */
    CS->KEY = CS_KEY_VAL;
    CS->CTL1 &= ~(CS_CTL1_SELA_MASK | CS_CTL1_SELB);
    CS->CTL1 |= CS_CTL1_SELA__LFXTCLK;                // Source LFXTCLK to ACLK & BCLK
    CS->CTL2 &= ~(CS_CTL2_LFXTDRIVE_MASK);               // Configure to lowest drive-strength    
    CS->CTL2 |= CS_CTL2_LFXT_EN;
    while (CS->IFG & CS_IFG_LFXTIFG)
        CS->CLRIFG |= CS_IFG_LFXTIFG;
    CS->KEY = 0;
    /* Turn off PSS high-side & low-side supervisors */
    PSS->KEY = PSS_KEY_KEY_VAL;
		BITBAND_PERI(PSS->CTL0, PSS_CTL0_SVSMHOFF_OFS) = 1;
		BITBAND_PERI(PSS->CTL0, RSTCTL_PSSRESET_STAT_SVSL_OFS) = 1;
    PSS->KEY = 0;
		
		// ToDo: check RTC CTL0 access
    //Configure RTC
	  RTC_C->CTL0 = RTC_C_KEY_VAL;                 // Unlock RTC key protected registers
	  RTC_C->CTL13 = RTC_C_CTL13_BCD | RTC_C_CTL13_HOLD;
	  RTC_C->PS0CTL = RTC_C_PS0CTL_RT0PSIE | RTC_C_PS0CTL_RT0IP__16;				 // Prescaler-0 wake up

	  RTC_C->CTL13 &= ~(RTC_C_CTL13_HOLD);         // Start RTC calendar mode
    RTC_C->CTL0 = 0;                             // Lock the RTC registers

	while(1) {
		osDelay(500);
		P1OUT ^= BIT0; 
	}
	
}

/* Port1 ISR */
void PORT1_IRQHandler(void)
//void Port1Handler(void)
{
    volatile uint32_t i, status;
	//volatile uint32_t sleep;
    /* Toggling the output on the LED */

	if(P1->IFG & BIT1)
        P1->OUT ^= BIT0;
		
    /* Delay for switch debounce */
    for(i = 0; i < 10000; i++)

    P1->IFG &= ~BIT1;

}
