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
//  MSP432P401 Demo - RTC in real time clock mode
//
//  Description: This program demonstrates the RTC mode by triggering the event
//  interrupt RTCRDYIE event, which triggers every minute change. P1.1 toggles
//  every minute to indicate this interrupt.
//  After configuring the RTC, device goes into LPM3 and waits for the RTC interrupts.
//  Notice that RTC starting second is set to be 45, so after starting the program
//  the first RTC interrupt should trigger after ~15 seconds. Consequent interrupts
//  should occur every minute.
//  This code recommends an external LFXT1 crystal for RTC accuracy.
//  ACLK = LFXT1 = 32768Hz, MCLK = SMCLK = default DCO = 32 x ACLK = 1048576Hz
//
//                MSP432p401rpz
//             -----------------
//        /|\ |              XIN|-
//         |  |                 | 32kHz
//         ---|RST          XOUT|-
//            |                 |
//            |            P1.0 |--> Toggles every minute
//            |                 |
//
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

    /* Configure P1.0 LED */
    P1DIR |= BIT0 ;
    P1OUT &= ~( BIT0 );


    // Configure RTC
    RTCCTL0_H = RTCKEY_H ;                 // Unlock RTC key protected registers
    RTCCTL0_L |= RTCTEVIE ;
    RTCCTL0_L &= ~(RTCTEVIFG);
    RTCCTL1 = RTCBCD | RTCHOLD ;
    // RTC enable, BCD mode, RTC hold
    // enable RTC read ready interrupt
    // enable RTC time event interrupt

    RTCYEAR = 0x2010;                         // Year = 0x2010
    RTCMON = 0x4;                             // Month = 0x04 = April
    RTCDAY = 0x05;                            // Day = 0x05 = 5th
    RTCDOW = 0x01;                            // Day of week = 0x01 = Monday
    RTCHOUR = 0x10;                           // Hour = 0x10
    RTCMIN = 0x32;                            // Minute = 0x32
    RTCSEC = 0x45;                            // Seconds = 0x45

    RTCADOWDAY = 0x2;                         // RTC Day of week alarm = 0x2
    RTCADAY = 0x20;                           // RTC Day Alarm = 0x20
    RTCAHOUR = 0x10;                          // RTC Hour Alarm
    RTCAMIN = 0x23;                           // RTC Minute Alarm

    RTCCTL1 &= ~(RTCHOLD);                    // Start RTC calendar mode

    RTCCTL0_H = 0;                            // Lock the RTC registers


    /* Enable all SRAM bank retentions prior to going to LPM3 (Deep-sleep) */
    SYSCTL_SRAM_BANKRET |= SYSCTL_SRAM_BANKRET_BNK7_RET;  

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_RTC_C - 16) & 31);


    SCB_SCR |= SCB_SCR_SLEEPONEXIT;        // Sleep on exit from ISR

      while (1)
      {
      
          /* Setting the sleep deep bit */
          SCB_SCR |= (SCB_SCR_SLEEPDEEP);

          __sleep();


          /* Clearing the sleep deep bit */
          SCB_SCR &= ~(SCB_SCR_SLEEPDEEP);

      }

}

// RTC interrupt service routine

void RtcIsrHandler(void)
{

    if (RTCCTL0 & RTCTEVIFG)
    {
        P1OUT ^= BIT0;
        RTCCTL0_H = RTCKEY_H ;
        RTCCTL0_L &= ~RTCTEVIFG;
        RTCCTL0_H = 0;
    }

}

