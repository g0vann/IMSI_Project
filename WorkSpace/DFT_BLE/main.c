/*

Copyright (c) 2017-2019 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.
By using this software you agree to the terms of the associated
Analog Devices Software License Agreement.

*/

#include "stdio.h"
#include "ADuCM3029.h"
#include "AD5940.h"
#include "adi_initialize.h"
#include "ConfigurationBLE.h"

/* Functions that used to initialize MCU platform */
uint32_t MCUPlatformInit(void *pCfg);
extern int32_t adi_initComponents(void);

/**********************************************************



----------------------BLE---------------------------------




**********************************************************/
/* Peripheral advertisement mode */
#define PERIPHERAL_ADV_MODE      ((ADI_BLE_GAP_MODE)(ADI_BLE_GAP_MODE_CONNECTABLE | ADI_BLE_GAP_MODE_DISCOVERABLE))

/* Global Data */
bool gbConnected;
ADI_BLE_GAP_MODE   geMode;


/* Local Functions used when Bluetooth is enabled */
static void ApplicationCallback(void * pCBParam, uint32_t Event, void * pArg);
void InitBluetoothLowEnergy(void);
void SetAdvertisingMode(void);



/* Local Functions */
static void Trap(void);
/**********************************************************
**********************************************************/

int main(void)
{
  //void AD5940_Main(void);
  void AD5940_MainBLE(void);
  MCUPlatformInit(0);
  adi_initComponents();
  AD5940_MCUResourceInit(0);
  
  //InitBluetoothLowEnergy();
  
  //printf("Hello AD5940-Build Time:%s\n",__TIME__);
  //AD5940_Main();
  AD5940_MainBLE();
}

/* Below functions are used to initialize MCU Platform */
uint32_t MCUPlatformInit(void *pCfg)
{
  int UrtCfg(int iBaud);

  /*Stop watch dog timer(ADuCM3029)*/
  pADI_WDT0->CTL = 0xC9;
  /* Clock Configure */
  pADI_CLKG0_OSC->KEY = 0xCB14;               // Select HFOSC as system clock.
  pADI_CLKG0_OSC->CTL =                       // Int 32khz LFOSC selected in LFMUX
    BITM_CLKG_OSC_CTL_HFOSCEN|BITM_CLKG_OSC_CTL_HFXTALEN;

  while((pADI_CLKG0_OSC->CTL&BITM_CLKG_OSC_CTL_HFXTALOK) == 0);

  pADI_CLKG0_OSC->KEY = 0xCB14; 
  pADI_CLKG0_CLK->CTL0 = 0x201;                   /* Select XTAL as system clock */
  pADI_CLKG0_CLK->CTL1 = 0;                   // ACLK,PCLK,HCLK divided by 1
  pADI_CLKG0_CLK->CTL5 = 0x00;                 // Enable clock to all peripherals - no clock gating

  UrtCfg(230400);/*Baud rate: 230400*/
  return 1;
}

/**
	@brief int UrtCfg(int iBaud, int iBits, int iFormat)
			==========Configure the UART.
	@param iBaud :{B1200,B2200,B2400,B4800,B9600,B19200,B38400,B57600,B115200,B230400,B430800}	\n
		Set iBaud to the baudrate required:
		Values usually: 1200, 2200 (for HART), 2400, 4800, 9600,
		        19200, 38400, 57600, 115200, 230400, 430800, or type in baud-rate directly
	@note
		- Powers up UART if not powered up.
		- Standard baudrates are accurate to better than 0.1% plus clock error.\n
		- Non standard baudrates are accurate to better than 1% plus clock error.
   @warning - If an external clock is used for the system the ullRtClk must be modified with \n
         the speed of the clock used.
**/

