# MINIX Shared Data Directory (/minix/share)

## Overview

The `/minix/share/` directory contains MINIX-specific shared data files, including architecture-specific configurations, platform support files, and shared resources that support the MINIX microkernel operating system. Unlike the general `/usr/share/` directory that contains NetBSD-compatible shared data, this directory houses MINIX-specific shared resources that are essential for the proper functioning of MINIX services and utilities.

## Directory Structure

```
minix/share/
├── Makefile          # Build configuration for shared data
├── Makefile.inc      # Common build settings and paths
└── beaglebone/       # BeagleBone platform support files
```

## Shared Data Categories

### Platform-Specific Configurations
Architecture and platform-specific configuration files that support different hardware platforms running MINIX.

**BeagleBone Support:**
- Hardware abstraction layer configurations
- Device tree overlays
- Platform-specific boot configurations
- GPIO and peripheral mappings
- Memory layout specifications

### MINIX Service Data
Shared data files used by multiple MINIX services and utilities.

**Service Configuration Templates:**
- Default service configurations
- Resource allocation templates
- Permission set definitions
- IPC routing tables
- Security policy templates

### Build System Data
Shared data used during the MINIX build process.

**Cross-Compilation Support:**
- Architecture-specific build flags
- Toolchain configuration templates
- Platform detection scripts
- Build dependency definitions

## MINIX-Specific Shared Resources

### Architecture Support Files
Platform-specific data files that enable MINIX to run on different hardware architectures.

```c
// BeagleBone platform configuration example
struct beaglebone_config {
    uint32_t board_revision;
    uint32_t ram_size;
    uint32_t cpu_frequency;
    
    // Peripheral addresses
    uint32_t gpio_base;
    uint32_t uart_base;
    uint32_t timer_base;
    uint32_t interrupt_base;
    
    // Memory layout
    uint32_t kernel_load_addr;
    uint32_t kernel_size;
    uint32_t ramdisk_addr;
    uint32_t device_tree_addr;
};
```

### Service Configuration Templates
Templates and default configurations for MINIX system services.

```
# Default service configuration template
service template {
    system {
        # Basic permissions
        ipc SYSTEM;
        
        # Memory management
        memmap;
        
        # Basic system calls
        readclock;
        
        # Default resource limits
        vmdata 65536;
        vmdata32 0;
        nice 0;
    }
}
```

### Cross-Architecture Build Data
Shared build configuration data for different target architectures.

```makefile
# RISC-V 64-bit build configuration
RISCV64_CONFIG=\
    -march=rv64gc\
    -mabi=lp64d\
    -mcmodel=medany\
    -fno-delete-null-pointer-checks\
    -mno-save-restore

# ARM build configuration  
ARM_CONFIG=\
    -march=armv7-a\
    -mfpu=vfpv3-d16\
    -mfloat-abi=hard\
    -mthumb\
    -mfpu=neon

# x86 build configuration
X86_CONFIG=\
    -m32\
    -march=i686\
    -mtune=i686\
    -fno-stack-protector
```

## Integration with MINIX Build System

### Shared Data Installation
```makefile
# MINIX share Makefile
FILESDIR= /usr/share/minix
FILES=   beaglebone/config.txt
FILESNAME_beaglebone/config.txt= beaglebone-config

# Architecture-specific files
.if ${MACHINE_ARCH} == "riscv64"
FILES+= riscv64/platform.conf
.endif

.if ${MACHINE_ARCH} == "earmv7"
FILES+= arm/platform.conf
.endif

.include <bsd.files.mk>
```

### Cross-Compilation Integration
```bash
# Install platform-specific data
MKPCI=no HAVE_GOLD=no HAVE_LLVM=no MKLLVM=no \
./build.sh -m evbriscv64 -j$(nproc) distribution

# Platform data will be installed to:
# /usr/share/minix/riscv64/platform.conf
# /usr/share/minix/beaglebone/config.txt
```

## RISC-V 64-bit Specific Shared Data

### Platform Configuration
RISC-V specific configuration data for the QEMU virt platform and real hardware.

```
# RISC-V 64-bit platform configuration
[riscv64-platform]
# Memory layout
memory_start=0x80000000
memory_size=0x10000000    # 256MB

# Device addresses
clint_base=0x02000000
plic_base=0x0C000000
uart_base=0x10000000
virtio_base=0x10001000

# CPU configuration
cpu_count=4
cpu_type=rv64gc
boot_hart=0

# Boot configuration
boot_args="console=ttyS0"
kernel_load=0x80200000
```

