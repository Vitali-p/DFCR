/***************************************************************************
 **
 **
 **    Master inlude file
 **
 **    Used with ARM IAR C/C++ Compiler
 **
 **    (c) Copyright IAR Systems 2007
 **
 **    $Revision: 28 $
 **
 ***************************************************************************/

#ifndef __INCLUDES_H
#define __INCLUDES_H
#include "logo.h"
#include "cursor_arrow.h"
#include "drv_touch_scr.h"
#include "autoON.h"
#include "autoOFF.h"
#include "buttomON.h"
#include "buttomOFF.h"

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <intrinsics.h>
#include <assert.h>
#include <nxp/iolpc2478.h>
#include <math.h>

#include "arm_comm.h"
#include "board.h"

#include "iar_logo.h"

#include "sys.h"
#include "sdram_64M_32bit_drv.h"
#include "drv_glcd.h"

#include "clock-arch.h"

#include "timer.h"
#include "uip-conf.h"
#include "uipopt.h"
#include "uip_arp.h"
#include "uip.h"
#include "tapdev.h"
#include "httpd.h"

#endif  // __INCLUDES_H
