########################################################################
#
#                           uip_webserver.eww
#
# $Revision: 28 $
#
########################################################################

DESCRIPTION
===========
   This example project shows how to use the IAR Embedded Workbench for ARM to
  develop code for IAR LPC2478-SK evaluation boards. This demo shows a
  web server  application  running on the top of the uIP 1.0 TCP-IP stack.
   This example can work standalone on the IAR LPC2478-SK board.
 
COMPATIBILITY
=============

   The project is compatible with the IAR LPC2478-SK evaluation board.
  By default, the project is configured to use the J-Link JTAG interface.
  
  The default IP address is:
   192.168.0.100 (set in main.c main subroutine)
  The physical MAC address is (defined in uipopt.h):
   00-ff-ff-ff-ff-ff
    
CONFIGURATION
=============

   After power-up the controller get clock from internal RC oscillator that
  is unstable and may fail with J-Link auto detect, therefore adaptive clocking
  should always be used. The adaptive clock can be select from menu:
  Project->Options..., section Debugger->J-Link/J-Trace  JTAG Speed - Adaptive.
  
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

      <installation-root>\arm\examples\NXP\
     LPC24xx\IAR-LPC-2478\uip_webserver\uip_webserver.eww

  3) Run the program.

  4) Connect a network cable between PC LAN Card, a hub, or a network card and
     IAR LPC-2478-SK board. Start a web browser and type 192.168.0.100 in
     address field of the browser.
     
     (In case you connect the board straight to the computer's network card,
     then set your default gateway to 192.168.0.100)
     