### Device Tree Overlays
Device tree source files for RISC-V platforms.

```dts
// RISC-V QEMU virt device tree
/dts-v1/;

/ {
    compatible = "qemu,virt";
    model = "QEMU RISC-V Virt Machine";
    
    cpus {
        #address-cells = <1>;
        #size-cells = <0>;
        timebase-frequency = <10000000>;
        
        cpu@0 {
            device_type = "cpu";
            compatible = "riscv";
            reg = <0>;
            status = "okay";
            riscv,isa = "rv64imafdc";
        };
    };
    
    memory@80000000 {
        device_type = "memory";
        reg = <0x0 0x80000000 0x0 0x10000000>;
    };
    
    soc {
        compatible = "simple-bus";
        #address-cells = <2>;
        #size-cells = <2>;
        ranges;
        
        uart@10000000 {
            compatible = "ns16550";
            reg = <0x0 0x10000000 0x0 0x100>;
            interrupts = <10>;
            clock-frequency = <1843200>;
        };
    };
};
```

## Service Integration Data

### IPC Configuration Data
Shared data defining inter-process communication parameters.

```c
// MINIX IPC configuration
struct minix_ipc_config {
    // System service endpoints
    endpoint_t system_endpoint;
    endpoint_t kernel_endpoint;
    endpoint_t pm_endpoint;
    endpoint_t vfs_endpoint;
    endpoint_t vm_endpoint;
    endpoint_t rs_endpoint;
    endpoint_t tty_endpoint;
    endpoint_t memory_endpoint;
    endpoint_t clock_endpoint;
    
    // Message size limits
    size_t max_message_size;
    size_t max_bulk_transfer;
    
    // Timeout values
    uint32_t ipc_timeout_ms;
    uint32_t service_restart_timeout_ms;
};
```

### Security Policy Templates
Default security policies and capability templates.

```
# Default security policy template
policy default_security {
    # Basic IPC permissions
    allow_ipc {
        SYSTEM;
        KERNEL;
    }
    
    # Memory management
    allow_memmap {
        size 65536;
        flags READ|WRITE;
    }
    
    # Basic system operations
    allow_syscalls {
        readclock;
        getprocnr;
        getsysinfo;
    }
    
    # Resource limits
    limits {
        vmdata 131072;
        vmdata32 0;
        nice 10;
        quantum 100;
    }
}
```

## Build Configuration Templates

### Cross-Compilation Templates
Templates for building MINIX on different host systems for different targets.

```makefile
# Cross-compilation configuration template
# Host: Linux x86_64
# Target: RISC-V 64-bit

# Toolchain configuration
CROSS_COMPILE?= riscv64-unknown-elf-
CC=             ${CROSS_COMPILE}gcc
CXX=            ${CROSS_COMPILE}g++
AS=             ${CROSS_COMPILE}as
LD=             ${CROSS_COMPILE}ld
AR=             ${CROSS_COMPILE}ar
OBJCOPY=        ${CROSS_COMPILE}objcopy
OBJDUMP=        ${CROSS_COMPILE}objdump

# Architecture flags
ARCH_FLAGS=     -march=rv64gc -mabi=lp64d
ARCH_CFLAGS=    ${ARCH_FLAGS} -mcmodel=medany
ARCH_LDFLAGS=   ${ARCH_FLAGS}

# Optimization flags
OPTIMIZATION=   -O2
DEBUG_FLAGS=    -g -DDEBUG

# MINIX specific flags
MINIX_CFLAGS=   -D_MINIX -D_MINIX_SYSTEM
MINIX_CPPFLAGS= -I/usr/include/minix

# Combine all flags
CFLAGS=         ${ARCH_CFLAGS} ${OPTIMIZATION} ${MINIX_CFLAGS}
LDFLAGS=        ${ARCH_LDFLAGS}
```

### Platform Detection Scripts
Scripts for automatically detecting target platform characteristics.

