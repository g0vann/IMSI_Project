/*
******************************************************************************
 @file:    UartCMD.c
 @author:  $ Author: FrancOlino $
 @brief:   handles the recognition of commands and parameters 
		   and sets the BIA application 
 @version: $ Revision: 2.0 $
 @date:    $ Date: 2022-06-27 $

*****************************************************************************/

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

#include "ConfigurationBLE.h"

#include <stdarg.h>
#include <math.h>
#include "BodyImpedance.h"
#include "ADuCM3029.h"

/************************* Variable Definitions ****************************/
extern float scelta;
#define APPBUFF_SIZE 512
uint32_t AppBuff[APPBUFF_SIZE];

#define  VERSION  "FrancOlino v2.0"
#define LINEBUFF_SIZE 128 /* Max input buffer size */
#define CMDTABLE_SIZE 4  /* Number of commands */
#define PI 3.141592654

/* Variables used to set BIA */
int b = 0; /* Semaphore to signal whether or not we are in measurement */
float scelta = 50000, Freq1, Freq2;
uint32_t Npunti = 0, Ncicli = 0;

/*********************************BLE*************************************/
extern bool gbConnected;
extern ADI_DATA_PACKET eDataPacket;
 

void Function_BIA(void);

/*Variables for Function_BIA*/
	uint32_t temp;
    BoolFlag SweepON; 

/* External Functions*/
extern uint32_t SPIReadReg(uint16_t RegAddr);
extern void SPIWriteReg(uint16_t RegAddr, uint32_t RegData);
extern void AD5940BIAStructInit(void);
extern int32_t BIAShowResult(uint32_t *pData, uint32_t DataCount);

/*Variables declarations that will be used to search for commands*/
char line_buffer[LINEBUFF_SIZE];
uint32_t line_buffer_index = 0;
uint32_t token_count = 0;
void *pObjFound = 0;
float parameter1, parameter2;
float parameter3,parameter4;

/* Declaration of the four commands functions*/
uint32_t Cli_start(float para1,float para2, float para3, float para4);
uint32_t Cli_stop(float para1,float para2, float para3, float para4);
uint32_t Cli_start2(float para1,float para2, float para3, float para4);
uint32_t Cli_reset(float para1,float para2, float para3, float para4);

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
  {(void*)Cli_start2, "start2", "Start with frequency sweep"},
  {(void*)Cli_reset, "rst", "Reset Application"},

};

/**
   @brief Reset the device
**/
uint32_t Cli_reset(float para1,float para2, float para3, float para4){
        /* resets semaphore and the board*/
		b=0;  
        NVIC_SystemReset();
}

/**
   @brief Start function for sweep measurement
   
   @param para1, para2, para3, para4 - arguments on the command line.

   @return 0 if correct.
**/
uint32_t Cli_start2(float para1,float para2, float para3, float para4){
        if(b==0){  //checks if measurement already strated
        SweepON = bTRUE;
        Freq1 = para1;
        Freq2 = para2;
        Npunti = (uint32_t)para3;
        Ncicli = (uint32_t)para4;
        }
        b=1;
	AD5940BIAStructInit(); /* Configure our parameters in this function */
	AppBIAInit(AppBuff, APPBUFF_SIZE); /* Initialize BIA application. Provide a buffer, which is used to store sequencer commands */
	AppBIACtrl(BIACTRL_START, 0);		
	return 0;
}

/**
   @brief Start function for single measurement
   
   @param para1 & para2 - arguments on the command line.

   @return 0 if correct.
**/
uint32_t Cli_start(float para1,float para2, float para3, float para4){
        if(b==0){  //checks if measurement already strated
          SweepON = bFALSE;
          scelta  = para1;
        }
        b=1;
	AD5940BIAStructInit(); /* Configure our parameters in this function */
	AppBIAInit(AppBuff, APPBUFF_SIZE); /* Initialize BIA application. Provide a buffer, which is used to store sequencer commands */
	AppBIACtrl(BIACTRL_START, 0);		
	return 0;
}

