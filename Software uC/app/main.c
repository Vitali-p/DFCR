#include <intrinsics.h>
#include <includes.h>
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
#include <Timer.h>
#include <Relay.h>
#include <LED.h>
#include <LCD.h>
#include "drv_touch_scr.h"
#include "NXP/iolpc2478.h"
#include "assert.h"


#define fs 10000
//#define T  0.0001
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
//Int32U _ii = 0;
Int32U ff = 0;

char _str7[7];

/*************************************************************************
 * Function Name: Timer0IntrHandler
 * Parameters: none
 * Return: none
 * Description: Timer 0 interrupt handler
 *************************************************************************/
void Timer0IntrHandler(void){ 
//  toogleTopLED();
 
  T0IR_bit.MR0INT = 1; // Clear the Timer 0 interrupt.
  VICADDRESS = 0;
  
  // ADC Related
  _ADCVal = ((ADDR2 & 0xFFC0)>>6);  //((AD0GDR & 0xFFC0)>>6);  //Get ADC Value
  _waveForm[_regCount] = _ADCVal;    // Store ADC Value         in circular register
  _regCount++;                       //Increment regcount
  if(_regCount == LenCircReg){             //Check if regcount need to start over
  _regCount = 0;
  CountFlag = true;
  }
}

void getString(double freq){
//Aquire 100s
  char hundreds = (char)(freq / 100);
  char tens = (char)((freq - (double)hundreds*100) / 10);
  char ones = (char)((freq - (double)hundreds*100 - (double)tens*10) / 1);
  char tenths = (char)((freq - (double)hundreds*100 - (double)tens*10 - (double)ones*1) / 0.1);
  char hundreths = (char)((freq - (double)hundreds*100 - (double)tens*10 - (double)ones*1 - (double)tenths*0.1) / 0.01);
  char thous = (char)((freq - (double)hundreds*100 - (double)tens*10 - (double)ones*1 - (double)tenths*0.1 - (double)hundreths*0.01) / 0.001);
  _str7[0] = hundreds + 0x30;
  _str7[1] =tens + 0x30;
  _str7[2] =ones + 0x30;
  _str7[3] =0x2E; // Decimal marker
  _str7[4] =tenths + 0x30;
  _str7[5] =hundreths + 0x30;
  _str7[6] =thous + 0x30;
}

double getRMS(Int32U Vector[]){
  Int32U length = sizeof(Vector);
  double result = 0;
  double sum = 0;
  for(Int32U ii = 0; ii< length; ii++){ //Sum up the array
    sum += Vector[ii]*Vector[ii];
  }
  result = sqrt(sum/(double)length);  // must include math.h
  return result;
}

double getAverage(Int32U Vector[]){
  Int32U length = sizeof(Vector);
  double result = 0;
  double sum = 0;
  for(Int32U ii = 0; ii< length; ii++){ //Sum up the array
    sum += Vector[ii]*Vector[ii];
  }
  result = sum/(double)length;
  return result;
}



void AD0IntrHandler (void) {
////   _ADCVal = ((AD0GDR & 0xFFC0)>>6);
////   _ADCStatus = ADSTAT;
}

// Lowpass filter (50Hz).
void lowPassFilter (Int32U input[], Int32U points, Int32U sampleRate){
  float fc = 50; //frequency
  float dt = 0.0001; //delta t maybe "1/sample rate"
  float RC = 1/(2*3.14159265359*fc);
  float alpha = dt/(RC+dt); //alpha
	
  for(Int32U ii = 1; ii <= LenCircReg-1; ii++){
    LOWfilterWave[ii] = (alpha*input[ii]) + (1-alpha)*LOWfilterWave[ii-1];
  }      
}

void clearArray( void ){
  Int32U len = sizeof(_CrossingsLocation);
  for (Int32U ii = 0; ii<len; ii++){
    _CrossingsLocation[ii] = 0;
  }
}


int main(void){ 
  Int32U cursor_x = (C_GLCD_H_SIZE - CURSOR_H_SIZE)/2, cursor_y = (C_GLCD_V_SIZE - CURSOR_V_SIZE)/2;
  ToushRes_t XY_Touch;
  Boolean Touch = FALSE;

  GpioInit();   // Init GPIO.
  InitClock();  // Init clock.
  VIC_Init();   // Init VIC.
  
  initADC2();         // Init ADC readings.
  Timer0Init(fs);     // Init Timer 0.
  initRelayPorts();   // Init Relay and it ports.
  initLEDs();         // Init USB Link LED.

  // Initiating LCD and touch screen:
  initLCD();    // Init LCD display.
  initCursor(cursor_x, cursor_y);  // Init cursor.
//  initADCtouchscreen();
//  Timer1Init();
  TouchScrInit();
  __enable_interrupt(); // Enable global interrup.

    
  // Init timer 0 interrupt: readings.
  VIC_SetVectoredIRQ(Timer0IntrHandler,0,VIC_TIMER0);
  VICINTENABLE |= 1UL << VIC_TIMER0;
  T0TCR_bit.CE = 1;     // Enable counting.
  
  // LCD touch screen and ADC.
  __enable_interrupt(); // Enable interrupt. 
  
//  __disable_interrupt(); // Disable interrupt.
    getString(50.01234);
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
    

    
    if(CountFlag == true)
    {
//      __disable_interrupt(); // Disable interrupt.
      _NumberOfCrossings = 0;
      clearArray();
      _CrossIndex = 0;     
      lowPassFilter(_waveForm, LenCircReg, fs);
//      _ii = 0;
      
      for(Int32U ii = 0; (ii <= LenCircReg-1) & (_CrossIndex<27); ii++)
      {
        //        if((_waveForm[ii]<511)^(_waveForm[ii + 1]<511)){
        if((LOWfilterWave[ii]<511)^(LOWfilterWave[ii + 1]<511)){
          _NumberOfCrossings++;
          _CrossingsLocation[_CrossIndex] = ii;
          _CrossIndex++;
        }
      }
       
      _freq = 10/((double)(_CrossingsLocation[25] - _CrossingsLocation[5])*(double)(1/(double)fs));
      RelayControl(_freq);
      CountFlag = false;
     // __enable_interrupt(); // Enable interrupt.  
      
      textToScreen(0,0,"Frequency:",_freq);
    }
  }
}
