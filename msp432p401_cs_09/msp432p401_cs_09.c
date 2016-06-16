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
//   MSP432P401 Demo - Device configuration for operation @ MCLK = DCO = 48MHz
//
//   Description: Proper device configuration to enable operation at MCLK=48MHz
//   including:
//   1. Configure VCORE level to 1
//   2. Configure flash wait-state to 2
//   3. Configure HFXT sourced by 48MHz XTAL
//   4. Source MCLK & HSMCLK from HFXT
//
//   After configuration is complete, MCLK & HSMCLK are output to port pins P4.3.
//
//                MSP432P401RPZ
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |             P4.3|----> MCLK
//            |             P4.4|----> HSMCLK
//            |                 |
//            |             PJ.2|------
//            |                 |     |
//            |                 |    HFXT @ 48MHz
//            |                 |     |
//            |             PJ.3|------

//

//   Dung Dang
//   Texas Instruments Inc.
//   July 2015
//   Built with Code Composer Studio V6.1
//******************************************************************************
#include "msp.h"
#include "stdint.h"

#define FLCTL_BANK0_RDCTL_WAIT__2    (2 << 12)
#define FLCTL_BANK1_RDCTL_WAIT__2    (2 << 12)

void error(void);

int main(void)
{
    uint32_t currentPowerState;
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT


    /* NOTE: This example assumes the default power state is AM0_LDO.
     * Refer to  MSP4322001_pcm_0x code examples for more complete PCM operations
     * to exercise various power state transitions between active modes.
     */

    /* Step 1: Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */

    /* Get current power state, if it's not AM0_LDO, error out */
    currentPowerState = PCMCTL0 & CPM_M;
    if (currentPowerState != CPM_0)
        error();

    while ((PCMCTL1 & PMR_BUSY));
    PCMCTL0 = PCM_CTL_KEY_VAL | AMR_1;
    while ((PCMCTL1 & PMR_BUSY));
    if (PCMIFG & AM_INVALID_TR_IFG)
        error();                            // Error if transition was not successful
    if ((PCMCTL0 & CPM_M) != CPM_1)
        error();                            // Error if device is not in AM1_LDO mode

    /* Step 2: Configure Flash wait-state to 2 for both banks 0 & 1 */
    FLCTL_BANK0_RDCTL = FLCTL_BANK0_RDCTL & ~FLCTL_BANK0_RDCTL_WAIT_M | FLCTL_BANK0_RDCTL_WAIT_2;
    FLCTL_BANK1_RDCTL = FLCTL_BANK0_RDCTL & ~FLCTL_BANK1_RDCTL_WAIT_M | FLCTL_BANK1_RDCTL_WAIT_2;

    /* Step 3: Configure HFXT to use 48MHz crystal, source to MCLK & HSMCLK*/


    PJSEL0 |= BIT2 | BIT3;                  // Configure PJ.2/3 for HFXT function
    PJSEL1 &= ~(BIT2 | BIT3);

    CSKEY = CSKEY_VAL;                      // Unlock CS module for register access
    CSCTL2 |= HFXT_EN | HFXTFREQ_6 | HFXTDRIVE;
    while(CSIFG & HFXTIFG)
    	CSCLRIFG |= CLR_HFXTIFG;

    /* Select MCLK & HSMCLK = HFXT, no divider */
    CSCTL1 = CSCTL1 & ~(SELM_M | DIVM_M | SELS_M | DIVHS_M) | SELM__HFXTCLK | SELS__HFXTCLK;
    
    CSKEY = 0;                             // Lock CS module from unintended accesses


    /* Step 4: Output MCLK to port pin to demonstrate 48MHz operation */
    P4DIR |= BIT3 | BIT4;
    P4SEL0 |=BIT3 | BIT4;                         // Output MCLK
    P4SEL1 &= ~(BIT3 | BIT4);

    /* Go to sleep */
    __sleep();
    __no_operation();                      // For debugger


}

void error(void)
{
    volatile uint32_t i;
    P1DIR |= BIT0;
    while (1)
    {
        P1OUT ^= BIT0;
        for(i=0;i<20000;i++);       // Blink LED forever
    }
}
