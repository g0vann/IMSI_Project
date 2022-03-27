#ifndef CHIPSEL_M355
#define CHIPSEL_M355
#endif
#include "ad5940.h"
#include "AD5940.h"
#include <stdio.h>
#include "string.h"
#include <stdlib.h>

#define REG_PROVA 0x20000000; // Indirizzo della RAM dati



void Cicciobello(void){
  
  unsigned long temp,data;
  AD5940_HWReset();        //setto l'HW reset
  AD5940_Initialize();     //Inizializzo la scheda slave
    
  data = rand()&0xffcc;   // numero casuale da 16 bit
  data <<= 16;            // shift a sinistra di 16 bit
  data |= rand()&0xffff;  // or bit a bit
  
  AD5940_WriteReg(REG_PROVA,data);
  
  
  
  temp = AD5940_ReadReg(REG_PROVA);
  printf("Read PROVA DATA register, got: 0x%04lx\n", temp);
 
}