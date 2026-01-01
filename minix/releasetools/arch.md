# MINIX Release Tools Directory (/minix/releasetools)

## Overview

The `/minix/releasetools/` directory contains specialized tools and scripts for creating MINIX releases, managing distribution images, and handling platform-specific release processes. These tools are essential for packaging MINIX for different architectures, creating bootable installation media, and managing the complete release lifecycle from build artifacts to distributable images.

## Directory Structure

```
minix/releasetools/
├── riscv64/              # RISC-V 64-bit specific release tools
│   ├── Makefile          # Build configuration for RISC-V release
│   ├── boot_order        # Boot configuration for RISC-V
│   ├── build_iso.sh      # ISO image creation script
│   ├── install.sh        # Installation script
│   ├── mkboot.sh         # Boot image creation
│   ├── mkdisk.sh         # Disk image creation
│   ├── release.conf      # Release configuration
│   ├── sets.list         # Package sets definition
│   ├── setup.sh          # System setup script
│   └── system.conf       # System configuration for release
```

## Key Release Tools

### build_iso.sh - ISO Image Creation
Comprehensive script for creating bootable ISO images for MINIX/RISC-V distribution.

**Key Features:**
- Bootable ISO creation with multiple boot options
- Integration with OpenSBI firmware
- GRUB configuration for RISC-V
- Package set integration
- Automated installation support

**Implementation Highlights:**
```bash
#!/bin/bash
#
# MINIX RISC-V ISO creation script
#

# ISO configuration
ISO_NAME="minix-riscv64.iso"
ISO_VOLUME="MINIX_RISCV64"
ISO_BOOT_CATALOG="boot.catalog"

# RISC-V specific boot configuration
RISCV_BOOT_FILES=(
    "opensbi/fw_jump.bin"
    "boot/minix-riscv64/kernel"
    "boot/minix-riscv64/ramdisk.img"
    "boot/grub/grub.cfg"
)

create_boot_structure() {
    local boot_dir="$WORK_DIR/boot"
    local grub_dir="$boot_dir/grub"
    
    # Create boot directory structure
    mkdir -p "$boot_dir"
    mkdir -p "$grub_dir"
    mkdir -p "$boot_dir/riscv64"
    
    # Copy RISC-V boot files
    for file in "${RISCV_BOOT_FILES[@]}"; do
        if [ -f "$MINIX_ROOT/$file" ]; then
            cp "$MINIX_ROOT/$file" "$boot_dir/"
        fi
    done
    
    # Create GRUB configuration for RISC-V
    create_grub_config "$grub_dir/grub.cfg"
}

create_grub_config() {
    local config_file=$1
    
    cat > "$config_file" << 'EOF'
# MINIX RISC-V GRUB configuration
set timeout=10
set default=0

menuentry "MINIX RISC-V 64-bit" {
    set root='(cd0)'
    
    # Load OpenSBI firmware
    load_env opensbi/fw_jump.bin
    
    # Load MINIX kernel
    linux /boot/minix-riscv64/kernel \
          console=ttyS0 \
          root=cd0 \
          boot=cdrom
    
    # Load initial ramdisk
    initrd /boot/minix-riscv64/ramdisk.img
}

menuentry "MINIX RISC-V (Recovery Mode)" {
    set root='(cd0)'
    
    load_env opensbi/fw_jump.bin
    
    linux /boot/minix-riscv64/kernel \
          console=ttyS0 \
          root=cd0 \
          boot=cdrom \
          single
    
    initrd /boot/minix-riscv64/ramdisk.img
}
EOF
}
```

### mkdisk.sh - Disk Image Creation
Advanced disk image creation with partitioning, filesystem creation, and boot sector setup.

**Key Features:**
- GPT partition table for RISC-V
- Multiple filesystem support (MFS, EXT2)
- Boot sector configuration
- Automated installation preparation
- Size optimization

**Disk Layout for RISC-V:**
```bash
# RISC-V disk layout
create_disk_layout() {
    local disk_image=$1
    local disk_size=$2
    
    # Create disk image
    dd if=/dev/zero of="$disk_image" bs=1M count="$disk_size"
    
    # Create GPT partition table
    parted -s "$disk_image" mklabel gpt
    
    # EFI System Partition (for future UEFI support)
    parted -s "$disk_image" mkpart EFI fat32 1MB 100MB
    parted -s "$disk_image" set 1 esp on
    
    # MINIX boot partition
    parted -s "$disk_image" mkpart MINIX-Boot ext2 100MB 500MB
    parted -s "$disk_image" set 2 legacy_boot on
    
    # MINIX root partition
    parted -s "$disk_image" mkpart MINIX-Root mfs 500MB 4096MB
    
    # MINIX home partition
    parted -s "$disk_image" mkpart MINIX-Home mfs 4096MB 100%
}
```

