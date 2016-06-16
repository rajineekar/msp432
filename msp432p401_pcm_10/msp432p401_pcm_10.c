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
//   MSP432P401 Demo - Enter LPM3.5 mode
//
//   Description: Go to LPM3.5 mode with port pin interrupt enabled.
//   For LPM3.5 with RTC operation, refer to msp432p401_rtc_0x code example.
//
//                MSP432P401R
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |            P6.7 |<---- GPIO
//            |                 |
//
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"


int main(void) {


    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT


    // Configure GPIO
    P6OUT |= BIT7;                          // Pull-up resistor on P6.6
    P6REN |= BIT7;                          // Select pull-up mode for P6.7
    P6DIR &= ~BIT7;                         // Set all but P6.7 to output direction
    P6IES |= BIT7;                          // P6.7 Hi/Lo edge
    P6IFG = 0;                              // Clear all P6 interrupt flags
    P6IE |= BIT7;                           // P6.7 interrupt enabled
    
    /* Enable all SRAM bank retentions prior to going to LPM3 (Deep-sleep) */
    SYSCTL_SRAM_BANKRET |= SYSCTL_SRAM_BANKRET_BNK7_RET;  
    __enable_interrupt();
    NVIC_ISER1 = 1 << ((INT_PORT6 - 16) & 31);
    

    /* Setting the sleep deep bit */
    SCB_SCR |= (SCB_SCR_SLEEPDEEP);

    PCMCTL0 = PCM_CTL_KEY_VAL | LPMR__LPM35; 

    /* Entering LPM3.5 with GPIO interrupt enabled */
    __sleep();
    __no_operation();                   // For debugger

}

/* Port 6 interrupt service routine */
void Port6IsrHandler(void) {
    P6IES ^= BIT7;                          // Toggle interrupt edge
    P6IFG &= ~BIT7;                         // Clear P1.1 IFG

}
