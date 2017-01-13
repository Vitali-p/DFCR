/*************************************************************************
 *    File name   : LCD.c
 *    Description : LCD screen main file.
 *
 *       Date        : 07.11.2017
 *       Author      : Vitali Parolia
 *       Description : LCD and touch screen functions. 
 **************************************************************************/
#include <includes.h>
#include <drv_glcd.h>
#include <logo.h>
#include <sys.h>
#include <stdlib.h> 
#include <stdio.h>
#include <math.h>
#include <string.h>

// LCD screen used in driver.
extern Int32U SDRAM_BASE_ADDR;
#define LCD_VRAM_BASE_ADDR ((Int32U)&SDRAM_BASE_ADDR)

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

// Fonts for the LCD screen size range;6, 9, 18.
extern FontType_t Terminal_6_8_6;
extern FontType_t Terminal_9_12_6;
extern FontType_t Terminal_18_24_12;


/*************************************************************************
 * Function Name: initLCD
 * Parameters: none
 * Return: none
 * Description: Initiazion LCD screen.
 *************************************************************************/
void initLCD(){
  
  GLCD_Ctrl (FALSE);   // Disable power LCD.
  
#ifndef SDRAM_DEBUG
  // MAM init
  MAMCR_bit.MODECTRL = 0;
  MAMTIM_bit.CYCLES  = 3;   // FCLK > 40 MHz
  MAMCR_bit.MODECTRL = 2;   // MAM functions fully enabled
  // SDRAM Init
  SDRAM_Init();
#endif // SDRAM_DEBUG  
  
  GLCD_Init (LogoPic.pPicStream, NULL);  // Initiate LDC. 
  GLCD_Ctrl (TRUE);   // Enable power to LCD.

 
#if 0
  SDRAM_Test();
#endif

#if 0
  pInt32U pDst = (pInt32U)LCD_VRAM_BASE_ADDR;
  for(Int32U k = 0; k < C_GLCD_V_SIZE; k++)
  {
    for(Int32U i = 0 ; 8 > i; i++)
    {
      for(Int32U j = 0; 40 > j; j++)
      {
        switch(i)
        {
        case 0:
          *pDst++ = 0;
           break;
        case 1:
          *pDst++ = 0xFF;
           break;
        case 2:
          *pDst++ = 0xFF00;
           break;
        case 3:
          *pDst++ = 0xFFFF;
           break;
        case 4:
          *pDst++ = 0xFF0000;
           break;
        case 5:
          *pDst++ = 0xFF00FF;
           break;
        case 6:
          *pDst++ = 0xFFFF00;
           break;
        case 7:
          *pDst++ = 0xFFFFFF;
           break;
        }
      }
    }
  }
#endif

  // TEST!
  GLCD_SetFont(&Terminal_18_24_12,0x000000,0xe8e8e8); // Set current font, font color and background color.
  GLCD_SetWindow(0,26,200,52);  // Set draw window XY coordinate in pixels. (X_Left, Y_Up, X_Right, Y_Down)  
  GLCD_TextSetPos(0,0);          // Set text X,Y coordinate in characters.
  GLCD_print("\fDFRC Systems");   // Print formated string on the LCD.
  /*
  GLCD_SetFont(&Terminal_9_12_6,0x000000,0xe8e8e8); // Set current font, font color and background color.
  GLCD_SetWindow(0,0,100,13);  // Set draw window XY coordinate in pixels. (X_Left, Y_Up, X_Right, Y_Down)  
  GLCD_TextSetPos(0,0);          // Set text X,Y coordinate in characters.
  GLCD_print("\fDFRC Systems");   // Print formated string on the LCD.

  GLCD_SetWindow(10,42,100,74);
  GLCD_TextSetPos(0,0);
  GLCD_print("\fSome text"); */
}

/*************************************************************************
 * Function Name: initCursor
 * Parameters: none
 * Return: none
 * Description: Initiazion screen cursor.
 *************************************************************************/
void initCursor(Int32U X_cursor, Int32U Y_cursor){
  GLCD_Cursor_Dis(0);
  GLCD_Copy_Cursor ((Int32U *)Cursor, 0, sizeof(Cursor)/sizeof(Int32U));
  GLCD_Cursor_Cfg(CRSR_FRAME_SYNC | CRSR_PIX_32);
  GLCD_Move_Cursor(X_cursor, Y_cursor);
  GLCD_Cursor_En(0);
}

/*************************************************************************
 * Function Name: textToScreen
 * Parameters: Position XY on screen, name of the reading, and value.
 * Return: none
 * Description: Print readings on to the screen.
 *************************************************************************/
void textToScreen(Int32U X_Pos, Int32U Y_Pos, char Desc[], double Reading){
  Int32U d1, d2; //DeltaY = 13;
  char digitString [10];
  
  // Calculation from digit to string.  
  d1 = (Int32U)Reading; // Decimal precision: 5 digits
  d2 = (Int32U)((Reading-d1)*100000);
  sprintf(digitString, "%d.%d", d1, d2); // Convert digital reading to string.
  
  // Set position on the screen.
  GLCD_SetFont(&Terminal_18_24_12,0x000000,0xe8e8e8); // Set current font, font color and background color.
  GLCD_SetWindow(0,0,200,26);  // Set draw window XY coordinate in pixels. (X_Left, Y_Up, X_Right, Y_Down)  
  GLCD_TextSetPos(0,0);       // Set text X,Y coordinate in characters. 
  
  // Print formated string on the LCD.
  GLCD_print(Desc);          
  GLCD_print(digitString);
   
  /*  
  GLCD_SetWindow(X_Pos,Y_Pos,100,DeltaY); // Volt.
  GLCD_SetWindow(X_Pos,40,100,74); // Current.
  GLCD_SetWindow(X_Pos,42,100,74); // Power.
  GLCD_SetWindow(X_Pos,42,100,74); // Frequency.
*/ 
}

