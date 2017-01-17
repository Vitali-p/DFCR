/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : LPC24xx_enet.h
 *    Description : MAC/DMA Controller with DMA (ENET) driver include file
 *
 *    History :
 *    1. Date        : August 13, 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 28 $
 **************************************************************************/
#include "includes.h"
#include <io_macros.h>

#ifndef __LPC24XX_ENET_H
#define __LPC24XX_ENET_H

#define EMAC_MAX_PACKET_SIZE (UIP_CONF_BUFFER_SIZE + 16)
#define ENET_DMA_DESC_NUMB   2
#define ENET_OK  (1)
#define ENET_NOK (0)

/* EMAC MODULE ID	*/
#define OLD_EMAC_MODULE_ID	((0x3902 << 16) | 0x2000)
#define MAC_MODULEID	      (*(volatile unsigned int *)(0xFFE00FFC))

#define AUTO_NEGOTIATION_ENA 0  // Enable PHY Auto-negotiation

#define PHY_TO               666666     // ~10sec

#ifndef MICRO_BLOX_LPC2478
// KS8721B1
#define PHY_BMCR                0x0000
#define PHY_BMSR                0x0001
#define PHY_PHYIDR1             0x0002
#define PHY_PHYIDR2             0x0003
#define PHY_ANAR                0x0004
#define PHY_ANLPAR              0x0005
#define PHY_ANLPARNP            0x0005
#define PHY_ANER                0x0006
#define PHY_ANNPTR              0x0007
#define PHY_LPNPA               0x0008

#define PHY_RECR                0x0015
#define PHY_INTCTRL             0x001B
#define PHY_100PHY              0x001F

/* BMCR bitmap */
#define BMCR_RESET              0x8000
#define BMCR_LOOPBACK           0x4000
#define BMCR_SPEED_100          0x2000
#define BMCR_AN                 0x1000
#define BMCR_POWERDOWN          0x0800
#define BMCR_ISOLATE            0x0400
#define BMCR_RE_AN              0x0200
#define BMCR_DUPLEX             0x0100

/* BMSR bitmap */
#define BMSR_100BE_T4           0x8000
#define BMSR_100TX_FULL         0x4000
#define BMSR_100TX_HALF         0x2000
#define BMSR_10BE_FULL          0x1000
#define BMSR_10BE_HALF          0x0800
#define BMSR_NOPREAM            0x0040
#define BMSR_AUTO_DONE          0x0020
#define BMSR_REMOTE_FAULT       0x0010
#define BMSR_NO_AUTO            0x0008
#define BMSR_LINK_ESTABLISHED   0x0004

/* PHY_ANAR bitmap */
#define ANAR_NEXT_PAGE          0x8000
#define ANAR_REMOTE_FAULT       0x2000
#define ANAR_PAUSE              0x0400
#define ANAR_100BE_T4           0x0200
#define ANAR_100BT_FULL         0x0100
#define ANAR_100BT              0x0080
#define ANAR_10BT_FULL          0x0040
#define ANAR_10BT               0x0020
#define ANAR_SELECTOR           0x001F

/* ANLPAR bitmap */
#define ANLPAR_NEXT_PAGE        0x8000
#define ANLPAR_ACKN             0x4000
#define ANLPAR_REMOTE_FAULT     0x2000
#define ANLPAR_PAUSE            0x0C00
#define ANLPAR_100BE_T4         0x0200
#define ANLPAR_100BT_FULL       0x0100
#define ANLPAR_100BT            0x0080
#define ANLPAR_10BT_FULL        0x0040
#define ANLPAR_10BT             0x0020
#define ANLPAR_SELECTOR         0x001F

/* PHY_100PHY setting */
#define PHYCR_MDIX_DIS          0x2000
#define PHYCR_ENR_DET           0x1000
#define PHYCR_FORCE_LINK        0x0800
#define PHYCR_POWER_SAVING      0x0400
#define PHYCR_INTR_LEVEL        0x0200
#define PHYCR_JABBER_ENA        0x0100
#define PHYCR_PAUSE_ENA         0x0080
#define PHYCR_PHY_ISO           0x0040
#define PHYCR_MODE              0x001C
#define PHYCR_SQE_TST_ENA       0x0002
#define PHYCR_SCRAM_DIS         0x0001

#define SPEED_100               1
#define SPEED_10                0
#define FULL_DUPLEX             1
#define HALF_DUPLEX             0

#define FIX_SPEED               SPEED_100
#define FIX_DUPLEX              FULL_DUPLEX
#else
// DP83640
#define PHY_BMCR                0x0000
#define PHY_BMSR                0x0001
#define PHY_PHYIDR1             0x0002
#define PHY_PHYIDR2             0x0003
#define PHY_ANAR                0x0004
#define PHY_ANLPAR              0x0005
#define PHY_ANLPARNP            0x0005
#define PHY_ANER                0x0006
#define PHY_ANNPTR              0x0007
#define PHY_LPNPA               0x0008

