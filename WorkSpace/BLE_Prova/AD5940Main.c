/*!
 *****************************************************************************
 @file:    AD5940Main.c
 @author:  Neo Xu
 @brief:   Used to control specific application and process data.
 -----------------------------------------------------------------------------

Copyright (c) 2017-2019 Analog Devices, Inc. All Rights Reserved.

This software is proprietary to Analog Devices, Inc. and its licensors.
By using this software you agree to the terms of the associated
Analog Devices Software License Agreement.
 
*****************************************************************************/
/** 
 * @addtogroup AD5940_System_Examples
 * @{
 *  @defgroup BioElec_Example
 *  @{
  */
#include "ad5940.h"
#include "AD5940.h"
#include <stdio.h>
#include "string.h"
#include "math.h"
#include "BodyImpedance.h"
#include "ConfigurationBLE.h"

/*****************************************************************************
---------------------------BLE Function--------------------------------------
*****************************************************************************/
extern void SetAdvertisingMode(void);
extern void InitBluetoothLowEnergy(void);
ADI_DATA_PACKET eDataPacket;
extern bool gbConnected;
extern ADI_BLE_GAP_MODE geMode;
extern void Function_BIA_BLE(void);
/****************************************************************************
****************************************************************************/


extern void Function_BIA(void);
extern int b;
float scelta,Freq1,Freq2;
uint32_t Npunti=0, Ncicli=0;
BoolFlag SweepON; 

/* It's your choice here how to do with the data. Here is just an example to print them to UART */
int32_t BIAShowResult(uint32_t *pData, uint32_t DataCount)
{
  ADI_BLER_RESULT eResult;
  ADI_BLER_CONN_INFO sConnInfo; 
  float freq,ph;;

  fImpPol_Type *pImp = (fImpPol_Type*)pData;
  AppBIACtrl(BIACTRL_GETFREQ, &freq);
  
  if(gbConnected == true) {
    adi_ble_GetConnectionInfo(&sConnInfo);
      if (DataCount>0){
      ph = pImp[0].Phase*180/MATH_PI; 
      if(ph>300)
        ph=ph-360;
      memcpy((uint8_t*)&eDataPacket.aPayload[0],(uint8_t*)&freq,4);
      memcpy((uint8_t*)&eDataPacket.aPayload[4],(uint8_t*)&pImp[0].Magnitude,4);
      memcpy((uint8_t*)&eDataPacket.aPayload[8],(uint8_t*)&ph,4);
      }
      eResult = adi_radio_DE_SendData(sConnInfo.nConnHandle, DATAEXCHANGE_PACKET_SIZE, (uint8_t*)&eDataPacket);
      if (eResult != ADI_BLER_SUCCESS) 
        printf("Error sending the data.\r\n");
  
  }else{
    if (DataCount>0){
    ph = pImp[0].Phase*180/MATH_PI; 
      if(ph>300)
        ph=ph-360;
    printf("%.2f ", freq);
    printf("%f %f \r\n",pImp[0].Magnitude,ph);
    }
    /*Process data*/
    //for(int i=0;i<DataCount;i++)
    //{
     
    //}
  }

  return 0;
}


