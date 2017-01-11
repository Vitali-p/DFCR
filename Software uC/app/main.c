#include <intrinsics.h>
#include <stdio.h>
#include "board.h"
#include "sys.h"
//#include "ADC.h"
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
//#include "sdram_64M_32bit_drv.h"
//#include "smb380_drv.h"
#include <Timer.h>
#include <Relay.h>
#include <LED.h>
//#include <LCD.h>

#define fs 10000
#define T  0.0001
#define LenCircReg 3000

Int32U _ADCVal;
Int32U _ADCStatus;
Int32U _regCount = 0;
Int32U _waveForm[LenCircReg];
Boolean CountFlag = false;
Int32U _NumberOfCrossings = 0;
Int32U _Debugvar = 0;
float LOWfilterWave[LenCircReg];
Boolean flip = true;

////////////////////////////////
double _freq;
Int32U _CrossingsLocation[29];
Int32U _CrossIndex;
Int32U _ii = 0;

//-Touch-screen-----------------------------------------------------------------------------
#include <includes.h>

#define NONPROT 0xFFFFFFFF
#define CRP1  	0x12345678
#define CRP2  	0x87654321
/*If CRP3 is selected, no future factory testing can be performed on the device*/
#define CRP3  	0x43218765

#ifndef SDRAM_DEBUG
#pragma segment=".crp"
#pragma location=".crp"
__root const unsigned crp = NONPROT;
#endif

#define LCD_VRAM_BASE_ADDR ((Int32U)&SDRAM_BASE_ADDR)

extern Int32U SDRAM_BASE_ADDR;
extern FontType_t Terminal_6_8_6;
extern FontType_t Terminal_9_12_6;
extern FontType_t Terminal_18_24_12;
//--------------------------------------------------------------------------------



/*************************************************************************
 * Function Name: Timer0IntrHandler
 * Parameters: none
 * Return: none
 * Description: Timer 0 interrupt handler
 *************************************************************************/
/*
void Timer0IntrHandler (void)
{ 
//  toogleTopLED();
  
  T0IR_bit.MR0INT = 1; // Clear the Timer 0 interrupt.
  VICADDRESS = 0;
  
  // ADC Related
  _ADCVal = ((AD0GDR & 0xFFC0)>>6);  //Get ADC Value
  _waveForm[_regCount] = _ADCVal;    // Store ADC Value in circular register
  _regCount++;                       //Increment regcount
  if(_regCount == LenCircReg){             //Check if regcount need to start over
  _regCount = 0;
  CountFlag = true;
  }
}
*/
/*
 void AD0IntrHandler (void) {
//   _ADCVal = ((AD0GDR & 0xFFC0)>>6);
//   _ADCStatus = ADSTAT;
 }
*/
/*
//Filter
void lowPassFilter (Int32U input[], Int32U points, Int32U sampleRate)
{
	float fc = 50; //frequency
	float dt = 0.0001; //delta t maybe "1/sample rate"
	float RC = 1/(2*3.3.1415*fc); //RC  3.14159265359
	float alpha = dt/(RC+dt); //alpha
	
	for(Int32U ii = 1; ii <= LenCircReg-1; ii++)
	{
	   LOWfilterWave[ii] = (alpha*input[ii]) + (1-alpha)*LOWfilterWave[ii-1];
	}      
}
*/
/*
void clearArray( void ){
  Int32U len = sizeof(_CrossingsLocation);
  for (Int32U ii = 0; ii<len; ii++){
    _CrossingsLocation[ii] = 0;
  }
}
*/


int main(void)
{ 
 Int32U cursor_x = (C_GLCD_H_SIZE - CURSOR_H_SIZE)/2, cursor_y = (C_GLCD_V_SIZE - CURSOR_V_SIZE)/2;
 ToushRes_t XY_Touch;
 Boolean Touch = FALSE;

  GLCD_Ctrl (FALSE);
  // Init GPIO
  GpioInit();
#ifndef SDRAM_DEBUG
  // MAM init
  MAMCR_bit.MODECTRL = 0;
  MAMTIM_bit.CYCLES  = 3;   // FCLK > 40 MHz
  MAMCR_bit.MODECTRL = 2;   // MAM functions fully enabled
  // Init clock
  InitClock();
  // SDRAM Init
  SDRAM_Init();
#endif // SDRAM_DEBUG
  // Init VIC
  VIC_Init();
  // GLCD init
  GLCD_Init (LogoPic.pPicStream, NULL);
  GLCD_Cursor_Dis(0);
  GLCD_Copy_Cursor ((Int32U *)Cursor, 0, sizeof(Cursor)/sizeof(Int32U));
  GLCD_Cursor_Cfg(CRSR_FRAME_SYNC | CRSR_PIX_32);
  GLCD_Move_Cursor(cursor_x, cursor_y);
  GLCD_Cursor_En(0);

  // Init touch screen
  TouchScrInit();



  __enable_interrupt();
  GLCD_Ctrl (TRUE);
  
  
//--------------------------------------------------------------------------------------------------  
/*
  // Init GPIO:
  GpioInit();
   
  // Init clock:
  InitClock();
  
  // Init VIC:
  VIC_Init();
  
  //Init LCD
  initLCD();
  
  // Init Relay and it ports:
  initRelayPorts();
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
  */

  
  while(1)
  {
    if(TouchGet(&XY_Touch))
    {
      cursor_x = XY_Touch.X;
      cursor_y = XY_Touch.Y;
      GLCD_Move_Cursor(cursor_x, cursor_y);
      if (FALSE == Touch)
      {
        Touch = TRUE;
        USB_H_LINK_LED_FCLR = USB_H_LINK_LED_MASK;
      }
    }
    else if(Touch)
    {
      USB_H_LINK_LED_FSET = USB_H_LINK_LED_MASK;
      Touch = FALSE;
    } 
    
    
    
    
 /*   
    if(CountFlag == true){
      __disable_interrupt();
      FIO0SET_bit.P0_11 = 1;
      
      _NumberOfCrossings = 0;
      
      clearArray();
      _CrossIndex = 0;
      
      lowPassFilter(_waveForm, LenCircReg, fs);
      _ii = 0;
      for (_ii = 0; (_ii <= LenCircReg-1) & (_CrossIndex<27); _ii++){
        
        
        
        //        if((_waveForm[ii]<511)^(_waveForm[ii + 1]<511)){
        if((LOWfilterWave[_ii]<511)^(LOWfilterWave[_ii + 1]<511)){
          _NumberOfCrossings++;
          _CrossingsLocation[_CrossIndex] = _ii;
          _CrossIndex++;
        }
      }
       
      _freq = 10/((double)(_CrossingsLocation[25] - _CrossingsLocation[5])*(double)(1/(double)fs));
      RelayControl(_freq);
      CountFlag = false;
      _Debugvar++;
      FIO0CLR_bit.P0_11 = 1;
      __enable_interrupt();
      
      textToScreen(0,0,"Frequency:",_freq);
     
    }
    
  */  
    
  }
}
