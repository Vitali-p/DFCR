#include "includes.h"

#define NONPROT 0xFFFFFFFF
#define CRP1  	0x12345678
#define CRP2  	0x87654321
/*If CRP3 is selected, no future factory testing can be performed on the device*/
#define CRP3  	0x43218765

#ifndef SDRAM_DEBUG
#pragma segment=".crp"
#pragma location=".crp"
__root const unsigned crp = NONPROT;
#endif

#define BUF ((struct uip_eth_hdr *)&uip_buf[0])
#define LCD_VRAM_BASE_ADDR ((Int32U)&SDRAM_BASE_ADDR)

//__________Screen_Text_____________
extern FontType_t Terminal_6_8_6;
extern FontType_t Terminal_9_12_6;
extern FontType_t Terminal_18_24_12;

extern Int32U SDRAM_BASE_ADDR;

Int32U cursor_x = (C_GLCD_H_SIZE - CURSOR_H_SIZE)/2, cursor_y = (C_GLCD_V_SIZE - CURSOR_V_SIZE)/2;

//____________Other_parameters______________
#define fs 10000
#define LenCircReg 3000
Boolean LightSwitch = false, LightAuto = false, SocketSwitch = false, SocketAuto = false;

Int32U _ADCVal;
Int32U _ADCStatus;
Int32U _regCount = 0;
Int32U _waveForm[LenCircReg];
Boolean _CountFlag = false;
Int32U _NumberOfCrossings = 0;
Int32U _Debugvar = 0;
float LOWfilterWave[LenCircReg];
Boolean flip = true;

////////////////////////////////
double _freq;
Int32U _CrossingsLocation[29];
Int32U _CrossIndex;
Int32U ff = 0;
char _str7[7];

/////// Current  /////////////////
Int32U _CurrentWave[LenCircReg];
double _CurrnetCorrectionFactor;
double _RMSCurrent;
/////// Power  /////////////////
double _RMSVoltage;
double _Power;

//****************General_Global_Functions*************************************
//____________Convert_Reading_to_3-digit_numbers______________
void getString(double freq){
  //Aquire 100s
  char hundreds = (char)(freq / 100);
  char tens = (char)((freq - (double)hundreds*100) / 10);
  char ones = (char)((freq - (double)hundreds*100 - (double)tens*10) / 1);
  char tenths = (char)((freq - (double)hundreds*100 - (double)tens*10 - (double)ones*1) / 0.1);
  char hundreths = (char)((freq - (double)hundreds*100 - (double)tens*10 - (double)ones*1 - (double)tenths*0.1) / 0.01);
  char thous = (char)((freq - (double)hundreds*100 - (double)tens*10 - (double)ones*1 - (double)tenths*0.1 - (double)hundreths*0.01) / 0.001);
  _str7[0] = hundreds + 0x30;
  _str7[1] =tens + 0x30;
  _str7[2] =ones + 0x30;
  _str7[3] =0x2E; // Decimal marker
  _str7[4] =tenths + 0x30;
  _str7[5] =hundreths + 0x30;
  _str7[6] =thous + 0x30;
}
double getRMS(Int32U Vector[], Int32U length){
  //  Int32U length = sizeof(Vector);
  double result = 0;
  double sum = 0;
  //  Int32U dd[] = Vector;
  //  Int32U length = sizeof(*Vector);
  for(Int32U ii = 0; ii< length; ii++){ //Sum up the array
    sum += Vector[ii]*Vector[ii];
  }
  result = sqrt(sum/(double)length);  // must include math.h
  return result;
}
double getAverage(Int32U Vector[]){
  Int32U length = sizeof(Vector);
  double result = 0;
  double sum = 0;
  for(Int32U ii = 0; ii< length; ii++){ //Sum up the array
    sum += Vector[ii]*Vector[ii];
  }
  result = sum/(double)length;
  return result;
}

