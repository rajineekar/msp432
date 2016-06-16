; <<< Use Configuration Wizard in Context Menu >>>
;******************************************************************************
;
; startup_rvmdk.S - Startup code for use with Keil's uVision.
;
; Copyright (c) 2012-2013 Texas Instruments Incorporated.  All rights reserved.
; Software License Agreement
;
; Texas Instruments (TI) is supplying this software for use solely and
; exclusively on TI's microcontroller products. The software is owned by
; TI and/or its suppliers, and is protected under applicable copyright
; laws. You may not combine this software with "viral" open-source
; software in order to form a larger program.
;
; THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
; NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
; NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
; A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
; CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
; DAMAGES, FOR ANY REASON WHATSOEVER.
;
; This is part of revision 1.1 of the EK-TM4C123GXL Firmware Package.
;
;******************************************************************************

;******************************************************************************
;
; <o> Stack Size (in Bytes) <0x0-0xFFFFFFFF:8>
;
;******************************************************************************
Stack   EQU     0x00000800

;******************************************************************************
;
; <o> Heap Size (in Bytes) <0x0-0xFFFFFFFF:8>
;
;******************************************************************************
Heap    EQU     0x00000000

;******************************************************************************
;
; Allocate space for the stack.
;
;******************************************************************************
        AREA    STACK, NOINIT, READWRITE, ALIGN=3
StackMem
        SPACE   Stack
__initial_sp

;******************************************************************************
;
; Allocate space for the heap.
;
;******************************************************************************
        AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
HeapMem
        SPACE   Heap
__heap_limit

;******************************************************************************
;
; Indicate that the code in this file preserves 8-byte alignment of the stack.
;
;******************************************************************************
        PRESERVE8

;******************************************************************************
;
; Place code into the reset code section.
;
;******************************************************************************
        AREA    RESET, CODE, READONLY
        THUMB

;******************************************************************************
;
; External declarations for the interrupt handlers used by the application.
;
;******************************************************************************
        EXTERN  Port1Handler

;******************************************************************************
;
; The vector table.
;
;******************************************************************************
        EXPORT  __Vectors
__Vectors
        DCD     StackMem + Stack            ; Top of Stack
        DCD     Reset_Handler               ; Reset Handler
        DCD     nmiSR                       ; NMI Handler
        DCD     faultISR                    ; Hard Fault Handler
        DCD     IntDefaultHandler           ; The MPU fault handler
        DCD     IntDefaultHandler           ; The bus fault handler
        DCD     IntDefaultHandler           ; The usage fault handler
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     0                           ; Reserved
        DCD     IntDefaultHandler           ; SVCall handler
        DCD     IntDefaultHandler           ; Debug monitor handler
        DCD     0                           ; Reserved
        DCD     IntDefaultHandler           ; The PendSV handler
        DCD     IntDefaultHandler           ; The SysTick handler
        DCD     IntDefaultHandler           ; PSS ISR
        DCD     IntDefaultHandler           ; CS ISR 
        DCD     IntDefaultHandler           ; PCM ISR
        DCD     IntDefaultHandler           ; WDT ISR
        DCD     IntDefaultHandler           ; FPU ISR
        DCD     IntDefaultHandler           ; FLCTL ISR
        DCD     IntDefaultHandler           ; COMP_E0_MODULE ISR
        DCD     IntDefaultHandler           ; COMP_E1_MODULE ISR
        DCD     IntDefaultHandler           ; TA0_0 ISR
        DCD     IntDefaultHandler           ; TA0_N ISR
        DCD     IntDefaultHandler           ; TA1_0 ISR
        DCD     IntDefaultHandler           ; TA1_N ISR
        DCD     IntDefaultHandler           ; TA2_0 ISR
        DCD     IntDefaultHandler           ; TA2_N ISR
        DCD     IntDefaultHandler           ; TA3_0 ISR
        DCD     IntDefaultHandler           ; TA3_N ISR
        DCD     IntDefaultHandler           ; EUSCIA0 ISR
        DCD     IntDefaultHandler           ; EUSCIA1 ISR
        DCD     IntDefaultHandler           ; EUSCIA2 ISR
        DCD     IntDefaultHandler           ; EUSCIA3 ISR
        DCD     IntDefaultHandler           ; EUSCIB0 ISR
        DCD     IntDefaultHandler           ; EUSCIB1 ISR
        DCD     IntDefaultHandler           ; EUSCIB2 ISR
        DCD     IntDefaultHandler           ; EUSCIB3 ISR
        DCD     IntDefaultHandler           ; ADC12 ISR
        DCD     IntDefaultHandler           ; T32_INT1 ISR
        DCD     IntDefaultHandler           ; T32_INT2 ISR
        DCD     IntDefaultHandler           ; T32_INTC ISR
        DCD     IntDefaultHandler           ; AES ISR
        DCD     IntDefaultHandler           ; RTC ISR
        DCD     IntDefaultHandler           ; DMA_ERR ISR
        DCD     IntDefaultHandler           ; DMA_INT3 ISR
        DCD     IntDefaultHandler           ; DMA_INT2 ISR
        DCD     IntDefaultHandler           ; DMA_INT1 ISR
        DCD     IntDefaultHandler           ; DMA_INT0 ISR
        DCD     Port1Handler                ; PORT1 ISR
        DCD     IntDefaultHandler           ; PORT2 ISR
        DCD     IntDefaultHandler           ; PORT3 ISR
        DCD     IntDefaultHandler           ; PORT4 ISR
        DCD     IntDefaultHandler           ; PORT5 ISR
        DCD     IntDefaultHandler           ; PORT6 ISR
        DCD     IntDefaultHandler           ; Reserved 41
        DCD     IntDefaultHandler           ; Reserved 42
        DCD     IntDefaultHandler           ; Reserved 43
        DCD     IntDefaultHandler           ; Reserved 44
        DCD     IntDefaultHandler           ; Reserved 45
        DCD     IntDefaultHandler           ; Reserved 46
        DCD     IntDefaultHandler           ; Reserved 47
        DCD     IntDefaultHandler           ; Reserved 48
        DCD     IntDefaultHandler           ; Reserved 49
        DCD     IntDefaultHandler           ; Reserved 50
        DCD     IntDefaultHandler           ; Reserved 51
        DCD     IntDefaultHandler           ; Reserved 52
        DCD     IntDefaultHandler           ; Reserved 53
        DCD     IntDefaultHandler           ; Reserved 54
        DCD     IntDefaultHandler           ; Reserved 55
        DCD     IntDefaultHandler           ; Reserved 56
        DCD     IntDefaultHandler           ; Reserved 57
        DCD     IntDefaultHandler           ; Reserved 58
        DCD     IntDefaultHandler           ; Reserved 59
        DCD     IntDefaultHandler           ; Reserved 60
        DCD     IntDefaultHandler           ; Reserved 61
        DCD     IntDefaultHandler           ; Reserved 62
        DCD     IntDefaultHandler           ; Reserved 63
        DCD     IntDefaultHandler           ; Reserved 64
