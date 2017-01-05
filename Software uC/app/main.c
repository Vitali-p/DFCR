#include <intrinsics.h>
#include <stdio.h>
#include "board.h"
#include "sys.h"
#include "ADC.h"
//#include "sdram_64M_32bit_drv.h"

#include "smb380_drv.h"
#include "Timer.h"

Int32U _ADCVal;
Int32U  _ADCStatus;
Int32U _regCount = 0;
Int32U  _waveForm[1000];

#define NONPROT 0xFFFFFFFF
#define CRP1  	0x12345678
#define CRP2  	0x87654321
/*If CRP3 is selected, no future factory testing can be performed on the device*/
#define CRP3  	0x43218765


/*************************************************************************
 * Function Name: Timer0IntrHandler
 * Parameters: none
 * Return: none
 * Description: Timer 0 interrupt handler
 *************************************************************************/
void Timer0IntrHandler (void)
{
  USB_D_LINK_LED_FIO ^= USB_D_LINK_LED_MASK; // Toggle USB Link LED
  
  T0IR_bit.MR0INT = 1; // Clear the timer 0 interrupt.
  VICADDRESS = 0;
  _ADCVal = ((AD0GDR & 0xFFC0)>>6);
  _waveForm[_regCount] = _ADCVal;
  _regCount++;
  if(_regCount == 1000){
  _regCount = 0;
  }
}

 void AD0IntrHandler (void) {
//   _ADCVal = ((AD0GDR & 0xFFC0)>>6);
//   _ADCStatus = ADSTAT;
 }

int main(void)
{
  // Init GPIO
  GpioInit();

  // MAM init
  MAMCR_bit.MODECTRL = 0;
  MAMTIM_bit.CYCLES  = 3;   // FCLK > 40 MHz
  MAMCR_bit.MODECTRL = 2;   // MAM functions fully enabled
  // Init clock
  InitClock();

  // Init VIC
  VIC_Init();

  // Init USB Link  LED
  USB_D_LINK_LED_FDIR = USB_D_LINK_LED_MASK;
  USB_D_LINK_LED_FSET = USB_D_LINK_LED_MASK;

  //Timer init ---
  Timer0Init(1);
  
  //Init timer 0 interrupt:
  VIC_SetVectoredIRQ(Timer0IntrHandler,0,VIC_TIMER0);
  VICINTENABLE |= 1UL << VIC_TIMER0;
  T0TCR_bit.CE = 1;     // Enable counting.
  initADC2();
  while(1){
  
  
  }
}
