/*!
 *****************************************************************************
 @file:    AD5940_SPI.c
 @author:  $Author: nxu2 $
 @brief:   Basic register read/write test example.
 @version: $Revision: 766 $
 @date:    $Date: 2017-08-21 14:09:35 +0100 (Mon, 21 Aug 2017) $
 -----------------------------------------------------------------------------

Copyright (c) 2017-2019 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.
By using this software you agree to the terms of the associated
Analog Devices Software License Agreement.

*****************************************************************************/

/**
 * This example shows how to read/write AD5940 registers through SPI.
 * Use function called AD5940_ReadReg and AD5940_WriteReg.
**/
#include "ad5940.h"
#include "AD5940.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>

#define REG_PROVAR0 0x20000008 // Indirizzo della RAM dati

void AD5940_Main(void)
{
  uint32_t data,temp,data2;
  
  AD5940_HWReset();        //setto l'HW reset
  AD5940_Initialize();     //Inizializzo la scheda slave
    
  data = rand()&0xffcc;   // numero casuale da 16 bit
  data <<= 16;            // shift a sinistra di 16 bit
  data |= rand()&0xffff;  // or bit a bit
  data2 = 0x25425449;
  
  *(volatile uint32_t *)(REG_PROVAR0) = data2;
  temp=*(volatile uint32_t *)(REG_PROVAR0);
   
  AD5940_WriteReg(REG_PROVAR0,data);
                        
  printf("Read PROVA DATA register, got: 0x%04lx\n", temp);
  
  
  //printf("Read PROVA DATA register, got: 0x%04lx\n", temp);

  
}