/**
   @brief Stop the Application
   
   @return 0 if correct.
**/
uint32_t Cli_stop(float para1,float para2, float para3, float para4){
	  /* resets semaphore and stop the BIA measurement*/
      b=0;      
      AppBIACtrl(BIACTRL_SHUTDOWN, 0);
      return 0;
}

/**
   @brief Recognize and remove all the spaces in the received buffer
   
   @return Received buffer without spaces
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
  if(i == line_buffer_index) {
    return;  /* All spaces... */
  }
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
   @brief Search for keywords corresponding to commands
   
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
static uint32_t Str2Num(char *s, float *Res)
{
   
  *Res = strtof((const char*)s, NULL);

  return 0;
}

/**
   @brief Interprets the buffer to capture parameters
   
   @param p - pointer to linebuffer

   @return the first and the second, the third and the fourth parameters
**/
void UARTCmd_TranslateParas(void)
{
  char *p = line_buffer;
  parameter1 = 0;
  parameter2 = 0;
  parameter3 = 0;
  parameter4 = 0;
  while(*p == '\0') p++;    /* goto command */
  while(*p != '\0') p++;    /* skip command. */
  while(*p == '\0') p++;    /* goto first parameter */
  if(Str2Num(p, &parameter1) != 0) return;
  if(token_count == 2) return;           /* Only one parameter */
  while(*p != '\0') p++;    /* skip first command. */
  while(*p == '\0') p++;    /* goto second parameter */
  if(Str2Num(p, &parameter2) != 0) return; 
  if(token_count == 3) return; 	/* Only two parameters */
  while(*p != '\0') p++;    /* skip first command. */
  while(*p == '\0') p++;	/* goto third parameter */
  if(Str2Num(p, &parameter3) != 0) return; /* Terzo parametro sicuro è un intero */
  if(token_count == 4) return; 	/* Only three parameters */
  while(*p != '\0') p++;    /* skip first command. */
  while(*p == '\0') p++;	/* goto third parameter */
  Str2Num(p, &parameter4); /* Terzo parametro sicuro è un intero */
}

/**
   @brief Command line interpreter process function called by UART external interrupt handler
   
   @param char c - byte form UART
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
    res = ((uint32_t (*)(float, float,float,float))(pObjFound))(parameter1, parameter2,parameter3,parameter4);
    
    line_buffer_index = 0;  /* Reset buffer */
  }
  else
  {
    line_buffer[line_buffer_index++] = c;
  }
}

/****************************************************************************
------------------------------------BLE--------------------------------------
****************************************************************************/

/**
   @brief Command line interpreter process function called by BLE handler
   
   @param char c [] - string from BLE
**/
void BLECmd_Process(char c[],int a){
  
  uint32_t res;
  line_buffer_index=a;
  
   for(int i = 0;i<line_buffer_index;i++){
      line_buffer[i] = c[i];
    }

  //printf("%s \n",line_buffer);
  UARTCmd_RemoveSpaces();
  if(token_count == 0)
  {
    line_buffer_index = 0; /* Reset buffer */
    return; /* No valid input */
  }
  UARTCmd_MatchCommand();
  if(pObjFound == 0)
  {
      line_buffer_index = 0; /* Reset buffer */
      printf("Unknow command\n");
      return;   /* Command not support */
  }
  if(token_count > 1)           /* There are parameters */
  {
    UARTCmd_TranslateParas();
  }
  /* Step3, call function */
  res = ((uint32_t (*)(float, float,float,float))(pObjFound))(parameter1, parameter2,parameter3,parameter4);
  line_buffer_index = 0;  /* Reset buffer */
  
}

/**
   @brief  Every time is interrupted call the BIA ISR to process data
   
**/
void Function_BIA(void){
    /* Checks if interrupt flag is up, which will be set when interrupt occurred. */
    if(AD5940_GetMCUIntFlag())
    {
      AD5940_ClrMCUIntFlag(); /* Clear this flag */
      temp = APPBUFF_SIZE;
      AppBIAISR(AppBuff, &temp); /* Deal with it and provide a buffer to store data we got */
      BIAShowResult(AppBuff, temp); /* Show the results to UART or BLE */
    } 
}
