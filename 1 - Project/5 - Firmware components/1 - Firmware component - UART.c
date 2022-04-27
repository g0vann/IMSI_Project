/*
******************************************************************************
 @file:    Componenti_firmware.C
 @author:  $ Author: FrancOlino $
 @brief:   Firmware components
 @version: $ Revision: 1.0 $
 @date:    $ Date: 2022-04-27 $
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

------------------------------- main.C -----------------------------------

-----------------------------------------------------------------------------

*****************************************************************************
*/

/***************************** Include Files ********************************/

#include "stdio.h"


#include "ADuCM3029.h"  /* Host board library by Analog Device */
#include "AD5940.h" /* Guest borad library by Analog Device */

/************************* Functions Definitions ****************************/

/* Below functions are used to initialize MCU Platform */
uint32_t MCUPlatformInit(void *pCfg)
{
  int UrtCfg(int iBaud);

  /*Stop watch dog timer(ADuCM3029)*/
  pADI_WDT0->CTL = 0xC9;
  /* Clock Configure */
  pADI_CLKG0_OSC->KEY = 0xCB14;               /* Select HFOSC as system clock. */
  pADI_CLKG0_OSC->CTL = 					  /* Int 32khz LFOSC selected in LFMUX */
  BITM_CLKG_OSC_CTL_HFOSCEN|BITM_CLKG_OSC_CTL_HFXTALEN; 

  while((pADI_CLKG0_OSC->CTL&BITM_CLKG_OSC_CTL_HFXTALOK) == 0);

  pADI_CLKG0_OSC->KEY = 0xCB14; 
  pADI_CLKG0_CLK->CTL0 = 0x201;               /* Select XTAL as system clock */
  pADI_CLKG0_CLK->CTL1 = 0;                   /* ACLK,PCLK,HCLK divided by 1 */
  pADI_CLKG0_CLK->CTL5 = 0x00;                /* Enable clock to all peripherals - no clock gating */

  UrtCfg(230400);							  /*Baud rate: 230400*/
  return 1;
}

