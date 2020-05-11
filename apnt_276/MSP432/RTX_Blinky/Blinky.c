/*----------------------------------------------------------------------------
 *      RL-ARM - RTX
 *----------------------------------------------------------------------------
 *      Name:    BLINKY.C
 *      Purpose: RTX example program
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2006-2013 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/
#include "cmsis_os.h"                   // ARM::CMSIS:RTOS:Keil RTX
#include "msp.h"                   			// Device header

osThreadId tid_phaseA;                        /* Thread id of task: phase_a */
osThreadId tid_phaseB;                        /* Thread id of task: phase_b */
osThreadId tid_phaseC;                        /* Thread id of task: phase_c */
osThreadId tid_phaseD;                        /* Thread id of task: phase_d */
osThreadId tid_clock;                         /* Thread id of task: clock   */


/*----------------------------------------------------------------------------
  Function that initializes LEDs
 *----------------------------------------------------------------------------*/
void LED_Initialize(void) {
		P1->DIR |= BIT0;              // Configure P1.0 as output
		P2->DIR |= 7;                 // Configure P2.0-2 as output

		P1->OUT = 0;                  // RED OFF
		P2->OUT = 0;                  // RED, Blue and Green off  */
}

/*----------------------------------------------------------------------------
 *      Function 'signal_func' called from multiple threads
 *---------------------------------------------------------------------------*/
void signal_func (osThreadId tid)  {
  osSignalSet(tid_clock, 0x0100);           /* set signal to clock thread    */
  osDelay(500);                             /* delay 500ms                   */
  osSignalSet(tid_clock, 0x0100);           /* set signal to clock thread    */
  osDelay(500);                             /* delay 500ms                   */
  osSignalSet(tid, 0x0001);                 /* set signal to thread 'thread' */
  //osDelay(500);                             /* delay 500ms                   */
}

/*----------------------------------------------------------------------------
 *      Thread 1 'phaseA': Phase A output
 *---------------------------------------------------------------------------*/
void phaseA (void const *argument) {
  for (;;) {
    osSignalWait(0x0001, osWaitForever);    /* wait for an event flag 0x0001 */
    P1->OUT = 1;                // Turn RED LED1 on
		signal_func(tid_phaseB);    /* call common signal function   */
		P1->OUT = 0;  							// Turn RED LED1 off
  }
} 

/*----------------------------------------------------------------------------
 *      Thread 2 'phaseB': Phase B output
 *---------------------------------------------------------------------------*/
void phaseB (void const *argument) {
  
	for (;;) {
   	osSignalWait(0x0001, osWaitForever);    /* wait for an event flag 0x0001 */
		P2->OUT = 1;							// Turn RED LED2 on
		signal_func(tid_phaseC);  /* call common signal function   */
    P2->OUT = 0;							// Turn RED LED2 off
  }
}

/*----------------------------------------------------------------------------
 *      Thread 3 'phaseC': Phase C output
 *---------------------------------------------------------------------------*/
void phaseC (void const *argument) {
  for (;;) {
    osSignalWait(0x0001, osWaitForever);    /* wait for an event flag 0x0001 */
    P2->OUT = 2;							// Turn GREEN LED2 on
    signal_func(tid_phaseD);  /* call common signal function   */
    P2->OUT = 0;							// Turn GREEN LED2 off
  }
}

/*----------------------------------------------------------------------------
 *      Thread 4 'phaseD': Phase D output
 *---------------------------------------------------------------------------*/
void phaseD (void  const *argument) {
  for (;;) {
    osSignalWait(0x0001, osWaitForever);    /* wait for an event flag 0x0001 */
    P2->OUT = 4;							// Turn BLUE LED2 on
    signal_func(tid_phaseA);  /* call common signal function   */
    P2->OUT = 0;							// Turn BLUE LED2 off
  }
}

/*----------------------------------------------------------------------------
 *      Thread 5 'clock': Signal Clock
 *---------------------------------------------------------------------------*/
void clock (void  const *argument) {
  for (;;) {
    osSignalWait(0x0100, osWaitForever);    /* wait for an event flag 0x0100 */
    osDelay(80);                            /* delay 80ms                    */
  }
}

osThreadDef(phaseA, osPriorityNormal, 1, 0);
osThreadDef(phaseB, osPriorityNormal, 1, 0);
osThreadDef(phaseC, osPriorityNormal, 1, 0);
osThreadDef(phaseD, osPriorityNormal, 1, 0);
osThreadDef(clock,  osPriorityNormal, 1, 0);

/*----------------------------------------------------------------------------
 *      Main: Initialize and start RTX Kernel
 *---------------------------------------------------------------------------*/
int main (void) {
	
	SystemCoreClockUpdate();
  LED_Initialize();                         /* Initialize the LEDs           */

  tid_phaseA = osThreadCreate(osThread(phaseA), NULL);
  tid_phaseB = osThreadCreate(osThread(phaseB), NULL);
  tid_phaseC = osThreadCreate(osThread(phaseC), NULL);
  tid_phaseD = osThreadCreate(osThread(phaseD), NULL);
  tid_clock  = osThreadCreate(osThread(clock),  NULL);

  osSignalSet(tid_phaseA, 0x0001);          /* set signal to phaseA thread   */

  osDelay(osWaitForever);
  while(1);
}
