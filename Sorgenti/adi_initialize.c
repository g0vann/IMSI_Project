/*
******************************************************************************
 @file:    Adi_initialize.C
 @author:  $ Author: FrancOlino $
 @brief:   Firmware component: pin initialize
 @version: $ Revision: 2.0 $
 @date:    $ Date: 2022-06-27 $
 -----------------------------------------------------------------------------
** adi_initialize.c source file generated on maggio 10, 2022 at 19:55:12.
**
** Copyright (C) 2000-2022 Analog Devices Inc., All Rights Reserved.
**
** This file is generated automatically. You should not modify this source file,
** as your changes will be lost if this source file is re-generated.
*/


/****************************************************************************/
/*                       Part of CMSIS Pack library                            */
/****************************************************************************/


#include <sys/platform.h>

#include "adi_initialize.h"

extern int32_t adi_initpinmux(void);

int32_t adi_initComponents(void)
{
	int32_t result = 0;

	/* Calls Pinmux function that anly sets SPI2 for the BLE Chip*/
	if (result == 0) {
		result = adi_initpinmux(); /* auto-generated code (order:0) */
	}

	return result;
}
