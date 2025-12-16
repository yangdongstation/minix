# Makefile for arch-dependent framebuffer code
.include <bsd.own.mk>

HERE=${.CURDIR}/arch/${MACHINE_ARCH}
.PATH:  ${HERE}

# RISC-V framebuffer support (e.g., virtio-gpu)
# Add platform-specific framebuffer code here
SRCS += fb_arch.c