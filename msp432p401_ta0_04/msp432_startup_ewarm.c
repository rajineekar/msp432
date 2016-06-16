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
 * MSP430 code examples are self-contained low-level programs that typically
 * demonstrate a single peripheral function or device feature in a highly
 * concise manner. For this the code may rely on the device's power-on default
 * register values and settings such as the clock configuration and care must
 * be taken when combining code from several examples to avoid potential side
 * effects. Also see http://www.ti.com/tool/mspdriverlib for an API functional
 * library & https://dev.ti.com/pinmux/ for a GUI approach to peripheral configuration.
 *
 * --/COPYRIGHT--*/
#include <stdint.h>

//*****************************************************************************
//
// Enable the IAR extensions for this source file.
//
//*****************************************************************************
#pragma language=extended

//*****************************************************************************
//
// Forward declaration of the default fault handlers.
//
//*****************************************************************************
void ResetISR(void);
static void NmiISR(void);
static void FaultISR(void);
static void intDefaultHandler(void);

//*****************************************************************************
//
// External declaration for the interrupt handler used by the application.
//
//*****************************************************************************
extern void TimerA0_NIsrHandler(void);

//*****************************************************************************
//
// The entry point for the application startup code.
//
//*****************************************************************************
extern void __iar_program_start(void);

//*****************************************************************************
//
// Reserve space for the system stack.
//
//*****************************************************************************
static uint32_t systemStack[128] @ ".noinit";

//*****************************************************************************
//
// A union that describes the entries of the vector table.  The union is needed
// since the first entry is the stack pointer and the remainder are function
// pointers.
//
//*****************************************************************************
typedef union
{
    void (*handler)(void);
    uint32_t ptr;
}
uVectorEntry;

//*****************************************************************************
//
// The vector table.  Note that the proper constructs must be placed on this to
// ensure that it ends up at physical address 0x0000.0000.
//
//*****************************************************************************
__root const uVectorEntry __vector_table[] @ ".intvec" =
{
    { .ptr = (uint32_t)systemStack + sizeof(systemStack) },
                                            // The initial stack pointer
    ResetISR,                               // The reset handler
    NmiISR,                                  // The NMI handler
    FaultISR,                               // The hard fault handler
    intDefaultHandler,                      // The MPU fault handler
    intDefaultHandler,                      // The bus fault handler
    intDefaultHandler,                      // The usage fault handler
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    0,                                      // Reserved
    intDefaultHandler,                      // SVCall handler
    intDefaultHandler,                      // Debug monitor handler
    0,                                      // Reserved
    intDefaultHandler,                      // The PendSV handler
    intDefaultHandler,                      // The SysTick handler
    intDefaultHandler,                      // PSS ISR
    intDefaultHandler,                      // CS ISR
    intDefaultHandler,                      // PCM ISR
    intDefaultHandler,                      // WDT ISR
    intDefaultHandler,                      // FPU ISR
    intDefaultHandler,                      // FLCTL ISR
    intDefaultHandler,                      // COMP0 ISR
    intDefaultHandler,                      // COMP1 ISR
    intDefaultHandler,                      // TA0_0 ISR
    TimerA0_NIsrHandler,                    // TA0_N ISR
    intDefaultHandler,                      // TA1_0 ISR
    intDefaultHandler,                      // TA1_N ISR
    intDefaultHandler,                      // TA2_0 ISR
    intDefaultHandler,                      // TA2_N ISR
    intDefaultHandler,                      // TA3_0 ISR
    intDefaultHandler,                      // TA3_N ISR
    intDefaultHandler,                      // EUSCIA0 ISR
    intDefaultHandler,                      // EUSCIA1 ISR
    intDefaultHandler,                      // EUSCIA2 ISR
    intDefaultHandler,                      // EUSCIA3 ISR
    intDefaultHandler,                      // EUSCIB0 ISR
    intDefaultHandler,                      // EUSCIB1 ISR
    intDefaultHandler,                      // EUSCIB2 ISR
    intDefaultHandler,                      // EUSCIB3 ISR
    intDefaultHandler,                      // ADC14 ISR
    intDefaultHandler,                      // T32_INT1 ISR
    intDefaultHandler,                      // T32_INT2 ISR
    intDefaultHandler,                      // T32_INTC ISR
    intDefaultHandler,                      // AES ISR
    intDefaultHandler,                      // RTC ISR
    intDefaultHandler,                      // DMA_ERR ISR
    intDefaultHandler,                      // DMA_INT3 ISR
    intDefaultHandler,                      // DMA_INT2 ISR
    intDefaultHandler,                      // DMA_INT1 ISR
    intDefaultHandler,                      // DMA_INT0 ISR
    intDefaultHandler,                      // PORT1 ISR
    intDefaultHandler,                      // PORT2 ISR
    intDefaultHandler,                      // PORT3 ISR
    intDefaultHandler,                      // PORT4 ISR
    intDefaultHandler,                      // PORT5 ISR
    intDefaultHandler,                      // PORT6 ISR
    intDefaultHandler,                      // Reserved 41
    intDefaultHandler,                      // Reserved 42
    intDefaultHandler,                      // Reserved 43
    intDefaultHandler,                      // Reserved 44
    intDefaultHandler,                      // Reserved 45
    intDefaultHandler,                      // Reserved 46
    intDefaultHandler,                      // Reserved 47
    intDefaultHandler,                      // Reserved 48
    intDefaultHandler,                      // Reserved 49
    intDefaultHandler,                      // Reserved 50
    intDefaultHandler,                      // Reserved 51
    intDefaultHandler,                      // Reserved 52
    intDefaultHandler,                      // Reserved 53
    intDefaultHandler,                      // Reserved 54
    intDefaultHandler,                      // Reserved 55
    intDefaultHandler,                      // Reserved 56
    intDefaultHandler,                      // Reserved 57
    intDefaultHandler,                      // Reserved 58
    intDefaultHandler,                      // Reserved 59
    intDefaultHandler,                      // Reserved 60
    intDefaultHandler,                      // Reserved 61
    intDefaultHandler,                      // Reserved 62
    intDefaultHandler,                      // Reserved 63
    intDefaultHandler                       // Reserved 64
};

//*****************************************************************************
//
// This is the code that gets called when the processor first starts execution
// following a reset event.  Only the absolutely necessary set is performed,
// after which the application supplied entry() routine is called.  Any fancy
// actions (such as making decisions based on the reset cause register, and
// resetting the bits in that register) are left solely in the hands of the
// application.
//
//*****************************************************************************
void ResetISR(void)
{


    //
    // Call the application's entry point.
    //
    __iar_program_start();
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a NMI.  This
// simply enters an infinite loop, preserving the system state for examination
// by a debugger.
//
//*****************************************************************************
static void NmiISR(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives a fault
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void FaultISR(void)
{
    //
    // Enter an infinite loop.
    //
    while(1)
    {
    }
}

//*****************************************************************************
//
// This is the code that gets called when the processor receives an unexpected
// interrupt.  This simply enters an infinite loop, preserving the system state
// for examination by a debugger.
//
//*****************************************************************************
static void intDefaultHandler(void)
{
    //
    // Go into an infinite loop.
    //
    while(1)
    {
    }
}