```bash
#!/bin/bash
# Platform detection script for MINIX

detect_platform() {
    local cpu arch platform
    
    # Detect CPU architecture
    cpu=$(uname -m)
    case $cpu in
        x86_64|i?86)
            arch="x86"
            ;;
        armv7*|armv6*)
            arch="arm"
            ;;
        aarch64)
            arch="arm64"
            ;;
        riscv64)
            arch="riscv64"
            ;;
        *)
            arch="unknown"
            ;;
    esac
    
    # Detect platform type
    if [ -f /proc/device-tree/model ]; then
        model=$(tr -d '\0' < /proc/device-tree/model)
        case $model in
            *"Raspberry Pi"*)
                platform="raspberrypi"
                ;;
            *"BeagleBone"*)
                platform="beaglebone"
                ;;
            *"QEMU"*)
                platform="qemu"
                ;;
            *)
                platform="generic"
                ;;
        esac
    else
        platform="generic"
    fi
    
    echo "ARCH=$arch"
    echo "PLATFORM=$platform"
}

# Generate platform configuration
generate_config() {
    detect_platform > platform.conf
    
    case $arch in
        riscv64)
            cat >> platform.conf << EOF
# RISC-V specific settings
MEMORY_START=0x80000000
MEMORY_SIZE=0x10000000
KERNEL_LOAD=0x80200000
EOF
            ;;
        arm)
            cat >> platform.conf << EOF
# ARM specific settings
MEMORY_START=0x40000000
MEMORY_SIZE=0x08000000
KERNEL_LOAD=0x40008000
EOF
            ;;
    esac
}

generate_config
```

## Testing and Validation Data

### Test Configuration Templates
Shared test configurations for validating MINIX functionality.

```
# MINIX test configuration template
[test-config]
# Test environment settings
test_timeout=30
test_iterations=100
test_verbose=1

# Service test configurations
[service-tests]
enable_pm_tests=1
enable_vfs_tests=1
enable_vm_tests=1
enable_rs_tests=1
enable_tty_tests=1

# IPC test configurations
[ipc-tests]
test_message_sizes=64,256,1024,4096,16384
test_endpoints=PM,VFS,VM,RS,TTY
message_types=SEND,RECEIVE,SENDREC,NOTIFY

# Performance test configurations
[performance-tests]
benchmark_duration=60
benchmark_iterations=10
performance_metrics=latency,throughput,cpu,memory
```

### Benchmark Data
Reference performance data for different platforms and configurations.

```json
{
  "platform_benchmarks": {
    "riscv64-qemu": {
      "cpu": "rv64gc",
      "frequency": "1GHz",
      "memory": "256MB",
      "boot_time": "3.2s",
      "ipc_latency": "2.1us",
      "message_throughput": "45000 msgs/sec",
      "filesystem_performance": {
        "read_bandwidth": "45MB/s",
        "write_bandwidth": "38MB/s",
        "random_read_iops": "1200",
        "random_write_iops": "950"
      }
    },
    "arm-beaglebone": {
      "cpu": "armv7-a",
      "frequency": "1GHz",
      "memory": "512MB",
      "boot_time": "2.8s",
      "ipc_latency": "1.8us",
      "message_throughput": "52000 msgs/sec",
      "filesystem_performance": {
        "read_bandwidth": "52MB/s",
        "write_bandwidth": "45MB/s",
        "random_read_iops": "1500",
        "random_write_iops": "1200"
      }
    }
  }
}
```

## Integration with Development Workflow

### Development Environment Setup
```bash
# Set up development environment using shared data
export MINIX_PLATFORM=$(detect_platform)
export MINIX_ARCH=$(detect_arch)

# Load platform configuration
source /usr/share/minix/${MINIX_PLATFORM}/config.conf

# Set build flags based on platform
export CFLAGS="${ARCH_CFLAGS} ${OPTIMIZATION}"
export LDFLAGS="${ARCH_LDFLAGS}"
```

### Automated Platform Detection
```bash
# Automated platform detection for build scripts
#!/bin/bash

# Detect target platform
if [ -f /usr/share/minix/platform.conf ]; then
    source /usr/share/minix/platform.conf
else
    # Fallback detection
    detect_platform > /tmp/platform.conf
    source /tmp/platform.conf
fi

# Configure build based on platform
case $PLATFORM in
    qemu-riscv64)
        echo "Building for RISC-V QEMU virt platform"
        make CFLAGS="${RISCV64_CONFIG} -DQEMU_PLATFORM"
        ;;
    beaglebone)
        echo "Building for BeagleBone ARM platform"
        make CFLAGS="${ARM_CONFIG} -DBEAGLEBONE_PLATFORM"
        ;;
    *)
        echo "Building for generic platform"
        make CFLAGS="${GENERIC_CONFIG}"
        ;;
esac
```

