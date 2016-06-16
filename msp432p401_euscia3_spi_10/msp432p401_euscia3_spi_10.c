/* --COPYRIGHT--,BSD_EX
 * Copyright (c) 2014, Texas Instruments Incorporated
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
//   MSP432P401 Demo - eUSCI_A3, SPI 3-Wire Slave Data Echo
//
//   Description: SPI slave talks to SPI master using 3-wire mode. Data received
//   from master is echoed back.
//   
//   Note: Ensure slave is powered up before master to prevent delays due to
//   slave init.
//
//
//                   MSP432P401R
//                 -----------------
//            /|\ |                 |
//             |  |                 |
//            -|->|                 |
//                |                 |
//                |             P9.7|<- Data In (UCA3SIMO)
//                |                 |
//                |             P9.6|-> Data Out (UCA3SOMI)
//                |                 |
//                |             P9.5|-> Serial Clock Out (UCA3CLK)
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
    WDTCTL = WDTPW|WDTHOLD;                   // Stop watchdog timer

    P9SEL0 |= BIT5 | BIT6 | BIT7;             // set 3-SPI pin as second function

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_EUSCIA3 - 16) & 31); // Enable eUSCIA3 interrupt in NVIC module

    UCA3CTLW0 |= UCSWRST;                     // **Put state machine in reset**
    UCA3CTLW0 |= UCSYNC|UCCKPL|UCMSB;         // 3-pin, 8-bit SPI slave
                                              // Clock polarity high, MSB
    UCA3CTLW0 |= UCSSEL__ACLK;                // ACLK
    UCA3BR0 = 0x01;                           // /2,fBitClock = fBRCLK/(UCBRx+1).
    UCA3BR1 = 0;                              //
    UCA3MCTLW = 0;                            // No modulation
    UCA3CTLW0 &= ~UCSWRST;                    // **Initialize USCI state machine**
    UCA3IE |= UCRXIE;                         // Enable USCI_A3 RX interrupt

    __sleep();
}

// SPI interrupt service routine
void eUSCIA3IsrHandler(void)
{
    if (UCA3IFG & UCRXIFG)
    {
    while (!(UCA3IFG&UCTXIFG));               // USCI_A3 TX buffer ready?
    UCA3TXBUF = UCA3RXBUF;                    // Echo received data
    }
}
