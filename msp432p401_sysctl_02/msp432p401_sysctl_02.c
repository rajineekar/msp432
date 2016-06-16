/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2013, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *******************************************************************************
 *
 *                       MSP432 CODE EXAMPLE DISCLAIMER
 *
 * MSP432 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see http://www.ti.com/tool/mspdriverlib for an API functional
 * library & https://dev.ti.com/pinmux/ for a GUI approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
//******************************************************************************
//   MSP432P401 Demo - Enable/disable SRAM bank accesses
//
//   Description: Configure SRAM banks to enable & disable selected banks.
//   Unused SRAM banks can be disabled to minimize power consumption.
//
//                MSP432p401rpz
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//            |             P1.0|-->LED
//
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"
#include "stdint.h"


int main(void)
{
    volatile uint32_t i;

    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT
    P1DIR |= BIT0;                          // P1.0 set as output

    /* Turn on all SRAM banks for active mode usage.
     * Banks 7 & below are enabled */
    SYSCTL_SRAM_BANKEN = SYSCTL_SRAM_BANKEN_BNK7_EN ;

    /* Write & read back from an SRAM memory location in bank 7*/
    * ( (uint32_t*) 0x2000EFFC) = 0xDEADBEEF;
    i = * ( (uint32_t*) 0x2000EFFC);
    if (i!= 0xDEADBEEF)
    {
        P1OUT |= BIT0;                      // P1.0 LED is on
        while(1);
    }

    /* Banks 3 & below are enabled, Banks 7-4 are disabled
     * Previous memory location in Bank 7 should return 0 now */
    SYSCTL_SRAM_BANKEN = SYSCTL_SRAM_BANKEN_BNK3_EN ;
    i = * ( (uint32_t*) 0x2000EFFC);
    if (i!= 0)
    {
        P1OUT |= BIT0;                      // P1.0 LED is on
        //while(1);
    }

    /* Write & read back from an SRAM memory location in bank 3*/
    * ( (uint32_t*) 0x20006FFC) = 0xCAFEBABE;
    i = * ( (uint32_t*) 0x20006FFC);
    if (i!= 0xCAFEBABE)
    {
        P1OUT |= BIT0;                      // P1.0 LED is on
        //while(1);
    }
    /* Enable bank 0 only */
    SYSCTL_SRAM_BANKEN = SYSCTL_SRAM_BANKEN_BNK0_EN ;

    /* Write & read back from an SRAM memory location in bank 3
     * the read back value should be 0 */
    * ( (uint32_t*) 0x20006FFC) = 0xCAFEBABE;
    i = * ( (uint32_t*) 0x20006FFC);
    if (i!= 0x0)
    {
        P1OUT |= BIT0;                      // P1.0 LED is on
        while(1);
    }


}

