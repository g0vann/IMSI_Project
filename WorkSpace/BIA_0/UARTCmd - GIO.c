/*!
 *****************************************************************************
 @file:    UARTCmd.C
 @author:  $Author: nxu2 $
 @brief:   UART Command process
 @version: $Revision: 766 $
 @date:    $Date: 2017-08-21 14:09:35 +0100 (Mon, 21 Aug 2017) $
 -----------------------------------------------------------------------------

Copyright (c) 2017-2019 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.
By using this software you agree to the terms of the associated
Analog Devices Software License Agreement.
 
*****************************************************************************/
#include "stdint.h"
#include "string.h"
#include "stdio.h"
#include <stdlib.h>
#include "ad5940.h"
#include <ctype.h>



#include <stdarg.h>
#include <math.h>
#include "BodyImpedance.h"
#include "ADuCM3029.h"

extern float scelta;
#define APPBUFF_SIZE 512
uint32_t AppBuff[APPBUFF_SIZE];

#define  VERSION  "FrancOlino v2.0"
#define LINEBUFF_SIZE 128
#define CMDTABLE_SIZE 3
#define PI 3.141592654

int b = 0;
extern float scelta,Freq1,Freq2, NFreq;

void Function_BIA(void);

/*Variables for Function_BIA*/
	static uint32_t IntCount;
	static uint32_t count;
	uint32_t temp;
        extern BoolFlag SweepON; 

/* Functions to Read/Write AD5940 Register*/
extern uint32_t SPIReadReg(uint16_t RegAddr);
extern void SPIWriteReg(uint16_t RegAddr, uint32_t RegData);
extern void AD5940BIAStructInit(void);
extern int32_t BIAShowResult(uint32_t *pData, uint32_t DataCount);

/*Variables declarations that will be used to search for commands*/
char line_buffer[LINEBUFF_SIZE];
uint32_t line_buffer_index = 0;
uint32_t token_count = 0;
void *pObjFound = 0;
float parameter1, parameter2, parameter3;

uint32_t Cli_start(float para1,float para2);
uint32_t Cli_stop(float para1,float para2);
//uint32_t Cli_frequence(float para1,float para2);
uint32_t Cli_start2(float para1,float para2, float para3);

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

};

uint32_t Cli_start2(float para1,float para2, float para3){
        if(b==0){  //mettere un if che controlla che la misurazione non sia ancora avviata
        SweepON = bTRUE;
        Freq1 = para1;
        Freq2 = para2;
		NFreq = para3;
        }
        b=1;
	AD5940BIAStructInit(); /* Configure your parameters in this function */
	AppBIAInit(AppBuff, APPBUFF_SIZE); /* Initialize BIA application. Provide a buffer, which is used to store sequencer commands */
	AppBIACtrl(BIACTRL_START, 0);		
	return 0;
}

uint32_t Cli_start(float para1,float para2){
        if(b==0){  //mettere un if che controlla che la misurazione non sia ancora avviata
          SweepON = bFALSE;
          scelta  = para1;
        }
        b=1;
	AD5940BIAStructInit(); /* Configure your parameters in this function */
	AppBIAInit(AppBuff, APPBUFF_SIZE); /* Initialize BIA application. Provide a buffer, which is used to store sequencer commands */
	AppBIACtrl(BIACTRL_START, 0);		
	return 0;
}

uint32_t Cli_stop(float para1,float para2){
       //printf("aoooooooooooooooooooooooooooooooooooooooooooooo\r\n");
       b=0;
      // AD5940_WriteReg(REG_AFE_ADCCON, REG_AFE_ADCCON_RESET);
      
       //return 0;
      AppBIACtrl(BIACTRL_SHUTDOWN, 0);
      NVIC_SystemReset();
      //return 0;
}

/*uint32_t Cli_frequence(float para1,float para2){
       if(b==0){  //mettere un if che controlla che la misurazione non sia ancora avviata
            scelta= para1;
    }
	   return 1;
}*/

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

/* Translate string 'p' to number, store results in 'Res', return error code */
/*static uint32_t Str2Num(char *s, uint32_t *Res)
{
   
  *Res = strtol((const char*)s, NULL, 16);

  return 0;
}*/

static uint32_t Str2Num(char *s, float *Res)
{
   
  *Res = strtof((const char*)s, NULL);

  return 0;
}

/* Translate string 'p' to number, store results in 'Res', return error code */
static uint32_t Str2Int(char *s, uint32_t *Res)
{
   
  *Res = strtol((const char*)s, NULL, 16);

  return 0;
}

void UARTCmd_TranslateParas(void)
{
  char *p = line_buffer;
  parameter1 = 0;
  parameter2 = 0;
  parameter3 = 0;
  while(*p == '\0') p++;    /* goto command */
  while(*p != '\0') p++;    /* skip command. */
  while(*p == '\0') p++;    /* goto first parameter */
  if(Str2Num(p, &parameter1) != 0) return;
  if(token_count == 2) return;           /* Only one parameter */
  while(*p != '\0') p++;    /* skip first command. */
  while(*p == '\0') p++;    /* goto second parameter */
  if(Str2Num(p, &parameter2) != 0) return; 
  if(token_count == 3) return 	/* Non c'è il terzo parameter */
  while(*p != '\0') p++;    /* skip first command. */
  while(*p == '\0') p++;	/* goto third parameter */
  Str2Int(p, &parameter3)	/* Terzo parametro sicuro è un intero */
}

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
    //res = ((uint32_t (*)(uint32_t, uint32_t))(pObjFound))(parameter1, parameter2);
    res = ((uint32_t (*)(float, float))(pObjFound))(parameter1, parameter2);
    //printf("res:0x%08x\n", res);
    line_buffer_index = 0;  /* Reset buffer */
  }
  else
  {
    line_buffer[line_buffer_index++] = c;
  }
}

void Function_BIA(void){


//	 
	
  while(1){
    /* Check if interrupt flag which will be set when interrupt occurred. */
    if(AD5940_GetMCUIntFlag())
    {
      IntCount++;
      AD5940_ClrMCUIntFlag(); /* Clear this flag */
      temp = APPBUFF_SIZE;
      AppBIAISR(AppBuff, &temp); /* Deal with it and provide a buffer to store data we got */
      BIAShowResult(AppBuff, temp); /* Show the results to UART */

      if(IntCount == 240)
      {
	IntCount = 0;
	//AppBIACtrl(BIACTRL_SHUTDOWN, 0);
      }
    }
    count++;
    if(count > 1000000)
    {
       count = 0;
      //AppBIAInit(0, 0);    /* Re-initialize BIA application. Because sequences are ready, no need to provide a buffer, which is used to store sequencer commands */
      //AppBIACtrl(BIACTRL_START, 0);          /* Control BIA measurement to start. Second parameter has no meaning with this command. */
    }
  }		
}