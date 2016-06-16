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
//*******************************************************************************
//  MSP432P401 Demo - Timer1_A3, PWM TA1.1-2, Up Mode, 32KHz ACLK
//
//  Description: This program generates two PWM outputs on P1.2,P1.3 using
//  Timer1_A configured for up mode. The value in CCR0, 100-1, defines the PWM
//  period and the values in CCR1 and CCR2 the PWM duty cycles. Using 32768Hz
//  ACLK as TACLK, the timer period is ~3ms with a 75% duty cycle on P4.0
//  and 25% on P8.3.
//  ACLK = TACLK =XT1~32768Hz, SMCLK = MCLK = 3MHz
//
//
//          MSP432P401
//         ---------------
//     /|\|            XIN|--
//      | |               |  ~32768Hz
//      --|RST        XOUT|--
//        |               |
//        |     P7.7/TA1.1|--> CCR1 - 75% PWM
//        |     P7.6/TA1.2|--> CCR2 - 25% PWM
//
//  Wei Zhao
//  Texas Instruments Inc.
//  Jan 2014
//  Built with IAR Embedded Workbench v6.10 & Code Composer Studio v6.0
//******************************************************************************
#include "msp.h"

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                         // Stop WDT
    
    // Configure GPIO
    P7DIR |= BIT6 | BIT7;                            // P7.6~7 set TA1.1~2
    P7SEL0 |= BIT6 | BIT7; 
    
    // Initialize LFXT1
    PJSEL0 |= BIT0 | BIT1;                 // Select for LFXT ports
    CSKEY = 0x695A;                        // Unlock CS module for register access
    CSCTL2 |= LFXT_EN;

    // Loop until XT1, XT2 & DCO fault flag is cleared
    do
    {
        // Clear XT2,XT1,DCO fault flags
       CSCLRIFG |= CLR_DCORIFG | CLR_HFXTIFG | CLR_LFXTIFG;
       SYSCTL_NMI_CTLSTAT &= ~ SYSCTL_NMI_CTLSTAT_CS_SRC;
    } while (SYSCTL_NMI_CTLSTAT & SYSCTL_NMI_CTLSTAT_CS_FLG);              // Test oscillator fault flag

    CSKEY = 0;                              // Lock CS module from unintended accesses
    
    // Setup Timer1_A
    TA1CCR0 = 100-1;                                  // PWM Period
    TA1CCTL1 = OUTMOD_7;                              // CCR1 reset/set
    TA1CCR1 = 75;                                     // CCR1 PWM duty cycle
    TA1CCTL2 = OUTMOD_7;                              // CCR2 reset/set
    TA1CCR2 = 25;                                     // CCR2 PWM duty cycle
    TA1CTL = TASSEL_1 | MC_1 | TACLR;                 // ACLK, up mode, clear TAR
    
    __sleep();
    __no_operation();                                 // For debugger
}
