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
ENTRY(_start)

/* Physical load address */
KERNEL_PHYS = 0x80200000;

/* Virtual address offset (high half mapping) */
KERNEL_VIRT_OFFSET = 0xFFFFFFFF00000000;

/* Kernel virtual base */
KERNEL_VIRT = KERNEL_PHYS + KERNEL_VIRT_OFFSET;

SECTIONS
{
    . = KERNEL_PHYS;

    /* Boot code (identity mapped initially) */
    .text.boot : {
        *(.text.boot)
    }

    . = ALIGN(4096);
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

    /* Discard unnecessary sections */
    /DISCARD/ : {
        *(.comment)
        *(.note*)
        *(.eh_frame*)
    }
}
