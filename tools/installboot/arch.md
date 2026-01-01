# MINIX 3 installboot Tool - Architecture Documentation

## Overview

The `tools/installboot/` directory provides the `installboot` utility, a critical system administration tool for installing and configuring boot blocks on various architectures and filesystem types. This tool is essential for making MINIX systems bootable by writing the appropriate boot code to disk devices, handling the diverse requirements of different hardware platforms and boot methodologies.

## Purpose and Role in Build System

`installboot` serves as the bridge between MINIX kernel images and bootable disk devices. It analyzes target disks, determines appropriate boot block formats, and installs the necessary boot code to enable system startup. For cross-compilation scenarios, particularly RISC-V development, it ensures that boot blocks are correctly configured for the target architecture regardless of the host build environment.

## Directory Structure

```
tools/installboot/
├── Makefile                     # Build configuration for host tool
└── obj/                        # Build artifacts directory
```

## Source Architecture

### Primary Source Location
The actual `installboot` source code resides in:
```
usr.sbin/installboot/
├── installboot.c               # Main implementation (15KB)
├── installboot.h               # Main header file (7KB)
├── installboot_nbsd.8          # Manual page (21KB)
├── bbinfo.c                    # Boot block information
├── ext2fs.c                    # EXT2 filesystem support
├── ffs.c                       # FFS filesystem support
├── minixfs3.c                  # MINIX v3 filesystem support
├── fstypes.c                   # Filesystem type detection
├── install_master.c            # Master boot record handling
├── machines.c                  # Machine type detection
├── sum.c                       # Checksum utilities
├── arch/                       # Architecture-specific modules
└── obj/                        # Object files
```

### Architecture-Specific Support
The `arch/` directory contains platform-specific boot code installation:
```
arch/
├── alpha.c                     # Alpha architecture support
├── amiga.c                     # Amiga platform support
├── emips.c                     # MIPS embedded systems
├── hp300.c                     # HP 300 series
├── hppa.c                      # HP PA-RISC
├── i386.c                      # Intel x86 (32-bit)
├── landisk.c                   # LANDISK platform
├── macppc.c                    # PowerPC Macintosh
├── pmax.c                      # DECstation/PMAX
├── sparc.c                     # SPARC 32-bit
├── sparc64.c                   # SPARC 64-bit
├── vax.c                       # VAX architecture
└── [other architectures]       # Additional platform support
```

**Note:** RISC-V 64-bit support is not yet implemented but would follow the same pattern as other architectures.

## Core Functionality

### Boot Block Installation Process

1. **Device Analysis** - Examine target disk device characteristics
2. **Filesystem Detection** - Identify filesystem type (FFS, EXT2, MINIX, etc.)
3. **Architecture Detection** - Determine target hardware platform
4. **Boot Code Selection** - Choose appropriate boot block for architecture
5. **Installation** - Write boot code with proper checksums and validation
6. **Verification** - Confirm successful installation and bootability

### Supported Filesystems

**Traditional UNIX Filesystems:**
- **FFS** (Fast File System) - BSD traditional filesystem
- **EXT2** (Second Extended Filesystem) - Linux compatible
- **MINIX v3** - Native MINIX filesystem

**Filesystem-Specific Features:**
- Boot block location identification
- Superblock validation
- Block size detection
- Partition table handling

### Architecture Support Matrix

| Architecture | Status | Boot Method | Notes |
|--------------|--------|-------------|---------|
| i386         | ✓      | MBR/BIOS    | Traditional PC boot |
| amd64        | ✓      | MBR/UEFI    | 64-bit PC systems |
| sparc/sparc64| ✓      | OpenBoot    | Sun/SPARC systems |
| alpha        | ✓      | SRM/ARC     | DEC Alpha systems |
| powerpc      | ✓      | OpenFirmware| Apple/IBM systems |
| RISC-V       | ✗      | OpenSBI     | **Not yet implemented** |

## Implementation Details

### Main Components (`installboot.c`)

**Core Functions:**
- `main()` - Command-line parsing and main flow control
- `process_disk()` - Analyze and prepare target device
- `install_bootblocks()` - Write boot code to device
- `verify_installation()` - Confirm successful installation

**Data Structures:**
- `struct ib_params` - Installation parameters and options
- `struct disk_info` - Target device characteristics
- `struct fs_info` - Filesystem-specific information
- `struct arch_info` - Architecture-specific data

### Architecture Interface (`installboot.h`)

**Architecture Operations:**
```c
struct arch_ops {
    const char *name;
    int (*match)(struct disk_info *);
    int (*install)(int, struct disk_info *, const char *);
    int (*clear)(int, struct disk_info *);
    int (*info)(int, struct disk_info *);
};
```

**Filesystem Operations:**
```c
struct fs_ops {
    const char *name;
    int (*match)(struct disk_info *);
    int (*read)(int, struct disk_info *);
    daddr_t (*bootblock)(struct disk_info *);
    daddr_t (*superblock)(struct disk_info *);
};
```

### Boot Block Formats

**Master Boot Record (MBR):**
- 512-byte boot sector
- Partition table support
- BIOS-compatible boot code
- Checksum validation

**Filesystem-Specific Boot Blocks:**
- FFS: Boot block in superblock area
- EXT2: Reserved boot sectors
- MINIX: Dedicated boot partition

