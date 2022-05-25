/*
 **
 ** Source file generated on maggio 10, 2022 at 23:10:16.	
 **
 ** Copyright (C) 2011-2022 Analog Devices Inc., All Rights Reserved.
 **
 ** This file is generated automatically based upon the options selected in 
 ** the Pin Multiplexing configuration editor. Changes to the Pin Multiplexing
 ** configuration should be made by changing the appropriate options rather
 ** than editing this file.
 **
 ** Selected Peripherals
 ** --------------------
 ** SPI2 (SCLK, MISO, MOSI, CS_0, RDY)
 ** ADC0_IN (ADC0_IN6)
 **
 ** GPIO (unavailable)
 ** ------------------
 ** P0_03, P1_02, P1_03, P1_04, P1_05, P2_09
 */

#include <sys/platform.h>
#include <stdint.h>

#define SPI2_SCLK_PORTP1_MUX  ((uint16_t) ((uint16_t) 1<<4))
#define SPI2_MISO_PORTP1_MUX  ((uint16_t) ((uint16_t) 1<<6))
#define SPI2_MOSI_PORTP1_MUX  ((uint16_t) ((uint16_t) 1<<8))

int32_t adi_initpinmux(void);

/*
 * Initialize the Port Control MUX Registers
 */
int32_t adi_initpinmux(void) {
    /* PORTx_MUX registers */
    *((volatile uint32_t *)REG_GPIO1_CFG) = SPI2_SCLK_PORTP1_MUX | SPI2_MISO_PORTP1_MUX
     | SPI2_MOSI_PORTP1_MUX;

    return 0;
}