int UrtCfg(int iBaud)
{
  int iBits = 3;//8bits, 
  int iFormat = 0;//, int iBits, int iFormat
  int i1;
  int iDiv;
  int iRtC;
  int iOSR;
  int iPllMulValue;
  unsigned long long ullRtClk = 16000000;                // The root clock speed


  /*Setup P0[11:10] as UART pins*/
  pADI_GPIO0->CFG = (1<<22)|(1<<20)|(pADI_GPIO0->CFG&(~((3<<22)|(3<<20))));

  iDiv = (pADI_CLKG0_CLK->CTL1& BITM_CLKG_CLK_CTL1_PCLKDIVCNT);                 // Read UART clock as set by CLKCON1[10:8]
  iDiv = iDiv>>8;
  if (iDiv == 0)
    iDiv = 1;
  iRtC = (pADI_CLKG0_CLK->CTL0& BITM_CLKG_CLK_CTL0_CLKMUX); // Check what is the root clock

  switch (iRtC)
  {
  case 0:                                               // HFOSC selected
    ullRtClk = 26000000;
    break;

  case 1:                                               // HFXTAL selected
    if ((pADI_CLKG0_CLK->CTL0 & 0x200)==0x200)           // 26Mhz XTAL used
        ullRtClk = 26000000;
    else
        ullRtClk = 16000000;                              // Assume 16MHz XTAL
    break;

  case 2:                                               // SPLL output
    iPllMulValue = (pADI_CLKG0_CLK->CTL3 &             // Check muliplication factor in PLL settings
                    BITM_CLKG_CLK_CTL3_SPLLNSEL);      // bits[4:0]. Assume div value of 0xD in bits [14:11]
    ullRtClk = (iPllMulValue *1000000);                // Assume straight multiplication by pADI_CLKG0_CLK->CTL3[4:0]
    break;

  case 3:
    ullRtClk = 26000000;                                //External clock is assumed to be 26MhZ, if different
    break;                                             //clock speed is used, this should be changed

  default:
    break;
  }
  //   iOSR = (pADI_UART0->COMLCR2 & 0x3);
  //   iOSR = 2^(2+iOSR);
  pADI_UART0->COMLCR2 = 0x3;
  iOSR = 32;
  //i1 = (ullRtClk/(iOSR*iDiv))/iBaud;	              // UART baud rate clock source is PCLK divided by OSR
  i1 = (ullRtClk/(iOSR*iDiv))/iBaud-1;   //for bigger M and N value
  pADI_UART0->COMDIV = i1;

  pADI_UART0->COMFBR = 0x8800|(((((2048/(iOSR*iDiv))*ullRtClk)/i1)/iBaud)-2048);
  pADI_UART0->COMIEN = 0;
  pADI_UART0->COMLCR = (iFormat&0x3c)|(iBits&3);


  pADI_UART0->COMFCR = (BITM_UART_COMFCR_RFTRIG & 0/*RX_FIFO_1BYTE*/ ) |BITM_UART_COMFCR_FIFOEN;
  pADI_UART0->COMFCR |= BITM_UART_COMFCR_RFCLR|BITM_UART_COMFCR_TFCLR;                                   // Clear the UART FIFOs
  pADI_UART0->COMFCR &= ~(BITM_UART_COMFCR_RFCLR|BITM_UART_COMFCR_TFCLR);                                // Disable clearing mechanism

  NVIC_EnableIRQ(UART_EVT_IRQn);              // Enable UART interrupt source in NVIC
  pADI_UART0->COMIEN = BITM_UART_COMIEN_ERBFI|BITM_UART_COMIEN_ELSI; /* Rx Interrupt */
  return pADI_UART0->COMLSR;
}
#include "stdio.h"
#ifdef __ICCARM__
int putchar(int c)
#else
int fputc(int c, FILE *f)
#endif
{
  pADI_UART0->COMTX = c;
  while((pADI_UART0->COMLSR&0x20) == 0);// tx fifo empty
  return c;
}

void UART_Int_Handler(void)
{
  void UARTCmd_Process(char);
  uint32_t flag;
  flag = pADI_UART0->LSR; //Line Status Register 16bit register (it indicates if there have been transmission errors)
  flag = pADI_UART0->IIR; //Interrupt ID register 16bit register
  
  // Now flag has 32 bit and give information about rx
  
  // First case: all bytes are recieved
  if((flag & 0x0e) == 0x04)  /* Receive Byte */
  {
    uint32_t count;
    count = pADI_UART0->RFC;  /* RFC Register Receive FIFO data bytes (received characters) count */
    for(int i=0;i < count; i++)
    {
      char c;
      c = pADI_UART0->COMRX&0xff; //COMRX contains the received character
      UARTCmd_Process(c); // this function recognizes received data
    }
  }
  
  // Second case: trasmission timeout, processing only what we have had
  if((flag & 0x0e) == 0xc)  /* Time-out */
  {
    uint32_t count;
    count = pADI_UART0->RFC;  /* Receive FIFO count */
    for(int i=0;i < count; i++)
    {
      char c;
      c = pADI_UART0->COMRX&0xff;
      UARTCmd_Process(c);
    }
  }
}


/**********************************************************************
***********************************************************************/

/*!
 * @brief      Trap function
 *
 * @details    In case of catastrophic errors this function is called to block
 *             infinitely.
 */