/*************************************************************************
* Function Name: textToScreen
* Parameters: Frequency reading.
* Return: none
* Description: Print readings on to the screen.
*************************************************************************/
void textToScreen(double Reading){
  /*
  Int32U d1, d2;
  char digitString [10];
  
  // Calculation from digit to string.  
  d1 = (Int32U)Reading; // Decimal precision: 5 digits
  d2 = (Int32U)((Reading-d1)*100000);
  sprintf(digitString, "%d.%d", d1, d2); // Convert digital reading to string.
  */   
  // Convert reading to comma values.
  getString(Reading);
  GLCD_SetFont(&Terminal_18_24_12,0,0xEFEFEF); // Set current font, font color and background color.
  
  //Power
  GLCD_SetWindow(17,67,99,86);  // Set draw window XY coordinate in pixels. (X_Left, Y_Up, X_Right, Y_Down)  
  GLCD_TextSetPos(0,0);         // Set text X,Y coordinate in characters.
  GLCD_print(_str7);            // Print formated string on the LCD.
  
  // Current
  GLCD_SetWindow(17,107,99,126);
  GLCD_TextSetPos(0,0);
  GLCD_print(_str7); 
  
  // Voltage
  GLCD_SetWindow(17,147,99,166);
  GLCD_TextSetPos(0,0);
  GLCD_print(_str7); 
  
  // Frequency
  GLCD_SetWindow(17,186,99,205);
  GLCD_TextSetPos(0,0);      
  GLCD_print(_str7); 
}
/*************************************************************************
 * Function Name: updateLCDgraphics
 * Parameters: none
 * Return: none
 * Description: Updates LCD graphics, and sets triggers for manual or auto settings.
*************************************************************************/ 
void updateLCDgraphics(){
  // Turn on/off light.
  if((cursor_x >= 121)&(cursor_x <= 240) & (cursor_y >= 109)&(cursor_y <= 155)){
    if(LightSwitch){
      GLCD_LoadPic(121,109,&buttomOFFPic,0); // Load picture to screen, position:"light off".
      LightSwitch = false;
    }
    else {
      GLCD_LoadPic(121,109,&buttomONPic,0); // Load picture to screen, position:"light on".
      GLCD_LoadPic(247,109,&autoOFFPic,0); // Load picture to screen, position "light auto off".
      LightSwitch = true;
      LightAuto = false;
    }
  }     
  // Turn on/off socket.
  if((cursor_x >= 121)&(cursor_x <= 240) & (cursor_y >= 190)&(cursor_y <= 235)){
    if(SocketSwitch){
      GLCD_LoadPic(121,190,&buttomOFFPic,0); // Load picture to screen, position:"socket off".
      SocketSwitch = false;
    }
    else {
      GLCD_LoadPic(121,190,&buttomONPic,0); // Load picture to screen, position:"socket on".
      GLCD_LoadPic(247,190,&autoOFFPic,0); // Load picture to screen, position:"socket auto off". 
      SocketSwitch = true;
      SocketAuto = false;
    }
  }

  // Turn auto light.
  if((cursor_x >= 247)&(cursor_x <= 319) & (cursor_y >= 109)&(cursor_y <= 155)){
    if(LightAuto){
      GLCD_LoadPic(247,109,&autoOFFPic,0); // Load picture to screen, position "light auto OFF".
      LightAuto = false;
    }
    else {
      GLCD_LoadPic(247,109,&autoONPic,0); // Load picture to screen, position "light auto ON".
      GLCD_LoadPic(121,109,&buttomOFFPic,0); // Load picture to screen, position:"light OFF".
      LightAuto = true;
      LightSwitch = false; // TEST!
    }
  } 
  // Turn auto socket.
  if((cursor_x >= 247)&(cursor_x <= 319) & (cursor_y >= 190)&(cursor_y <= 238)){
    if(SocketAuto){
      GLCD_LoadPic(247,190,&autoOFFPic,0); // Load picture to screen, position "socket auto OFF".  
      SocketAuto = false;
    }
    else {
      GLCD_LoadPic(247,190,&autoONPic,0); // Load picture to screen, position "socket auto ON".  
      GLCD_LoadPic(121,190,&buttomOFFPic,0); // Load picture to screen, position:"socket OFF".
      SocketAuto = true; 
      SocketSwitch = false; // TEST!!
    }
  } 
}

