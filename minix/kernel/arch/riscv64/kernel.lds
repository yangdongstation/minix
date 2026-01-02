/*
 * RISC-V 64 MINIX kernel linker script
 * Target: QEMU virt platform
 *
 * Memory layout:
 * - Kernel loaded at 0x80200000 (after OpenSBI)
 * - Virtual address: 0xFFFFFFFF80200000 (high half)
 */

OUTPUT_FORMAT("elf64-littleriscv")
OUTPUT_ARCH(riscv)
ENTRY(__k_unpaged__start)

/* Physical load address */
KERNEL_PHYS = 0x80200000;

SECTIONS
{
    . = KERNEL_PHYS;

    __k_unpaged__kern_unpaged_start = .;

    /* Unpaged bootstrap code/data */
    .unpaged_text ALIGN(4096) : {
        unpaged_*.o(.text.boot .text .text.*)
    }
    .unpaged_data ALIGN(4096) : {
        unpaged_*.o(.rodata .rodata.*)
        unpaged_*.o(.srodata .srodata.*)
        unpaged_*.o(.data .data.*)
        unpaged_*.o(.sdata .sdata.*)
    }

    __k_unpaged___bss_start = .;
    .unpaged_bss ALIGN(4096) : {
        unpaged_*.o(.bss .bss.*)
        unpaged_*.o(.sbss .sbss.*)
        unpaged_*.o(COMMON)
    }
    __k_unpaged___bss_end = .;
    __k_unpaged__kern_unpaged_end = .;

    . = ALIGN(4096);
    usermapped_start = .;
    .usermapped_glo : {
        *(.usermapped_glo)
    }
    . = ALIGN(4096);
    usermapped_nonglo_start = .;
    .usermapped : {
        *(.usermapped)
    }
    . = ALIGN(4096);
    usermapped_end = .;

    _text_start = .;

    /* Main kernel text */
    .text : AT(ADDR(.text)) {
        *(.text .text.*)
    }

    . = ALIGN(4096);
    _text_end = .;

    /* Read-only data */
    .rodata : AT(ADDR(.rodata)) {
        *(.rodata .rodata.*)
        *(.srodata .srodata.*)
    }

    . = ALIGN(4096);
    _rodata_end = .;

    /* Data section */
    .data : AT(ADDR(.data)) {
        *(.data .data.*)
        *(.sdata .sdata.*)
    }

    . = ALIGN(4096);
    _data_end = .;

    /* BSS section */
    __bss_start = .;
    .bss : AT(ADDR(.bss)) {
        *(.bss .bss.*)
        *(.sbss .sbss.*)
        *(COMMON)
    }
    . = ALIGN(4096);
    __bss_end = .;

    _kernel_end = .;

    /* Symbols for kernel */
    PROVIDE(_kernel_phys_start = KERNEL_PHYS);
    PROVIDE(_kernel_phys_end = _kernel_end);
    PROVIDE(_kernel_size = _kernel_end - KERNEL_PHYS);

    /* Provide aliases for unpaged namespace users */
    PROVIDE(__k_unpaged_kernel_main = kernel_main);
    PROVIDE(__k_unpaged_exception_handler = exception_handler);
    PROVIDE(__k_unpaged_sbi_console_putchar = sbi_console_putchar);
    PROVIDE(__k_unpaged_sbi_console_getchar = sbi_console_getchar);
    PROVIDE(__k_unpaged_kinfo = kinfo);
    PROVIDE(__k_unpaged_riscv_cons_init = riscv_cons_init);
    PROVIDE(__k_unpaged_riscv_cons_putc = riscv_cons_putc);
    PROVIDE(__k_unpaged_riscv_cons_getc = riscv_cons_getc);
    PROVIDE(__k_unpaged_minix_shutdown = minix_shutdown);
    PROVIDE(__k_unpaged_ser_putc = ser_putc);
    PROVIDE(__k_unpaged_kmessages = kmessages);
    PROVIDE(__k_unpaged_send_diag_sig = send_diag_sig);
    PROVIDE(__k_unpaged_sbi_system_reset = sbi_system_reset);
    PROVIDE(__k_unpaged_sbi_shutdown = sbi_shutdown);
    PROVIDE(__k_unpaged_arch_set_timer_freq = arch_set_timer_freq);
    PROVIDE(__k_unpaged_plic_init = plic_init);

    /* Discard unnecessary sections */
    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}
