/***************************************************************************
 **    Master inlude file.
 **
 **    Used with ARM IAR C/C++ Compiler
 **
**    Revertion: 
           v0.1: Changes; #include "iar_logo.h" -- to --> #include "logo.h"
                     
 ***************************************************************************/

#ifndef __INCLUDES_H
#define __INCLUDES_H

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>
#include <limits.h>
#include <intrinsics.h>
#include <assert.h>
#include <nxp/iolpc2478.h>

#include "arm_comm.h"
#include "board.h"

#include "logo.h"
#include "cursor_arrow.h"

#include "sys.h"
#include "sdram_64M_32bit_drv.h"
#include "drv_glcd.h"
#include "drv_touch_scr.h"

#endif  // __INCLUDES_H