;******************************************************************************
;
; This is the code that gets called when the processor first starts execution
; following a reset event.
;
;******************************************************************************
        EXPORT  Reset_Handler
Reset_Handler
        ;
        ; Enable the floating-point unit.  This must be done here to handle the
        ; case where main() uses floating-point and the function prologue saves
        ; floating-point registers (which will fault if floating-point is not
        ; enabled).  Any configuration of the floating-point unit using
        ; DriverLib APIs must be done here prior to the floating-point unit
        ; being enabled.
        ;
        ; Note that this does not use DriverLib since it might not be included
        ; in this project.
        ;
        MOVW    R0, #0xED88
        MOVT    R0, #0xE000
        LDR     R1, [R0]
        ORR     R1, #0x00F00000
        STR     R1, [R0]

        ;
        ; Call the C library enty point that handles startup.  This will copy
        ; the .data section initializers from flash to SRAM and zero fill the
        ; .bss section.
        ;
        IMPORT  __main
        B       __main

;******************************************************************************
;
; This is the code that gets called when the processor receives a NMI.  This
; simply enters an infinite loop, preserving the system state for examination
; by a debugger.
;
;******************************************************************************
nmiSR
        B       nmiSR

;******************************************************************************
;
; This is the code that gets called when the processor receives a fault
; interrupt.  This simply enters an infinite loop, preserving the system state
; for examination by a debugger.
;
;******************************************************************************
faultISR
        B       faultISR

;******************************************************************************
;
; This is the code that gets called when the processor receives an unexpected
; interrupt.  This simply enters an infinite loop, preserving the system state
; for examination by a debugger.
;
;******************************************************************************
IntDefaultHandler
        B       IntDefaultHandler

;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Some code in the normal code section for initializing the heap and stack.
;
;******************************************************************************
        AREA    |.text|, CODE, READONLY

;******************************************************************************
;
; The function expected of the C library startup code for defining the stack
; and heap memory locations.  For the C library version of the startup code,
; provide this function so that the C library initialization code can find out
; the location of the stack and heap.
;
;******************************************************************************
    IF :DEF: __MICROLIB
        EXPORT  __initial_sp
        EXPORT  __heap_base
        EXPORT  __heap_limit
    ELSE
        IMPORT  __use_two_region_memory
        EXPORT  __user_initial_stackheap
__user_initial_stackheap
        LDR     R0, =HeapMem
        LDR     R1, =(StackMem + Stack)
        LDR     R2, =(HeapMem + Heap)
        LDR     R3, =StackMem
        BX      LR
    ENDIF

;******************************************************************************
;
; Make sure the end of this section is aligned.
;
;******************************************************************************
        ALIGN

;******************************************************************************
;
; Tell the assembler that we're done.
;
;******************************************************************************
        END