### mkboot.sh - Boot Image Creation
Specialized boot image creation for RISC-V with OpenSBI integration.

**RISC-V Boot Process:**
```
OpenSBI (M-mode) → MINIX Kernel (S-mode) → User Processes (U-mode)
```

**Boot Image Structure:**
```bash
# RISC-V boot image creation
create_riscv_boot_image() {
    local boot_image=$1
    local kernel=$2
    local ramdisk=$3
    
    # Create boot image directory
    local boot_dir=$(mktemp -d)
    
    # Copy OpenSBI firmware
    cp "opensbi/fw_jump.bin" "$boot_dir/"
    
    # Copy MINIX kernel
    cp "$kernel" "$boot_dir/minix-riscv64"
    
    # Copy initial ramdisk
    cp "$ramdisk" "$boot_dir/ramdisk.img"
    
    # Create boot configuration
    cat > "$boot_dir/boot.conf" << EOF
# MINIX RISC-V boot configuration
OPENSBI=fw_jump.bin
KERNEL=minix-riscv64
RAMDISK=ramdisk.img
BOOTARGS=console=ttyS0 root=/dev/sda2
EOF
    
    # Create boot image
    (cd "$boot_dir" && tar czf "$boot_image" *)
    
    # Cleanup
    rm -rf "$boot_dir"
}
```

### install.sh - Installation Script
Automated installation script for MINIX/RISC-V with user interaction and error handling.

**Installation Flow:**
```bash
# MINIX installation process
main_installation() {
    # Welcome and license
    show_welcome_screen
    
    # Hardware detection
    detect_riscv_hardware
    
    # Disk selection and partitioning
    select_installation_disk
    create_partitions
    
    # Filesystem creation
    create_filesystems
    
    # System installation
    install_minix_system
    
    # Boot loader installation
    install_boot_loader
    
    # Configuration
    configure_system
    
    # Completion
    show_completion_screen
}

detect_riscv_hardware() {
    echo "Detecting RISC-V hardware..."
    
    # Detect CPU features
    if [ -f /proc/cpuinfo ]; then
        RISCV_EXTENSIONS=$(grep "isa" /proc/cpuinfo | cut -d: -f2 | tr -d ' ')
        echo "RISC-V ISA: $RISCV_EXTENSIONS"
    fi
    
    # Detect memory size
    if [ -f /proc/meminfo ]; then
        MEMORY_KB=$(grep MemTotal /proc/meminfo | awk '{print $2}')
        MEMORY_MB=$((MEMORY_KB / 1024))
        echo "Memory: ${MEMORY_MB}MB"
    fi
    
    # Detect available disks
    AVAILABLE_DISKS=$(ls /dev/sd* 2>/dev/null | grep -v '[0-9]$')
    echo "Available disks: $AVAILABLE_DISKS"
}
```

## RISC-V 64-bit Specific Features

### RISC-V Platform Detection
Advanced hardware detection for RISC-V platforms:

```bash
# RISC-V hardware detection
detect_riscv_platform() {
    local platform_info=""
    
    # Check for QEMU virt platform
    if [ -f /proc/device-tree/model ]; then
        local dt_model=$(cat /proc/device-tree/model)
        if [[ "$dt_model" == *"QEMU"* ]]; then
            platform_info="qemu-virt"
        fi
    fi
    
    # Check for SiFive platform
    if [ -f /proc/device-tree/compatible ]; then
        local dt_compat=$(cat /proc/device-tree/compatible)
        if [[ "$dt_compat" == *"sifive"* ]]; then
            platform_info="sifive-hifive"
        fi
    fi
    
    # Default to generic RISC-V
    if [ -z "$platform_info" ]; then
        platform_info="riscv64-generic"
    fi
    
    echo "$platform_info"
}
```

### RISC-V Memory Layout
Optimized memory layout for RISC-V platforms:

```bash
# RISC-V memory layout configuration
configure_riscv_memory() {
    local platform=$1
    
    case $platform in
        qemu-virt)
            # QEMU virt platform memory map
            MEMORY_START=0x80000000
            MEMORY_SIZE=0x10000000    # 256MB default
            KERNEL_LOAD=0x80200000
            DEVICE_TREE=0x82200000
            INITRD_LOAD=0x83000000
            ;;
            
        sifive-hifive)
            # SiFive HiFive Unleashed memory map
            MEMORY_START=0x80000000
            MEMORY_SIZE=0x80000000    # 2GB
            KERNEL_LOAD=0x80200000
            DEVICE_TREE=0x88000000
            INITRD_LOAD=0x89000000
            ;;
            
        *)
            # Generic RISC-V memory map
            MEMORY_START=0x80000000
            MEMORY_SIZE=0x08000000    # 128MB
            KERNEL_LOAD=0x80200000
            ;;
    esac
}
```

