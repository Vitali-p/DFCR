#include <intrinsics.h>
#include <stdio.h>
#include "board.h"
#include "sys.h"
#include "ADC.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
//#include "sdram_64M_32bit_drv.h"

#include "smb380_drv.h"
#include "Timer.h"

#define LenCircReg 2000
#define fs 100

Int32U _ADCVal;
Int32U _ADCStatus;
Int32U _regCount = 0;
Int32U _waveForm[LenCircReg];
Boolean CountFlag = false;
Int32U _NumberOfCrossings = 0;
Int32U _Debugvar = 0;
float LOWfilterWave[LenCircReg];
Boolean flip = false;


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
  //PINSEL1_bit.P0_26 = 0;//!PINSEL1_bit.P0_26;
  if(flip)
  {
    IO0SET_bit.P0_26 = 1;
  }
  else
  {
    IO0CLR_bit.P0_26 = 1;  
  }
  flip = !flip;
  
    
  USB_D_LINK_LED_FIO ^= USB_D_LINK_LED_MASK; // Toggle USB Link LED
  
  T0IR_bit.MR0INT = 1; // Clear the timer 0 interrupt.
  VICADDRESS = 0;
  
  // ADC Related
  _ADCVal = ((AD0GDR & 0xFFC0)>>6);  //Get ADC Value
  _waveForm[_regCount] = _ADCVal;    // Store ADC Value in circular register
  _regCount++;                       //Increment regcount
  if(_regCount == 1000){             //Check if regcount need to start over
  _regCount = 0;
  CountFlag = true;
  }
}

 void AD0IntrHandler (void) {
//   _ADCVal = ((AD0GDR & 0xFFC0)>>6);
//   _ADCStatus = ADSTAT;
 }

//Filter
void lowPassFilter (Int32U input[], Int32U points, Int32U sampleRate)
{
	float fc = 50; //frequency
	float dt = 0.0001; //delta t maybe "1/sample rate"
	float RC = 1/(2*3.1415*fc); //RC
	float alpha = dt/(RC+dt); //alpha
	
	for(Int32U ii = 1; ii <= LenCircReg-1; ii++)
	{
	   LOWfilterWave[ii] = (alpha*input[ii]) + (1-alpha)*LOWfilterWave[ii-1];
	}      
}

int main(void)
{
  // Init GPIO
  GpioInit();
  //IO0PIN_bit.P0_26 = 1;
  IO0DIR_bit.P0_26 = 1;

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
  Timer0Init(fs);
  
  //Init timer 0 interrupt:
  VIC_SetVectoredIRQ(Timer0IntrHandler,0,VIC_TIMER0);
  VICINTENABLE |= 1UL << VIC_TIMER0;
  T0TCR_bit.CE = 1;     // Enable counting.
  initADC2();
  while(1){
    if(CountFlag == true){
      _NumberOfCrossings = 0;
      for (Int32U ii = 1; ii <= LenCircReg-1; ii++){
        
        lowPassFilter(_waveForm, LenCircReg, fs);
               
        //if((_waveForm[ii]<511)^(_waveForm[ii + 1]<511)){
        if(((Int32U)LOWfilterWave[ii]<511)^((Int32U)LOWfilterWave[ii + 1]<511)){
          _NumberOfCrossings++;
        }
      }
      CountFlag = false;
      _Debugvar++;
    }
  }
}
