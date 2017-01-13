/*************************************************************************
 *    File name   : drv_touch_scr.c
 *    Description : Touch screen driver module
 
 *       Date        : August, 8 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *  Touch screen sampling algorithm
 *   1. Set Y1, Y2 to output 1, Set X1 - ADC input (ch 1), X2 input with pull-down
 *   2. Wait setup delay
 *   3. Check X2 state - if high the screen is touched set Y0 to low and wait
 *                        setup delay
 *                       else wait until interrupt by rising edge from X2
 *      When interrupt arise Y0 to low and wait setup delay
 *   4. Star conversion
 *   5. After TS_SAMPLES change Y1 high Y2 low and result calculate  1023 - ADC
 *   6. After TS_SAMPLES change Y1 to ADC  input Y2 input without pulls X1 and X2
 *      output in low and high level
 *   7. Same like X
 *   8. Apply sample delay and return back to poin 1
 **************************************************************************/
#include "drv_touch_scr.h"
typedef enum _TouchScrState_t
{
  TS_INTR_SETUP_DLY = 0, TS_WAIT_FOR_TOUCH,
  TS_X1_SETUP_DLY, TS_X1_MEASURE,
  TS_X2_SETUP_DLY, TS_X2_MEASURE,
  TS_Y1_SETUP_DLY, TS_Y1_MEASURE,
  TS_Y2_SETUP_DLY, TS_Y2_MEASURE,
} TouchScrState_t;

#define LEFT_UP_X     100 * TS_SAMPLES * 2
#define RIGHT_UP_X    915 * TS_SAMPLES * 2

#define LEFT_UP_Y     125 * TS_SAMPLES * 2
#define RIGHT_UP_Y    900 * TS_SAMPLES * 2


static volatile Boolean Touch;
static volatile Int16U  X,Y;
static volatile Int16U  X_temp,Y_temp;
static volatile Boolean Touch_temp;

static volatile TouchScrState_t State;
static volatile Int32U Samples;

/*************************************************************************
 * Function Name: ADC_Intr_Handler
 * Parameters: none
 * Return: none
 * Description: End conversion interrupt handler (for port ADC port 0 and 1).
 *************************************************************************/
void ADC_Intr_Handler (void){
  // Init variables.
//  Int32U Data, d1;

//  AD0CR_bit.START = 0;  // Control start of ADC.
  
  // CHNGES from original.
//    Data = AD0GDR_bit.RESULT; // Original.
//  AD0GDR_bit.RESULT = (ADDR1&0xFFFF)/0x40);
//  Data = ((ADDR0&0xFFFF)/0x40);
//  d1 = ((ADDR1&0xFFFF)/0x40);
  
  switch(State)
  {
  case TS_X1_MEASURE:
    Y_temp += ((ADDR1&0xFFFF)/0x40);
    if(++Samples >= TS_SAMPLES)
    {
      Samples = 0;
      State = TS_X2_SETUP_DLY;
      // Y2 = 0, Y1 = 1;
      TS_Y2_FCLR = TS_Y2_MASK;
      TS_Y1_FSET = TS_Y1_MASK;
      // Init setup delay
      T1MR0 = TS_SETUP_DLY;
      T1TCR = 1;
    }
    else
    {
      //AD0CR_bit.START = 1; // REPLAYCE
       ADC_Intr_Handler();
    }
    break;
  case TS_X2_MEASURE:
    Y_temp += 1023UL - ((ADDR1&0xFFFF)/0x40);
    if(++Samples >= TS_SAMPLES)
    {
      Samples = 0;
      State = TS_Y1_SETUP_DLY;

      // X1 = 0, X2 = 1;
      TS_X1_FCLR  = TS_X1_MASK;
      TS_X2_FSET  = TS_X2_MASK;
      TS_X1_FDIR |= TS_X1_MASK;
      TS_X2_FDIR |= TS_X2_MASK;
      TS_X1_SEL   = 0;

      // Y1 - ADC Ch0, Y2 input
      TS_Y1_FDIR &= ~TS_Y1_MASK;
      TS_Y2_FDIR &= ~TS_Y2_MASK;
      TS_Y1_SEL   = 1;            // assign to ADC0 Ch0
//      AD0CR_bit.SEL  = 1UL<<0;    // select Ch0

      // Init setup delay
      T1MR0 = TS_SETUP_DLY;
      T1TCR = 1;
    }
    else
    {
      // AD0CR_bit.START = 1; // REPLAYCED
      ADC_Intr_Handler();
    }
    break;
  case TS_Y1_MEASURE:
    X_temp += 1023UL - ((ADDR0&0xFFFF)/0x40);
    if(++Samples >= TS_SAMPLES)
    {
      Samples = 0;
      State = TS_Y2_SETUP_DLY;
      // X2 = 0, X1 = 1;
      TS_X2_FCLR = TS_X2_MASK;
      TS_X1_FSET = TS_X1_MASK;
      // Init setup delay
      T1MR0 = TS_SETUP_DLY;
      T1TCR = 1;
    }
    else
    {
      //AD0CR_bit.START = 1; // REPLAYCED
      ADC_Intr_Handler();
    }
    break;
  case TS_Y2_MEASURE:
    X_temp += ((ADDR0&0xFFFF)/0x40);
    if(++Samples >= TS_SAMPLES)
    {
      State = TS_INTR_SETUP_DLY;

      // Y1 = 1, Y2 = 1;
      TS_Y1_FSET  = TS_Y1_MASK;
      TS_Y2_FSET  = TS_Y2_MASK;
      TS_Y1_FDIR |= TS_Y1_MASK;
      TS_Y2_FDIR |= TS_Y2_MASK;
      TS_Y1_SEL   = 0;

      // X1 - ADC Ch1, X2 input with pull down
      TS_X1_FDIR &= ~TS_X1_MASK;
      TS_X2_FDIR &= ~TS_X2_MASK;
      TS_X1_SEL   = 1;            // assign to ADC0 Ch1
      TS_X2_MODE  = 3;            // enable pull-down
//      AD0CR_bit.SEL  = 1UL<<1;    // select Ch1

      // Init setup delay
      T1MR0 = TS_SAMPLE_DLY;
      T1TCR = 1;
      Touch_temp = TRUE;
    }
    else
    {
      // AD0CR_bit.START = 1; // REPLAYCED
      ADC_Intr_Handler();
    }
    break;
  default:
    assert(0);
  }
  VICADDRESS = 0;
}

