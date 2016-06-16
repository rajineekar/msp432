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
//  MSP432P401 Demo - Timer_A3, Toggle P1.0;P7.3,P2.4-5, Cont. Mode ISR, 32kHz ACLK
//
//  Description: Use Timer0_A3 CCRx units and overflow to generate four
//  independent timing intervals. For demonstration, TA0CCR0, TA0CCR1 and TA0CCR2
//  output units are optionally selected with port pins P7.3, P2.4 and P2.5
//  in toggle mode. As such, these pins will toggle when respective TA0CCRx
//  registers match the TA0R counter. Interrupts are also enabled with all
//  TA0CCRx units, software loads offset to next interval only - as long as the
//  interval offset is added to TA0CCRx, toggle rate is generated in hardware.
//  Timer0_A1 overflow ISR is used to toggle P1.0 with software. Proper use
//  of the TA0IV interrupt vector generator is demonstrated.
//  ACLK = TACLK = 32kHz, MCLK = SMCLK = default DCO ~3MHz
//  //* An external watch crystal on XIN XOUT is required for ACLK *//
//
//  As coded and with TA0CLK = 32768Hz, toggle rates are:
//  P1.1 = TA0CCR0 = 32768/(2*4) = 4096Hz
//  P1.2 = TA0CCR1 = 32768/(2*16) = 1024Hz
//  P1.3 = TA0CCR2 = 32768/(2*100) = 163.84Hz
//  P1.0 = overflow = 32768/(2*65536) = 0.25Hz
//
//              MSP432P401
//            -----------------
//        /|\|                 |
//         | |                 |
//         --|RST              |
//           |                 |
//           |       P7.3/TA0.0|--> TA0CCR0
//           |       P2.4/TA0.1|--> TA0CCR1
//           |       P2.5/TA0.2|--> TA0CCR2
//           |             P1.0|--> Overflow/software
//
//
//
//   Wei Zhao
//   Texas Instruments Inc.
//   June 2014
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                 // Stop WDT

    // Configure GPIO
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;
    
    P7DIR |= BIT3;                             // P7.3 option selection TA0.0
    P7SEL0 |= BIT3;
    
    P2DIR |= BIT4 | BIT5;                      // P2.4-5 option selection TA0.1-2
    P2SEL0 |= BIT4 | BIT5;

    // Configure Timer_A
    TA0CCTL0 = OUTMOD_4 + CCIE;                // TA0CCR0 toggle, interrupt enabled
    TA0CCTL1 = OUTMOD_4 + CCIE;                // TACCR1 toggle, interrupt enabled
    TA0CCTL2 = OUTMOD_4 + CCIE;                // TACCR2 toggle, interrupt enabled
    TA0CTL = TASSEL_1 + MC_2 + TAIE;           // ACLK, cont mode, interrupt enabled

    SCB_SCR |= SCB_SCR_SLEEPONEXIT;           // Enable sleep on exit from ISR

    __enable_interrupt();
    NVIC_ISER0 |= 1 << ((INT_TA0_N - 16) & 31);
    NVIC_ISER0 |= 1 << ((INT_TA0_0 - 16) & 31);

    __sleep();

    __no_operation();                         // For debugger

}

// Timer A0 interrupt service routine
void TimerA0_0IsrHandler(void)
{
    TA0CCTL0 &= ~CCIFG;
    TA0CCR0 += 4;                  // Add Offset to TA0CCR0

}

void TimerA0_NIsrHandler(void)
{
    if(TA0CCTL1&CCIFG)
    {
        TA0CCTL1 &= ~CCIFG;
        TA0CCR1 += 16;                  // Add Offset to TA0CCR1
    }
    if(TA0CCTL2&CCIFG)
    {
        TA0CCTL2 &= ~CCIFG;
        TA0CCR2 += 100;                  // Add Offset to TA0CCR2
    }
    if(TA0CTL&TAIFG)
    {
        TA0CTL &= ~TAIFG;
        P1OUT ^= BIT0;                 // Timer0_A3 overflow
    }

}


