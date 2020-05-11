   void Init(void {
	 
	 
	 /* Hold the watchdog */
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;
    /* Configuring P1.0 as output and P1.1 (switch) as input with pull-up resistor*/
    /* Rest of pins are configured as output low */
    /* Notice intentional '=' assignment since all P1 pins are being deliberately configured */
	   
	  //P1->DIR = ~(uint8_t) ~(BIT1);
	  P1->DIR = ~(uint8_t) BIT1;								// puts 0xFD into P1->DIR
    P1->OUT = BIT1;
    P1->REN = BIT1;                           // Enable pull-up resistor (P1.1 output high)
    P1->SEL0 = 0;
    P1->SEL1 = 0;
    P1->IFG = 0;                              // Clear all P1 interrupt flags
    P1->IE = BIT1;                            // Enable interrupt for P1.1
    P1->IES = BIT1;                               // Interrupt on high-to-low transition

    // Enable Port 1 interrupt on the NVIC
    NVIC->ISER[1] = 1 << ((PORT1_IRQn) & 31);

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

    /* Configure Port J */
    PJ->DIR |= (BIT2 | BIT3); PJ->OUT &= ~(BIT2 | BIT3);

    /* PJ.0 & PJ.1 configured for XT1 */
    PJ->SEL0 |= BIT0 | BIT1;
    PJ->SEL1 &= ~(BIT0 | BIT1);

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

    /* Enable PCM rude mode, which allows to device to enter LPM3 without waiting for peripherals */
    PCM->CTL1 = PCM_CTL1_KEY_VAL | PCM_CTL1_FORCE_LPM_ENTRY;


    /* Enable all SRAM bank retentions prior to going to LPM3  */
    SYSCTL->SRAM_BANKRET |= SYSCTL_SRAM_BANKRET_BNK7_RET;
    __enable_interrupt();
    SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk;        // Do not wake up on exit from ISR
			
    /* Setting the sleep deep bit */
    //SCB->SCR |= (SCB_SCR_SLEEPDEEP_Msk);
    /* Go to LPM3 */
   //	__wfi();
		//__sleep();
}
	 }