/*************************************************************************
 * Function Name: TimerIntr_Handler
 * Parameters: none
 * Return: none
 * Description: Sample timer 1 interrupt handler.
 *************************************************************************/
void TimerIntr_Handler (void){
  T1IR_bit.MR0INT = 1;  // clear pending interrupt
  T1TCR_bit.CR = 1;
  
  switch(State)
  {
    case TS_X1_SETUP_DLY:
    case TS_X2_SETUP_DLY:
    case TS_Y1_SETUP_DLY:
    case TS_Y2_SETUP_DLY:
      ++State;
      
      //AD0CR_bit.START = 1; // REPLAYCED
      ADC_Intr_Handler();
      
      break;
    case TS_INTR_SETUP_DLY:
      ++State;
      TS_X2_INTR_CLR = TS_X2_MASK;
      if(0 == (TS_X2_FIO & TS_X2_MASK))
      {
        Touch_temp = Touch = FALSE;
        TS_X2_INTR_R |= TS_X2_MASK;
      }
      else
      {
        // Update X and Y
        if(Touch_temp)
        {
          X = X_temp;
          Y = Y_temp;
        }

        Touch = Touch_temp;

        // Y1 = 0, Y2 = 1;
        TS_Y1_FCLR = TS_Y1_MASK;
        // Disable X2 pull down
        TS_X2_MODE = 2;
        // Reset sample counter
        Samples = 0;
        // Clear accumulators
        X_temp = Y_temp = 0;
        // Init setup delay
        if(Touch)
        {
          T1MR0 = TS_SETUP_DLY;
        }
        else
        {
          T1MR0 = TS_INIT_DLY;
        }
          State = TS_X1_SETUP_DLY;
          T1TCR = 1;
        }
        break;
    default:
    assert(0);
    break;
  }
  VICADDRESS = 0;
}

/*************************************************************************
 * Function Name: OnTouchIntr_Handler
 * Parameters: none
 * Return: none
 * Description: On touch interrupt handler
 *************************************************************************/
void OnTouchIntr_Handler (void){
  // Disable and clear interrupt
  TS_X2_INTR_R  &= ~TS_X2_MASK;
  TS_X2_INTR_CLR =  TS_X2_MASK;
  // Init ACD measure setup delay
  if(TS_WAIT_FOR_TOUCH == State)
  {
    // Y1 = 0, Y2 = 1;
    TS_Y1_FCLR = TS_Y1_MASK;
    // Disable X2 pull down
    TS_X2_MODE = 2;
    // Reset sample counter
    Samples = 0;
    // Clear accumulators
    X_temp = Y_temp = 0;
    // Init setup delay
    if(Touch)
    {
      T1MR0 = TS_SETUP_DLY;
    }
    else
    {
      T1MR0 = TS_INIT_DLY;
    }
    State = TS_X1_SETUP_DLY;
    T1TCR = 1;
  }
  else
  {
    assert(0);
  }
  VICADDRESS = 0;
}

