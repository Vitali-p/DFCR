/**
This File contains methods concerning the ADC

Author: Toke Andersen

**/
#include <intrinsics.h>
#include <stdio.h>
#include <time.h>
#include "board.h"
#include "sys.h"
#include "sdram_64M_32bit_drv.h"
#include "drv_glcd.h"
#include "smb380_drv.h"

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
    Int32U result = ADDR0;    
return result;
}




void initADC( void ){ 
 PCONP_bit.PCAD = 1;         // Enable ADC clocks
 AD0CR_bit.SEL  = 1UL<<1;    // select Ch1
 AD0CR_bit.CLKDIV = 0xF;     // Set the clock divider to get 4.5 MHz as 72/15 = 4.5
 AD0CR_bit.BURST  = 0;       // Conversions are software controlled and require 11 clocks
 AD0CR_bit.PDN  = 1;         // converter is operational
 AD0CR_bit.START = 0;
 AD0CR_bit.CLKS   = 0;       // 10 bits resolution
 
}