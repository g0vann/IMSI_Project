/*!
 *****************************************************************************
 * @file:    Cli.h
 * @brief:   CLI
 * @version: $Revision$
 * @date:    $Date$
 *-----------------------------------------------------------------------------
 *
Copyright (c) 2015-2017 Analog Devices, Inc.

All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:
  - Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.
  - Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.
  - Modified versions of the software must be conspicuously marked as such.
  - This software is licensed solely and exclusively for use with processors
    manufactured by or for Analog Devices, Inc.
  - This software may not be combined or merged with other code in any manner
    that would cause the software to become subject to terms and conditions
    which differ from those listed here.
  - Neither the name of Analog Devices, Inc. nor the names of its
    contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.
  - The use of this software may or may not infringe the patent rights of one
    or more patent holders.  This license does not release you from the
    requirement that you obtain separate licenses from these patent holders
    to use this software.

THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES, INC. AND CONTRIBUTORS "AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
TITLE, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN
NO EVENT SHALL ANALOG DEVICES, INC. OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, PUNITIVE OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, DAMAGES ARISING OUT OF CLAIMS OF INTELLECTUAL
PROPERTY RIGHTS INFRINGEMENT; PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *****************************************************************************/
#ifndef CLI_H_
#define CLI_H_

#include <stddef.h>
#include "drivers/uart/adi_uart.h"
#include "Communication.h"

/* CLI SW version */
#define  VERSION  "V2.0"


/* ASCII code */
#define CTRL_C_	3
#define BS_		8
#define TAB_	9
#define CR_		13
#define LF_		10
#define SPC_	32
#define SP_		127

/* Used values */
#define UART_CMD_LENGTH			2
#define UART_CMD_USUALMAXLEN	32

extern ADI_UART_HANDLE p_device_handler;

typedef  int32_t (*msUartCPFun)(uint8_t *);

extern uint8_t          *kwnUrtMonCmd[];
extern msUartCPFun   urtMonCmdFun[];

extern int32_t			Cli_CmdHelp(uint8_t *args);
extern int32_t			Cli_CmdVersion(uint8_t *args);
extern int32_t			Cli_CmdDump(uint8_t *args);
extern int32_t			Cli_CmdReset(uint8_t *args);

extern uint8_t		*Cli_FindNextArgv(uint8_t *args);
extern int32_t			Cli_GetArgv(uint8_t *dst, uint8_t *args);
extern int32_t			Cli_IsSeparator(uint8_t c);

extern int32_t			Cli_Prompt(void);
extern void			Cli_Process(void);
extern msUartCPFun	Cli_FindCommandFunction(uint8_t *);

extern void			Cli_Printf(const char *fmt, ...);
extern void			Cli_Int_Handler(void);
#endif /* CLI_H_ */
