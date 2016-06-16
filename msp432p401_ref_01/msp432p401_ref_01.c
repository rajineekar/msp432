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
//   MSP432P401 Demo - Output reference module voltage to a port pin
//
//   Description: Configure and enable the reference module. Output the
//   reference voltage to a port pin. Cycle through the available voltage
//   levels, which can be observed on the oscilloscope/meter at port pin P5.6.
//
//
//                MSP432p401rpz
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |             P5.6|-->VREF
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

    /* Configure P5.6 to its analog function to output VREF */
    P5SEL0 |= BIT6;
    P5SEL1 |= BIT6;

    REFCTL0 |= REFON;                     // Turn on reference module
    REFCTL0 |= REFOUT;                    // Output reference voltage to a pin

    while (1)
    {
        /* Output VREF = 1.2V */
        REFCTL0 &= ~(REFVSEL_3);              // Clear existing VREF voltage level setting
        REFCTL0 |= REFVSEL_0;                 // Set VREF = 1.2V
        while (REFCTL0 & REFGENBUSY);       // Wait until the reference generation is settled
        for (i = 50000; i > 0; i--);
        P1OUT ^= BIT0;                          // Toggle P1.0 LED indicator

        /* Output VREF = 1.45V */
        REFCTL0 &= ~(REFVSEL_3);              // Clear existing VREF voltage level setting
        REFCTL0 |= REFVSEL_1;                 // Set VREF = 1.45V
        while (REFCTL0 & REFGENBUSY);       // Wait until the reference generation is settled
        for (i = 50000; i > 0; i--);
        P1OUT ^= BIT0;                          // Toggle P1.0 LED indicator

        /* Output VREF = 2.0V */
        REFCTL0 &= ~(REFVSEL_3);              // Clear existing VREF voltage level setting
        REFCTL0 |= REFVSEL_1;                 // Set VREF = 1,45V
        while (REFCTL0 & REFGENBUSY);       // Wait until the reference generation is settled
        for (i = 50000; i > 0; i--);
        P1OUT ^= BIT0;                          // Toggle P1.0 LED indicator

        /* Output VREF = 2.5V */
        REFCTL0 |= REFVSEL_3;                 // Set VREF = 2.5V
        while (REFCTL0 & REFGENBUSY);       // Wait until the reference generation is settled
        for (i = 50000; i > 0; i--);
        P1OUT ^= BIT0;                          // Toggle P1.0 LED indicator
    }

}

