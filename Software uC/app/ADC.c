/**
This File contains methods concerning the ADC

Author: Toke Andersen

**/
#include "sys.h"
#include "ADC.h"

/************************************************************************
Initialize the ADC Peripheral
************************************************************************/
/*
void initADC( void ){ 
  // PCONP_bit.PCAD = 1;         // Enable ADC clocks
  // PCLKSEL0_bit.PCLK_ADC = 1;
  // PINSEL1_bit.P0_25 = 1;
  // PINSEL1_bit.P0_26 = 1;
  // 
  // AD0CR_bit.SEL  = 1UL;    // select Ch1
  // AD0CR_bit.SEL  = 1UL<<1;    // select Ch1
  // AD0CR_bit.SEL  = 1UL<<2;    // select Ch1
  // AD0CR_bit.SEL  = 1UL<<3;    // select Ch1
  // 
  // AD0CR_bit.CLKDIV = 0x5;     // Set the clock divider to get 4.5 MHz as 72/15 = 4.5
  // AD0CR_bit.BURST  = 1;       // Conversions are software controlled and require 11 clocks
  // AD0CR_bit.PDN  = 1;         // converter is operational
  // AD0CR_bit.START = 0;
  // AD0CR_bit.CLKS   = 0;       // 10 bits resolution
  
  AD0CR_bit.PDN = 0;         // Enable ADC.
  PCONP_bit.PCAD = 1;        // Power on ADC.
  PCLKSEL0_bit.PCLK_ADC = 1; // Enable ADC clock, CCLK = 72MHz.

  AD0CR_bit.CLKDIV = 17;  //72MHz/(17+1)= 4MHz<=4.5 MHz
  AD0CR_bit.BURST = 0;    //ADC is set to operate in software controlled mode

  PINSEL1_bit.P0_25 = 0x1; //Set pin P0.25 to AD0[2].
  PINSEL1_bit.P0_26 = 0x1; //Set pin P0.25 to AD0[3].
  
  PINMODE1_bit.P0_25 = 0x2;
  PINMODE1_bit.P0_26 = 0x2;
  
  ADINTEN_bit.ADGINTEN = 0;
  ADINTEN_bit.ADINTEN0 = 1; //Enable interrupt
  ADINTEN_bit.ADINTEN1 = 1;
  ADINTEN_bit.ADINTEN2 = 1;
  ADINTEN_bit.ADINTEN3 = 1;
  AD0CR_bit.START = 0;
//  VIC_SetVectoredIRQ (AD0IntrHandler,2,VIC_AD0);
  VICINTENABLE |= 1UL << VIC_AD0;
  AD0CR_bit.SEL = 0x4;
  AD0CR_bit.PDN = 1; //The A/D Converter is operational
  
}
*/
/************************************************************************
This Function will start the ADC and return the value synchronus
************************************************************************/
Int32U runADC(void){
  AD0CR_bit.START = 1;
  //Wait 11 cylcles here
  __asm("nop"); // 1  Cycles
  __asm("nop");
  __asm("nop");
  __asm("nop"); // 4  Cycles
  __asm("nop");
  __asm("nop");
  __asm("nop");
  __asm("nop"); // 8  Cycles
  __asm("nop");
  __asm("nop");
  __asm("nop");
  __asm("nop"); // 12 Cycles
  Int32U result = AD0GDR;    
  return result;
}

void initADC2(void){
  AD0CR_bit.PDN = 0;         // Disable ADC.
  PCONP_bit.PCAD = 1;        // Power on ADC.

  PCLKSEL0_bit.PCLK_ADC = 1; // Enable ADC clock, CCLK = 72MHz.
  AD0CR_bit.CLKDIV = 17;    // 72MHz/(17+1)= 4MHz<=4.5 MHz.
  AD0CR_bit.BURST = 1;     // ADC is set to operate burst mode.
  
  PINSEL1_bit.P0_25 = 1; // Set pin P0.25 to AD0[2].
  PINSEL1_bit.P0_26 = 1; // Set pin P0.25 to AD0[3]. 
  
//  PINMODE1_bit.P0_25 = 0x2;
//  PINMODE1_bit.P0_26 = 0x2;
/*
  ADINTEN_bit.ADGINTEN = 0;
  ADINTEN_bit.ADINTEN0 = 1; // Enable interrupt
  ADINTEN_bit.ADINTEN1 = 1;
  ADINTEN_bit.ADINTEN2 = 1;
  ADINTEN_bit.ADINTEN3 = 1;

//  AD0CR_bit.START = 0;  // Control start of ADC.
  
  VIC_SetVectoredIRQ (AD0IntrHandler,1,VIC_AD0); // The priority is hight for ADC.
  VICINTENABLE |= 1UL << VIC_AD0;
*/  
  AD0CR_bit.SEL |= 0x0000000C; // Select Ch.2 and 3. Selects which of the AD0.7:0 pins are to be sampled and converted.
  AD0CR_bit.PDN = 1; // Enable the A/D, the converter is operational.
}
/************************************************************************
Start the ADC in async mode (Interrupt driven)
************************************************************************/
void runADCasync(void){
  AD0CR_bit.START = 0;
}

void initADCtouchscreen(){
  // Init ADC
  PCONP_bit.PCAD = 1;      // Enable ADC clocks
  PINSEL1_bit.P0_23 = 1;   // Set pin P0.25 to AD0[0].
  PINSEL1_bit.P0_24 = 1;   // Set pin P0.25 to AD0[1].
  AD0CR_bit.SEL |= 0x00000003;  // Select Ch.2 and 3.
  AD0CR_bit.PDN  = 1;      // converter is operational
}
