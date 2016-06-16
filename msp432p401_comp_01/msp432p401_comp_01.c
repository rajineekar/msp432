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
//   MSP432P401 Demo -   COMP output Toggle in LPM0 ; input channel C01;
//                     Vcompare is compared against internal 2.0V reference
//
// Description: Use Comp and internal reference to determine if input'Vcompare'
// is high or low.  When Vcompare exceeds 2.0V C0OUT goes high and when Vcompare
// is less than 2.0V then C0OUT goes low.
//
//                MSP432p401rpz
//             ------------------
//         /|\|                  |
//          | |                  |
//          --|RST       P8.0/C01|<--Vcompare
//            |                  |
//            |        P7.1/C0OUT|----> 'high'(Vcompare>2.0V); 'low'(Vcompare<2.0V)
//            |                  |
//
//
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************

#include "msp.h"
#include "stdint.h"

int main(void) {
    volatile uint32_t i;

    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT


    /* Configure C0OUT port ping */
    P7DIR |= BIT1;                          // P3.5 output direction
    P7SEL0 |= BIT1;                         // Select C0OUT function on P7.1

    // Setup Comparator_E
    CE0CTL0 = CEIPEN | CEIPSEL_M;            // Enable V+, input channel CE1
    CE0CTL1 = CEPWRMD_1;                     // normal power mode
    CE0CTL2 = CEREFL_2 | CERS_3 | CERSEL;    // VREF is applied to -terminal
                                            // R-ladder off; bandgap ref voltage
                                            // supplied to ref amplifier to get Vcref=2.0V
    CE0CTL3 = BIT1;                          // Input Buffer Disable @P1.1/CE1
    CE0CTL1 |= CEON;                         // Turn On Comparator_E
    for (i=0;i<75;i++);                     // delay for the reference to settle

    __sleep();
    __no_operation();                         // For debug
}



