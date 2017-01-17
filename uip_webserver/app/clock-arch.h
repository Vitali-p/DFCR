/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2006
 *
 *    File name   : clock-arch.h
 *    Description : Implementation of architecture-specific clock
 *                  functionality include file
 *
 *    History :
 *    1. Date        : October 4, 2006
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 28 $
**************************************************************************/
#include "includes.h"

#ifndef __CLOCK_ARCH_H__
#define __CLOCK_ARCH_H__

#define TICK_PER_SEC        100

typedef Int32U clock_time_t;

#define CLOCK_CONF_SECOND TICK_PER_SEC

extern Int32U SYS_GetFpclk(Int32U Periphery);
extern void VIC_SetVectoredIRQ(void(*pIRQSub)(), unsigned int Priority,
                               unsigned int VicIntSource);

/*************************************************************************
 * Function Name: Tim0Handler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 0 interrupt handler
 *
 *************************************************************************/
static
void Timer0IntrHandler (void);

/*************************************************************************
 * Function Name: clock_init
 * Parameters: Int32U IntrPriority
 *
 * Return: none
 *
 * Description: Timer init
 *
 *************************************************************************/
void clock_init(Int32U IntrPriority);

/*************************************************************************
 * Function Name: clock_init
 * Parameters: none
 *
 * Return: none
 *
 * Description: The current clock time, measured in system ticks
 *
 *************************************************************************/
clock_time_t clock_time(void);

#endif /* __CLOCK_ARCH_H__ */