#define PHY_RECR                0x0015
#define PHY_PHYSTS              0x0010

/* BMCR bitmap */
#define BMCR_RESET              0x8000
#define BMCR_LOOPBACK           0x4000
#define BMCR_SPEED_100          0x2000
#define BMCR_AN                 0x1000
#define BMCR_POWERDOWN          0x0800
#define BMCR_ISOLATE            0x0400
#define BMCR_RE_AN              0x0200
#define BMCR_DUPLEX             0x0100

/* BMSR bitmap */
#define BMSR_100BE_T4           0x8000
#define BMSR_100TX_FULL         0x4000
#define BMSR_100TX_HALF         0x2000
#define BMSR_10BE_FULL          0x1000
#define BMSR_10BE_HALF          0x0800
#define BMSR_NOPREAM            0x0040
#define BMSR_AUTO_DONE          0x0020
#define BMSR_REMOTE_FAULT       0x0010
#define BMSR_NO_AUTO            0x0008
#define BMSR_LINK_ESTABLISHED   0x0004

/* PHY_ANAR bitmap */
#define ANAR_NEXT_PAGE          0x8000
#define ANAR_REMOTE_FAULT       0x2000
#define ANAR_PAUSE              0x0400
#define ANAR_100BE_T4           0x0200
#define ANAR_100BT_FULL         0x0100
#define ANAR_100BT              0x0080
#define ANAR_10BT_FULL          0x0040
#define ANAR_10BT               0x0020
#define ANAR_SELECTOR           0x001F

/* ANLPAR bitmap */
#define ANLPAR_NEXT_PAGE        0x8000
#define ANLPAR_ACKN             0x4000
#define ANLPAR_REMOTE_FAULT     0x2000
#define ANLPAR_PAUSE            0x0C00
#define ANLPAR_100BE_T4         0x0200
#define ANLPAR_100BT_FULL       0x0100
#define ANLPAR_100BT            0x0080
#define ANLPAR_10BT_FULL        0x0040
#define ANLPAR_10BT             0x0020
#define ANLPAR_SELECTOR         0x001F

/* PHY_PHYSTS setting */
#define PHYCR_REMOTE_FAULT      0x0040
#define PHYCR_JABBER            0x0020
#define PHYCR_AUTO_DONE         0x0010
#define PHYCR_FULL_DUPEX        0x0004
#define PHYCR_SPEED             0x0002
#define PHYCR_LINK              0x0001

#define SPEED_100               1
#define SPEED_10                0
#define FULL_DUPLEX             1
#define HALF_DUPLEX             0

#define FIX_SPEED               SPEED_100
#define FIX_DUPLEX              FULL_DUPLEX
#endif // MICRO_BLOX_LPC2478
typedef union _EnetRxCR_t
{
  Int32U Data;
  struct {
    Int32U  DMA_XFERCOUNT :12;
    Int32U  CONT_EN       : 1;
    Int32U                : 1;
    Int32U  NXT_EN        : 1;
    Int32U  DLY_EN        : 1;
    Int32U                : 1;
    Int32U  ENTRY_TRIG    : 5;
    Int32U  ADDR_WRAP     :10;
  };
} EnetRxCR_t, * pEnetRxCR_t;

typedef union _EnetRxSR_t
{
  Int32U Data;
  struct {
    Int32U  FrameLength     :11;
    Int32U                  : 1;
    Int32U  Overlength      : 1;
    Int32U  FalseCarrier    : 1;
    Int32U  WatchdogTO      : 1;
    Int32U  RuntFrame       : 1;
    Int32U  Valid           : 1;
    Int32U  LateCollision   : 1;
    Int32U  FrameType       : 1;
    Int32U  MIIError        : 1;
    Int32U  ExtraBits       : 1;
    Int32U  CRCError        : 1;
    Int32U  OneLevelVLAN    : 1;
    Int32U  TwoLevelVLAN    : 1;
    Int32U  LengthError     : 1;
    Int32U  ControlFrame    : 1;
    Int32U  UnsupportedFrame: 1;
    Int32U  MulticastFrame  : 1;
    Int32U  BroadcastFrame  : 1;
    Int32U  FilteringFail   : 1;
    Int32U  PacketFilter    : 1;
    Int32U  FrameAbort      : 1;
  };
} EnetRxSR_t, * pEnetRxSR_t;

