/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : LPC24xx_enet.c
 *    Description : MAC/DMA Controller with DMA (ENET) driver
 *
 *    History :
 *    1. Date        : August 13, 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 4280 $
 **************************************************************************/
#include "LPC24xx_enet.h"

#pragma segment="EMAC_DMA_RAM"

#pragma location="EMAC_DMA_RAM"
#pragma data_alignment=4
__no_init EnetDmaRxDesc_t EnetDmaRx[ENET_DMA_DESC_NUMB];
#pragma location="EMAC_DMA_RAM"
#pragma data_alignment=8
__no_init EnetDmaRxStatus_t EnetDmaRxSta[ENET_DMA_DESC_NUMB];
#pragma location="EMAC_DMA_RAM"
#pragma data_alignment=4
__no_init Int8U RxBuff0[EMAC_MAX_PACKET_SIZE];
#pragma location="EMAC_DMA_RAM"
#pragma data_alignment=4
__no_init Int8U RxBuff1[EMAC_MAX_PACKET_SIZE];


#pragma location="EMAC_DMA_RAM"
#pragma data_alignment=4
__no_init EnetDmaTxDesc_t EnetDmaTx[ENET_DMA_DESC_NUMB];
#pragma location="EMAC_DMA_RAM"
#pragma data_alignment=4
__no_init EnetDmaTxStatus_t EnetDmaTxSta[ENET_DMA_DESC_NUMB];
#pragma location="EMAC_DMA_RAM"
#pragma data_alignment=4
__no_init Int8U TxBuff0[EMAC_MAX_PACKET_SIZE];
#pragma location="EMAC_DMA_RAM"
#pragma data_alignment=4
__no_init Int8U TxBuff1[EMAC_MAX_PACKET_SIZE];

#ifndef MICRO_BLOX_LPC2478
static Int8U PhyAddr;
#else
static const Int8U PhyAddr = 1;
#endif

#define MIND_BUSY        (1u<<0)
#define MIND_NOT_VALID   (1u<<2)
/*************************************************************************
 * Function Name:
 * Parameters: None
 *
 * Return: Boolean
 *
 * Description: Init  MAC/DMA Controller
 *
 *************************************************************************/
