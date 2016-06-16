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
//   MSP432P401 Demo - WDT, Toggle P1.0, Interval Overflow ISR, 32kHz ACLK
//
//  Description:  Toggle P1.0 using software timed by WDT ISR. Toggle rate is
//  exactly 250ms based on 32kHz ACLK WDT clock source. In this example the
//  WDT is configured to divide 32768 watch-crystal- with an ISR
//  triggered @ 4Hz = [WDT CLK source/32768].
//  ACLK = LFXT1 = 32.768kHz , MCLK = SMCLK = DCO ~8MHz
//
//               MSP432p401rpz
//             -----------------
//         /|\|                 |
//          | |              XIN|
//          --|RST              |  32KHz Crystal
//            |             XOUT|
//            |             P1.0|-->LED
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"

int main(void)
{
    
    WDTCTL = WDTHOLD | WDTPW;
    // Configure GPIO
    P1DIR |= BIT0;                            // Set P1.0 to output direction
    // Configure GPIO
    P4DIR |= BIT2;                            // P4.2 output
    P4SEL0 |= BIT2;                           // P4.2 option select
    
    PJSEL0 |= BIT0 | BIT1;                    // set LFXT pin as second function
    
    CSKEY = 0x695A;                           // Unlock CS module for register access
    CSCTL2 |= LFXT_EN;                         // LFXT on
    // Loop until XT1, XT2 & DCO fault flag is cleared
    do
    {
        // Clear XT2,XT1,DCO fault flags
       CSCLRIFG |= CLR_DCORIFG | CLR_HFXTIFG | CLR_LFXTIFG;
       SYSCTL_NMI_CTLSTAT &= ~ SYSCTL_NMI_CTLSTAT_CS_SRC;
    } while (SYSCTL_NMI_CTLSTAT & SYSCTL_NMI_CTLSTAT_CS_FLG); // Test oscillator fault flag
    
	CSCTL1 = CSCTL1 & ~(SELS_M | DIVS_M) | SELA_0; // Select ACLK as LFXTCLK
    CSKEY = 0;                                // Lock CS module from unintended accesses

    SCB_SCR &= ~SCB_SCR_SLEEPONEXIT;        // Wake up after ISR

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_WDT_A - 16) & 31);

    WDTCTL = WDTIS_4;                    // WDT 250ms, ACLK, interval timer
    while (1)
    {
        /* Go to LPM0 mode */
        __sleep();
        __no_operation();                       // For debugger
    }
}


/* Watchdog Timer interrupt service routine */
void WdtIsrHandler(void)
{
    P1OUT ^= BIT0;                          // Toggle P1.0 LED
}


