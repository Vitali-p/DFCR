/*************************************************************************
 *    File name   : Relay.c
 *    Description : <Relay controler main file.
 *
 *       Date        : 06.11.2017
 *       Author      : Vitali Parolia
 *       Description : Relay source file.
 */
#include "board.h"


/*************************************************************************
 * Function Name: initRelayPorts
 * Parameters: none
 * Return: none
 * Description: Initiation of ports for controlling the relays.
 *************************************************************************/ 
void initRelayPorts(void)
{
  // Set bit 11 and 19 as outputs.
  FIO0DIR_bit.P0_11 = 1;
  FIO0DIR_bit.P0_19 = 1;
 
}

/*************************************************************************
 * Function Name: RelayCont
 * Parameters: Frequency
 * Return: none
 * Description: Controlling output ports for the relay
 *************************************************************************/ 
void RelayControl(float freq)
{
  if(freq <= 49.975)  // Switch off the relays.
  {
    // Set ports P0.11 and P0.19 as low. 
    FIO0CLR_bit.P0_11 = 1;
    FIO0CLR_bit.P0_19 = 1;
  }
  else if(freq >= 50.025) // Switch on the relays.
  {
    // Set ports P0.11 and P0.19 as high.
    FIO0SET_bit.P0_11 = 1;
    FIO0SET_bit.P0_19 = 1;
  }
  else //If beween 49.975 and 50.025, keep port high, switch on relays.
  {
    // Set ports P0.11 and P0.19 as high.
    FIO0SET_bit.P0_11 = 1;
    FIO0SET_bit.P0_19 = 1;
  } 
}

/*************************************************************************
 * Function Name: initLED
 * Parameters: none
 * Return: none
 * Description: Initiation of ports for controlling the intern board LEDs.
 *************************************************************************/ 
void initLEDs(){
  USB_D_LINK_LED_FDIR = USB_D_LINK_LED_MASK;
  USB_D_LINK_LED_FSET = USB_D_LINK_LED_MASK;
}





