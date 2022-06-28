/*
******************************************************************************
 @file:    Main.C
 @author:  $ Author: FrancOlino $
 @brief:   Firmware components
 @version: $ Revision: 1.0 $
 @date:    $ Date: 2022-06-27 $
 -----------------------------------------------------------------------------

Copyright (c) 2022 FrancOlino, Inc. All Rights Reserved.

The code is designed to be part of a firmware executable on the 
AD5940-Bioz board of the Analog Device.
This script contains pieces of code that with a lot of probability
will become part of the Insulinmeter 2.0 final firmware. The examples
and Data Sheet provided by Analog Device were taken into consideration 
to write the code.

*****************************************************************************
*/


/*
*****************************************************************************

-----------------------------------------------------------------------------

------------------------------- main.C --------------------------------------

-----------------------------------------------------------------------------

*****************************************************************************
*/

/***************************** Include Files ********************************/
#include "stdio.h"
#include "ADuCM3029.h"
#include "AD5940.h"
#include "adi_initialize.h"
#include "ConfigurationBLE.h"
#include <radio/adi_ble_radio.h>
#include "adi_ble_radio_internal.h"

extern ADI_BLE_RADIO_Device *pBLERadio;

/* Functions used to initialize MCU platform */
uint32_t MCUPlatformInit(void *pCfg);
extern int32_t adi_initComponents(void);
void Received_Data(void);

/**************************************************************************

------------------------------- BLE ---------------------------------------

***************************************************************************/
/* Peripheral BLE advertisement mode */
#define PERIPHERAL_ADV_MODE      ((ADI_BLE_GAP_MODE)(ADI_BLE_GAP_MODE_CONNECTABLE | ADI_BLE_GAP_MODE_DISCOVERABLE))

/* Global Data */
bool gbConnected;
ADI_BLE_GAP_MODE   geMode;
static char start2[50]="start2 ";


/* Local Functions used when Bluetooth is enabled */
static void ApplicationCallback(void * pCBParam, uint32_t Event, void * pArg);
static void InitBluetoothLowEnergy(void);
static void SetAdvertisingMode(void);



/* Local Functions */
static void Trap(void);
/*************************************************************************
**************************************************************************/

int main(void)
{
  
  void AD5940_MainBLE(void);
  
  MCUPlatformInit(0);
  adi_initComponents();
  AD5940_MCUResourceInit(0);
  
  InitBluetoothLowEnergy();

  AD5940_MainBLE();
}