Boolean tapdev_init(void)
{
Int32U Reg,to;

  // clk enable
  PCONP_bit.PCENET = 1;
  POWERDOWN_bit.POWERDOWN = 0;

  // Pins assignment
  //------------------------------------------------------
  // Write to PINSEL2/3 to select the PHY functions on P1[17:0]
  // P1.6, ENET-TX_CLK, has to be set for Rev '-' devices and it
  // must not be set for Rev 'A’ and newer devices
  //------------------------------------------------------
  Reg = MAC_MODULEID;

  if ( Reg == OLD_EMAC_MODULE_ID )
  {
    // On Rev. '-', MAC_MODULEID should be equal to OLD_EMAC_MODULE_ID, P1.6 should be set.
    // selects P1[0,1,4,6,8,9,10,14,15]
    PINSEL2 &= BIN32(00001111,11000000,11001100,11110000);
    PINSEL2 |= BIN32(01010000,00010101,00010001,00000101);
  }
  else
  {
    // on rev. 'A', MAC_MODULEID should not equal to OLD_EMAC_MODULE_ID, P1.6 should not be set.
    // selects P1[0,1,4,8,9,10,14,15]
    PINSEL2 &= BIN32(00001111,11000000,11111100,11110000);
    PINSEL2 |= BIN32(01010000,00010101,00000001,00000101);
  }

  // selects P1[17:16]
  PINSEL3 &= BIN32(11111111,11111111,11111111,11110000);
  PINSEL3 |= BIN32(00000000,00000000,00000001,00000101);

  // Reset entire MAC
  MAC1 = 0x0000CF00;
  COMMAND = 0x0038; // reset all control registers
  MAC1 = 0;
  COMMAND_bit.RMII = 1;
  SUPP = 0;
  TEST = 0;

  // write the station address registers
  SA0 = (UIP_ETHADDR1<<8) | UIP_ETHADDR0;
  SA1 = (UIP_ETHADDR3<<8) | UIP_ETHADDR2;
  SA2 = (UIP_ETHADDR5<<8) | UIP_ETHADDR4;

  MAXF = 0x600;
  MCFG = 0x8018;  // clk/20
  MCMD = 0;
  // MIIM init
  MCFG_bit.RSTMIIMGMT = 0;

  // descriptors init
  MAC1_bit.RSTMCSTX = 1;
  COMMAND_bit.TXENABLE = 0;
  MAC1_bit.RSTTX = 1;
  COMMAND_bit.TXRESET = 1;
  MAC1_bit.RSTMCSTX = 0;
  MAC1_bit.RSTTX = 0;

  MAC1_bit.RE = 0;
  MAC1_bit.RSTMCSRX = 1;
  COMMAND_bit.RXENABLE = 0;
  MAC1_bit.RSTRX = 1;
  COMMAND_bit.RXRESET = 1;
  MAC1_bit.RSTMCSRX = 0;
  MAC1_bit.RSTRX = 0;

  RXDESCRIPTORNUMBER = ENET_DMA_DESC_NUMB-1;
  TXDESCRIPTORNUMBER = ENET_DMA_DESC_NUMB-1;
  COMMAND_bit.PASSRUNTFRAME = 1;
  COMMAND_bit.PASSRXFILTER = 1;
  MAC1_bit.PARF = 1;
  MAC2 = 0x30;

  CLRT_bit.RM    = 0xF;
  CLRT_bit.CW    = 0x37;
  IPGR_bit.IPGR2 = 0x12;
  IPGR_bit.IPGR1 = 0x0c;
  IPGT           = 0x12;
  MAC2_bit.FD = COMMAND_bit.FULLDUPLEX = 0;
  SUPP_bit.SPEED = 0;


  // find PHY address
#ifndef MICRO_BLOX_LPC2478
  for(PhyAddr = 1; PhyAddr < 32; ++PhyAddr)
  {
    // See Micrel PHY KS8721 Users Manual for more details
    if((ENET_MIIReadRegister(PhyAddr, PHY_PHYIDR1) & 0xFFFF) != 0x0022)
    {
      continue;
    }

    if ((ENET_MIIReadRegister(PhyAddr, PHY_PHYIDR2) & 0xFFFF) == 0x1619)
    {
      break;
    }
  }

  if(PhyAddr == 32)
  {
    return(FALSE);
  }
#endif
#ifdef TRACE_PHY
  printf("PHY Address - %d\r\n",PhyAddr);
#endif
  ENET_MIIWriteRegister(PhyAddr,PHY_BMCR,BMCR_RESET);

  to = PHY_TO;

  while(to)
  {
    Reg = ENET_MIIReadRegister(PhyAddr,PHY_BMCR);

    if(!(Reg & BMCR_RESET))
    {
      break;
    }
  }

  if(!to)
  {
    return(FALSE);
  }

#ifdef TRACE_PHY
  Reg = ENET_MIIReadRegister(PhyAddr,0);
  printf("PHY_BMCR(0) - 0x%04X\n\r",Reg);
  Reg = ENET_MIIReadRegister(PhyAddr,1);
  printf("PHY_BMSR(1) - 0x%04X\n\r",Reg);
  Reg = ENET_MIIReadRegister(PhyAddr,0x4);
  printf("PHY_ANAR(4) - 0x%04X\n\r",Reg);
  Reg = ENET_MIIReadRegister(PhyAddr,0x5);
  printf("PHY_ANLPAR(5) - 0x%04X\n\r",Reg);
  Reg = ENET_MIIReadRegister(PhyAddr,0x6);
  printf("PHY_ANER(6) - 0x%04X\n\r",Reg);
  Reg = ENET_MIIReadRegister(PhyAddr,0x1f);
  printf("PHY_100PHY(1F) - 0x%04X\n\r",Reg);
#endif // TRACE_PHY

  if(ENET_MIIReadRegister(PhyAddr,PHY_BMSR ) & BMSR_NOPREAM)
  {
    // PHY support preamble suppression
    MCFG |= 1 << 1;
  }


  ENET_MIIReadRegister(PhyAddr,PHY_PHYIDR1);
  ENET_MIIReadRegister(PhyAddr,PHY_PHYIDR2);

#if AUTO_NEGOTIATION_ENA != 0
  // PHY interrupt init, clear and enable
  // Set Auto-Negotiation Advertisement
  ENET_MIIWriteRegister(PhyAddr,PHY_ANAR, 1 | ANAR_10BT | ANAR_10BT_FULL | ANAR_100BT | ANAR_100BT_FULL);
  // Enable Auto-Negotiation
  Reg = BMCR_AN | BMCR_RE_AN;
#else

  Reg = 0;

  if(FIX_DUPLEX == FULL_DUPLEX)
  {
    Reg |= BMCR_DUPLEX;
  }

  if(FIX_SPEED == SPEED_100)
  {
    Reg |= BMCR_SPEED_100;
  }

#endif

  ENET_MIIWriteRegister(PhyAddr,PHY_BMCR,Reg);

  to = PHY_TO;

  while(to)
  {
    --to;
    if(ENET_MIIReadRegister(PhyAddr,PHY_BMSR) & BMSR_LINK_ESTABLISHED)
    {
      break;
    }
  }

  if(!to)
  {
    return(FALSE);
  }

#ifdef TRACE_PHY
  Reg = ENET_MIIReadRegister(PhyAddr,0);
  printf("PHY_BMCR(0) - 0x%04X\n\r",Reg);
  Reg = ENET_MIIReadRegister(PhyAddr,1);
  printf("PHY_BMSR(1) - 0x%04X\n\r",Reg);
  Reg = ENET_MIIReadRegister(PhyAddr,0x4);
  printf("PHY_ANAR(4) - 0x%04X\n\r",Reg);
  Reg = ENET_MIIReadRegister(PhyAddr,0x5);
  printf("PHY_ANLPAR(5) - 0x%04X\n\r",Reg);
  Reg = ENET_MIIReadRegister(PhyAddr,0x6);
  printf("PHY_ANER(6) - 0x%04X\n\r",Reg);
  Reg = ENET_MIIReadRegister(PhyAddr,0x1f);
  printf("PHY_100PHY(1F) - 0x%04X\n\r",Reg);
#endif // TRACE_PHY

#ifndef MICRO_BLOX_LPC2478
  Reg = ENET_MIIReadRegister(PhyAddr,PHY_100PHY) & PHYCR_MODE;

  switch(Reg)
  {
  case 0x04:  // 10 BASE T Half-duplex
#ifdef TRACE_PHY
    printf("10 BASE T Half-duplex\r\n");
#endif
    Reg = 0;
    break;
  case 0x08:  // 100 BASE TX Half-duplex
#ifdef TRACE_PHY
    printf("100 BASE TX Half-duplex\r\n");
#endif
    Reg = BMCR_SPEED_100;
    SUPP_bit.SPEED = 1;
    break;
  case 0x14: // 10 BASE T Full-duplex
#ifdef TRACE_PHY
    printf("10 BASE T Full-duplex\r\n");
#endif
    Reg = BMCR_DUPLEX;
    IPGT = 0x15;
    MAC2_bit.FD = COMMAND_bit.FULLDUPLEX = 1;
    break;
  case 0x18: // 100 BASE TX Full-duplex
#ifdef TRACE_PHY
    printf("100 BASE TX Full-duplex\r\n");
#endif
    Reg = BMCR_SPEED_100 | BMCR_DUPLEX;
    IPGT = 0x15;
    MAC2_bit.FD = COMMAND_bit.FULLDUPLEX = 1;
    SUPP_bit.SPEED = 1;
    break;
  default:
    return(FALSE);
  }
#else
  Reg = ENET_MIIReadRegister(PhyAddr,PHY_PHYSTS) & (PHYCR_LINK | PHYCR_SPEED | PHYCR_FULL_DUPEX);

  switch(Reg)
  {
  case 0x03:  // 10 BASE T Half-duplex
#ifdef TRACE_PHY
    printf("10 BASE T Half-duplex\r\n");
#endif
    Reg = 0;
    break;
  case 0x01:  // 100 BASE TX Half-duplex
#ifdef TRACE_PHY
    printf("100 BASE TX Half-duplex\r\n");
#endif
    Reg = BMCR_SPEED_100;
    SUPP_bit.SPEED = 1;
    break;
  case 0x07: // 10 BASE T Full-duplex
#ifdef TRACE_PHY
    printf("10 BASE T Full-duplex\r\n");
#endif
    Reg = BMCR_DUPLEX;
    IPGT = 0x15;
    MAC2_bit.FD = COMMAND_bit.FULLDUPLEX = 1;
    break;
  case 0x5: // 100 BASE TX Full-duplex
#ifdef TRACE_PHY
    printf("100 BASE TX Full-duplex\r\n");
#endif
    Reg = BMCR_SPEED_100 | BMCR_DUPLEX;
    IPGT = 0x15;
    MAC2_bit.FD = COMMAND_bit.FULLDUPLEX = 1;
    SUPP_bit.SPEED = 1;
    break;
  default:
    return(FALSE);
  }
#endif
  // Disable Auto-Negotiation and update speed and duplex settings
  ENET_MIIWriteRegister(PhyAddr,PHY_BMCR,Reg);

  to = PHY_TO;

  while(to)
  {
    --to;
    if(ENET_MIIReadRegister(PhyAddr,PHY_BMSR) & BMSR_LINK_ESTABLISHED)
    {
      break;
    }
  }

  if(!to)
  {
    return(FALSE);
  }

  for(Int32U i = 0; i < ENET_DMA_DESC_NUMB; ++i)
  {
    EnetDmaRx[i].EnetRxCtrl.Size = EMAC_MAX_PACKET_SIZE-1;
    EnetDmaRx[i].EnetRxCtrl.Intr = 1;
    EnetDmaRxSta[i].Data[0] = EnetDmaRxSta[i].Data[1] = 0;

    EnetDmaTx[i].EnetTxCtrl.Data = (1<<31) | (1<<30) | (1<<29) | (1<<28) | (1<<26) | (EMAC_MAX_PACKET_SIZE-1);
    EnetDmaTxSta[i].Data = 0;

  }

  EnetDmaRx[0].pBuffer = (pInt32U)RxBuff0;
  EnetDmaRx[1].pBuffer = (pInt32U)RxBuff1;
  EnetDmaTx[0].pBuffer = (pInt32U)TxBuff0;
  EnetDmaTx[1].pBuffer = (pInt32U)TxBuff1;
  RXCONSUMEINDEX = TXPRODUCEINDEX = 0;
  TXDESCRIPTOR = (Int32U)EnetDmaTx;
  TXSTATUS = (Int32U)EnetDmaTxSta;
  RXDESCRIPTOR = (Int32U)EnetDmaRx;
  RXSTATUS = (Int32U)EnetDmaRxSta;

  COMMAND_bit.RXENABLE = 1;
  MAC1_bit.RE = 1;
  COMMAND_bit.TXENABLE = 1;

  return(TRUE);
}

