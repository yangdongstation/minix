/*
 * RISC-V 64 exception vectors
 *
 * This file provides alternative entry points for different exception types
 * Most exceptions are handled through trap_entry in head.S
 */

#include "include/sconst.h"
#include "include/archconst.h"

    .section .text
    .align 2

/*
 * Exception handler stubs
 * These are called from the main trap handler for specific exception types
 */

/*
 * Divide by zero (not a hardware exception on RISC-V)
 * RISC-V handles division by zero in software
 */
    .globl exc_divide
    .type exc_divide, @function
exc_divide:
    /* This shouldn't be called on RISC-V */
    ret

/*
 * Debug exception
 */
    .globl exc_debug
    .type exc_debug, @function
exc_debug:
    /* Handle breakpoint/debug trap */
    ret

/*
 * NMI (Non-Maskable Interrupt)
 * On RISC-V, NMI is handled differently (usually by M-mode)
 */
    .globl exc_nmi
    .type exc_nmi, @function
exc_nmi:
    /* S-mode doesn't handle NMI directly */
    ret

/*
 * Breakpoint exception
 */
    .globl exc_breakpoint
    .type exc_breakpoint, @function
exc_breakpoint:
    /* Already handled in exception.c */
    ret

/*
 * Page fault handlers
 */
    .globl exc_page_fault
    .type exc_page_fault, @function
exc_page_fault:
    /* Already handled in exception.c */
    ret

/*
 * General protection fault (not directly applicable to RISC-V)
 */
    .globl exc_general_protection
    .type exc_general_protection, @function
exc_general_protection:
    ret

/*
 * Invalid opcode
 */
    .globl exc_invalid_opcode
    .type exc_invalid_opcode, @function
exc_invalid_opcode:
    ret

/*
 * Coprocessor error (FPU)
 */
    .globl exc_coprocessor_error
    .type exc_coprocessor_error, @function
exc_coprocessor_error:
    ret

/*
 * Alignment check
 */
    .globl exc_alignment_check
    .type exc_alignment_check, @function
exc_alignment_check:
    ret

/*
 * Machine check
 */
    .globl exc_machine_check
    .type exc_machine_check, @function
exc_machine_check:
    ret

/*
 * SIMD exception
 */
    .globl exc_simd
    .type exc_simd, @function
exc_simd:
    ret
