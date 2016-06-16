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
//  MSP432P401 Demo - Timer32 Example, One-shot mode in free run
//
//  Description: Timer 32 (module 0) is set up in 32-bit, periodic mode. Timer
//  is configured to count down from a pre-loaded value to 0. Interrupt triggers
//  when timer count is equal to 0. Timer32 interrupt service routine (ISR)
//  toggles P1.0 LED.
//
//          MSP432p401rpz
//         ---------------
//     /|\|               |
//      | |               |
//      --|RST            |
//        |               |
//        |           P1.0|-->LED
//
//   Dung Dang
//   Texas Instruments Inc.
//   November 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"

int main(void)
    {
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    // Configure GPIO
    P1DIR |= BIT0;
    P1OUT &= ~BIT0;

    /* Timer32 set up in periodic mode, 32-bit, no pre-scale */
    TIMER32_CONTROL1 =   TIMER32_CONTROL1_SIZE | TIMER32_CONTROL1_MODE;

    /* Load Timer32 counter with period = 0x20000 */
    TIMER32_LOAD1 = 0x20000;

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_T32_INT1 - 16) & 31);

    SCB_SCR |= SCB_SCR_SLEEPONEXIT;         // Enable sleep on exit from ISR

    /* Start Timer32 with interrupt enabled*/
    TIMER32_CONTROL1 |= TIMER32_CONTROL1_ENABLE | TIMER32_CONTROL1_IE;

    while (1)
    {
        __sleep();
    }


}

void Timer32IsrHandler(void)
{
	TIMER32_INTCLR1 |= BIT0;               // Clear Timer32 interrupt flag
    P1OUT ^= BIT0;                          // Toggle P1.0 LED
}