// Lowpass filter (50Hz).
void lowPassFilter (Int32U input[], Int32U points, Int32U sampleRate){
  float fc = 50; //frequency
  float dt = 0.0001; //delta t maybe "1/sample rate"
  float RC = 1/(2*3.14159265359*fc);
  float alpha = dt/(RC+dt); //alpha
  
  for(Int32U ii = 1; ii <= LenCircReg-1; ii++){
    LOWfilterWave[ii] = (alpha*input[ii]) + (1-alpha)*LOWfilterWave[ii-1];
  }      
}

void clearArray(void){
  Int32U len = sizeof(_CrossingsLocation)/sizeof(Int32U);
  for (Int32U ii = 0; ii<len; ii++){
    _CrossingsLocation[ii] = 0;
  }
}

// Ports.
/**************************************************************************
* Function Name: initRelayPorts
* Parameters: none
* Return: none
* Description: Initiation of ports for controlling the relays as output.
*************************************************************************/ 
void initRelayPorts(void){
  // Set bit 11 and 19 as outputs.
  FIO0DIR_bit.P0_11 = 1;
  FIO0DIR_bit.P0_19 = 1;
}
/*************************************************************************
* Function Name: RelayCont
* Parameters: Frequency
* Return: none
* Description: Controlling output ports for the relay. In auto and manual mode.
*************************************************************************/ 
void RelayControl(float freq){
  if(freq <= 49.975 & (LightAuto || SocketAuto)){
    // Switch off the relays. Set ports individual ports P0.11 or P0.19 as low.
    if(LightAuto){
      FIO0CLR_bit.P0_11 = 1;    // Light.
    }
    if(SocketAuto){
      FIO0CLR_bit.P0_19 = 1;    // Socket.
    }
  }
  else if(freq >= 50.025 & (LightAuto || SocketAuto)){
    // Switch on the relays. Set ports individual ports P0.11 or P0.19 as high.
    if(LightAuto){
      FIO0SET_bit.P0_11 = 1;    // Light.
    }
    if(SocketAuto){
      FIO0SET_bit.P0_19 = 1;    // Socket.
    }
  }
  else if((LightSwitch || SocketSwitch)){
    if(LightSwitch){
      FIO0SET_bit.P0_11 = 1;    // Light on.
    }
    if(SocketSwitch){
      FIO0SET_bit.P0_19 = 1;    // Socket on.
    }    
  }
  else if((LightSwitch == false || SocketSwitch == false)&(LightAuto == false || SocketAuto == false)){
    if(LightSwitch == false){
      FIO0CLR_bit.P0_11 = 1;    // Light off.    
    }
    if(SocketSwitch == false){
      FIO0CLR_bit.P0_19 = 1;    // Socket off.    
    }
  }
}
/*************************************************************************
* Function Name: toogleLED
* Parameters: LEDnumber (1 "upper" or 2 "lower" LED).
* Return: none
* Description: Toggle boar LED.
*************************************************************************/ 
void toogleLED(int LEDnumber){
  if(LEDnumber == 1){
    USB_D_LINK_LED_FIO ^= USB_D_LINK_LED_MASK;    // Toggle USB Link LED.
  }
  if(LEDnumber == 2){
    USB_H_LINK_LED_FIO ^= USB_H_LINK_LED_MASK;    // Touch screen.
  }
}


/*************************************************************************
 * Function Name: initLED
 * Parameters: none
 * Return: none
 * Description: Initiation of ports for controlling the intern board LEDs.
 *************************************************************************/ 
