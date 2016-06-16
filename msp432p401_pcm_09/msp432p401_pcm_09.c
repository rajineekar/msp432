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
//   MSP432P401 Demo - device operation at 48MHz with DC-DC Regulator
//
//
//   Description: Configure device to operate at 48MHz and use the DC-DC
//   regulator instead of the default LDO. The following steps must be taken in
//   sequence shown below to ensure proper Flash operation at 48MHz and with
//   DC-DC regulator:
//   1. VCORE LEVEL = 1
//   2. Switch from LDO to DC-DC.
//   3. Configure flash Wait-state = 2 (Flash max frequency = 16MHz)
//   4. Configure DCO to 48MHz
//   5. Switch MCLK to use DCO as source
//
//   Note: DC-DC usage requires a 4.7uH inductor connected between VSW and VCORE
//   pins. VCORE pin still requires its regular 100nF and 4.7uF capacitors.
//
//   Note: the code in this example assumes the device is currently in AM0_LDO
//  (Active Mode using LDO, VCore=0/1 respectively). Refer to previous
//  msp432p401_pcm_0x code examples for more complete power state transitions
//  between active modes.
//
//   Transition from DCDC mode to Low-Frequency Mode requires intermediate transition through
//   LDO mode. For more information refer to the PCM chapter in the device user's
//   guide.
//
//   AM_DCDC_VCORE1  <--@-->  AM_LDO_VCORE1  <-----> AM_LF_VCORE1
//                                  ^
//                                  |
//                                  @
//                                  |
//                                  v
//   AM_DCDC_VCORE0  <----->  AM_LDO_VCORE0* <-----> AM_LF_VCORE0
//
//   *: power state condition after reset
//   @: transitions demonstrated in this code example
//
//                MSP432P401R
//             -----------------
//         /|\|                 |
//          | |           VCORE |------
//          --|RST              |     |
//            |                 |    4.7uH
//            |                 |     |
//            |           VSW   |------
//            |                 |
//            |           P1.0  |------> LED
//            |                 |
//            |           P4.3  |------> MCLK

//
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
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
    P1DIR |= BIT0;



    /* NOTE: This example assumes the default power state is AM_LDO_VCORE0.
     * Refer to previous MSP4322001_pcm_0x code examples for more complete
     * power state transitions between active modes.
     */

    /* Step 1: Transition to VCORE Level 1: AM_LDO_VCORE0 --> AM_LDO_VCORE1 */

    /* Get current power state, if it's not AM_LDO_VCORE0, error out */
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

    /* Step 2: Transition from AM1_LDO to AM1_DCDC */
    while ((PCMCTL1 & PMR_BUSY));
    PCMCTL0 = PCM_CTL_KEY_VAL | AMR_5;
    while ((PCMCTL1 & PMR_BUSY));
    if (PCMIFG & AM_INVALID_TR_IFG)
        error();                            // Error if transition was not successful
    if ((PCMCTL0 & CPM_M) != CPM_5)
        error();                            // Error if device is not in AM_DCDC_VCORE1 mode


    /* Step 3: Configure Flash wait-state to 2 for both banks 0 & 1 */
    FLCTL_BANK0_RDCTL = FLCTL_BANK0_RDCTL & ~FLCTL_BANK0_RDCTL_WAIT_M | FLCTL_BANK0_RDCTL_WAIT_2;
    FLCTL_BANK1_RDCTL = FLCTL_BANK0_RDCTL & ~FLCTL_BANK1_RDCTL_WAIT_M | FLCTL_BANK1_RDCTL_WAIT_2;

    /* Step 4&5: Configure DCO to 48MHz, ensure MCLK uses DCO as source*/

    CSKEY = CSKEY_VAL;                         // Unlock CS module for register access
    CSCTL0 = 0;                            // Reset tuning parameters
    CSCTL0 = DCORSEL_5;           // Set DCO to 48MHz
    /* Select MCLK = DCO, no divider */
    CSCTL1 = CSCTL1 & ~(SELM_M | DIVM_M) | SELM_3;
    CSKEY = 0;                             // Lock CS module from unintended accesses


    P1OUT |= BIT0;                          // All operations successful

    /* Output MCLK to port pin to demonstrate 48MHz operation */
    P4DIR |= BIT3;
    P4SEL0 |=BIT3;                         // Output MCLK
    P4SEL1 &= ~(BIT3);

    __no_operation();
    while(1);


}

void error(void)
{
    volatile uint32_t i;
    while (1)
    {
        P1OUT ^= BIT0;
        for(i=0;i<20000;i++);       // Blink LED forever
    }
}
