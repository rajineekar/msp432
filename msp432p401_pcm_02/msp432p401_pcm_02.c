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
//   MSP432P401 Demo - Enter LPM3 with ACLK = REFO
//
//  Description: MSP432 device is configured to enter LPM3 mode with GPIOs properly 
//  terminated. P1.1 is configured as an input. Pressing the button connect to P1.1
//  results in device waking up and servicing the Port1 ISR. LPM3 current can be 
//  measured when P1.0 is output low (e.g. LED off). 
// 
//  ACLK = 32kHz, MCLK = SMCLK = default DCO
//
//
//               MSP432p401rpz
//            -----------------
//        /|\|                 |
//         | |                 |
//         --|RST              |
//     /|\   |                 |
//      --o--|P1.1         P1.0|-->LED
//     \|/
//
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************

#include "msp.h"
int main(void)
{   
    /* Hold the watchdog */
    WDTCTL = WDTPW | WDTHOLD;
    /* Configuring P1.0 as output and P1.1 (switch) as input with pull-up resistor*/
    /* Rest of pins are configured as output low */
    /* Notice intentional '=' assignment since all P1 pins are being deliberately configured */
    P1DIR = ~(BIT1);
    P1OUT = BIT1;
    P1REN = BIT1;                           // Enable pull-up resistor (P1.1 output high)
    P1SEL0 = 0;
    P1SEL1 = 0;
    P1IFG = 0;                              // Clear all P1 interrupt flags
    P1IE = BIT1;                            // Enable interrupt for P1.1
    P1IES = BIT1;                               // Interrupt on high-to-low transition

    // Enable Port 1 interrupt on the NVIC
    NVIC_ISER1 = 1 << ((INT_PORT1 - 16) & 31);

    // Terminate all remaining pins on the device
    P2DIR |= 0xFF; P2OUT = 0;
    P3DIR |= 0xFF; P3OUT = 0;
    P4DIR |= 0xFF; P4OUT = 0;
    P5DIR |= 0xFF; P5OUT = 0;
    P6DIR |= 0xFF; P6OUT = 0;
    P7DIR |= 0xFF; P7OUT = 0;
    P8DIR |= 0xFF; P8OUT = 0;
    P9DIR |= 0xFF; P9OUT = 0;
    P10DIR |= 0xFF; P10OUT = 0;

    /* Configure Port J */
    PJDIR |= (BIT2 | BIT3); PJOUT &= ~(BIT2 | BIT3);

    /* PJ.0 & PJ.1 configured for XT1 */
    PJSEL0 |= BIT0 | BIT1;
    PJSEL1 &= ~(BIT0 | BIT1);

    /* Starting LFXT in non-bypass mode without a timeout. */
    CSKEY = CSKEY_VAL;
    CSCTL1 &= ~(SELA_M | SELB);
    CSCTL1 |= SELA__LFXTCLK;                // Source LFXTCLK to ACLK & BCLK
    CSCTL2 &= ~(LFXTDRIVE_M);               // Configure to lowest drive-strength    
    CSCTL2 |= LFXT_EN;
    while (CSIFG & LFXTIFG)
        CSCLRIFG |= LFXTIFG;
    CSKEY = 0;
    /* Turn off PSS high-side & low-side supervisors */
    PSSKEY = PSS_KEY_KEY_VAL;
    PSSCTL0 |= SVSMHOFF | SVSLOFF;
    PSSKEY = 0;

    /* Enable PCM rude mode, which allows to device to enter LPM3 without waiting for peripherals */
    PCMCTL1 = PCM_CTL_KEY_VAL | FORCE_LPM_ENTRY;


    /* Enable all SRAM bank retentions prior to going to LPM3  */
    SYSCTL_SRAM_BANKRET |= SYSCTL_SRAM_BANKRET_BNK7_RET;
    __enable_interrupt();
    SCB_SCR |= SCB_SCR_SLEEPONEXIT;        // Do not wake up on exit from ISR


    /* Setting the sleep deep bit */
    SCB_SCR |= (SCB_SCR_SLEEPDEEP);
    /* Go to LPM3 */
    __sleep();

}

/* Port1 ISR */
void Port1Handler(void)
{
    volatile uint32_t i, status;
    /* Toggling the output on the LED */
    if(P1IFG & BIT1)
        P1OUT ^= BIT0;

    /* Delay for switch debounce */
    for(i = 0; i < 10000; i++)

    P1IFG &= ~BIT1;

}