void initLEDs(){
  // Top board LED (USB link).
  USB_D_LINK_LED_FDIR = USB_D_LINK_LED_MASK;
  USB_D_LINK_LED_FSET = USB_D_LINK_LED_MASK;
  
  // Bottom board LED (touched indication LED).
  USB_H_LINK_LED_SEL = 0; // GPIO
  USB_H_LINK_LED_FDIR |= USB_H_LINK_LED_MASK;
  USB_H_LINK_LED_FSET = USB_H_LINK_LED_MASK;
}

// Software delay function.
void softDelay(Int32U delayU32BitCount){
  while(delayU32BitCount > 0){
    delayU32BitCount--;
  }
}

/*Timer1*Handler***********************************************************
* Function Name: Timer1IntrHandler
* Parameters: none
* Return: none
* Description: Timer 1 interrupt handler
*************************************************************************/
void Timer1IntrHandler(void){ 
//  toogleTopLED();
  toogleLED(1);
  
  T1IR_bit.MR0INT = 1; // Clear the Timer 0 interrupt.
  VICADDRESS = 0;
  
  
  // ADC Related
  _ADCVal = ((ADDR2 & 0xFFC0)>>6);  //((AD0GDR & 0xFFC0)>>6);  //Get ADC Value
  _waveForm[_regCount] = _ADCVal;   // Store ADC Value in circular register
  
  _CurrentWave[_regCount] = ((ADDR3 & 0xFFC0)>>6);
  
  _regCount++;                      //Increment regcoun
  if(_regCount == LenCircReg){      //Check if regcount need to start over
    _regCount = 0;
    _CountFlag = true;
  }
}

/*************************************************************************
 * Function Name: uip_log
 * Parameters: none
 * Return: none
 * Description: Events logging
 *************************************************************************/
void uip_log (char *m)
{ }


