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
//  MSP432P401 Demo - ADC14, Window Comparator, 2.5V ref
//
//  Description; A1 is sampled in single ch/ single conversion mode.
//  Window comparator is used to generate interrupts to
//  indicate when the input voltage goes above the High_Threshold or below the
//  Low_Threshold or is in between the high and low thresholds. TimerB0 is used
//  as an interval timer used to control the LED at P1.0 to toggle slow/fast
//  or turn off according to the ADC12 Hi/Lo/IN interupts.
//
//               MSP432p401rpz
//            -----------------
//        /|\|                 |
//         | |                 |
//         --|RST              |
//           |                 |
//       >---|P5.4/A1     P1.0 |--> LED
//
//   Dung Dang
//   Texas Instruments Inc.
//   November 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"
#include "stdint.h"

#define High_Threshold 0xAAAA         // ~2V
#define Low_Threshold  0x5555         // ~1V

volatile unsigned int SlowToggle_Period = 20000-1;
volatile unsigned int FastToggle_Period = 1000-1;


int main(void) {
	volatile unsigned int i;
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT

    // GPIO Setup
    P1OUT &= ~BIT0;                         // Clear LED to start
    P1DIR |= BIT0;                          // Set P1.0/LED to output
    P5SEL1 |= BIT4;                         // Configure P5.4 for ADC A1
    P5SEL0 |= BIT4;

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_ADC14 - 16) & 31);         // Enable ADC interrupt in NVIC module
    NVIC_ISER0 = 1 << ((INT_TA0_0 - 16) & 31);         // Enable Timer_A0 interrupt in NVIC module

    // Configure internal reference
    while(REFCTL0 & REFGENBUSY);            // If ref generator busy, WAIT
    REFCTL0 |= REFVSEL_3 |REFON;            // Select internal ref = 2.5V
                                            // Internal Reference ON
    for (i = 75; i > 0; i--);               // Delay (~75us) for Ref to settle

    // Configure ADC14
    // tsample = 16ADC14CLK cycles, tconvert = 16 ADC12CLK cycles
    // software trigger for SOC, MODOSC, single ch-single conversion,
    // tsample controlled by SHT0x settings
    // Channel 1, reference = internal, enable window comparator
    // Set thresholds for ADC14 interrupts
    // Enable Interrupts
    ADC14CTL0 = ADC14SHT0_2 | ADC14SHP | ADC14ON;
    ADC14CTL1 = ADC14SHS_0 | ADC14SSEL_0 | ADC14CONSEQ_0 | ADC14RES_3;

    ADC14MCTL0 |= ADC14INCH_1 | ADC14VRSEL_1 | ADC14WINC;
    ADC14HI0 = High_Threshold;
    ADC14LO0 = Low_Threshold;
    ADC14IER1 |= ADC14HIIE | ADC14LOIE | ADC14INIE;



    // Configure TA0 period-timer for LED toggle
    TA0CCTL0 = CCIE;                          // CCR0 interrupt enabled
    TA0CTL = TASSEL_1 | TACLR;                // ACLK, clear TBR

    SCB_SCR &= ~SCB_SCR_SLEEPONEXIT;           // Wake up on exit from ISR

    while (1)
    {
      for (i = 20000; i > 0; i--);            // Delay
      ADC14CTL0 |= ADC14ENC | ADC14SC;        // Start sampling/conversion
      __sleep();


      __no_operation();                       // For debugger
    }




}

// ADC14 interrupt service routine

void ADC14IsrHandler(void) {
    if (ADC14IFGR1 & ADC14HIIFG)
    {
        ADC14IFGR1 &= ~ADC14HIIFG;            // Clear interrupt flag
        TA0CTL &= ~MC_1;                      // Turn off Timer
        TA0CCR0 = FastToggle_Period;          // Set Timer Period for fast LED toggle
        TA0CTL |= MC_1;
    }
    if (ADC14IFGR1 & ADC14LOIFG)
    {
        ADC14IFGR1 &= ~ADC14LOIFG;            // Clear interrupt flag
        TA0CTL &= ~MC_1;                      // Turn off Timer
        P1OUT &= ~BIT0;                         // Turn off LED
    }
    if (ADC14IFGR1 & ADC14INIFG)
    {
        ADC14IFGR1 &= ~ADC14INIFG;            // Clear interrupt flag
        TA0CTL &= ~MC_1;                      // Turn off Timer
        TA0CCR0 = SlowToggle_Period;          // Set Timer Period for slow LED toggle
        TA0CTL |= MC_1;
    }
}

// Timer A0 interrupt service routine

void TimerA0_0IsrHandler(void) {
    P1OUT ^= BIT0;
}