/* Below functions are used to initialize MCU Platform */
uint32_t MCUPlatformInit(void *pCfg)
{
  int UrtCfg(int iBaud);

  /*Stop watch dog timer(ADuCM3029)*/
  pADI_WDT0->CTL = 0xC9;
  /* Clock Configure */
  pADI_CLKG0_OSC->KEY = 0xCB14; /* Access key to modify CLK0_OSC */
  pADI_CLKG0_OSC->CTL =
    BITM_CLKG_OSC_CTL_HFOSCEN|BITM_CLKG_OSC_CTL_HFXTALEN;  /* HFOSC selected */

  while((pADI_CLKG0_OSC->CTL&BITM_CLKG_OSC_CTL_HFXTALOK) == 0); /* Waiting for the configuration */

  pADI_CLKG0_OSC->KEY = 0xCB14; /* Access key to modify CLK0_OSC */
  pADI_CLKG0_CLK->CTL0 = 0x201; /* Select XTAL as system clock */
  pADI_CLKG0_CLK->CTL1 = 0; /* ACLK,PCLK,HCLK divided by 1 */
  pADI_CLKG0_CLK->CTL5 = 0x00; /* Enable clock to all peripherals - no clock gating */

  UrtCfg(230400); /*Baud rate: 230400*/
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
  int iBits = 3; /*8bits*/ 
  int iFormat = 0; /*int iBits, int iFormat */
  int i1;
  int iDiv;
  int iRtC;
  int iOSR;
  int iPllMulValue;
  unsigned long long ullRtClk = 16000000; /* The root clock speed* /


  /*Setup P0[11:10] as UART pins*/
  pADI_GPIO0->CFG = (1<<22)|(1<<20)|(pADI_GPIO0->CFG&(~((3<<22)|(3<<20))));

  iDiv = (pADI_CLKG0_CLK->CTL1& BITM_CLKG_CLK_CTL1_PCLKDIVCNT); /*Read UART clock as set by CLKCON1[10:8] */
  iDiv = iDiv>>8;
  if (iDiv == 0)
    iDiv = 1;
  iRtC = (pADI_CLKG0_CLK->CTL0& BITM_CLKG_CLK_CTL0_CLKMUX); /*Check what is the root clock */


  /* Our case is 1 due to the MCUPlatformInit settings: HFXTAL clock source*/	
  switch (iRtC)
  {
  case 0:                                               /* HFOSC selected */
    ullRtClk = 26000000;
    break;

  case 1:                                               /* HFXTAL selected */
    if ((pADI_CLKG0_CLK->CTL0 & 0x200)==0x200)          /* 26Mhz XTAL used */
        ullRtClk = 26000000;
    else
        ullRtClk = 16000000;                            /* Assume 16MHz XTAL */
    break;

  case 2:                                               /* SPLL output */
    iPllMulValue = (pADI_CLKG0_CLK->CTL3 &              /* Check muliplication factor in PLL settings */
                    BITM_CLKG_CLK_CTL3_SPLLNSEL);       /* bits[4:0]. Assume div value of 0xD in bits [14:11] */
    ullRtClk = (iPllMulValue *1000000);                 /* Assume straight multiplication by pADI_CLKG0_CLK->CTL3[4:0] */
    break;

  case 3:
    ullRtClk = 26000000;                                /* External clock is assumed to be 26MhZ, if different */
    break;                                              /* Clock speed is used, this should be changed */

  default:
    break;
  }

  pADI_UART0->COMLCR2 = 0x3;
  iOSR = 32;
										 /* UART baud rate clock source is PCLK divided by OSR */	
  i1 = (ullRtClk/(iOSR*iDiv))/iBaud-1;   /* For bigger M and N value */
  pADI_UART0->COMDIV = i1;

  pADI_UART0->COMFBR = 0x8800|(((((2048/(iOSR*iDiv))*ullRtClk)/i1)/iBaud)-2048);
  pADI_UART0->COMIEN = 0;
  pADI_UART0->COMLCR = (iFormat&0x3c)|(iBits&3);


  pADI_UART0->COMFCR = (BITM_UART_COMFCR_RFTRIG & 0/*RX_FIFO_1BYTE*/ ) |BITM_UART_COMFCR_FIFOEN;
  pADI_UART0->COMFCR |= BITM_UART_COMFCR_RFCLR|BITM_UART_COMFCR_TFCLR; /* Clear the UART FIFOs */
  pADI_UART0->COMFCR &= ~(BITM_UART_COMFCR_RFCLR|BITM_UART_COMFCR_TFCLR); /* Disable clearing mechanism */

  NVIC_EnableIRQ(UART_EVT_IRQn); /* Enable UART interrupt source in NVIC */
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
  while((pADI_UART0->COMLSR&0x20) == 0); /* tx fifo empty */
  return c;
}

