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
//   MSP432P401 Demo - - RTC, LPM3.5, & alarm
//
//   Description: The RTC module is used to set the time, start RTC operation,
//   and read the time from the respective RTC registers. Software will set the
//   original time to 11:59:45 am on Friday October 7, 2011. Then the RTC will
//   be activated through software, and an alarm will be created for the next
//   minute (12:00:00 pm). The device will then enter LPM3.5 awaiting
//   the event interrupt. Upon being woken up by the event, the LED on the board
//   will be set.
//
//   NOTE: To ensure that LPM3.5 is entered properly, you would need to use an
//   external power supply.
//
//  //* An external watch crystal on XIN XOUT is required for ACLK *//
//   ACLK = 32.768kHz, MCLK = SMCLK = default DCO~1MHz
//
//                MSP432p401rpz
//
//             -----------------
//         /|\|              XIN|-
//          | |                 | 32kHz
//          --|RST          XOUT|-
//            |                 |
//            |             P1.0|--> LED
//
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"

int main(void)
{
    volatile unsigned int i;

    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    // GPIO Port Configuration for lowest power configuration
    P1OUT = 0x00; P2OUT = 0x00; P3OUT = 0x00; P4OUT = 0x00; P5OUT = 0x00; P6OUT = 0x00;
    P7OUT = 0x00; P8OUT = 0x00; P9OUT = 0x00; PJOUT = 0x00; P10OUT = 0x00;
    P1DIR = 0xFF; P2DIR = 0xFF; P3DIR = 0xFF; P4DIR = 0xFF; P5DIR = 0xFF; P6DIR = 0xFF;
    P7DIR = 0xFF; P8DIR = 0xFF; P9DIR = 0xFF; PJDIR = 0xFF; P10DIR = 0xFF;

    // Initialize LFXT1
    PJSEL0 |= BIT0 | BIT1;                 // Select for LFXT ports
    CSKEY = 0x695A;                        // Unlock CS module for register access
    CSCTL2 |= LFXT_EN;

    // Loop until XT1, XT2 & DCO fault flag is cleared
    do
    {
        // Clear XT2,XT1,DCO fault flags
       CSCLRIFG &= ~(CLR_DCORIFG | CLR_HFXTIFG | CLR_LFXTIFG);
       SYSCTL_NMI_CTLSTAT &= ~ SYSCTL_NMI_CTLSTAT_CS_SRC;
    } while (SYSCTL_NMI_CTLSTAT & SYSCTL_NMI_CTLSTAT_CS_FLG);              // Test oscillator fault flag

    CSKEY = 0;                              // Lock CS module from unintended accesses

    // Configure RTC_C
    RTCCTL0 = RTCKEY_VAL;           // Unlock RTC key protected registers
    RTCCTL0_L |= RTCTEVIE;                  // Enable RTC time event interrupt
    RTCCTL1 |= RTCBCD | RTCTEV_0 | RTCHOLD; // BCD mode, RTC hold, Set RTCTEV for 1 minute alarm
                                            // event interrupt
    RTCYEAR = 0x2011;                       // Year = 0x2011
    RTCMON = 0x10;                          // Month = 0x10 = October
    RTCDAY = 0x07;                          // Day = 0x07 = 7th
    RTCDOW = 0x05;                          // Day of week = 0x05 = Friday
    RTCHOUR = 0x11;                         // Hour = 0x11
    RTCMIN = 0x59;                          // Minute = 0x59
    RTCSEC = 0x45;                          // Seconds = 0x45

    RTCCTL1 &= ~(RTCHOLD);                  // Start RTC calendar mode
    RTCCTL0_H = 0;                          // Lock the RTC registers

     __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_RTC_C - 16) & 31);

    SCB_SCR &= ~SCB_SCR_SLEEPONEXIT;        // Wake up on exit from ISR


    /* Entering LPM3.5 with GPIO interrupt */
    /* Clearing the Power Mode Requests */
    PCMCTL0 = PCM_PMR_KEY_VAL | LPMR__LPM35;

    /* Setting the sleep deep bit */
    SCB_SCR |= (SCB_SCR_SLEEPDEEP);

    __sleep();
    __no_operation();                   // For debugger

    // Code should NOT get here. This means that LPM3.5 was not properly entered.
    // Ensure that an external power supply was ued. Or else JTAG will put the CPU
    // in LPM0 mode.

    // Stop the RTC
    RTCCTL0 = RTCKEY_VAL;           // Unlock RTC_C module
    RTCCTL1 |= RTCHOLD;                     // RTC hold
    RTCCTL0_H = 0;                          // Lock RTC_C module

    // Blink LED slowly if LPM3.5 not entered properly
    while(1)
    {
        P1OUT ^= BIT0;                      // XOR P1.0
        for (i = 500000; i > 0; i--);        // Delay
    }
}

void RtcIsrHandler(void)
{
    volatile unsigned int i;

    if (RTCCTL0 & RTCTEVIFG)
    {
        // If woken up at noon, set LED and stay loop there
        if((RTCHOUR == 0x12)&&(RTCMIN == 0x00)&&(RTCSEC == 0x00))
        {
            P1OUT |= BIT0;                  // Set P1.0 on
            while(1);
        }
        // If values do not match, blink LED fast
        else
        {
            while(1)                        // continuous loop
            {
                P1OUT ^= BIT0;              // XOR P1.0
                for (i = 20000; i > 0; i--);        // Delay
            }
        }
    }
}