### RISC-V Boot Configuration
Specialized boot configuration for RISC-V architecture:

```
# RISC-V boot configuration (boot.conf)
[riscv-boot]
# Platform identification
platform=qemu-virt
cpu-count=4
isa=rv64imafdc

# Memory configuration
memory-start=0x80000000
memory-size=0x10000000

# Kernel location
kernel-addr=0x80200000
kernel-size=0x00400000

# Initial ramdisk
initrd-addr=0x83000000
initrd-size=0x01000000

# Device tree
fdt-addr=0x82200000

# Boot arguments
bootargs=console=ttyS0 root=/dev/sda2 rw

# OpenSBI configuration
opensbi-addr=0x80000000
opensbi-version=1.0
```

## Advanced Release Features

### Package Set Management
Sophisticated package system for MINIX releases:

```bash
# Package set definition (sets.list)
define_package_sets() {
    cat > "$SETS_FILE" << 'EOF'
# MINIX RISC-V Package Sets

[base]
description=MINIX base system
packages=
    base/minix-base
    base/minix-kernel
    base/minix-modules
    base/minix-libc
    base/minix-commands
    base/minix-servers

[development]
description=Development tools and libraries
packages=
    devel/gcc
    devel/binutils
    devel/make
    devel/gdb
    devel/headers
    devel/libraries

[network]
description=Network stack and utilities
packages=
    net/lwip
    net/network-utils
    net/ssh
    net/dhcp

[riscv-specific]
description=RISC-V specific packages
packages=
    riscv/opensbi
    riscv/riscv-toolchain
    riscv/device-tree

[documentation]
description=Documentation and manual pages
packages=
    docs/minix-man-pages
    docs/minix-guide
    docs/riscv-manual
EOF
}
```

### System Configuration Management
Automated system configuration for release images:

```bash
# System configuration generation (system.conf)
generate_system_config() {
    local config_file=$1
    local platform=$2
    
    cat > "$config_file" << EOF
# MINIX RISC-V System Configuration
# Auto-generated for $platform

# Essential system services
service kernel {
    system {
        kernel;
    }
}

service pm {
    system {
        ipc SYSTEM RS VM VFS TTY;
        privctl;
        irqctl;
    }
}

service vfs {
    system {
        ipc SYSTEM PM VM RS TTY MEMORY;
        memmap;
    }
}

service vm {
    system {
        ipc SYSTEM PM VFS RS MEMORY;
        memmap;
    }
}

service rs {
    system {
        ipc SYSTEM PM VM VFS TTY MEMORY;
        readclock;
    }
}

# RISC-V specific services
service tty {
    system {
        ipc SYSTEM PM VFS RS;
        irq 10;  # UART interrupt
        io 0x10000000:0x100;  # UART base address
    }
}

service memory {
    system {
        ipc SYSTEM PM VM RS;
        memmap;
    }
}

# Network services
service lwip {
    system {
        ipc SYSTEM PM VFS;
        irq 11;  # Network interrupt
    }
}

# Device drivers
service drv_memory {
    system {
        ipc SYSTEM PM VM VFS;
        memmap;
    }
}

EOF
}
```

### Release Validation
Comprehensive validation of release images:

```bash
# Release validation
validate_release() {
    local release_dir=$1
    local errors=0
    
    echo "Validating MINIX release in $release_dir"
    
    # Check required files
    required_files=(
        "boot/kernel"
        "boot/ramdisk.img"
        "boot/opensbi/fw_jump.bin"
        "etc/system.conf"
        "bin/sh"
        "sbin/init"
    )
    
    for file in "${required_files[@]}"; do
        if [ ! -f "$release_dir/$file" ]; then
            echo "ERROR: Missing required file: $file"
            errors=$((errors + 1))
        fi
    done
    
    # Validate kernel image
    if [ -f "$release_dir/boot/kernel" ]; then
        echo "Validating kernel image..."
        riscv64-unknown-elf-readelf -h "$release_dir/boot/kernel" > /dev/null
        if [ $? -ne 0 ]; then
            echo "ERROR: Invalid kernel image"
            errors=$((errors + 1))
        fi
    fi
    
    # Check file permissions
    check_file_permissions "$release_dir"
    
    # Validate system configuration
    if [ -f "$release_dir/etc/system.conf" ]; then
        validate_system_conf "$release_dir/etc/system.conf"
    fi
    
    echo "Validation complete: $errors errors found"
    return $errors
}
```