/**
   @brief External interrupt handler of the UART
	
   @note for each character calls UARTCmd_Process()
**/
void UART_Int_Handler(void)
{
  void UARTCmd_Process(char);
  uint32_t flag;
  flag = pADI_UART0->LSR; /* Line Status Register 16bit register (it indicates if there have been transmission errors)*/
  flag = pADI_UART0->IIR; /* Interrupt ID register 16bit register */
  
  /* Now flag has 32 bit and give information about rx */
  
  /* First case: all bytes are recieved */
  if((flag & 0x0e) == 0x04)  /* Receive Byte */
  {
    uint32_t count;
    count = pADI_UART0->RFC;  /* RFC Register Receive FIFO data bytes (received characters) count */
    for(int i=0;i < count; i++)
    {
      char c;
      c = pADI_UART0->COMRX&0xff; /* COMRX contains the received character */
      UARTCmd_Process(c); /* This function recognizes received data */
    }
  }
  
  /* Second case: trasmission timeout, processing only what we have had */
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
* --------------------------BLE CONFIG --------------------------------*
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
static void SetAdvertisingMode(void)
{
    ADI_BLER_RESULT eResult;

    eResult = adi_radio_SetMode(PERIPHERAL_ADV_MODE, 0u, 0u);
    if(eResult!=ADI_BLER_SUCCESS){
      printf("Error setting the mode.\r\n");
    }
    
    eResult = adi_ble_WaitForEventWithTimeout(GAP_EVENT_MODE_CHANGE, 5000u); /* checks what event occourd */ 
    if(eResult!=ADI_BLER_SUCCESS){
      printf("Error waiting for GAP_EVENT_MODE_CHANGE.\r\n");
    }   

    eResult = adi_radio_GetMode(&geMode);
    if(eResult!=ADI_BLER_SUCCESS){
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
static void InitBluetoothLowEnergy(void)
{
    ADI_BLER_RESULT eResult;
    uint8_t *       aDataExchangeName = (unsigned char *) "FrancOlino_BLE";

    /* Initialize radio and framework layer */
    eResult = adi_ble_Init(ApplicationCallback, NULL);
    if(eResult!=ADI_BLER_SUCCESS){
      printf("Error initializing the radio.\r\n");
    }
    /* Configure radio */
    eResult = adi_radio_RegisterDevice(ADI_BLE_ROLE_PERIPHERAL);
    if(eResult!=ADI_BLER_SUCCESS){
      printf("Error registering the radio.\r\n");
    }

    eResult = adi_radio_SetLocalBluetoothDevName(aDataExchangeName, strlen((const char *) aDataExchangeName), 0u, 0u);
    if(eResult!=ADI_BLER_SUCCESS){
      printf("Error setting local device name.\r\n");
    }

    SetAdvertisingMode();

    /* Initialize data exchange profile */
    eResult = adi_radio_Register_DataExchangeServer();
    if(eResult!=ADI_BLER_SUCCESS){
      printf("Error registering data exchange server.\r\n");
    }    

   
}

/*!
 * @brief      Application Callback
 *
 * @details    Called by the framework layer (adi_ble_noos.c) when an event occurs that the application did NOT
 *             explicity expect by calling #adi_ble_WaitForEventWithTimeout.
 *
 * @param [in] pCBParam : Callback parameter (unused)
 *
 * @param [in] Event : Event of type #ADI_BLER_EVENT.
 *
 * @param [in] pArg : Callback argument (unused)
 *
 * @note       The application should NOT call other radio functions (adi_ble_radio.c) from this callback that issue
 *             a command to the radio. The application may call radio functions that simply extract data from the
 *             companion module, these are located below #adi_ble_GetEvent in adi_ble_radio.c. Ideally this callback
 *             should just be used for flags and semaphores.
 */
static void ApplicationCallback(void * pCBParam, uint32_t Event, void * pArg)
{
	/* All the possible events, useful for debugging */
    switch(Event) {
        case GAP_EVENT_CONNECTED:
            printf("Connected!\r\n");
            gbConnected = true;
            break;

        case GAP_EVENT_DISCONNECTED:
            printf("Disconnected!\r\n");
            geMode      = ADI_BLE_GAP_MODE_NOTCONNECTABLE;
            gbConnected = false;
            NVIC_SystemReset();
            break;

        case DATA_EXCHANGE_RX_EVENT:
            //printf("Data received!\r\n");
			
			/* Calls the function to handle received data packets */
            Received_Data();  
            break;

        case DATA_EXCHANGE_TX_COMPLETE:
            //printf("Data sent!\r\n");
            break;

        case GAP_EVENT_MODE_CHANGE:
            printf("GAP mode changed.\r\n");
            break;

        case GAP_EVENT_CONNECTION_UPDATED:
            printf("Connection interval updated.\r\n");
            break;

        case BLE_RADIO_ERROR_READING:
 
            break;

        case BLE_RADIO_ERROR_PARSING:
            printf("Failed to parse a packet from the radio.\r\n");
            break;

        case BLE_RADIO_ERROR_PROCESSING:
            printf("Failed to process a packet from the radio.\r\n");
            break;

        default:
           // printf("Unexpected event received.\r\n");
            break;
    }
}


/**
   @brief External handler of the BLE
	
   @note for each rxDataPkt calls BLECmd_Process()
**/
/* Same as UART_Int_Handler*/
void Received_Data(void){
  void BLECmd_Process(char [],int);
  char aStringReceive[20];
  char aTemp[20];
  
  for(int i = 0;i<20;i++){
    aTemp[i] = pBLERadio->rxDataPkt.rxPkt[i];
  }
  
  /* Invert the order of the char */
  for (int i = 0;i<20;i++){
    aStringReceive[i] = aTemp [19-i];  
  }
  
  /* Prints received command */
  for(int i = 0;i<20;i++){  
  printf("%c ",aStringReceive[i]);
  if(i==19)
    printf("\r\n");
  }
  
  /* Due to the lack of space for sending commands and all parameters, */
  /* For the sweep we send only parameters to distinguish the two different measurement modes*/
  if(aStringReceive[0]!= 's' && aStringReceive[0]!= 'r'){
    
    for(int i = 7;i<26;i++){
      start2[i] = aStringReceive[i-7];
    }
    
    printf("%s \n",start2);
    BLECmd_Process(start2,27); 
  }
  else
  BLECmd_Process(aStringReceive,20);
}










