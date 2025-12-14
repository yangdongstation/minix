# Makefile for MINIX 3 RISC-V 64-bit Release Tools
#
# Usage:
#   make iso          - Build ISO image
#   make disk         - Build disk image
#   make all          - Build everything
#   make clean        - Clean build artifacts
#

MACHINE=		evbriscv64
MACHINE_ARCH=		riscv64

# Directories
MINIX_ROOT?=		$(shell cd ../.. && pwd)
DESTDIR?=		/usr/obj/minix/riscv64
RELEASEDIR?=		$(DESTDIR)/release
ISO_ROOT=		$(DESTDIR)/iso_root

# Tools
CROSS_PREFIX?=		riscv64-unknown-elf-
CC=			$(CROSS_PREFIX)gcc
LD=			$(CROSS_PREFIX)ld
OBJCOPY=		$(CROSS_PREFIX)objcopy
STRIP=			$(CROSS_PREFIX)strip

# Output files
ISO_NAME=		minix-$(MACHINE)-$(shell date +%Y%m%d).iso
DISK_NAME=		minix-$(MACHINE).img
DISK_SIZE_MB=		256

# Build flags
CFLAGS=			-march=rv64gc -mabi=lp64d -O2 -Wall
LDFLAGS=		-march=rv64gc -mabi=lp64d

.PHONY: all iso disk clean distclean

all: iso disk

iso:
	@echo "Building ISO image..."
	./build_iso.sh -d $(DESTDIR) -o $(RELEASEDIR)/$(ISO_NAME)

disk:
	@echo "Creating disk image..."
	./mkdisk.sh -d $(DESTDIR) -o $(RELEASEDIR)/$(DISK_NAME) -s $(DISK_SIZE_MB)

# Create distribution directories
dirs:
	mkdir -p $(DESTDIR)
	mkdir -p $(RELEASEDIR)
	mkdir -p $(ISO_ROOT)

# Build kernel
kernel:
	cd $(MINIX_ROOT) && \
	./build.sh -m $(MACHINE) -D $(DESTDIR) kernel

# Build servers
servers:
	cd $(MINIX_ROOT) && \
	./build.sh -m $(MACHINE) -D $(DESTDIR) servers

# Build drivers
drivers:
	cd $(MINIX_ROOT) && \
	./build.sh -m $(MACHINE) -D $(DESTDIR) drivers

# Build userland
userland:
	cd $(MINIX_ROOT) && \
	./build.sh -m $(MACHINE) -D $(DESTDIR) commands

# Full distribution build
distribution: dirs kernel servers drivers userland
	@echo "Distribution build complete"

clean:
	rm -rf $(ISO_ROOT)
	rm -f $(RELEASEDIR)/$(ISO_NAME)
	rm -f $(RELEASEDIR)/$(DISK_NAME)

distclean: clean
	rm -rf $(DESTDIR)