## Integration with Build System

### Release Build Integration
```makefile
# MINIX release tools Makefile
PROG=   release-tools

# Release targets
release-iso: build_iso.sh
	./build_iso.sh -o minix-riscv64.iso

release-disk: mkdisk.sh
	./mkdisk.sh -s 8G -o minix-riscv64.img

release-boot: mkboot.sh
	./mkboot.sh -k kernel -r ramdisk.img -o boot.img

release-all: release-iso release-disk release-boot
	./validate_release.sh release/

# Installation target
install: install.sh
	./install.sh -t /target/directory

.include <bsd.prog.mk>
```

### Cross-Architecture Release Support
```bash
# Multi-architecture release building
build_all_releases() {
    local architectures="riscv64 arm i386"
    local release_dir="releases"
    
    mkdir -p "$release_dir"
    
    for arch in $architectures; do
        echo "Building release for $arch..."
        
        # Build MINIX for this architecture
        ./build.sh -m "evb$arch" -j$(nproc) distribution
        
        # Create architecture-specific release
        cd "releasetools/$arch"
        ./build_iso.sh -o "$release_dir/minix-$arch.iso"
        ./mkdisk.sh -o "$release_dir/minix-$arch.img"
        cd ../..
        
        # Validate release
        validate_release "$release_dir/$arch"
    done
    
    echo "All releases built successfully"
}
```

## Testing and Quality Assurance

### Release Testing Framework
```bash
# Automated release testing
test_release() {
    local release_image=$1
    local test_results="test_results.log"
    
    echo "Testing release: $release_image"
    
    # Boot test
    echo "Boot test..." >> "$test_results"
    if boot_test "$release_image"; then
        echo "BOOT: PASSED" >> "$test_results"
    else
        echo "BOOT: FAILED" >> "$test_results"
        return 1
    fi
    
    # Service test
    echo "Service test..." >> "$test_results"
    if service_test "$release_image"; then
        echo "SERVICES: PASSED" >> "$test_results"
    else
        echo "SERVICES: FAILED" >> "$test_results"
    fi
    
    # Performance test
    echo "Performance test..." >> "$test_results"
    if performance_test "$release_image"; then
        echo "PERFORMANCE: PASSED" >> "$test_results"
    else
        echo "PERFORMANCE: FAILED" >> "$test_results"
    fi
    
    echo "Release testing complete. Results in $test_results"
}

# Boot test validation
boot_test() {
    local image=$1
    local timeout=60
    
    # Start QEMU with test image
    qemu-system-riscv64 \
        -machine virt \
        -cpu rv64 \
        -m 256M \
        -drive file="$image",format=raw \
        -serial stdio \
        -nographic > boot.log 2>&1 &
    
    local qemu_pid=$!
    
    # Wait for successful boot
    local elapsed=0
    while [ $elapsed -lt $timeout ]; do
        if grep -q "MINIX boot complete" boot.log; then
            kill $qemu_pid
            return 0
        fi
        
        if grep -q "Kernel panic" boot.log; then
            kill $qemu_pid
            return 1
        fi
        
        sleep 1
        elapsed=$((elapsed + 1))
    done
    
    kill $qemu_pid
    return 1
}
```

## Common Release Issues

### Build Issues
- **Missing dependencies**: Ensure all build dependencies are installed
- **Cross-compilation errors**: Verify toolchain installation
- **Disk space exhaustion**: Check available disk space
- **Permission errors**: Verify file permissions and ownership

### Runtime Issues
- **Boot failures**: Check boot loader configuration
- **Service startup failures**: Validate system.conf
- **Hardware detection issues**: Verify device tree configuration
- **Memory allocation failures**: Check memory layout

### Installation Issues
- **Partition creation failures**: Check disk permissions
- **Filesystem creation errors**: Verify filesystem tools
- **Package installation failures**: Check package integrity
- **Configuration errors**: Validate configuration syntax

## Future Enhancements

### Planned Release Features
- Cloud-based release building
- Automated release testing
- Delta update generation
- Container-based releases
- Live system creation

### Release Process Evolution
- Continuous release integration
- Automated quality gates
- Community release validation
- Release rollback capabilities
- Enhanced security validation

These release tools represent the complete pipeline for creating distributable MINIX releases, providing sophisticated automation for the complex process of packaging a microkernel operating system for multiple platforms while ensuring quality, consistency, and reliability of the final release artifacts.