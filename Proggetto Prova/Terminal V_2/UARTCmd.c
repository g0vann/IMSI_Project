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

#define  VERSION  "FrancOlino v2.0"
#define LINEBUFF_SIZE 128
#define CMDTABLE_SIZE 9

/* Functions to Read/Write AD5940 Register*/
extern uint32_t SPIReadReg(uint16_t RegAddr);
extern void SPIWriteReg(uint16_t RegAddr, uint32_t RegData);

/*dichiarazioni variabili che serviranno per cercare i comandi*/
char line_buffer[LINEBUFF_SIZE];
uint32_t line_buffer_index = 0;
uint32_t token_count = 0;
void *pObjFound = 0;
uint32_t parameter1, parameter2;

uint32_t help(uint32_t para1, uint32_t para2);
uint32_t say_hello(uint32_t para1, uint32_t para2);
uint32_t Cli_CmdVersion(uint32_t para1,uint32_t para2);
uint32_t Cli_CmdDump(uint32_t para1,uint32_t para2);
uint32_t Cli_CmdPush(uint32_t para1,uint32_t para2);
uint32_t Cli_CmdReset(uint32_t para1,uint32_t para2);
uint32_t Cli_CmdDumpAD5940(uint32_t para1,uint32_t para2);
uint32_t Cli_CmdPushAD5940(uint32_t para1,uint32_t para2);

struct __uartcmd_table
{
  void *pObj;
  const char *cmd_name;
  const char *pDesc;
}uart_cmd_table[CMDTABLE_SIZE]=
{
  {(void*)help, "help", "Display available commands"},
  {(void*)help, "?", "Display available commands"},
  {(void*)say_hello, "hello", "Print parameteres and say hello"},
  {(void*)Cli_CmdVersion, "version", "SW version"},
  {(void*)Cli_CmdDump, "dump", "[begaddr][,count] - Dumps the memory"},
  {(void*)Cli_CmdPush, "push", "[addr][,data] - Push data in memory"},
  {(void*)Cli_CmdDumpAD5940, "dumpAD5940", "[address] - Dumps AD5940 Registers"},
  {(void*)Cli_CmdPushAD5940, "pushAD5940", "[address] [,data] - Push data in AD5940 Registers"},
  {(void*)Cli_CmdReset, "reset", "Reset application"},
  
};


uint32_t help(uint32_t para1, uint32_t para2)
{
  int i = 0;
  printf("*****help menu*****\nbelow are supported commands:\n");
  for(;i<CMDTABLE_SIZE;i++)
  {
    if(uart_cmd_table[i].pObj)
      printf("%-8s --\t%s\n", uart_cmd_table[i].cmd_name, uart_cmd_table[i].pDesc);
  }
  printf("***table end***\n");
  return 0x87654321;
}

uint32_t Cli_CmdVersion(uint32_t para1,uint32_t para2)
{
	printf(VERSION);
	printf("\n");
	return 0x12345678;
}

/*la lascio perché se dovesse partire questa sicuro funziona credo*/
uint32_t say_hello(uint32_t para1, uint32_t para2)
{
  printf("para1:0x%08x, para2:0x%08x\n", para1, para2);
  printf("Hello\n");
  return 0x12345678;
}

uint32_t Cli_CmdDump(uint32_t para1,uint32_t para2){
	int32_t			fl = 0;
	static uint32_t ix;
	if (para2 > 0x40) {   // Max offset
		para2 = 0x40;
	}
	printf("HEX dump of the memory (0x%02X bytes from 0x%08X):\r\n", para1, para2);
	fl = 1;
	
	        /* Cicle to print data*/
	for (ix = para1; ix < (para1 + para2); ix += 4){
		if ((ix != para1) && (ix % 16 == 0)) {
			fl = 1;
			printf("\r\n");
		}

		if (fl) {
			printf(" %08X: ", ix);
			fl = 0;
		}

		printf("%08X ", *((uint32_t *)ix));
	}
	return 0;
}

uint32_t Cli_CmdPush(uint32_t para1,uint32_t para2){
	static uint32_t ix;
	printf("HEX Push 0x%02X in 0x%08X\r\n", para2, para1);
	ix=*(volatile uint32_t *)(para1); // Volatile to read phisic memory
    if(ix != 0x00000000){
		printf("Occupied memory area!\r\n");
    }else {
		*(volatile uint32_t *)(para1) = para2; // Volatile to write phisic memory
    }
	return 0;
}

uint32_t Cli_CmdReset(uint32_t para1,uint32_t para2){
	//NVIC_SystemReset(); // Non sono sicuro
       	return 0x12345678; //dobbiamo ritornare qualcosa questo è il return del cmd_version() 
}

uint32_t Cli_CmdDumpAD5940(uint32_t para1,uint32_t para2){ //potremmo togliere il secondo parametro
	static uint32_t ix;
	ix=SPIReadReg(para1);
	printf("AD5940 Register 0x%02X contains 0x%02X%!\r\n",para1,ix);
	return 0;
}

uint32_t Cli_CmdPushAD5940(uint32_t para1,uint32_t para2){
	static uint32_t ix;
	printf("HEX Push 0x%02X in 0x%02X\r\n", para2, para1);
	/* in this case SPIReadReg & SPIWriteReg is used to Read/write on AD5940 Register*/
    ix=SPIReadReg(para1);
    if(ix != 0x00000000){
		printf("AD5940 Register occupied!\r\n");
    }else {
        SPIWriteReg(para1, para2);
    }
	return 0;
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
  char *p;
  unsigned int base=10;
   
  *Res = strtoul( s, &p, base );

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
      return;   /* Command not support */
    }
    if(token_count > 1)           /* There is parameters */
    {
      UARTCmd_TranslateParas();
    }
    /* Step3, call function */
    res = ((uint32_t (*)(uint32_t, uint32_t))(pObjFound))(parameter1, parameter2);
    printf("res:0x%08x\n", res);
    line_buffer_index = 0;  /* Reset buffer */
  }
  else
  {
    line_buffer[line_buffer_index++] = c;
  }
}