int main(void){
  
  unsigned int i;
  uip_ipaddr_t ipaddr;
  struct timer periodic_timer, arp_timer;

  typedef Int32U ram_unit;
  ToushRes_t XY_Touch;
  Boolean Touch = FALSE;
  GLCD_Ctrl (FALSE);
  
  // Init GPIO
  GpioInit();
  
#ifndef SDRAM_DEBUG
  // MAM init
  MAMCR_bit.MODECTRL = 0;
  MAMTIM_bit.CYCLES  = 3;   // FCLK > 40 MHz
  MAMCR_bit.MODECTRL = 2;   // MAM functions fully enabled
  // Init clock
  InitClock();
  // SDRAM Init
  SDRAM_Init();
#endif // SDRAM_DEBUG

  // Init VIC
  VIC_Init();

  // Sys timer init 1/100 sec tick
  clock_init(2);
  timer_set(&periodic_timer, CLOCK_SECOND / 2);
  timer_set(&arp_timer, CLOCK_SECOND * 10);

  __enable_interrupt();

  // Initialize the ethernet device driver
  do
  {
//    GLCD_TextSetPos(0,0);
  }
  while(!tapdev_init());
//  GLCD_TextSetPos(0,0);

  // uIP web server
  // Initialize the uIP TCP/IP stack.
  uip_init();

  uip_ipaddr(ipaddr, 192,168,0,100);
  uip_sethostaddr(ipaddr);
  uip_ipaddr(ipaddr, 192,168,0,1);
  uip_setdraddr(ipaddr);
  uip_ipaddr(ipaddr, 255,255,255,0);
  uip_setnetmask(ipaddr);

  // Initialize the HTTP server.
  httpd_init();

  //______________INIT_FUNCTIONS_____________________
  initRelayPorts();
  initLEDs();
  
  //***_TIMER1_*************************************************
  // Enable TIMER1 clocks
  PCONP_bit.PCTIM1 = 1; // enable clock
  // Init Time1
  T1TCR_bit.CE = 0;     // counting  disable
  T1TCR_bit.CR = 1;     // set reset
  T1TCR_bit.CR = 0;     // release reset
  T1CTCR_bit.CTM = 0;   // Timer Mode: every rising PCLK edge
  T1MCR_bit.MR0I = 1;   // Enable Interrupt on MR0
  T1MCR_bit.MR0R = 1;   // Enable reset on MR0
  T1MCR_bit.MR0S = 0;   // Disable stop on MR0
  // set timer 1 period
  T1PR = 0;
  T1MR0 = SYS_GetFpclk(TIMER1_PCLK_OFFSET)/(fs);
  // init timer 1 interrupt
  T1IR_bit.MR0INT = 1;  // clear pending interrupt
  VIC_SetVectoredIRQ(Timer1IntrHandler,2,VIC_TIMER1);
  VICINTENABLE_bit.TIMER1 = 1;  // Enable interrupt on Timer 1.
  T1TCR_bit.CE = 1;             // Enable ounting.
  
  //***_ADC_*************************************************
  AD0CR_bit.PDN = 0;         // Disable ADC.
  PCONP_bit.PCAD = 1;        // Power on ADC, enable clock.
  
  PCLKSEL0_bit.PCLK_ADC = 1; // Enable ADC clock, CCLK = 72MHz.
  AD0CR_bit.CLKDIV = 17;     // 72MHz/(17+1)= 4MHz<=4.5 MHz.
  AD0CR_bit.BURST = 1;       // ADC is set to operate burst mode.
  
  PINSEL1_bit.P0_23 = 1;  // Set pin P0.25 to AD0[0], touch screen.
  PINSEL1_bit.P0_24 = 1;  // Set pin P0.25 to AD0[1], touch screen.  
  PINSEL1_bit.P0_25 = 1;  // Set pin P0.25 to AD0[2], ADC Voltage.
  PINSEL1_bit.P0_26 = 1;  // Set pin P0.25 to AD0[3], ADC Current.
  
  ADINTEN_bit.ADGINTEN = 0;   // Disable A/D global interrupt.
  AD0CR_bit.START = 0;        // Controll A/D conversion, stop conversion. 
  
  AD0CR_bit.SEL |= 0x0000000F; // Select Ch.0 to 3. Selects which of the AD0.7:0 pins are to be sampled.
  AD0CR_bit.PDN = 1;           // Enable the A/D, the converter is operational. 
  
  //***_LCD_*************************************************
  // GLCD init
  GLCD_Init (LogoPic.pPicStream, NULL); 
  
  // Init cursor
//  GLCD_Cursor_Dis(0);
//  GLCD_Copy_Cursor ((Int32U *)Cursor, 0, sizeof(Cursor)/sizeof(Int32U));
//  GLCD_Cursor_Cfg(CRSR_FRAME_SYNC | CRSR_PIX_32);
//  GLCD_Move_Cursor(cursor_x, cursor_y);
//  GLCD_Cursor_En(0);

    // Print on screen.
    GLCD_LoadPic(121,109,&buttomOFFPic,0); // Load picture to screen, position "light".
    GLCD_LoadPic(247,109,&autoOFFPic,0); // Load picture to screen, position "light auto".
  
    GLCD_LoadPic(121,190,&buttomOFFPic,0); // Load picture to screen, position "socket".
    GLCD_LoadPic(247,190,&autoOFFPic,0); // Load picture to screen, position "socket auto".  
  
    // Init touch screen
    TouchScrInit();
    
    __enable_interrupt();   // Enable global interrupt.
    GLCD_Ctrl (TRUE);       // Turn on LCD.
  //__________________________________________________________ 
  
// Test memmory.
#if 0
  SDRAM_Test();
#endif  
#if 0
  pInt32U pDst = (pInt32U)LCD_VRAM_BASE_ADDR;
  for(Int32U k = 0; k < C_GLCD_V_SIZE; k++)
  {
    for(Int32U i = 0 ; 8 > i; i++)
    {
      for(Int32U j = 0; 40 > j; j++)
      {
        switch(i)
        {
        case 0:
          *pDst++ = 0;
          break;
        case 1:
          *pDst++ = 0xFF;
          break;
        case 2:
          *pDst++ = 0xFF00;
          break;
        case 3:
          *pDst++ = 0xFFFF;
          break;
        case 4:
          *pDst++ = 0xFF0000;
          break;
        case 5:
          *pDst++ = 0xFF00FF;
          break;
        case 6:
          *pDst++ = 0xFFFF00;
          break;
        case 7:
          *pDst++ = 0xFFFFFF;
          break;
        }
      }
    }
  }
#endif
  
  
  while(1)
  {
//_____uIP_Web_session_______________________________________________    
    uip_len = tapdev_read(uip_buf);
    if(uip_len > 0){
      if(BUF->type == htons(UIP_ETHTYPE_IP))
      {
        uip_arp_ipin();
        uip_input();
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0){
          uip_arp_out();
          tapdev_send(uip_buf,uip_len);
        }
      }
      else if(BUF->type == htons(UIP_ETHTYPE_ARP)){
        uip_arp_arpin();
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0){
           tapdev_send(uip_buf,uip_len);
        }
      }
    }
    else if(timer_expired(&periodic_timer)){
      timer_reset(&periodic_timer);
      for(i = 0; i < UIP_CONNS; i++){
      	uip_periodic(i);
        /* If the above function invocation resulted in data that
           should be sent out on the network, the global variable
           uip_len is set to a value > 0. */
        if(uip_len > 0){
          uip_arp_out();
          tapdev_send(uip_buf,uip_len);
        }
      }
        #if UIP_UDP
        for(i = 0; i < UIP_UDP_CONNS; i++){
          uip_udp_periodic(i);
          /* If the above function invocation resulted in data that
             should be sent out on the network, the global variable
             uip_len is set to a value > 0. */
          if(uip_len > 0){
            uip_arp_out();
            tapdev_send();
          }
        }
        #endif /* UIP_UDP */
        /* Call the ARP timer function every 10 seconds. */
        if(timer_expired(&arp_timer)){
          timer_reset(&arp_timer);
          uip_arp_timer();
        }
      }