/**
	@brief int UrtCfg(int iBaud, int iBits, int iFormat)
			==========Configure the UART.
	@param iBaud :{B1200,B2200,B2400,B4800,B9600,B19200,B38400,B57600,B115200,B230400,B430800}	
		Set iBaud to the baudrate required:
		Values usually: 1200, 2200 (for HART), 2400, 4800, 9600,
		        19200, 38400, 57600, 115200, 230400, 430800, or type in baud-rate directly
	@note
		- Powers up UART if not powered up.
		- Standard baudrates are accurate to better than 0.1% plus clock error.
		- Non standard baudrates are accurate to better than 1% plus clock error.
   @warning - If an external clock is used for the system the ullRtClk must be modified with 
         the speed of the clock used.
**/
int UrtCfg(int iBaud)
{
  int iBits = 3;										/* 8bits */ 
  int iFormat = 0;                                      /* int iBits, int iFormat */
  int i1;
  int iDiv;
  int iRtC;
  int iOSR;
  int iPllMulValue;
  unsigned long long ullRtClk = 16000000;               /* The root clock speed */


  /*Setup P0[11:10] as UART pins*/
  pADI_GPIO0->CFG = (1<<22)|(1<<20)|(pADI_GPIO0->CFG&(~((3<<22)|(3<<20))));

  iDiv = (pADI_CLKG0_CLK->CTL1& BITM_CLKG_CLK_CTL1_PCLKDIVCNT);    /* Read UART clock as set by CLKCON1[10:8] */
  iDiv = iDiv>>8;
  if (iDiv == 0)
    iDiv = 1;
  iRtC = (pADI_CLKG0_CLK->CTL0& BITM_CLKG_CLK_CTL0_CLKMUX); 	   /* Check what is the root clock */

  switch (iRtC)
  {
  case 0:                                               		   /* HFOSC selected */
    ullRtClk = 26000000;
    break;

  case 1:                                                          /* HFXTAL selected */
    if ((pADI_CLKG0_CLK->CTL0 & 0x200)==0x200)                     /* 26Mhz XTAL used*/
        ullRtClk = 26000000;
    else
        ullRtClk = 16000000;                                       /* Assume 16MHz XTAL */
    break;

  case 2:                                               		   /* SPLL output */
    iPllMulValue = (pADI_CLKG0_CLK->CTL3 &                         /* Check muliplication factor in PLL settings */
                    BITM_CLKG_CLK_CTL3_SPLLNSEL);      			   /* bits[4:0]. Assume div value of 0xD in bits [14:11] */
    ullRtClk = (iPllMulValue *1000000);                			   /* Assume straight multiplication by pADI_CLKG0_CLK->CTL3[4:0] */
    break;

  case 3:
    ullRtClk = 26000000;                                		   /*External clock is assumed to be 26MhZ, if different*/
    break;                                             			   /*clock speed is used, this should be changed*/

  default:
    break;
  }

  pADI_UART0->COMLCR2 = 0x3;
  iOSR = 32;
																   /* UART baud rate clock source is PCLK divided by OSR */
  i1 = (ullRtClk/(iOSR*iDiv))/iBaud-1;   						   /* for bigger M and N value */
  pADI_UART0->COMDIV = i1;

  pADI_UART0->COMFBR = 0x8800|(((((2048/(iOSR*iDiv))*ullRtClk)/i1)/iBaud)-2048);
  pADI_UART0->COMIEN = 0;
  pADI_UART0->COMLCR = (iFormat&0x3c)|(iBits&3);


  pADI_UART0->COMFCR = (BITM_UART_COMFCR_RFTRIG & 0/*RX_FIFO_1BYTE*/ ) |BITM_UART_COMFCR_FIFOEN;
  pADI_UART0->COMFCR |= BITM_UART_COMFCR_RFCLR|BITM_UART_COMFCR_TFCLR;                                   /* Clear the UART FIFOs */
  pADI_UART0->COMFCR &= ~(BITM_UART_COMFCR_RFCLR|BITM_UART_COMFCR_TFCLR);                                /* Disable clearing mechanism */

  NVIC_EnableIRQ(UART_EVT_IRQn);              /* Enable UART interrupt source in NVIC */
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

/**
   @brief External interrupt handler of the UART
	
   @note for each character calls UARTCmd_Process()
**/
void UART_Int_Handler(void)
{
  void UARTCmd_Process(char);
  uint32_t flag;
  flag = pADI_UART0->LSR; /* Line Status Register 16bit register (it indicates if there have been transmission errors) */
  flag = pADI_UART0->IIR; /* Interrupt ID register 16bit register*/
  
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
      UARTCmd_Process(c);         /* This function recognizes received data */
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





/*
*****************************************************************************

-----------------------------------------------------------------------------

------------------------------- UARTCmd.C -----------------------------------

-----------------------------------------------------------------------------

*****************************************************************************
*/

/***************************** Include Files ********************************/

#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include "ad5940.h"
#include <ctype.h>
#include <stdarg.h>
#include <math.h>
#include "ADuCM3029.h"

/************************* Variable Definitions ****************************/

#define  VERSION  "FrancOlino v1.0"
#define LINEBUFF_SIZE 128
#define CMDTABLE_SIZE 2

/*Variables declarations that will be used to search for commands*/
char line_buffer[LINEBUFF_SIZE];
uint32_t line_buffer_index = 0;
uint32_t token_count = 0;
void *pObjFound = 0;
uint32_t parameter1, parameter2;
int b = 0;

uint32_t Cli_start(uint32_t para1,uint32_t para2);
uint32_t Cli_stop(uint32_t para1,uint32_t para2);


/** 
	@brief	Struct which describes functions that can be called 
		    by external commands.
	@var	pObj - pointer to the function prototype
			cmd_name - function name
			pDesc - function description
**/
struct __uartcmd_table
{
  void *pObj; 
  const char *cmd_name;
  const char *pDesc;
}uart_cmd_table[CMDTABLE_SIZE]=
{
  {(void*)Cli_start, "start", "Start Application"},
  {(void*)Cli_stop, "stop", "Stop Application"}, 
};


/**
   @brief Start function
   
   @param para1 & para2 - arguments on the command line.

   @return 0 if correct.
**/
uint32_t Cli_start(uint32_t para1,uint32_t para2){
        DFT_config();	/* This function is used only to test purposes */
	return 0;
}


/**
   @brief Stop function, in this case the system will be reset
   
   @param para1 & para2 - arguments on the command line.

   @return 'return 0' is never executed because  system is resetted
**/
uint32_t Cli_stop(uint32_t para1,uint32_t para2){
       NVIC_SystemReset();
       //return 0;
}

/**
   @brief Remove Spaces from command received
   
   @param it is a procedure without parameters, it manipulates  line_buffer

   @return  
**/
void UARTCmd_RemoveSpaces(void)
{
  int i = 0;
  token_count = 0;
  char flag_found_token = 0;
  while(i<line_buffer_index)
  {
    if(line_buffer[i] == ' ') line_buffer[i] = '\0';
    else break;
    i++;
  }
  if(i == line_buffer_index) return;  /* All spaces... */
  while(i<line_buffer_index)
  {
    if(line_buffer[i] == ' ')
    {
      line_buffer[i] = '\0';
      flag_found_token = 0;
    }
    else
    { 
      if(flag_found_token == 0)
        token_count ++;
      flag_found_token = 1;
    }
    i++;
  }
}


/**
   @brief it assigns the received string to the functions
   
   @param line_buffer(command line received)

   @return pObjFound - pointer to command function
**/
void UARTCmd_MatchCommand(void)
{
  char *pcmd;
  int i = 0;
  pObjFound = 0;
  while(i<line_buffer_index)
  {
    if(line_buffer[i] != '\0')
    {
      pcmd = &line_buffer[i];
      break;
    }
    i++;
  }
  for(i=0;i<CMDTABLE_SIZE;i++)
  {
    if(strcmp(uart_cmd_table[i].cmd_name, pcmd) == 0)
    {
      /* Found you! */
      pObjFound = uart_cmd_table[i].pObj;
      break;
    }
  }
}


/**
   @brief Translate string 'p' to number, store results in 'Res' 
   
   @param s - string in input 
		  Res - pointer to number in output

   @return return error code or 0 if correct
**/
static uint32_t Str2Num(char *s, uint32_t *Res)
{
   
  *Res = strtol((const char*)s, NULL, 16);

  return 0;
}


/**
   @brief Interprets the buffer to capture parameters
   
   @param p - pointer to linebuffer

   @return the first and the second parameters
**/
void UARTCmd_TranslateParas(void)
{
  char *p = line_buffer;
  parameter1 = 0;
  parameter2 = 0;
  while(*p == '\0') p++;    /* goto command */
  while(*p != '\0') p++;    /* skip command. */
  while(*p == '\0') p++;    /* goto first parameter */
  if(Str2Num(p, &parameter1) != 0) return;
  if(token_count == 2) return;           /* Only one parameter */
  while(*p != '\0') p++;    /* skip first command. */
  while(*p == '\0') p++;    /* goto second parameter */
  Str2Num(p, &parameter2);
}

/**
   @brief Command line interpreter process function called by UART external interrupt handler
   
   @param c - byte from UART
**/
void UARTCmd_Process(char c)
{
  if(line_buffer_index >= LINEBUFF_SIZE-1)
    line_buffer_index = 0;  /* Error: buffer overflow */
  if( (c == '\r') || (c == '\n'))
  {
    uint32_t res;
    line_buffer[line_buffer_index] = '\0';
    /* Start to process command */
    if(line_buffer_index == 0) 
    {
      line_buffer_index = 0; /* Reset buffer */
      return;  /* No command inputs, return */
    }
    /* Step1, remove space */
    UARTCmd_RemoveSpaces();
    if(token_count == 0)
    {
      line_buffer_index = 0; /* Reset buffer */
      return; /* No valid input */
    }
    /* Step2, match commands */
    UARTCmd_MatchCommand();
    if(pObjFound == 0)
    {
      line_buffer_index = 0; /* Reset buffer */
      printf("Unknow command\n");
      return;   /* Command not support */
    }
    if(token_count > 1)           /* There is parameters */
    {
      UARTCmd_TranslateParas();
    }
    /* Step3, call function */
    res = ((uint32_t (*)(uint32_t, uint32_t))(pObjFound))(parameter1, parameter2);
    line_buffer_index = 0;  /* Reset buffer */
  }
  else
  {
    line_buffer[line_buffer_index++] = c;
  }
}















