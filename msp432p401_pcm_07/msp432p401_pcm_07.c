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
//   MSP432P401 Demo - Using Low-Frequency LPM0 mode
//
//
//   Description: Configure device to operate in LPM0 in Low-Frequency mode with 
//   TimerA operation sourced by a low-frequency clock source. This example
//   demonstrates peripheral functionality (PWM) as well as interrupt generation
//   capability in SL_Low-Frequency Mode mode. TimerA is configured to generate 4 PWM signals
//   on TA0CCR1-4 channels to pins 2.4-7 respectively. Additionally, TA0CCR0
//   interrupt is enabled, the interrupt service routine toggles pin P1.0 LED.
//
//   This examples builds up on msp432p401_pcm_05, which exercises the transition
//   into Low-Frequency Mode from active mode (LDO).
//
//   Note: the code in this example assumes the device is currently in LDO mode
//   AM0_LDO or AM1_LDO (Active Mode using LDO, VCore=0/1 respectively)
//
//   Transition from DCDC mode to Low-Frequency mode requires intermediate 
//   transition through LDO mode. For more information refer to the PCM chapter in
//   the device user's guide.
//
//          AM_DCDC_VCORE0 <----->  AM_LDO_VCORE0* <--@--> AM_LF_VCORE0
//                                       ^
//                                       |
//                                       |
//                                       v
//          AM_DCDC_VCORE1 <----->  AM_LDO_VCORE1  <--@--> AM_Low-Frequency Mode_VCORE1
//
//   *: power state condition after reset
//   @: transitions demonstrated in this code example
//
//                MSP432p401rpz
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |             P1.0|--->LED
//            |                 |
//            |           P2.4  |--->TA0CCR1 PWM
//            |           P2.5  |--->TA0CCR2 PWM
//            |           P2.6  |--->TA0CCR3 PWM
//            |           P2.7  |--->TA0CCR4 PWM
//
//   Dung Dang
//   Texas Instruments Inc.
//   Nov 2013
//   Built with Code Composer Studio V6.0
//******************************************************************************
#include "msp.h"
#include "stdint.h"


void error(void);

int main(void)
{
    uint32_t currentPowerState;
    volatile uint32_t i;
    WDTCTL = WDTPW | WDTHOLD;               // Stop WDT
    P1DIR |= BIT0;


    /* Switch MCLK, SMCLK, ACLK sources over to REFO clock for low frequency operation first */
    CSKEY = CSKEY_VAL;                        // Unlock CS module
    CSCTL1 = SELM_2 | SELS_2 | SELA_2;
    CSKEY = 0;                             // Lock CS module

    /* Get current power state */
    currentPowerState = PCMCTL0 & CPM_M;
    /* Transition to Low-Frequency Mode from current LDO power state properly */
    switch (currentPowerState)
    {
        case CPM_0:                // AM_LDO_VCORE0, need to switch to AM_Low-Frequency Mode_VCORE0
            while ((PCMCTL1 & PMR_BUSY));
            PCMCTL0 = PCM_CTL_KEY_VAL | AMR_8;
            while ((PCMCTL1 & PMR_BUSY));
            if (PCMIFG & AM_INVALID_TR_IFG)
                error();                    // Error if transition was not successful
            break;
        case CPM_1:                // AM_LDO_VCORE1, need to switch to AM_Low-Frequency Mode_VCORE1
            while ((PCMCTL1 & PMR_BUSY));
            PCMCTL0 = PCM_CTL_KEY_VAL | AMR_9;
            while ((PCMCTL1 & PMR_BUSY));
            if (PCMIFG & AM_INVALID_TR_IFG)
                error();                    // Error if transition was not successful
            break;
        case CPM_8:                // Device is already in AM_Low-Frequency Mode_VCORE0
            break;
        case CPM_9:                // Device is already in AM_Low-Frequency Mode_VCORE1
            break;
        default:                            // Device is in some other state, which is unexpected
            error();
    }

    /* Setup Port Pins */
    P2DIR  |= BIT4 | BIT5 | BIT6 | BIT7;     // P2.4 - P2.7 output
    P2SEL0 |= BIT4 | BIT5 | BIT6 | BIT7;     // P2.4 - P2.7  Port Map functions
    P2SEL1 &= ~(BIT4 | BIT5 | BIT6 | BIT7);  // P2.4 - P2.7 default PM functions = TimerA CCRx



    // Setup TA0
    TA0CCTL0 =  CCIE;                       // TACCR0 interrupt enabled
    TA0CCR0 = 256;                          // PWM Period/2
    TA0CCTL1 = OUTMOD_6;                    // CCR1 toggle/set
    TA0CCR1 = 192;                          // CCR1 PWM duty cycle
    TA0CCTL2 = OUTMOD_6;                    // CCR2 toggle/set
    TA0CCR2 = 128;                          // CCR2 PWM duty cycle
    TA0CCTL3 = OUTMOD_6;                    // CCR3 toggle/set
    TA0CCR3 = 96;                           // CCR3 PWM duty cycle
    TA0CCTL4 = OUTMOD_6;                    // CCR4 toggle/set
    TA0CCR4 = 48;                           // CCR4 PWM duty cycle
    TA0CTL = TASSEL_1 + MC_3;               // ACLK, up-down mode


    SCB_SCR |= SCB_SCR_SLEEPONEXIT;           // Enable sleep on exit from ISR

    __enable_interrupt();
    NVIC_ISER0 = 1 << ((INT_TA0_0 - 16) & 31);

    /* From Low-Frequency Active Mode, go to Low-Frequency LPM0  */
    while(1)
    {
        __sleep();
        __no_operation();                       // For debugger
    }

}

// Timer A0 interrupt service routine

void TimerA0_0IsrHandler(void)
{
    TA0CCTL0 &= ~CCIFG;
    P1OUT ^= BIT0;
}


void error(void)
{
    volatile uint32_t i;
    while (1)
    {
        P1OUT ^= BIT0;
        for(i=0;i<20000;i++);       // Blink LED forever
    }
}
