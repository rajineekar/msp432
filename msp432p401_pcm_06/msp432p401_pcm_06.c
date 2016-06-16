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
//   MSP432P401 Demo - Use TimerA in Low-Frequency mode
//
//
//   Description: Configure device to operate in Low-Frequency LPM0 with TimerA
//   running and sourced by a low-frequency clock source. This examples builds
//   up on msp432p401_pcm_05, which exercises the transition into LF active mode 
//   from active mode (LDO). 
//
//   Note: the code in this example assumes the device is currently in LDO mode
//   AM0_LDO or AM1_LDO (Active Mode using LDO, VCore=0/1 respectively)
//
//   Transition from DCDC mode to LF requires intermediate transition through
//   LDO mode. For more information refer to the PCM chapter in the device user's
//   guide.
//
//          AM1_DCDC <----->  AM1_LDO  <--@--> AM1_LF
//                               ^
//                               |
//                               |
//                               v
//          AM0_DCDC <----->  AM0_LDO* <--@--> AM0_LF 
//                                               ^
//                                               |
//                                               |
//                                               v
//                                             LPM0_LF 
//   *: power state condition after reset
//   @: transitions demonstrated in this code example
//
//                MSP432p401rpz
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |             P1.0|--->LED
//            |                 |
//
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"
#include "stdint.h"


void error(void);

int main(void)
{
    uint32_t currentPowerState;
    volatile uint32_t i;

    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT
    P1DIR |= BIT0;


    /* Switch MCLK, SMCLK, ACLK sources over to REFO clock for low frequency operation first */
    CSKEY = CSKEY_VAL;                        // Unlock CS module
    CSCTL1 = SELM_2 | SELS_2 | SELA_2;
    CSKEY = 0;                             // Lock CS module

    /* Get current power state */
    currentPowerState = PCMCTL0 & CPM_M;
    /* Transition to Low-Frequency Mode from current LDO power state properly */
    switch (currentPowerState)
    {
        case CPM_0:                // AM0_LDO, need to switch to AM0_Low-Frequency Mode
            while ((PCMCTL1 & PMR_BUSY));
            PCMCTL0 = PCM_CTL_KEY_VAL | AMR_8;
            while ((PCMCTL1 & PMR_BUSY));
            if (PCMIFG & AM_INVALID_TR_IFG)
                error();                    // Error if transition was not successful
            break;
        case CPM_1:                // AM1_LDO, need to switch to AM1_Low-Frequency Mode
            while ((PCMCTL1 & PMR_BUSY));
            PCMCTL0 = PCM_CTL_KEY_VAL | AMR_9;
            while ((PCMCTL1 & PMR_BUSY));
            if (PCMIFG & AM_INVALID_TR_IFG)
                error();                    // Error if transition was not successful
            break;
        case CPM_8:                // Device is already in AM0_Low-Frequency Mode
            break;
        case CPM_9:                // Device is already in AM1_Low-Frequency Mode
            break;
        default:                            // Device is in some other state, which is unexpected
            error();
    }

    /* Configure TimerA to run off SMCLK =  REFO */

    TA0CCTL0 = OUTMOD_0;                   // Output Mode 0 (=0)
    TA0CCR0 = 1000;
    TA0CTL = TASSEL__SMCLK | MC__CONTINUOUS; // SMCLK, continuous mode


    while(1)
    {
        if (TA0CCTL0 & CCIFG)               // TA0CCIFG flag set
        {
            P1OUT ^= BIT0;                  // Toggle P1.0 LED
            TA0CCTL0 &= ~ CCIFG;
            TA0CCR0 +=1000;
        }

    }

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
