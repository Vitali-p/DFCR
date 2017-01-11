/*************************************************************************
 *    File name   : LED.c
 *    Description : LED main config file.
 *
 *       Date        : 11.11.2017
 *       Author      : Vitali Parolia
 *       Description : LED source file.
 **************************************************************************/
#include "board.h"

/*************************************************************************
 * Function Name: initLED
 * Parameters: none
 * Return: none
 * Description: Initiation of ports for controlling the intern board LEDs.
 *************************************************************************/ 
void initLEDs(){
  // Top board LED.
  USB_D_LINK_LED_FDIR = USB_D_LINK_LED_MASK;
  USB_D_LINK_LED_FSET = USB_D_LINK_LED_MASK;
  
  // Bottom board LED (touched indication LED).
  USB_H_LINK_LED_SEL = 0; // GPIO
  USB_H_LINK_LED_FDIR |= USB_H_LINK_LED_MASK;
  USB_H_LINK_LED_FSET = USB_H_LINK_LED_MASK;
}

/*************************************************************************
 * Function Name: funcLED
 * Parameters: func
 * Return: none
 * Description: Control boar LED.
 *************************************************************************/ 
void toogleTopLED(){
  USB_D_LINK_LED_FIO ^= USB_D_LINK_LED_MASK; // Toggle USB Link LED
}