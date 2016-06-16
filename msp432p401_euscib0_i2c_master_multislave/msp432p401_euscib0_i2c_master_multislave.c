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
//  MSP432P401 Demo - eUSCI_B0 I2C Master TX bytes to Multiple Slaves
//
//  Description: This demo connects two MSP432's via the I2C bus.
//  The master transmits to 4 different I2C slave addresses 0x0A,0x0B,0x0C&0x0D.
//  Each slave address has a specific related data in the array TXData[].
//  At the end of four I2C transactions the slave address rolls over and begins
//  again at 0x0A.
//  
//  Use with msp432p401_euscia0_i2c_multislave.c
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
#include <stdint.h>

uint8_t TXData[]= {0xA1,0xB1,0xC1,0xD1};        // Pointer to TX data
uint8_t SlaveAddress[]= {0x0A,0x0B,0x0C,0x0D};
uint8_t TXByteCtr;
uint8_t SlaveFlag = 0;

int main(void)
{
    volatile uint32_t i;
    WDTCTL = WDTPW | WDTHOLD;                         // Stop watchdog timer

    // Configure Pins for I2C
    P1SEL0 |= BIT6 | BIT7;                            // I2C pins

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_EUSCIB0 - 16) & 31); // Enable eUSCIB0 interrupt in NVIC module

    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 |= UCSWRST;                             // put eUSCI_B in reset state
    UCB0CTLW0 |= UCMODE_3 | UCMST;                    // I2C master mode, SMCLK
    UCB0BRW = 0x0018;                                 // baudrate = SMCLK /24
    UCB0CTLW0 &=~ UCSWRST;                            // clear reset register
    UCB0IE |= UCTXIE0 | UCNACKIE;                     // transmit and NACK interrupt enable

    SlaveFlag =0;

    while(1)
    {
    SCB_SCR |= SCB_SCR_SLEEPONEXIT;                   // Don't wake up on exit from ISR
    for (i = 1000; i > 0; i--);                       // Delay between transmissions
    UCB0I2CSA = SlaveAddress[SlaveFlag];              // configure slave address
    TXByteCtr = 1;                                    // Load TX byte counter
    while (UCB0CTLW0 & UCTXSTP);                      // Ensure stop condition got sent
    UCB0CTLW0 |= UCTR | UCTXSTT;                      // I2C TX, start condition

    __sleep();
    __no_operation();
    // Change Slave address
    SlaveFlag++;
    if (SlaveFlag>3)                                  // Roll over slave address
      {
        SlaveFlag =0;
      }
    }
}

// I2C interrupt service routine
void eUSCIB0IsrHandler(void)
{
    if (UCB0IFG & UCNACKIFG)
    {
    	UCB0IFG &= ~ UCNACKIFG;
        UCB0CTL1 |= UCTXSTT;                  // I2C start condition
    }
    if (UCB0IFG & UCTXIFG0)
    {
    	UCB0IFG &= ~ UCTXIFG0;
        if (TXByteCtr)                                // Check TX byte counter
           {
            UCB0TXBUF = TXData[SlaveFlag];            // Load TX buffer
            TXByteCtr--;                              // Decrement TX byte counter
           }
        else
           {
            UCB0CTLW0 |= UCTXSTP;                     // I2C stop condition
            UCB0IFG &= ~UCTXIFG;                      // Clear USCI_B0 TX int flag
            SCB_SCR &= ~SCB_SCR_SLEEPONEXIT;          // Wake up on exit from ISR
           }
    }
}

