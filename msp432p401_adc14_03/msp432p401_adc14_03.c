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
//  MSP432P401 Demo- ADC14, Sample A2-A3 Differential,Set P1.0 if i/p> 1V
//
//   Description: A single sample is made on differential channel A2-A3
//   with reference to AVcc.
//   A resistor ladder is used to provide two different voltage inputs one at
//   A2 and the other at A3. Since equal resistors are used, the input voltage
//   at A2 and A3 are 2/3Vin and 1/3Vin respectively.
//   Vin is varied from 1V to 3V. When Vin = 3V, A2 = 2V and A3 = 1V providing
//   a differential voltage of 1V across the ADC input.
//   If A2-A3 >= 1V, P1.0 set, else reset. The full, correct handling of
//   and ADC14 interrupt is shown as well.
//
//
//                msp432p401
//  Vin= 1V-3V -----------------
//     |      |                 |
//    10K     |                 |
//     |      |                 |
//     |      |                 |
//     -------| A2=2/3Vin       |
//     |      |                 |
//     10K    |                 |
//     |      |                 |
//     -------| A3=1/3Vin       |
//     |      |                 |
//    10K     |             P1.0|-->LED
//     |      |                 |
//    GND
//
//   Dung Dang
//   Texas Instruments Inc.
//   November 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"
#include "stdint.h"

int main(void) {
	volatile unsigned int i;
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

    // GPIO Setup
    P1OUT &= ~BIT0;                           // Clear LED to start
    P1DIR |= BIT0;                            // Set P1.0/LED to output
    P5SEL1 |= BIT3 | BIT2;                    // Configure P5.3/2 (A2/3) for ADC
    P5SEL0 |= BIT3 | BIT2;

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_ADC14 - 16) & 31);         // Enable ADC interrupt in NVIC module

    // Configure ADC14
    ADC14CTL0 = ADC14SHT0_2 | ADC14SHP | ADC14ON;          // Sampling time, S&H=16, ADC14 on
    ADC14CTL1 = ADC14RES_2;                   // Use sampling timer, 12-bit conversion results
    ADC14MCTL0 |= ADC14INCH_2 | ADC14DIF;     // A2 ADC input in dif. mode select; Vref=AVCC
    ADC14IER0 |= ADC14IE0;                    // Enable ADC conv complete interrupt

    SCB_SCR &= ~SCB_SCR_SLEEPONEXIT;           // Wake up on exit from ISR

    while (1)
    {
    	for (i = 20000; i > 0; i--);           // Delay
      ADC14CTL0 |= ADC14ENC | ADC14SC;         // Start sampling/conversion

      __sleep();

      __no_operation();                        // For debugger
    }
}

// ADC14 interrupt service routine

void ADC14IsrHandler(void) {
    if (ADC14MEM0 >= 0x07FF)                   // ADC12MEM0 = A1 > 0.5AVcc?
      P1OUT |= BIT0;                           // P1.0 = 1
    else
      P1OUT &= ~BIT0;                          // P1.0 = 0
}

