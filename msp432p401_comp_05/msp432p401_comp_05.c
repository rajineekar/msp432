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
//   MSP432P401 Demo -   COMPE Hysteresis, COUT Toggle in SL; High speed mode
//
//  Use CompE and shared reference to determine if input 'Vcompare'
//  is high or low.  Shared reference is configured to generate hysteresis.
//  When Vcompare exceeds Vcc*3/4 COUT goes high and when Vcompare is less
//  than Vcc*1/4 then COUT goes low.
//
//                MSP432p401rpz
//             ------------------
//         /|\|                  |
//          | |                  |
//          --|RST       P8.0/C01|<--Vcompare
//            |                  |
//            |        P7.1/C0OUT|-->'high'(>Vcc*3/4);'low'(<Vcc*1/4)
//            |                  |
//
//
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V5.5
//******************************************************************************

#include "msp.h"
#include "stdint.h"


int main(void) {
    volatile uint32_t i;

    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT


    /* Configure C0OUT port ping */
    P7DIR |= BIT1;                          // P7.1 output direction
    P7SEL0 |= BIT1;                         // Select C0OUT function on P7.1

    /* Setup Comparator_E */
    CE0CTL0 = CEIPEN | CEIPSEL_M;              // Enable V+, input channel CE1
    CE0CTL1 = CEPWRMD_0;                       // CEMRVS=0 => select VREF1 as ref when CEOUT
                                            // is high and VREF0 when CEOUT is low
                                            // High-Speed Power mode
    CE0CTL2 = CERS_1 | CERSEL | CEREFL_0 | CEREFL_3;  // VRef is applied to -terminal
                                            // VREF1 is Vcc*1/4
                                            // VREF0 is Vcc*3/4

    CE0CTL3 = BIT1;                            // Input Buffer Disable @P1.1/CE1
    CE0CTL1 |= CEON;                           // Turn On ComparatorE

    for (i=0;i<75;i++);                     // delay for the reference to settle

    __sleep();                              // Go to LPM0
    __no_operation();                         // For debug
}



