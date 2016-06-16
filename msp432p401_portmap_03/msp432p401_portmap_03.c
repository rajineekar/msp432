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
//   MSP432P401 Demo - Port Map single function to multiple pins;
//      Multiple runtime configurations
//
// Description:  The port mapping of TIMERB0 is changed with each WDT interrupt.
//               TimerA0 provides 4 PWMs with 4 different duty cycles.
//               TimerA0 is sourced from ACLK.  ACLK is REFO, 32kHz
//
//                 MSP432p401rpz
//             ------------------------
//         /|\|                        |
//          | |                        |
//          --|RST                     |
//            |     P2.0(TB0CCR1,2,3,4)|--> 25,50,75,87.5%
//            |     P2.1(TB0CCR1,2,3,4)|--> 25,50,75,87.5%
//            |     P2.2(TB0CCR1,2,3,4)|--> 25,50,75,87.5%
//            |     P2.3(TB0CCR1,2,3,4)|--> 25,50,75,87.5%
//            |     P2.4(TB0CCR1,2,3,4)|--> 25,50,75,87.5%
//            |     P2.5(TB0CCR1,2,3,4)|--> 25,50,75,87.5%
//            |     P2.6(TB0CCR1,2,3,4)|--> 25,50,75,87.5%
//            |     P2.7(TB0CCR1,2,3,4)|--> 25,50,75,87.5%
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"
#include "stdint.h"

#define PORT_MAP_RECFG                      // Multiple runtime Port Map configurations



/* Port2 Port Mapping definitions */
const uint8_t PortSequence[4] = { PM_TA0CCR1A, PM_TA0CCR2A, PM_TA0CCR3A, PM_TA0CCR4A };
uint8_t count=0;

void Port_Mapping(void)
{
    uint8_t i;
    volatile uint8_t *ptr;

    PMAPKEYID = PMAP_KEYID_VAL;                      // Enable Write-access to modify port mapping registers

    #ifdef PORT_MAP_RECFG
    PMAPCTL = PMAPRECFG;                    // Allow reconfiguration during runtime
    #endif

    ptr = (volatile uint8_t *) (&P2MAP01);
    for (i = 0; i < 8; i++)
    {
        *ptr = PortSequence[count];
        ptr++;
    }

    PMAPKEYID = 0;                            // Disable Write-Access to modify port mapping registers

}





int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    Port_Mapping();

    /* Setup Port Pins */
    P2DIR |= 0xFF;                          // P2.0 - P2.7 output
    P2SEL0 |= 0xFF;                         // P2.0 - P2.6 Port Map functions
    P2SEL1 = 0;                             // P2.0 - P2.6 Port Map functions

    /* Setup TA0 */
    TA0CCR0 = 256;                          // PWM Period/2
    TA0CCTL1 = OUTMOD_6;                    // CCR1 toggle/set
    TA0CCR1 = 192;                          // CCR1 PWM duty cycle
    TA0CCTL2 = OUTMOD_6;                    // CCR2 toggle/set
    TA0CCR2 = 128;                          // CCR2 PWM duty cycle
    TA0CCTL3 = OUTMOD_6;                    // CCR3 toggle/set
    TA0CCR3 = 64;                           // CCR3 PWM duty cycle
    TA0CCTL4 = OUTMOD_6;                    // CCR4 toggle/set
    TA0CCR4 = 32;                           // CCR4 PWM duty cycle
    TA0CTL = TASSEL_1 + MC_3;               // ACLK, up-down mode

    SCB_SCR &= ~SCB_SCR_SLEEPONEXIT;        // Wake up after ISR

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_WDT_A - 16) & 31);

    WDTCTL = WDTIS_4;                   // WDT 1s, ACLK, interval timer

    while (1)
    {
        /* Go to LPM0 mode */
        __sleep();
        __no_operation();                       // For debugger
        count++;
        if(count==4)
          count = 0;
        Port_Mapping();

    }
}


/* Watchdog Timer interrupt service routine */
void WdtISR(void)
{
    /* Do nothing, just need to wake up to main */
}


