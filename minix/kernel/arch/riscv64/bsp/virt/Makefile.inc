# Makefile for QEMU virt platform BSP (RISC-V 64)

HERE_BSP=${HERE}/bsp/virt
.PATH: ${HERE_BSP}

# BSP specific objects that need to be unpaged
BSP_OBJS_UNPAGED= bsp_serial.o bsp_init.o

# BSP source files
SRCS+=	bsp_init.c bsp_serial.c bsp_timer.c bsp_reset.c bsp_intr.c