/*************************************************************************
 * Function Name: tapdev_read
 * Parameters:
 * Return:
 *
 * Description: Read data for MAC/DMA Controller
 *
 *************************************************************************/
Int32U tapdev_read(void * pPacket)
{
Int32U Indx = RXCONSUMEINDEX;
Int32U Size = EMAC_MAX_PACKET_SIZE;
  if(Indx == RXPRODUCEINDEX)
  {
    return(0);
  }
  Size = MIN(Size,(EnetDmaRxSta[Indx].RxSize+1));
  memcpy(pPacket,EnetDmaRx[Indx].pBuffer,Size);
  if(++Indx > RXDESCRIPTORNUMBER)
  {
    Indx = 0;
  }
  RXCONSUMEINDEX = Indx;
  return(Size);
}

/*************************************************************************
 * Function Name: tapdev_send
 * Parameters:
 * Return: Boolean
 *
 * Description: Send data to MAC/DMA Controller
 *
 *************************************************************************/
Boolean tapdev_send(void *pPacket, Int32U size)
{
Int32U Indx, IndxHold = TXPRODUCEINDEX + 1;
  if(size == 0)
  {
    return(TRUE);
  }
  if(IndxHold > TXDESCRIPTORNUMBER)
  {
    IndxHold = 0;
  }
  if(IndxHold == TXCONSUMEINDEX)
  {
    return(FALSE);
  }
  Indx = TXPRODUCEINDEX;
  size = MIN(size,EMAC_MAX_PACKET_SIZE);
  memcpy(EnetDmaTx[Indx].pBuffer,pPacket,size);
  EnetDmaTx[Indx].EnetTxCtrl.Size = size - 1;
  TXPRODUCEINDEX = IndxHold;

  return(TRUE);
}