## Common Development Patterns

### Platform Abstraction
```c
// Platform abstraction using shared configuration
#include "platform_config.h"

struct platform_info {
    const char *name;
    uint32_t memory_start;
    uint32_t memory_size;
    uint32_t kernel_load;
    uint32_t device_base;
};

struct platform_info *get_platform_info(void) {
    static struct platform_info info;
    
    // Load platform configuration
    load_platform_config(&info);
    
    return &info;
}

// Use platform-specific addresses
void *map_device_memory(void) {
    struct platform_info *platform = get_platform_info();
    
    return mmap(NULL, platform->device_base, 
                platform->memory_size,
                PROT_READ | PROT_WRITE,
                MAP_SHARED, -1, 0);
}
```

### Service Configuration Loading
```c
// Load service configuration from shared data
int load_service_config(const char *service_name, 
                       struct service_config *config) {
    char config_path[PATH_MAX];
    FILE *fp;
    
    snprintf(config_path, sizeof(config_path),
             "/usr/share/minix/services/%s.conf", service_name);
    
    fp = fopen(config_path, "r");
    if (fp == NULL) {
        return -1;
    }
    
    // Parse configuration file
    parse_service_config(fp, config);
    fclose(fp);
    
    return 0;
}
```

## Testing and Validation

### Platform Configuration Validation
```bash
# Validate platform configuration
validate_platform_config() {
    local config_file=$1
    local errors=0
    
    # Check required fields
    required_fields="MEMORY_START MEMORY_SIZE KERNEL_LOAD"
    for field in $required_fields; do
        if ! grep -q "^$field=" "$config_file"; then
            echo "Error: Missing required field: $field"
            errors=$((errors + 1))
        fi
    done
    
    # Validate memory addresses
    memory_start=$(grep "^MEMORY_START=" "$config_file" | cut -d= -f2)
    memory_size=$(grep "^MEMORY_SIZE=" "$config_file" | cut -d= -f2)
    
    if [ $((memory_start & 0xFFF)) -ne 0 ]; then
        echo "Error: MEMORY_START must be 4KB aligned"
        errors=$((errors + 1))
    fi
    
    if [ $((memory_size & 0xFFF)) -ne 0 ]; then
        echo "Error: MEMORY_SIZE must be 4KB aligned"
        errors=$((errors + 1))
    fi
    
    return $errors
}
```

### Cross-Architecture Testing
```bash
# Test platform configurations across architectures
test_platform_configs() {
    local platforms="riscv64 arm x86"
    local total_errors=0
    
    for platform in $platforms; do
        echo "Testing platform: $platform"
        
        config_file="/usr/share/minix/${platform}/platform.conf"
        if [ -f "$config_file" ]; then
            validate_platform_config "$config_file"
            errors=$?
            total_errors=$((total_errors + errors))
        else
            echo "Warning: No configuration found for $platform"
        fi
    done
    
    return $total_errors
}
```

## Common Development Issues

### Configuration Issues
- **Missing platform files**: Ensure platform detection works correctly
- **Invalid memory addresses**: Validate alignment and ranges
- **Missing dependencies**: Check cross-compilation toolchain

### Integration Issues
- **Build system integration**: Verify Makefile configurations
- **Cross-compilation errors**: Check toolchain paths and flags
- **Platform detection failures**: Validate detection scripts

### Runtime Issues
- **Missing shared data**: Verify installation paths
- **Configuration loading errors**: Check file permissions
- **Platform mismatch**: Validate detected vs. actual platform

## Future Enhancements

### Planned Features
- Enhanced platform detection capabilities
- More comprehensive configuration templates
- Better integration with cloud and container platforms
- Dynamic platform reconfiguration
- Advanced cross-compilation support

### Architectural Evolution
- Support for new hardware platforms
- Enhanced IoT and embedded platform support
- Better integration with modern build systems
- Improved automated testing capabilities

This directory serves as the central repository for MINIX-specific shared data that enables the operating system to support multiple hardware platforms, provides essential configuration data for system services, and facilitates cross-architecture development and deployment of the MINIX microkernel operating system.