/*************************************************************************
 * Function Name: TouchScrInit
 * Parameters: none
 * Return: none
 * Description: Init Touch screen
 *************************************************************************/
void TouchScrInit (void)
{
  // Init variable
  Touch_temp = Touch = FALSE;
  X = Y = 0;
  State = TS_INTR_SETUP_DLY;

  // Init GPIOs
  TS_X1_SEL   = 1;   // ADC0 Ch1
  TS_X1_MODE  = 2;   // disable pulls
  TS_X2_SEL   = 0;   // general IO
  TS_X2_MODE  = 3;   // enable pull-down

  TS_Y1_SEL   = 0;   // general IO
  TS_Y1_MODE  = 2;   // disable pulls
  TS_Y2_SEL   = 0;   // general IO
  TS_Y2_MODE  = 2;   // disable pulls

  TS_X1_FDIR &= ~TS_X1_MASK;
  TS_X2_FDIR &= ~TS_X2_MASK;
  TS_Y1_FDIR |=  TS_Y1_MASK;
  TS_Y2_FDIR |=  TS_Y2_MASK;

  TS_Y1_FSET  =  TS_Y1_MASK;
  TS_Y2_FSET  =  TS_Y2_MASK;

  // Init Port interrupt
  TS_X2_INTR_R  &= ~TS_X2_MASK; // disable X2 rising edge interrupt
  TS_X2_INTR_CLR =  TS_X2_MASK;
  EXTINT = 1UL<<3;
  VIC_SetVectoredIRQ(OnTouchIntr_Handler,TS_INTR_PRIORITY,VIC_EINT3);
  VICINTENABLE |= 1UL << VIC_EINT3;
/*
// Init ADC
  PCONP_bit.PCAD = 1;         // Enable ADC clocks
  AD0CR_bit.PDN  = 1;         // converter is operational
//  AD0CR_bit.START = 0;        // Control start of ADC.  
  
//  AD0CR_bit.SEL  = 1UL<<1;    // select Ch1 
//  AD0CR_bit.SEL  = 1UL<<0;    // select Ch0
  AD0CR_bit.SEL  = 3;
  
  AD0CR_bit.CLKDIV = 17; //SYS_GetFpclk(ADC_PCLK_OFFSET)/ 500000;
  
  AD0CR_bit.BURST  = 1;       // disable burst (CHANGED original settings from 0 to 1)
  AD0CR_bit.CLKS   = 0;       // 10 bits resolution
*/
/*  
  // clear all pending interrupts
  while(ADSTAT_bit.ADINT)
  {
    volatile Int32U Dummy = AD0GDR;
  }
*/
//  ADINTEN_bit.ADGINTEN = 1;   // Enable global interrupt
//  VIC_SetVectoredIRQ(ADC_Intr_Handler,TS_INTR_PRIORITY,VIC_AD0);
//  VICINTENABLE |= 1UL << VIC_AD0;

/*
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
  VICINTENABLE |= 1UL << VIC_TIMER1;
  T1TCR = 1;            // start timer 1
*/
}

/*************************************************************************
 * Function Name: TouchScrGetStatus
 * Parameters: ToushRes_t * pData X,Y data
 * Return: Int32U 0 - untouched
 *                1 - touched
 * Description: Return current state of the touch screen.
 *************************************************************************/
Boolean TouchGet (ToushRes_t * pData)
{
Boolean  TouchResReadyHold = Touch;
Int32U X_coordinate, Y_coordinate;

  if(TouchResReadyHold)
  {
    X_coordinate = X;
    Y_coordinate = Y;
    // Calculate X coordinate in pixels
    if (X_coordinate <= LEFT_UP_X)
    {
      X_coordinate = 0;
    }
    else if (X_coordinate >= RIGHT_UP_X)
    {
      X_coordinate = IMAGE_WIDTH;
    }
    else
    {
      X_coordinate = ((X_coordinate - LEFT_UP_X) * IMAGE_WIDTH)/(RIGHT_UP_X-LEFT_UP_X);
    }
    // Calculate Y coordinate in pixels
    if (Y_coordinate <= LEFT_UP_Y)
    {
      Y_coordinate = 0;
    }
    else if (X_coordinate >= RIGHT_UP_Y)
    {
      Y_coordinate = IMAGE_HEIGHT;
    }
    else
    {
      Y_coordinate = ((Y_coordinate - LEFT_UP_Y) * IMAGE_HEIGHT)/(RIGHT_UP_Y-LEFT_UP_Y);
    }
    pData->X = X_coordinate;
    pData->Y = Y_coordinate;
  }
  return(TouchResReadyHold);
}