/*************************************************************************
 * Function Name: ENET_MIIWriteRegister
 * Parameters: Int8U DevId, Int8U RegAddr, Int32U Value
 * Return: none
 *
 * Description: Writes a value on the PHY registers
 *
 *************************************************************************/
static void ENET_MIIWriteRegister (Int8U DevId, Int8U RegAddr, Int32U Value)
{
  MCMD = 0;                   // set read operation
  MADR_bit.PHY_ADDR = DevId;  // set the MII Physical address
  MADR_bit.REGADDR = RegAddr; // set the MII register address
  MWTD = Value;
  while(MIND_bit.BUSY);
}

/*************************************************************************
 * Function Name: ENET_MIIReadRegister
 * Parameters: Int8U DevId, Int8U RegAddr, Int32U Value
 * Return: Int32U
 *
 * Description: Read a value from the PHY registers
 *
 *************************************************************************/
static Int32U ENET_MIIReadRegister (Int8U DevId, Int8U RegAddr)
{
  MCMD = 0;
  MADR_bit.PHY_ADDR = DevId;  // set the MII Physical address
  MADR_bit.REGADDR = RegAddr; // set the MII register address
  MCMD = 1;                   // set read operation
  while(MIND &(MIND_BUSY | MIND_NOT_VALID));
  return(MRDD);
}


