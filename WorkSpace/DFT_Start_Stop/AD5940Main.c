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
#include "ad5940.h"
#include "AD5940.h"

#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

void DFT_config(void);
extern void Function_DFT(void);
extern int b;


#define APPBUFF_SIZE 512
uint32_t AppBuff[APPBUFF_SIZE];


/* Initialize AD5940 basic blocks like clock */
static int32_t AD5940PlatformCfg(void)
{
  CLKCfg_Type clk_cfg;
  FIFOCfg_Type fifo_cfg;
  SEQCfg_Type seq_cfg;
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
  /* Configure sequencer and stop it */
  seq_cfg.SeqMemSize = SEQMEMSIZE_2KB;
  seq_cfg.SeqBreakEn = bFALSE;
  seq_cfg.SeqIgnoreEn = bFALSE;
  seq_cfg.SeqCntCRCClr = bTRUE;
  seq_cfg.SeqEnable = bFALSE;
  seq_cfg.SeqWrTimer = 0;
  AD5940_SEQCfg(&seq_cfg);

  /* Step3. Interrupt controller */
  AD5940_INTCCfg(AFEINTC_1, AFEINTSRC_ALLINT, bTRUE);           /* Enable all interrupt in Interrupt Controller 1, so we can check INTC flags */
  AD5940_INTCCfg(AFEINTC_0, AFEINTSRC_DATAFIFOTHRESH, bTRUE);   /* Interrupt Controller 0 will control GP0 to generate interrupt to MCU */
  AD5940_INTCClrFlag(AFEINTSRC_ALLINT);
  /* Step4: Reconfigure GPIO */
  gpio_cfg.FuncSet = GP6_SYNC|GP5_SYNC|GP4_SYNC|GP2_EXTCLK|GP1_SYNC|GP0_INT;
  gpio_cfg.InputEnSet = AGPIO_Pin2;
  gpio_cfg.OutputEnSet = AGPIO_Pin0|AGPIO_Pin1|AGPIO_Pin4|AGPIO_Pin5|AGPIO_Pin6;
  gpio_cfg.OutVal = 0;
  gpio_cfg.PullEnSet = 0;
  AD5940_AGPIOCfg(&gpio_cfg);
	
  AD5940_SleepKeyCtrlS(SLPKEY_UNLOCK);  /* Enable AFE to enter sleep mode. */

  /*funzione per la DFT*/
  
  //DFT_config();
  
  return 0;
}

void DFT_config(void){
  DSPCfg_Type dsp_cfg;
  WGCfg_Type wg_cfg;
  /* Configure AFE power mode and bandwidth */
  AD5940_AFEPwrBW(AFEPWR_LP, AFEBW_250KHZ);
  
  AD5940_StructInit(&dsp_cfg, sizeof(dsp_cfg));
  /* Initialize ADC basic function */
  dsp_cfg.ADCBaseCfg.ADCMuxP = ADCMUXP_VCE0;
  dsp_cfg.ADCBaseCfg.ADCMuxN = ADCMUXN_VSET1P1;
  dsp_cfg.ADCBaseCfg.ADCPga = ADCPGA_1;
  
  /* Initialize ADC filters ADCRawData-->SINC3-->SINC2+NOTCH */
  dsp_cfg.ADCFilterCfg.ADCSinc3Osr = ADCSINC3OSR_4;
  dsp_cfg.ADCFilterCfg.ADCSinc2Osr = ADCSINC2OSR_1333;
  dsp_cfg.ADCFilterCfg.ADCAvgNum = ADCAVGNUM_2;         /* Don't care about it. Average function is only used for DFT */
  dsp_cfg.ADCFilterCfg.ADCRate = ADCRATE_800KHZ;        /* If ADC clock is 32MHz, then set it to ADCRATE_1P6MHZ. Default is 16MHz, use ADCRATE_800KHZ. */
  dsp_cfg.ADCFilterCfg.BpNotch = bTRUE;                 /* SINC2+Notch is one block, when bypass notch filter, we can get fresh data from SINC2 filter. */
  dsp_cfg.ADCFilterCfg.BpSinc3 = bFALSE;                /* We use SINC3 filter. */
  dsp_cfg.ADCFilterCfg.Sinc2NotchEnable = bTRUE;        /* Enable the SINC2+Notch block. You can also use function AD5940_AFECtrlS */
  dsp_cfg.DftCfg.DftNum = DFTNUM_16384;
  dsp_cfg.DftCfg.DftSrc = DFTSRC_SINC3;
  AD5940_DSPCfgS(&dsp_cfg);

  AD5940_StructInit(&wg_cfg, sizeof(wg_cfg));
  wg_cfg.WgType = WGTYPE_SIN;
  wg_cfg.SinCfg.SinFreqWord = AD5940_WGFreqWordCal(500.0, 16000000.0);  /* 10kHz */
  AD5940_WGCfgS(&wg_cfg);

  /* Enable all interrupt at Interrupt Controller 1. So we can check the interrupt flag */
  AD5940_AFECtrlS(AFECTRL_ADCPWR|AFECTRL_SINC2NOTCH|AFECTRL_WG, bTRUE);
  AD5940_AFECtrlS(AFECTRL_DFT, bTRUE);
  AD5940_ADCConvtCtrlS(bTRUE);
}

void AD5940_Main(void)
{
   
  AD5940PlatformCfg(); // Board configuration
  
  //OTTIMOOOOOOOO!
  
  while(1)
  {
      if (b==1){
          
	Function_DFT();
      }
  }
}





