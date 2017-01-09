/*************************************************************************
 *    File name   : LCD.c
 *    Description : LCD screen main file.
 *
 *       Date        : 07.11.2017
 *       Author      : Vitali Parolia
 *       Description : LCD and touch screen functions. 
 */
#include "drv_glcd.h"
#include "logo.h"
#include "Cursor.h"
#include "sys.h"

extern Int32U SDRAM_BASE_ADDR;
#define LCD_VRAM_BASE_ADDR ((Int32U)&SDRAM_BASE_ADDR)

/*************************************************************************
 * Function Name: initLCD
 * Parameters: none
 * Return: none
 * Description: Initiazion LCD screen.
 *************************************************************************/
void initLCD(){
  int cursor_x = (C_GLCD_H_SIZE - CURSOR_H_SIZE)/2, cursor_y = (C_GLCD_V_SIZE - CURSOR_V_SIZE)/2;
  GLCD_Ctrl (FALSE);
  
#ifndef SDRAM_DEBUG
  // Init MAM:
  MAMCR_bit.MODECTRL = 0;
  MAMTIM_bit.CYCLES  = 3;   // FCLK > 40 MHz
  MAMCR_bit.MODECTRL = 2;   // MAM functions fully enabled

  // SDRAM Init
  SDRAM_Init();
#endif // SDRAM_DEBUG
  
  
  
  GLCD_Init (LogoPic.pPicStream, NULL);

  GLCD_Cursor_Dis(0);

  GLCD_Copy_Cursor ((Int32U *)Cursor, 0, sizeof(Cursor)/sizeof(Int32U));

  GLCD_Cursor_Cfg(CRSR_FRAME_SYNC | CRSR_PIX_32);

  GLCD_Move_Cursor(cursor_x, cursor_y);

  GLCD_Cursor_En(0);
  
  GLCD_Ctrl (TRUE);
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
}