/* Initialize AD5940 basic blocks like clock */
static int32_t AD5940PlatformCfg(void)
{
  CLKCfg_Type clk_cfg;
  FIFOCfg_Type fifo_cfg;
  AGPIOCfg_Type gpio_cfg;

  /* Use hardware reset */
  AD5940_HWReset();
  /* Platform configuration */
  AD5940_Initialize();
  /* Step1. Configure clock */
  clk_cfg.ADCClkDiv = ADCCLKDIV_1;
  clk_cfg.ADCCLkSrc = ADCCLKSRC_HFOSC;
  clk_cfg.SysClkDiv = SYSCLKDIV_1;
  clk_cfg.SysClkSrc = SYSCLKSRC_HFOSC;
  clk_cfg.HfOSC32MHzMode = bFALSE;
  clk_cfg.HFOSCEn = bTRUE;
  clk_cfg.HFXTALEn = bFALSE;
  clk_cfg.LFOSCEn = bTRUE;
  AD5940_CLKCfg(&clk_cfg);
  /* Step2. Configure FIFO and Sequencer*/
  fifo_cfg.FIFOEn = bFALSE;
  fifo_cfg.FIFOMode = FIFOMODE_FIFO;
  fifo_cfg.FIFOSize = FIFOSIZE_4KB;                       /* 4kB for FIFO, The reset 2kB for sequencer */
  fifo_cfg.FIFOSrc = FIFOSRC_DFT;
  fifo_cfg.FIFOThresh = 4;//AppBIACfg.FifoThresh;        /* DFT result. One pair for RCAL, another for Rz. One DFT result have real part and imaginary part */
  AD5940_FIFOCfg(&fifo_cfg);                             /* Disable to reset FIFO. */
  fifo_cfg.FIFOEn = bTRUE;  
  AD5940_FIFOCfg(&fifo_cfg);                             /* Enable FIFO here */
  
  /* Step3. Interrupt controller */
  
  AD5940_INTCCfg(AFEINTC_1, AFEINTSRC_ALLINT, bTRUE);           /* Enable all interrupt in Interrupt Controller 1, so we can check INTC flags */
  AD5940_INTCCfg(AFEINTC_0, AFEINTSRC_DATAFIFOTHRESH, bTRUE);   /* Interrupt Controller 0 will control GP0 to generate interrupt to MCU */
  AD5940_INTCClrFlag(AFEINTSRC_ALLINT);
  /* Step4: Reconfigure GPIO */
  gpio_cfg.FuncSet = GP6_SYNC|GP5_SYNC|GP4_SYNC|GP2_TRIG|GP1_SYNC|GP0_INT;
  gpio_cfg.InputEnSet = AGPIO_Pin2;
  gpio_cfg.OutputEnSet = AGPIO_Pin0|AGPIO_Pin1|AGPIO_Pin4|AGPIO_Pin5|AGPIO_Pin6;
  gpio_cfg.OutVal = 0;
  gpio_cfg.PullEnSet = 0;

  AD5940_AGPIOCfg(&gpio_cfg);
  AD5940_SleepKeyCtrlS(SLPKEY_UNLOCK);  /* Allow AFE to enter sleep mode. */
  return 0;
}

/* !!Change the application parameters here if you want to change it to none-default value */
void AD5940BIAStructInit(void)
{
  AppBIACfg_Type *pBIACfg;
  
  AppBIAGetCfg(&pBIACfg);
  
  pBIACfg->SeqStartAddr = 0;
  pBIACfg->MaxSeqLen = 512; /** @todo add checker in function */
  
  pBIACfg->RcalVal = 10000.0;
  pBIACfg->DftNum = DFTNUM_16384;
  //pBIACfg->NumOfData = -1;      /* Never stop until you stop it manually by AppBIACtrl() function */
  pBIACfg->BiaODR = 20;         /* ODR(Sample Rate) 20Hz */
  pBIACfg->FifoThresh = 4;      /* 4 */
  pBIACfg->ADCSinc3Osr = ADCSINC3OSR_2;
  pBIACfg->SinFreq = scelta;
  pBIACfg->SweepCfg.SweepLog = bTRUE;
  pBIACfg->SweepCfg.SweepEn = SweepON;
  if(SweepON == bFALSE){
    pBIACfg->SweepCfg.SweepStart = 10000;
    pBIACfg->SweepCfg.SweepStop = 150000.0;
    pBIACfg->SweepCfg.SweepPoints = 100;
    pBIACfg->NumOfData = -1;
  }else{
    pBIACfg->SweepCfg.SweepStart = Freq1;
    pBIACfg->SweepCfg.SweepStop = Freq2;
    pBIACfg->SweepCfg.SweepPoints = Npunti;
    pBIACfg->NumOfData = (int32_t) Ncicli*Npunti;
  }
}

/*void AD5940_Main(void)
{
	
  AD5940PlatformCfg();
 
  while(1)
  {
       
	if (b==1){
		Function_BIA();
	}
  }
}*/

/****************************************************************************
----------------------------------BLE---------------------------------------
****************************************************************************/
void AD5940_MainBLE(){
  
  AD5940PlatformCfg(); // Board configuration

  ADI_BLER_RESULT eResult;  

  // Bit:0 Sensor Data Packet Bits:1-7 : Sensor ID 
  eDataPacket.nPacketHeader = ADI_SET_HEADER(ADI_SENSOR_PACKET, BIA_ID);

  printf("OTTIMOOOOOOOO!\r\n");
  
  while(1)
  {
    eResult = adi_ble_DispatchEvents(1);
    if(eResult==ADI_BLER_FAILURE){
      printf("Error dispatching events to the callback.\r\n");
    }
    
     if (b==1){
        Function_BIA();
     }
  } 
}

/**
 * @}
 * @}
 * */
 