## Cross-Compilation Architecture

### Host Tool Design
`installboot` is built as a **host tool**:
- Compiled for the development host architecture
- Executes during installation and deployment
- Not installed on the target MINIX system

### Build System Integration

**Makefile Configuration:**
```makefile
HOSTPROGNAME= ${_TOOL_PREFIX}installboot
HOST_SRCDIR= usr.sbin/installboot
HOST_CPPFLAGS+= -I. -I${.CURDIR} -I${TOOLDIR}/include/nbinclude
.include "${.CURDIR}/../Makefile.nbincludes"
.include "${.CURDIR}/../Makefile.host"
```

**Platform Detection:**
- Automatic architecture detection based on target device
- Filesystem type identification
- Boot method selection
- Cross-platform compatibility

### RISC-V Cross-Compilation Support

**Current Status:**
- RISC-V 64-bit architecture support not yet implemented
- Would require `arch/riscv64.c` module
- OpenSBI boot methodology integration needed

**Proposed RISC-V Implementation:**
```c
/* arch/riscv64.c */
static int
riscv64_match(struct disk_info *di)
{
    return (di->arch == EM_RISCV && di->bits == 64);
}

static int
riscv64_install(int fd, struct disk_info *di, const char *bootfile)
{
    /* OpenSBI + MINIX kernel boot block */
    /* S-mode entry point setup */
    /* Device tree blob handling */
}
```

## Command-Line Interface

### Basic Usage
```bash
installboot [-nv] [-m machine] [-v[ersion]] device bootprog
```

**Options:**
- `-n` - Dry run (don't write to device)
- `-v` - Verbose operation
- `-m machine` - Override machine type detection
- `-V` - Display version information

### Advanced Options
```bash
installboot [-F fstype] [-B bbinfo] [-o options] device bootprog
```

**Advanced Options:**
- `-F fstype` - Specify filesystem type
- `-B bbinfo` - Boot block information file
- `-o options` - Architecture-specific options

## Filesystem Integration

### FFS (Fast File System)
- Boot block in cylinder group 0
- Superblock validation required
- Inode allocation for boot program
- Block size considerations (512B-64KB)

### EXT2 Support
- Reserved boot sectors
- Superblock backup handling
- Block group descriptor validation
- Inode table location

### MINIX v3 Filesystem
- Dedicated boot partition support
- Superblock validation
- Bitmaps for allocation tracking
- Zone/block addressing

## Architecture-Specific Details

### x86/i386 Support
```c
/* arch/i386.c */
- MBR partition table handling
- BIOS boot sector format
- Real mode to protected mode transition
- Memory layout considerations
```

### SPARC Support
```c
/* arch/sparc.c */
- OpenBoot PROM interface
- Disk label handling
- Boot block location
- Sun partition format
```

### Alpha Support
```c
/* arch/alpha.c */
- SRM console interface
- ARC/AlphaBIOS support
- Disk partitioning
- Boot block format
```

## Error Handling and Diagnostics

### Error Categories
- **Device Errors** - Permission, I/O, hardware issues
- **Filesystem Errors** - Corruption, unsupported types
- **Architecture Errors** - Unsupported platforms
- **Boot Code Errors** - Invalid boot programs

### Diagnostic Information
- Verbose mode shows detailed operations
- Boot block information extraction
- Filesystem parameter reporting
- Architecture capability detection

## Build Dependencies

### Required Components
- Host C compiler
- `libnbcompat` for platform compatibility
- Raw disk device access capabilities
- Filesystem development headers

### Optional Features
- MD5/SHA checksum support
- Large device support (>2TB)
- Advanced partition schemes (GPT)
- Network boot support

## Testing and Validation

### Installation Verification
- Boot sector checksum validation
- Partition table consistency checks
- Boot program accessibility
- Device bootability confirmation

### Cross-Platform Testing
- Architecture-specific validation
- Filesystem compatibility verification
- Host/target environment testing
- RISC-V boot simulation (when implemented)

## Security Considerations

### Privilege Requirements
- Requires raw device access permissions
- May need elevated privileges for installation
- Secure boot considerations
- Boot code integrity verification

### Validation Measures
- Boot sector signature verification
- Checksum validation
- Boot program authenticity
- Device access restrictions

## Development Guidelines

### Adding New Architecture Support
1. Create `arch/newarch.c` with required functions
2. Implement match, install, clear, info operations
3. Add architecture detection logic
4. Update main architecture table
5. Test cross-compilation scenarios

### Filesystem Support Extension
1. Implement filesystem operations structure
2. Add detection and validation logic
3. Define boot block location method
4. Test with various filesystem configurations

### RISC-V Implementation Roadmap
1. Research OpenSBI boot requirements
2. Design RISC-V boot block format
3. Implement `arch/riscv64.c` module
4. Add QEMU virt platform support
5. Test with MINIX RISC-V kernel

## Performance Characteristics

### Processing Speed
- Linear device scanning complexity
- Efficient filesystem detection
- Optimized boot code installation
- Minimal memory footprint

### Scalability
- Supports devices from MB to TB range
- Handles complex partition schemes
- Multiple filesystem types per device
- Concurrent installation capabilities

This tool is crucial for MINIX system deployment, ensuring that bootable systems can be created across diverse hardware platforms, with particular importance for cross-compilation scenarios where development hosts differ from target architectures.