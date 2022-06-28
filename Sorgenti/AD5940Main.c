/*!
******************************************************************************
 @file:    AD5940Main.C
 @author:  $ Author: FrancOlino $
 @brief:   Firmware component: Application Main
 @version: $ Revision: 2.0 $
 @date:    $ Date: 2022-06-27 $
 -----------------------------------------------------------------------------

The code is designed to be part of a firmware executable on the 
AD5940-Bioz board of the Analog Device.
This script contains pieces of code that with a lot of probability
will become part of the Insulinmeter 2.0 final firmware. The examples
and Data Sheet provided by Analog Device were taken into consideration 
to write the code
 
*****************************************************************************/

  
/***************************** Include Files ********************************/
#include "ad5940.h"
#include "AD5940.h"
#include <stdio.h>
#include "string.h"
#include "math.h"
#include "BodyImpedance.h"
#include "ConfigurationBLE.h"

/************************* Functions Definitions ****************************/

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

/************************* Variable Definitions ****************************/
extern void Function_BIA(void);
extern int b;
extern float scelta,Freq1,Freq2;
extern uint32_t Npunti, Ncicli;
extern BoolFlag SweepON; 

/* Sends BIA results */
/** 
	@brief	Sends BIA results
	
	@var	pData - pointer to the data
			DataCount - number of stored data
	
	@return 0 if correct
**/
int32_t BIAShowResult(uint32_t *pData, uint32_t DataCount)
{
  ADI_BLER_RESULT eResult;
  ADI_BLER_CONN_INFO sConnInfo; 
  float freq,ph;;

  fImpPol_Type *pImp = (fImpPol_Type*)pData;
  AppBIACtrl(BIACTRL_GETFREQ, &freq);
  
  /* if BLE connected */
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
  
  }
  /* if BLE disconnected */
  else{
    if (DataCount>0){
    ph = pImp[0].Phase*180/MATH_PI; 
      if(ph>300)
        ph=ph-360;
    printf("%.2f ", freq);
    printf("%f %f \r\n",pImp[0].Magnitude,ph);
    }
 
  }

  return 0;
}

/** 
	@brief	Initialize AD5940 basic blocks like clock
		
	@return 0 if correct
**/
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

/** 
	@brief	Initialization of the BIA Application
**/
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
  //pBIACfg->SinFreq = scelta;
  pBIACfg->SweepCfg.SweepLog = bTRUE;
  pBIACfg->SweepCfg.SweepEn = SweepON;
  if(SweepON == bFALSE){
    pBIACfg->SinFreq = scelta;
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


/****************************************************************************
----------------------------------MAIN---------------------------------------
****************************************************************************/

/** 
	@brief	Msin of Firmware, it calls the function Function_BIA() cyclically
**/
void AD5940_MainBLE(){
  
  AD5940PlatformCfg(); /* Board configuration */

  ADI_BLER_RESULT eResult;  

  /* Bit:0 Sensor Data Packet Bits:1-7 : Sensor ID */
  eDataPacket.nPacketHeader = ADI_SET_HEADER(ADI_SENSOR_PACKET, BIA_ID);

  while(1)
  {
    eResult = adi_ble_DispatchEvents(1);
    if(eResult==ADI_BLER_FAILURE){
      printf("Error dispatching events to the callback.\r\n");
    }
    
     if (b==1){
		/* Do Impedence calculation*/
        Function_BIA();
     }
  } 
}

 
