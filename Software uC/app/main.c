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
//#include "smb380_drv.h"
#include "Timer.h"
#include "Relay.h"
#include "LCD.h"


#define NONPROT 0xFFFFFFFF
#define CRP1  	0x12345678
#define CRP2  	0x87654321
/*If CRP3 is selected, no future factory testing can be performed on the device*/
#define CRP3  	0x43218765

#define fs 10
#define LenCircReg 1000

Int32U _ADCVal;
Int32U _ADCStatus;
Int32U _regCount = 0;
Int32U _waveForm[LenCircReg];
Boolean CountFlag = false;
Int32U _NumberOfCrossings = 0;
Int32U _Debugvar = 0;
float LOWfilterWave[LenCircReg];
Boolean flip = true;


/*************************************************************************
 * Function Name: Timer0IntrHandler
 * Parameters: none
 * Return: none
 * Description: Timer 0 interrupt handler
 *************************************************************************/
void Timer0IntrHandler (void)
{ 
  toogleLED();
  
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
  // Init GPIO:
  GpioInit();
  
  // Init Relay and it ports:
  initRelayPorts();
  
  
  // Init clock:
  InitClock();
  
  // Init VIC:
  VIC_Init();
  
  //Init LCD
  initLCD();

  // Init USB Link  LED:
  initLEDs();


  // Init Timer:
  Timer0Init(fs);
  
  // Init timer 0 interrupt:
  VIC_SetVectoredIRQ(Timer0IntrHandler,0,VIC_TIMER0);
  VICINTENABLE |= 1UL << VIC_TIMER0;
  T0TCR_bit.CE = 1;     // Enable counting.
  
  // Init ADC:
  initADC2();
  

  
  
  while(1)
  {
    
    
    
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
