/*************************************************************************
 *    File name   : Timer.c
 *    Description : Timer main file.
 *
 *       Date        : 04.11.2017
 *       Author      : Vitali Parolia
 *       Description : Timer init doc.
 */

/*************************************************************************
 * Function Name: Timer0Init
 * Parameters: Timer ticker pr. second.
 * Return: none
 * Description: Initiazion of timer 0, and enables global interrupt.
 *************************************************************************/
void Timer0Init(int TIMER0_TICK_PER_SEC){
  // Enable TIM0 clock.
  PCONP_bit.PCTIM0 = 1; // Enable clock.

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
  if(TIMER0_TICK_PER_SEC > 0)
  {
     T0MCR_bit.MR0I = 1;   // Enable Interrupt on MR0
    //Match register value to make interrupt 1/10000s.
    T0MR0 = SYS_GetFpclk(TIMER0_PCLK_OFFSET)/(TIMER0_TICK_PER_SEC);
   
    T0IR_bit.MR0INT = 1;  // Clear pending interrupt.
    __enable_interrupt(); // Enable global interrup.
    
  }
 
}