static void Trap()
{
    while(1);
}

/*!
 * @brief      Set Advertising Mode
 *
 * @details    Helper function to avoid repeated code in main.
 *
 */
void SetAdvertisingMode(void)
{
    ADI_BLER_RESULT eResult;

    eResult = adi_radio_SetMode(PERIPHERAL_ADV_MODE, 0u, 0u);
    if(eResult==ADI_BLER_SUCCESS){
      printf("Mode set successfully\r\n");
    }else{
      printf("Error setting the mode.\r\n");
    }
    
    eResult = adi_ble_WaitForEventWithTimeout(GAP_EVENT_MODE_CHANGE, 5000u);
    if(eResult==ADI_BLER_SUCCESS){
      printf("Waiting an event\r\n");
    }else{
      printf("Error waiting for GAP_EVENT_MODE_CHANGE.\r\n");
    }   

    eResult = adi_radio_GetMode(&geMode);
    if(eResult==ADI_BLER_SUCCESS){
      printf("Mode obtained\r\n");
    }else{
      printf("Error getting the mode.\r\n");
    }       

    if(geMode != PERIPHERAL_ADV_MODE) {
       printf("Error in SetAdvertisingMode.\r\n");
    }
}

/*!
 * @brief      Initializes the bluetooth
 *
 * @details    Data Exchange profile is initialized to send
 *             data to the connected central device.
 */
void InitBluetoothLowEnergy(void)
{
    ADI_BLER_RESULT eResult;
    uint8_t *       aDataExchangeName = (unsigned char *) "DFT_BLE";

    /* Initialize radio and framework layer */
    eResult = adi_ble_Init(ApplicationCallback, NULL);
    if(eResult==ADI_BLER_SUCCESS){
      printf("Initialization successful\r\n");
    }else{
      printf("Error initializing the radio.\r\n");
    }
    /* Configure radio */
    eResult = adi_radio_RegisterDevice(ADI_BLE_ROLE_PERIPHERAL);
    if(eResult==ADI_BLER_SUCCESS){
      printf("Registered radio\r\n");
    }else{
      printf("Error registering the radio.\r\n");
    }

    eResult = adi_radio_SetLocalBluetoothDevName(aDataExchangeName, strlen((const char *) aDataExchangeName), 0u, 0u);
    if(eResult==ADI_BLER_SUCCESS){
      printf("Local device name set successfull\r\n");
    }else{
      printf("Error setting local device name.\r\n");
    }

    SetAdvertisingMode();

    /* Initialize data exchange profile */
    eResult = adi_radio_Register_DataExchangeServer();
    if(eResult==ADI_BLER_SUCCESS){
      printf("Data exchange server registrated\r\n");
    }else{
      printf("Error registering data exchange server.\r\n");
    }    

    /* Now enter infinite loop waiting for connection and then data exchange events */
    printf("Waiting for connection. Initiate connection on central device please.\r\n");
}


static void ApplicationCallback(void * pCBParam, uint32_t Event, void * pArg)
{
    switch(Event) {
        case GAP_EVENT_CONNECTED:
            printf("Connected!\r\n");
            gbConnected = true;
            break;

        case GAP_EVENT_DISCONNECTED:
            printf("Disconnected!\r\n");
            geMode      = ADI_BLE_GAP_MODE_NOTCONNECTABLE;
            gbConnected = false;
            break;

        case DATA_EXCHANGE_RX_EVENT:
            printf("Data received!\r\n");
            break;

        case DATA_EXCHANGE_TX_COMPLETE:
            printf("Data sent!\r\n");
            break;

        case GAP_EVENT_MODE_CHANGE:
            printf("GAP mode changed.\r\n");
            break;

        case GAP_EVENT_CONNECTION_UPDATED:
            printf("Connection interval updated.\r\n");
            break;

        case BLE_RADIO_ERROR_READING:
            /* If you want to enable this print statement, please be aware that the first
             * packet sent from the radio on startup will cause this error. It is a known bug
             * and will not have any adverse effects on the application.
             *
             *DEBUG_MESSAGE("Failed to read a packet from the radio.\r\n");
             *
             */
            break;

        case BLE_RADIO_ERROR_PARSING:
            printf("Failed to parse a packet from the radio.\r\n");
            break;

        case BLE_RADIO_ERROR_PROCESSING:
            printf("Failed to process a packet from the radio.\r\n");
            break;

        default:
            printf("Unexpected event received.\r\n");
            break;
    }
}

