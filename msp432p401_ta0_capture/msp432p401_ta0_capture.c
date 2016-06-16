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
//***************************************************************************************
//  MSP432P401 Demo - Timer0_A3 Capture of ACLK
//
//  Description; Capture a number of periods of the ACLK clock and store them in an array.
//  When the set number of periods is captured the program is trapped and the LED on
//  P1.0 is toggled. At this point halt the program execution read out the values using
//  the debugger.
//  ACLK = REFOCLK = 32kHz, MCLK = SMCLK = default DCODIV = 3MHz.
//
//                MSP432P401
//             -----------------
//         /|\|                 |
//          | |             P2.5|<-- TA0.CCI2A
//          --|RST              |  |
//            |             P4.2|--> ACLK
//            |                 |
//            |             P1.0|-->LED
//
//
//   Wei Zhao
//   Texas Instruments Inc.
//   June 2014
//   Built with Code Composer Studio V6.0
//***************************************************************************************
#include "msp.h"
#include <stdint.h>

#define NUMBER_TIMER_CAPTURES        20

volatile uint16_t timerAcaptureValues[NUMBER_TIMER_CAPTURES];
uint16_t timerAcapturePointer = 0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                       // Stop watchdog timer

    // Configure GPIO
    P1DIR |= BIT0;                                  // Set P1.0 as output
    P1OUT |= BIT0;                                  // P1.0 high
    P2SEL0 |= BIT5;                                 // TA0.CCI2A input capture pin, second function
    P2DIR &= ~BIT5;
    P4SEL0 |= BIT2;                                 // Set as ACLK pin, second function
    P4DIR |= BIT2;

    CSKEY = 0x695A;                                 // Unlock CS module for register access
    // Select ACLK = REFO, SMCLK = MCLK = DCO
    CSCTL1 = SELA_2 | SELS_3 | SELM_3;
    CSKEY = 0;                                      // Lock CS module from unintended accesses


    // Timer0_A3 Setup
    TA0CCTL2 |= CM_1 | CCIS_0 | CCIE | CAP | SCS;
                                                    // Capture rising edge,
                                                    // Use CCI2A=ACLK,
                                                    // Synchronous capture,
                                                    // Enable capture mode,
                                                    // Enable capture interrupt

    TA0CTL |= TASSEL_2 | MC_2 | TACLR;              // Use SMCLK as clock source, clear TA0R
                                                    // Start timer in continuous mode
    SCB_SCR |= SCB_SCR_SLEEPONEXIT;                 // Enable sleep on exit from ISR
    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_TA0_N - 16) & 31);

    __sleep();
    __no_operation();
}

// Timer A0 interrupt service routine
void TimerA0_NIsrHandler(void)
{
    volatile uint32_t i;
    timerAcaptureValues[timerAcapturePointer++] = TA0CCR2;
    if (timerAcapturePointer >= 20)
    {
        while (1) 
        {
            P1OUT ^= 0x01;                         // Toggle P1.0 (LED)
            for (i = 30000; i > 0; i--);
        }
    } 
}
