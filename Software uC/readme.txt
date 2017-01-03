########################################################################
#
#                           DFCR.eww
#
########################################################################

DESCRIPTION
===========
  This example project shows how to use the IAR Embedded Workbench for ARM
 to develop code for IAR-LPC-2478 board. It shows basic use of I/O,
 timer, interrupt and LCD controllers.

  The IAR, NXP and Olimex logos appear on the LCD and the cursor
 moves as the board moves (the acceleration sensor is used).
  
COMPATIBILITY
=============

   The example project is compatible with IAR-LPC-2478
  evaluation board. By default, the project is configured to use the
  J-Link JTAG interface.

CONFIGURATION
=============

   After power-up the controller get clock from internal RC oscillator that
  is unstable and may fail with J-Link auto detect, therefore adaptive clocking
  should always be used. The adaptive clock can be select from menu:
  Project->Options..., section Debugger->J-Link/J-Trace  JTAG Speed - Adaptive.

   The GettingStarted application is downloaded to the iFlash.
  
  Jumpers:
    EXT/JLINK  - depending of power source
    ISP_E      - unfilled
    RST_E      - unfilled
    BDS_E      - unfilled
    C/SC       - SC

  The LCD shares pins with Trace port. If ETM is enabled the LCD will not work.
  
GETTING STARTED
===============

  1) Start the IAR Embedded Workbench for ARM.

  2) Select File->Open->Workspace...
     Open the following workspace:

      <installation-root>DFCR\DFCR.eww

  3) Run the program.
