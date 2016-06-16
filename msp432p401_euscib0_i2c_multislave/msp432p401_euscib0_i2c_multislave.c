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
//  MSP432P401 Demo - eUSCI_B0 I2C 4 Hardware I2C slaves
//
//  Description: This demo connects two MSP432's via the I2C bus.
//  This code configures the MSP432 USCI to be addressed as 4 independent I2C
//  slaves. Each slave has its own interrupt flag and data variable to store
//  incoming data.
//  Use with MSP430G6021x_uscib0_i2c_15.c
//  
//
//    *****used with "msp432p401_euscia0_i2c_master_multislave.c"****
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

uint8_t RXData0=0;
uint8_t RXData1=0;
uint8_t RXData2=0;
uint8_t RXData3=0;

int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;                             // Stop watchdog timer

    P1DIR = BIT0|BIT1;
    P1OUT = BIT0|BIT1;

    // Configure Pins for I2C
    P1SEL0 |= BIT6 | BIT7;                                // I2C pins

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_EUSCIB0 - 16) & 31); // Enable eUSCIB0 interrupt in NVIC module

    // Configure USCI_B0 for I2C mode
    UCB0CTLW0 |= UCSWRST;                                 //Software reset enabled
    UCB0CTLW0 |= UCMODE_3;                                //I2C slave mode, SMCLK
    UCB0I2COA0 = 0x0A | UCOAEN;                           //SLAVE0 own address is 0x0A| enable
    UCB0I2COA1 = 0x0B | UCOAEN;                           //SLAVE1 own address is 0x0B| enable
    UCB0I2COA2 = 0x0C | UCOAEN;                           //SLAVE2 own address is 0x0C| enable
    UCB0I2COA3 = 0x0D | UCOAEN;                           //SLAVE3 own address is 0x0D| enable
    UCB0CTLW0 &=~UCSWRST;                                 //clear reset register

    UCB0IE |=  UCRXIE0 | UCRXIE1| UCRXIE2 | UCRXIE3;      //receive interrupt enable

    __sleep();
    __no_operation();
}

// I2C interrupt service routine
void eUSCIB0IsrHandler(void)
{
    if (UCB0IFG & UCRXIFG3)
    {
    	UCB0IFG &= ~ UCRXIFG3;
        RXData3 = UCB0RXBUF;
    }
    if (UCB0IFG & UCRXIFG2)
    {
    	UCB0IFG &= ~ UCRXIFG2;
        RXData2 = UCB0RXBUF;
    } 
    if (UCB0IFG & UCRXIFG1)
    {
    	UCB0IFG &= ~ UCRXIFG1;
        RXData1 = UCB0RXBUF;
    }
    if (UCB0IFG & UCRXIFG0)
    {
    	UCB0IFG &= ~ UCRXIFG0;
        RXData0 = UCB0RXBUF;
    } 
}