typedef union _EnetTxSR_t
{
  Int32U Data;
  struct {
    Int32U  FrameAborted    : 1;
    Int32U                  : 1;
    Int32U  NoCarrier       : 1;
    Int32U  LossOfCarrier   : 1;
    Int32U  ExcessiveDef    : 1;
    Int32U  LateCollision   : 1;
    Int32U  ExcessiveColl   : 1;
    Int32U  UnderRun        : 1;
    Int32U  Deferred        : 1;
    Int32U  LateCollisionObs: 1;
    Int32U  CollisionCount  : 4;
    Int32U                  : 2;
    Int32U  Valid           : 1;
    Int32U                  : 1;
    Int32U  ByteCounter     :13;
    Int32U  PacketRetry     : 1;
  };
} EnetTxSR_t, * pEnetTxSR_t;


typedef union _EnetTxCtrl_t
{
  Int32U Data;
  struct
  {
    Int32U Size     : 11;
    Int32U          : 15;
    Int32U Override :  1;
    Int32U Huge     :  1;
    Int32U Pad      :  1;
    Int32U CRC      :  1;
    Int32U Last     :  1;
    Int32U Intr     :  1;
  };
} EnetTxCtrl_t, *pEnetTxCtrl_t;

typedef struct _EnetDmaTxDesc_t
{
  pInt32U      pBuffer;
  EnetTxCtrl_t EnetTxCtrl;
} EnetDmaTxDesc_t, * pEnetDmaTxDesc_t;

typedef union _EnetDmaTxStatus_t
{
  Int32U Data;
  struct
  {
    Int32U                    :21;
    Int32U CollisionCount     : 4;
    Int32U Defer              : 1;
    Int32U ExcessiveDefer     : 1;
    Int32U ExcessiveCollision : 1;
    Int32U LateCollision      : 1;
    Int32U Underrun           : 1;
    Int32U NoDescriptor       : 1;
    Int32U Error              : 1;
  };
} EnetDmaTxStatus_t, * pEnetDmaTxStatus_t;

typedef struct _EnetRxCtrl_t
{
  Int32U Size     : 11;
  Int32U          : 20;
  Int32U Intr     :  1;
} EnetRxCtrl_t, *pEnetRxCtrl_t;

typedef struct _EnetDmaRxDesc_t
{
  pInt32U      pBuffer;
  EnetRxCtrl_t EnetRxCtrl;
} EnetDmaRxDesc_t, * pEnetDmaRxDesc_t;

typedef union _EnetDmaRxStatus_t
{
  Int32U Data[2];
  struct
  {
    Int32U RxSize         :11;
    Int32U                : 7;
    Int32U ControlFrame   : 1;
    Int32U VLAN           : 1;
    Int32U FailFilter     : 1;
    Int32U Multicast      : 1;
    Int32U Broadcast      : 1;
    Int32U CRCError       : 1;
    Int32U SymbolError    : 1;
    Int32U LengthError    : 1;
    Int32U RangeError     : 1;
    Int32U AlignmentError : 1;
    Int32U Overrun        : 1;
    Int32U NoDescriptor   : 1;
    Int32U LastFlag       : 1;
    Int32U Error          : 1;
    Int32U SAHashCRC      : 8;
    Int32U                : 8;
    Int32U DAHashCRC      : 8;
    Int32U                : 8;
  };
} EnetDmaRxStatus_t, * pEnetDmaRxStatus_t;

/*************************************************************************
 * Function Name:
 * Parameters: None
 *
 * Return: None
 *
 * Description: Init  MAC/DMA Controller
 *
 *************************************************************************/
Boolean tapdev_init(void);

/*************************************************************************
 * Function Name: tapdev_read
 * Parameters:
 * Return:
 *
 * Description: Read data for MAC/DMA Controller
 *
 *************************************************************************/
Int32U tapdev_read(void * pPacket);

/*************************************************************************
 * Function Name: tapdev_send
 * Parameters:
 * Return: Boolean
 *
 * Description: Send data to MAC/DMA Controller
 *
 *************************************************************************/
Boolean tapdev_send (void *pPacket, Int32U size);

/*************************************************************************
 * Function Name: ENET_MIIWriteRegister
 * Parameters: Int8U DevId, Int8U RegAddr, Int32U Value
 * Return: none
 *
 * Description: Writes a value on the PHY registers
 *
 *************************************************************************/
static void ENET_MIIWriteRegister (Int8U DevId, Int8U RegAddr, Int32U Value);

/*************************************************************************
 * Function Name: ENET_MIIReadRegister
 * Parameters: Int8U DevId, Int8U RegAddr, Int32U Value
 * Return: Int32U
 *
 * Description: Read a value from the PHY registers
 *
 *************************************************************************/
static Int32U ENET_MIIReadRegister (Int8U DevId, Int8U RegAddr);

#endif // __LPC24XX_ENET_H

