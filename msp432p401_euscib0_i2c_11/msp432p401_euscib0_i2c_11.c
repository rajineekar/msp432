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
//  MSP432P401 Demo - eUSCI_B0 I2C Slave TX multiple bytes to MSP432 Master
//
//  Description: This demo connects two MSP432's via the I2C bus. The master
//  reads from the slave. This is the SLAVE code. The TX data begins at 0
//  and is incremented each time it is sent. A stop condition
//  is used as a trigger to initialize the outgoing data.
//  The USCI_B0 TX interrupt is used to know
//  when to TX.
//  
//
//    *****used with "MSP432P401_euscib0_i2c_10.c"****
//
//                                /|\  /|\
//               MSP432P401      10k  10k     MSP432P401
//                   slave         |    |        master
//             -----------------   |    |   -----------------
//            |     P1.6/UCB0SDA|<-|----|->|P1.6/UCB0SDA     |
//            |                 |  |       |                 | 
//            |                 |  |       |                 |
//            |     P1.7/UCB0SCL|<-|------>|P1.7/UCB0SCL     |
//            |                 |          |                 |
//
//   Wei Zhao
//   Texas Instruments Inc.
//   June 2014
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"

uint8_t TXData;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    // Configure GPIO
    P1SEL0 |= BIT6 | BIT7;                    // I2C pins

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_EUSCIB0 - 16) & 31); // Enable eUSCIB0 interrupt in NVIC module
    
    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 = UCSWRST;                      // Software reset enabled
    UCB0CTLW0 |= UCMODE_3 | UCSYNC;           // I2C mode, sync mode
    UCB0I2COA0 = 0x48 | UCOAEN;               // own address is 0x48 + enable
    UCB0CTLW0 &= ~UCSWRST;                    // clear reset register
    UCB0IE |= UCTXIE0 | UCSTPIE;              // transmit,stop interrupt enable

    SCB_SCR |= SCB_SCR_SLEEPONEXIT;           // Don't wake up on exit from ISR

    __sleep();
    __no_operation();
}

// I2C interrupt service routine
void eUSCIB0IsrHandler(void)
{
    if (UCB0IFG & UCSTPIFG)
    {
    	UCB0IFG &= ~ UCSTPIFG;
        TXData = 0;
        UCB0IFG &= ~UCSTPIFG;                 // Clear stop condition int flag
    }
    if (UCB0IFG & UCTXIFG0)
    {
    	UCB0IFG &= ~ UCTXIFG0;
        UCB0TXBUF = TXData++;
    }  
}