//_____Touch_Screen_session_______________________________________________
    if(TouchGet(&XY_Touch)){
      // On touch.
      cursor_x = XY_Touch.X;
      cursor_y = XY_Touch.Y;
      GLCD_Move_Cursor(cursor_x, cursor_y);
      if (FALSE == Touch){
        Touch = TRUE;
        toogleLED(2);       // Toggle bottom LED.
      }
      updateLCDgraphics();  // Update LDC graphics.   
      softDelay(1000000);   // Delay program, prevent jitter on buttom event.
    }
    else if(Touch){
      toogleLED(2);         // Toggle bottom LED.
      Touch = FALSE;
    }
    
    //    RelayControl(47);  // TEST frequency (remove).
    //    textToScreen(50.1234);
    
//_____Calculation_of_readings_session___________________________________________        
    if(_CountFlag == true){
      _NumberOfCrossings = 0;
      clearArray();
      _CrossIndex = 0;     
      lowPassFilter(_waveForm, LenCircReg, fs);
      
      for(Int32U ii = 0; (ii <= LenCircReg-1) & (_CrossIndex<27); ii++){
        if((LOWfilterWave[ii]<511)^(LOWfilterWave[ii + 1]<511)){
          _NumberOfCrossings++;
          _CrossingsLocation[_CrossIndex] = ii;
          _CrossIndex++;
        }
      }
      
      _freq = 10/((double)(_CrossingsLocation[25] - _CrossingsLocation[5])*(double)(1/(double)fs));
      textToScreen(_freq);
      RelayControl(_freq);
      
      _RMSCurrent = getRMS(_CurrentWave,sizeof(_CurrentWave)/sizeof(Int32U) );
      _RMSVoltage = getRMS(_waveForm, sizeof(_waveForm)/sizeof(Int32U));
      _Power = _RMSCurrent * _RMSVoltage;
      _CountFlag = false;
    }
  }
  
}
