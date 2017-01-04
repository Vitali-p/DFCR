/*************************************************************************
 *    File name   : main.c
 *    Description : Main module
 *
 *       Date        : 
 *       Author      : 
 *       Description : 
 *
 * This example project shows how to use the IAR Embedded Workbench for ARM
 * to develop code for IAR-LPC-2478 board. It shows basic use of I/O,
 * timer, interrupt and LCD controllers.
 *
 * The IAR, NXP and Olimex logos appear on the LCD and the cursor
 * moves as the board moves(the acceleration sensor is used).
 *
 * Jumpers:
 *  EXT/JLINK  - depending of power source
 *  ISP_E      - unfilled
 *  RST_E      - unfilled
 *  BDS_E      - unfilled
 *  C/SC       - SC
 *
 * Note:
 *  After power-up the controller get clock from internal RC oscillator that
 * is unstable and may fail with J-Link auto detect, therefore adaptive clocking
 * should always be used. The adaptive clock can be select from menu:
 *  Project->Options..., section Debugger->J-Link/J-Trace  JTAG Speed - Adaptive.
 *
 * The LCD shares pins with Trace port. If ETM is enabled the LCD will not work.
 *
 * Vision: 
 **************************************************************************/
#include <intrinsics.h>
#include <stdio.h>
#include "board.h"
#include "sys.h"
//#include "sdram_64M_32bit_drv.h"

#include "smb380_drv.h"
#include "Timer.h"
   
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
  Timer0Init(50);
  
  //Init timer 0 interrupt:
  VIC_SetVectoredIRQ(Timer0IntrHandler,0,VIC_TIMER0);
  VICINTENABLE |= 1UL << VIC_TIMER0;
  T0TCR_bit.CE = 1;     // Enable counting.
  
  while(1){}
}
