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
//  MSP432P401 Demo  - eUSCI_B0 I2C Master RX multiple bytes from MSP432 Slave
//
//  Description: This demo connects two MSP432's via the I2C bus. The master
//  reads 5 bytes from the slave. This is the MASTER CODE. The data from the slave
//  transmitter begins at 0 and increments with each transfer.
//  The USCI_B0 RX interrupt is used to know when new data has been received.
//
//    *****used with "MSP432P401_euscib0_i2c_11.c"****
//
//                                /|\  /|\
//               MSP432P401      10k  10k     MSP432P401
//                   slave         |    |        master
//             -----------------   |    |   -----------------
//            |     P1.6/UCB0SDA|<-|----|->|P1.6/UCB0SDA     |
//            |                 |  |       |                 | 
//            |                 |  |       |                 |
//            |     P1.7/UCB0SCL|<-|------>|P1.7/UCB0SCL     |
//            |                 |          |             P1.0|--> LED
//
//   Wei Zhao
//   Texas Instruments Inc.
//   June 2014
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"
#include <stdint.h>

uint8_t RXData = 0;

int main(void)
{
    volatile uint32_t i;
    WDTCTL = WDTPW | WDTHOLD;

    // Configure GPIO
    P1OUT &= ~BIT0;                         // Clear P1.0 output latch
    P1DIR |= BIT0;                          // For LED
    P1SEL0 |= BIT6 | BIT7;                   // I2C pins
    
    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_EUSCIB0 - 16) & 31); // Enable eUSCIB0 interrupt in NVIC module
    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 |= UCSWRST;                   // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCMST | UCSYNC; // I2C mode, Master mode, sync
    UCB0CTLW1 |= UCASTP_2;                  // Automatic stop generated
                                            // after UCB0TBCNT is reached
    UCB0BRW = 0x0018;                       // baudrate = SMCLK / 8
    UCB0TBCNT = 0x0005;                     // number of bytes to be received
    UCB0I2CSA = 0x0048;                     // Slave address
    UCB0CTLW0 &= ~UCSWRST;
    UCB0IE |= UCRXIE | UCNACKIE | UCBCNTIE;

    while (1)
    {

        for (i = 2000; i > 0; i--);
        while (UCB0CTLW0 & UCTXSTP);         // Ensure stop condition got sent
        UCB0CTLW0 |= UCTXSTT;                // I2C start condition

        __sleep();							// Go to LPM0
    }
}

// I2C interrupt service routine
void eUSCIB0IsrHandler(void)
{
    if (UCB0IFG & UCNACKIFG)
    {
    	UCB0IFG &= ~ UCNACKIFG;
    	SCB_SCR |= SCB_SCR_SLEEPONEXIT;     // Don't wake up on exit from ISR
    	UCB0CTLW0 |= UCTXSTT;                  // I2C start condition
    }
    if (UCB0IFG & UCRXIFG0)
    {
    	UCB0IFG &= ~ UCRXIFG0;
    	SCB_SCR &= ~SCB_SCR_SLEEPONEXIT;      // Wake up on exit from ISR
        RXData = UCB0RXBUF;                   // Get RX data
    }
    if (UCB0IFG & UCBCNTIFG)
    {
    	UCB0IFG &= ~ UCBCNTIFG;
    	SCB_SCR |= SCB_SCR_SLEEPONEXIT;     // Don't wake up on exit from ISR
        P1OUT ^= BIT0;                        // Toggle LED on P1.0
    }

}
