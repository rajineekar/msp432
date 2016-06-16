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
//  MSP432P401 Demo - ADC14, Sample A12 Temp and Convert to oC and oF
//
//  Description: This example works on Single-Channel Single-Conversion Mode.
//  A single sample is made on A22 (temperature) with reference to
//  internal 1.5V Vref. Software sets ADCSC to start sample and conversion
//  - ADCSC automatically cleared at EOC. ADC internal oscillator times
//  sample and conversion. In Mainloop MSP430 waits in LPM3 to save power until
//  ADC conversion complete, ADC14_ISR will force exit from any LPMx in
//  Mainloop on reti.
//  ACLK = default REFO ~32768Hz, MCLK = SMCLK = default DCODIV ~3MHz.
//
//  Uncalibrated temperature measured from device to device will vary due to
//  slope and offset variance from device to device - please see datasheet.
//
//  This code example show how to use the TLV calibrated data to measure temperature.
//
//
//                MSP432P401
//             -----------------
//         /|\|                 |
//          | |                 |
//          --|RST              |
//            |                 |
//        >---|A22(TEMP)        |
//
//
//  Wei Zhao
//  Texas Instruments Inc.
//  Jan 2014
//  Built with IAR Embedded Workbench v5.60 & Code Composer Studio v5.5
//******************************************************************************
#include "msp.h"
#include <stdint.h>


#define CALADC_15V_30C  *((unsigned int *)0x1A1A)               // Temperature Sensor Calibration-30 C
                                                                // See device datasheet for TLV table memory mapping
#define CALADC_15V_85C  *((unsigned int *)0x1A1C)               // Temperature Sensor Calibration-85 C

volatile long temp;
volatile long IntDegF;
volatile long IntDegC;

int main(void)
{
  WDTCTL = WDTPW | WDTHOLD;                                     // Stop WDT

  // Initialize the shared reference module
  // By default, REFMSTR=1 => REFCTL is used to configure the internal reference
  while(REFCTL0 & REFGENBUSY);              // If ref generator busy, WAIT
  REFCTL0 |= REFVSEL_0 + REFON;             // Enable internal 1.2V reference

  // Configure ADC - Pulse sample mode; ADC14SC trigger
  ADC14CTL0 |= ADC14SHT0_5 | ADC14ON | ADC14SHP;                                  // ADC ON,temperature sample period>30us
  ADC14CTL1 |= ADC14TCMAP;                   // Enable internal temperature sensor
  ADC14MCTL0 = ADC14VRSEL_1 + ADC14INCH_22; // ADC input ch A22 => temp sense
  ADC14IER0 = 0x0001;                        // ADC_IFG upon conv result-ADCMEM0

  while(!(REFCTL0 & REFGENRDY));            // Wait for reference generator
                                            // to settle
  ADC14CTL0 |= ADC14ENC;

  __enable_interrupt();
  NVIC_ISER0 = 1 << ((INT_ADC14 - 16) & 31);// Enable ADC interrupt in NVIC module

  SCB_SCR &= ~SCB_SCR_SLEEPONEXIT;          // Wake up on exit from ISR
    
  while(1)
  {
    ADC14CTL0 |= ADC14SC;                                // Sampling and conversion start

    __sleep();
    __no_operation();                                         // Only for debugger

    // Temperature in Celsius
    // The temperature (Temp, C)=
    IntDegC = (temp-CALADC_15V_30C)*(85-30)/(CALADC_15V_85C-CALADC_15V_30C)+30;

    // Temperature in Fahrenheit
    // Tf = (9/5)*Tc | 32
    IntDegF = 9*IntDegC/5+32;

    __no_operation();                                          // Only for debugger
    }
}

// ADC14 interrupt service routine
void ADC14IsrHandler(void) 
{
    if (ADC14IFGR0 & ADC14IFG0)
    {
      temp = ADC14MEM0;
    }

}
