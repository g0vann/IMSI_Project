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
#include <string.h>

#include "ConfigurationBLE.h"

#include <stdarg.h>
#include <math.h>
#include "ADuCM3029.h"


#define  VERSION  "FrancOlino v2.0"
#define LINEBUFF_SIZE 128
#define CMDTABLE_SIZE 2
#define PI 3.141592654
static float Mag,Ph;
static int count;
int b = 0;

/**********BLE*********************/
extern bool gbConnected;
extern ADI_DATA_PACKET eDataPacket;

void Function_DFT(void);
extern void DFT_config(void);
/* Functions to Read/Write AD5940 Register*/
extern uint32_t SPIReadReg(uint16_t RegAddr);
extern void SPIWriteReg(uint16_t RegAddr, uint32_t RegData);

/*Variables declarations that will be used to search for commands*/
char line_buffer[LINEBUFF_SIZE];
uint32_t line_buffer_index = 0;
uint32_t token_count = 0;
void *pObjFound = 0;
uint32_t parameter1, parameter2;

uint32_t Cli_start(uint32_t para1,uint32_t para2);
uint32_t Cli_stop(uint32_t para1,uint32_t para2);


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


uint32_t Cli_start(uint32_t para1,uint32_t para2){
        DFT_config();
        b=1;
	return 0;
}

uint32_t Cli_stop(uint32_t para1,uint32_t para2){
       //printf("aoooooooooooooooooooooooooooooooooooooooooooooo\r\n");
       b=0;
      // AD5940_WriteReg(REG_AFE_ADCCON, REG_AFE_ADCCON_RESET);
       NVIC_SystemReset();
       //return 0;
}

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
static uint32_t Str2Num(char *s, uint32_t *Res)
{
   
  *Res = strtol((const char*)s, NULL, 16);

  return 0;
}

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
    //printf("res:0x%08x\n", res);
    line_buffer_index = 0;  /* Reset buffer */
  }
  else
  {
    line_buffer[line_buffer_index++] = c;
  }
}

void BLECmd_Process(char c[20]){
  
  uint32_t res;
  line_buffer_index=20;
  for(int i = 0;i<20;i++){
    line_buffer[i] = c[i];
  }
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
  if(token_count > 1)           /* There is parameters */
  {
    UARTCmd_TranslateParas();
  }
  /* Step3, call function */
  res = ((uint32_t (*)(uint32_t, uint32_t))(pObjFound))(parameter1, parameter2);
  //printf("res:0x%08x\n", res);
  line_buffer_index = 0;  /* Reset buffer */
  
}


void Function_DFT(void){
  ADI_BLER_RESULT eResult;
  ADI_BLER_CONN_INFO sConnInfo;
  char MagLen[50];
  char PhLen[50];
  while(1)
  {
    eResult = adi_ble_DispatchEvents(150);
    if(eResult==ADI_BLER_FAILURE){
      printf("Error dispatching events to the callback.\r\n");
    }
        
    int32_t real, image;
    Mag=0;
    char Mag1[50];
    
    Ph=0;
    char Ph1[50];

    if(AD5940_INTCTestFlag(AFEINTC_1,AFEINTSRC_DFTRDY))
    {
      AD5940_INTCClrFlag(AFEINTSRC_DFTRDY);
      real = AD5940_ReadAfeResult(AFERESULT_DFTREAL);
      if(real&(1<<17))
        real |= 0xfffc0000; /* Data is 18bit in two's complement, bit17 is the sign bit */
      //printf("DFT: %d,", real);      
      image = AD5940_ReadAfeResult(AFERESULT_DFTIMAGE);
      if(image&(1<<17))
        image |= 0xfffc0000; /* Data is 18bit in two's complement, bit17 is the sign bit */
      //printf("%d,", image);      
      /*A noi servono modulo e fase, se vogliamo esercitarci solo su matlab conviene per ora usare solo il modulo*/
      /*Mag sta a rappresentare il modulo, conviene metterlo in una variabile*/

      Mag=sqrt((float)real*real + (float)image*image);
      if((count % 100) == 0 ){ //ogni 100 campioni voglio alteraere il modulo
        Mag=Mag+60;  //provo ad iniettare dei valori sballati ogni 100 stampe per vedere come reagisce il plotting
      }
      /*Ph sta a rappresentare la fase*/
      Ph=atan2f((float)real,(float)image);
      Ph=Ph * 180.0/PI;
      if(Mag<400){
        if(gbConnected == true) {
          adi_ble_GetConnectionInfo(&sConnInfo);

          sprintf(Mag1,"%g",Mag);
          sprintf(Ph1,"%g",Ph);
          //strcat(Mag1," ");
          //strcat(Mag1,Ph1);
          
          sprintf(MagLen,"%g",(double)strlen(Mag1));
          sprintf(PhLen,"%g",(double)strlen(Ph1));
          strcat(MagLen,Mag1);
          strcat(PhLen,Ph1);
          
          
          eDataPacket.eStringData.nStringSize = 13;
          
          memcpy(eDataPacket.eStringData.aStringData, MagLen,13);

  
          eResult = adi_radio_DE_SendData(sConnInfo.nConnHandle, DATAEXCHANGE_PACKET_SIZE, (uint8_t*)&eDataPacket);
          if (eResult != ADI_BLER_SUCCESS) 
            printf("Error sending the data.\r\n");
        
          eDataPacket.eStringData.nStringSize = 13;
          memcpy(eDataPacket.eStringData.aStringData, PhLen,13 );
          
          eResult = adi_radio_DE_SendData(sConnInfo.nConnHandle, DATAEXCHANGE_PACKET_SIZE, (uint8_t*)&eDataPacket);
          if (eResult != ADI_BLER_SUCCESS) 
            printf("Error sending the data.\r\n");
          /*strcat(MagLen," ");
          strcat(MagLen,PhLen);
          printf("%s\r\n",MagLen);*/
        }else {
          printf("%f %f\r\n",Mag,Ph);        
        }
      }
      count = count+1; //incremento il contatore
    }
  }
}