/*
*********************************************************************
*
*   IIR Oscillator and filter example
*
*   This example program generates a sinusoidal signal, disturbed by 
*   another sine signal of higher frequency.
*   An IIR low pass filter is then used to filter the disturbed signal
*   to reconstruct the original sine wave.
*
*   2011 - Tecnologix srl  Bruno Coppi  Milan, Italy
*
*********************************************************************
*/

#include <stdio.h>
#include "msp.h"
#include "cmsis_os.h" 

#include "arm_math.h"
#include "sine_generator.h"
#include "low_pass_filter.h"
#include "RTE_Components.h"             // Component selection

osThreadId tid_sine_gen;
osThreadId tid_noise_gen;
osThreadId tid_disturb_gen;
osThreadId tid_filter_tsk;
osThreadId tid_sync_tsk;

unsigned int t=1;		//for osDelay value in the threads

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <e>Oscillator Configuration
// <i>This is a demonstration on how to use the Configuration Wizard to create an easy configuration menu.

#define ENABLE_CONFIG 1
// =============================
//   <o>Oscillator Sampling Frequency [Hz] <1000-10000>
//   <i> Set the oscillator sampling frequency.
//   <i> Default: 5000  (5 KHz)
#define SAMPLING_FREQ 1000  // generating task (5 KHz)

//   <o>Noise Frequency [Hz] <50-10000>
//   <i> Set the noise signal frequency.
//   <i> Default: 1500 Hz
#define NOISE_FREQ    50  // noise (2 KHz)

//   <o>Signal Frequency [Hz] <10-1000>
//   <i> Set the signal frequency.
//   <i> Default: 330 Hz
#define SIGNAL_FREQ    10  // disturbed signal (250 Hz)

// </e>
//------------- <<< end of configuration section >>> -----------------------

sine_generator_q15_t Signal_set;
sine_generator_q15_t Noise_set;

q15_t sine;
q15_t noise;
q15_t disturbed;
q15_t filtered;


/*********************************************************************
*
* Sine Generator thread
*
*********************************************************************
*/

void sine_gen(void  const *argument)
{
  for (;;)
  {
		osSignalWait (0x0001, 0);
		osDelay(t);
    sine = sine_calc_sample_q15(&Signal_set) / 2;
		osSignalSet(tid_noise_gen, 0x0001);                 /* set signal to thread 'thread' */
  }
}


/*
*********************************************************************
*
* Noise Sine Generator thread
*
*********************************************************************
*/

void noise_gen(void  const *argument)
{
  for (;;)
  {
		osSignalWait(0x0001, 0);
		osDelay(t);
    noise = sine_calc_sample_q15(&Noise_set) / 6;
		osSignalSet(tid_disturb_gen, 0x0001);
  }
}

/*
*********************************************************************
*
* 1st Disturb Generator thread
*
* Adds Noise to Signal
*
*********************************************************************
*/

void disturb_gen(void const *argument)
{
  for (;;)
  {
		osSignalWait(0x0001, 0);
		osDelay(t);
    disturbed = sine + noise;
		osSignalSet(tid_filter_tsk, 0x0001);
  }
}

/*
*********************************************************************
*
* Filter thread
*
* Extract Signal from disturbed signal by filtering out Noise
*
*********************************************************************
*/

void filter_tsk(void const *argument)
{
  for (;;)
  {
		osSignalWait(0x0001, 0);
		osDelay(t);
    filtered = low_pass_filter(&disturbed); 
		osSignalSet(tid_sync_tsk, 0x0001);
  }
}

/*
*********************************************************************
*
* Synchronization thread
*
* Periodically starts the signal generation process
*
*********************************************************************
*/

void sync_tsk(void  const *argument)
{
  for (;;)
  {
		osDelay(t);
		osSignalWait(0x0001, osWaitForever);
		osSignalSet(tid_sine_gen, 0x0001);
  }
}

osThreadDef(sine_gen, osPriorityNormal, 1, 0);
osThreadDef(noise_gen, osPriorityNormal, 1, 0);
osThreadDef(disturb_gen, osPriorityNormal, 1, 0);
osThreadDef(filter_tsk, osPriorityNormal, 1, 0);
osThreadDef(sync_tsk, osPriorityNormal, 1, 0);

/*
*********************************************************************
*
* Initialization function
*
*********************************************************************
*/
void init() {
	  WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;           // Stop watchdog timer
	// The following code turns RED LED on: P1.0 port
    P1->DIR |= BIT0;               // Configure P1.0 as output
		P1->OUT = 1;
	// compute coefficients for IIR sine generators
	sine_generator_init_q15(&Noise_set, NOISE_FREQ, SAMPLING_FREQ);
  sine_generator_init_q15(&Signal_set, SIGNAL_FREQ, SAMPLING_FREQ);
	printf ("Sine Generator Initialised\n\r");
  
	// initialize low pass filter
  low_pass_filter_init();
	printf ("Low Pass Filter Initialised\n\r");
	
	//initialize the threads
	tid_sine_gen = osThreadCreate(osThread(sine_gen), NULL);
	printf ("sine_gen Thread Initialised\n\r");
  tid_noise_gen = osThreadCreate(osThread(noise_gen), NULL);
	printf ("noise_gen Thread Initialised\n\r");
  tid_disturb_gen = osThreadCreate(osThread(disturb_gen), NULL);
	printf ("disturb_gen Thread Initialised\n\r");
  tid_filter_tsk = osThreadCreate(osThread(filter_tsk), NULL);
	printf ("filter_tsk Thread Initialised\n\r");
  tid_sync_tsk = osThreadCreate(osThread(sync_tsk), NULL);
	printf ("sync_tsk Thread Initialised\n\r");
}


int main(void)
{
	osKernelInitialize ();
	SystemCoreClockUpdate();
	init();
	osKernelStart ();
	
  printf ("Application Running\n\r");
	osSignalSet(tid_sine_gen, 0x0001);
	osDelay(osWaitForever);
	while(1);

}

