/*************************************************************************
 *
*    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name   : smb380_drv.c
 *    Description : SMB380 acceleration sensor driver (I2C data mode)
 *
 *    History :
 *    1. Date        : 13, February 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *
 *    $Revision: 28 $
 **************************************************************************/
#include "smb380_drv.h"
#include "i2c0_drv.h"

/*************************************************************************
 * Function Name: SMB380_Init
 * Parameters: none
 *
 * Return: SMB380_Status_t
 *
 * Description: SMB380 init
 *
 *************************************************************************/
SMB380_Status_t SMB380_Init(void)
{
  //Init I2C module as master
  I2C_InitMaster(I2C_SPEED);
  
  return SMB380_PASS;
}
/*************************************************************************
 * Function Name: SMB380_GetID
 * Parameters: none
 *
 * Return: SMB380_Status_t
 *
 * Description: SMB380 get chip ID and revision
 *
 *************************************************************************/
SMB380_Status_t SMB380_GetID (pInt8U pChipId, pInt8U pRevision)
{
unsigned char buf[2] = {SMB380_CHIP_ID};
  //Write the address of Chip ID register
  I2C_MasterWrite(SMB380_ADDR, buf, 1);
  
  I2C_MasterRead(SMB380_ADDR, buf, 2);
  *pChipId = buf[0];
  *pRevision = buf[1];
  
  return SMB380_PASS;
}

/*************************************************************************
 * Function Name: SMB380_Init
 * Parameters: none
 *
 * Return: SMB380_Status_t
 *
 * Description: SMB380 init
 *
 *************************************************************************/
SMB380_Status_t SMB380_GetData (pSMB380_Data_t pData)
{
  unsigned char regaddr = SMB380_ACCX_ADDR;
  
  I2C_MasterWrite(SMB380_ADDR, &regaddr, 1);
  
  I2C_MasterRead(SMB380_ADDR, (unsigned char *)pData, sizeof(SMB380_Data_t));
  
  return SMB380_PASS;
}


