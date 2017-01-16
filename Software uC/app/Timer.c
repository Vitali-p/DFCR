/*************************************************************************
 *    File name   : Timer.c
 *    Description : Timer main file.
 *
 *       Date        : 04.11.2017
 *       Author      : Vitali Parolia
 *       Description : Timer init doc.
 **************************************************************************/
#include "board.h"
#include "sys.h"
#include "drv_touch_scr.h"

/*************************************************************************
 * Function Name: Timer0Init
 * Parameters: Timer ticker (pr. second.)
 * Return: none
 * Description: Initiazion of Timer 0, and enables global interrupt.
 *************************************************************************/
void Timer0Init(int TIMER0_TICK_PER_SEC){
  // Enable TIM0 clock.
  PCONP_bit.PCTIM0 = 1; // Enable TIM0 clock.

  // Init Time0
  T0TCR_bit.CE = 0;     // Counting is disabled.
  T0TCR_bit.CR = 1;     // Set reset time, relies on MR0 to reset the timer of the program.
  T0TCR_bit.CR = 0;     // Release reset.
  T0CTCR_bit.CTM = 0;   // Timer Mode: non set to every rising PCLK edge.
  
  T0MCR_bit.MR0I = 0;   // Disable Interrupt on MR0
  T0MCR_bit.MR0R = 1;   // Enable reset on MR0
  T0MCR_bit.MR0S = 0;   // Disable stop on MR0
  
  // Set timer 0 period
  T0PR = 0;   // The prescale register not set.
 
  //Interrupt timer 0 setup. Activate only interrupt if time-tick is active.
  if(TIMER0_TICK_PER_SEC > 0){
     T0MCR_bit.MR0I = 1;   // Enable Interrupt on MR0
    //Match register value to make interrupt 1/10000s.
    T0MR0 = SYS_GetFpclk(TIMER0_PCLK_OFFSET)/(TIMER0_TICK_PER_SEC);
    T0IR_bit.MR0INT = 1;  // Clear pending interrupt.
    __enable_interrupt(); // Enable global interrup.
  }
}

/*************************************************************************
 * Function Name: Timer1Init
 * Parameters: none
 * Return: none
 * Description: Initiazion of Timer 1, for touch screen.
 *************************************************************************/
void Timer1Init(){
// Init delay Timer 1
  PCONP_bit.PCTIM1 = 1; // Enable TIM1 clocks.
  T1TCR = 2;            // Stop and reset timer 1.
  T1CTCR_bit.CTM = 0;   // Timer Mode: every rising PCLK edge
  T1MCR_bit.MR0S = 1;   // stop timer 1 if MR1 matches the TC
  T1MCR_bit.MR0R = 1;   // enable timer 1 reset if MR1 matches the TC
  T1MCR_bit.MR0I = 1;   // Enable Interrupt on MR1
  T1PR = (SYS_GetFpclk(TIMER1_PCLK_OFFSET)/ 1000000) - 1; // 1us resolution
  T1MR0 = TS_SETUP_DLY;
  T1IR_bit.MR0INT = 1;  // clear pending interrupt
  VIC_SetVectoredIRQ(TimerIntr_Handler,TS_INTR_PRIORITY,VIC_TIMER1);
  VICINTENABLE_bit.TIMER1 = 1;
  T1TCR = 1;            // start timer 1
}
