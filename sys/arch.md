# MINIX System Headers Architecture

This directory contains the system header files that define the interface between user space and the kernel, including system calls, data structures, and constants. These headers are essential for system programming and kernel development.

## 本仓库实际包含/可选或上游组件

本仓库实际包含：以下为顶层条目示例（完整清单以目录实际内容为准）：`arch/`, `dev/`, `net/`, `ufs/`, `uvm/`, `compat/`, `Makefile`.
可选或上游组件：上文条目可能包含上游/可选组件示例，当前仓库未必包含全部。

## Directory Structure

### Core System Headers (sys/)
The main system interface definitions:
- **syscall.h**: System call numbers and interface
- **types.h**: Basic system data types
- **stat.h**: File status information
- **wait.h**: Process wait status definitions
- **signal.h**: Signal handling definitions
- **time.h**: Time-related structures and functions
- **resource.h**: Resource usage and limits
- **utsname.h**: System identification
- **ioctl.h**: I/O control commands
- **mount.h**: File system mounting definitions
- **reboot.h**: System reboot codes
- **socket.h**: Socket interface definitions
- **ioctl_compat.h**: Compatibility ioctl definitions

### Architecture Support (arch/)
Architecture-specific header configurations:
- **arm/**: ARM 32-bit architecture support
- **evbarm/**: ARM evaluation board support
- **evbriscv64/**: RISC-V 64-bit evaluation board (active development)
- **i386/**: Intel 32-bit x86 architecture
- **riscv/**: RISC-V generic architecture support

### Device Interfaces (dev/)
Hardware device interface definitions:
- **pci/**: PCI bus definitions
- **usb/**: USB device interfaces
- **wscons/**: Workstation console definitions
- **disk/**: Disk device interfaces
- **audio/**: Audio device interfaces
- **video/**: Video device interfaces

### File System Headers (fs/)
File system specific definitions:
- **ffs/**: Fast File System (UFS1/UFS2)
- **mfs/**: MINIX File System
- **cd9660/**: ISO 9660 file system
- **msdos/**: FAT file system
- **ntfs/**: NTFS file system
- **ext2fs/**: Extended 2 file system

### Network Protocol Headers (net/)
Network stack definitions:
- **if.h**: Network interface definitions
- **route.h**: Routing table structures
- **bpf.h**: Berkeley Packet Filter
- **if_ether.h**: Ethernet interface definitions
- **if_media.h**: Network media types

### Internet Protocol Headers (netinet/)
TCP/IP protocol definitions:
- **in.h**: Internet address definitions
- **ip.h**: IP protocol header
- **tcp.h**: TCP protocol header
- **udp.h**: UDP protocol header
- **icmp.h**: ICMP protocol definitions
- **igmp.h**: IGMP protocol definitions
- **ip_icmp.h**: ICMP structures
- **tcp_timer.h**: TCP timer definitions

### IPv6 Support (netinet6/)
IPv6 protocol definitions:
- **in6.h**: IPv6 address definitions
- **ipv6.h**: IPv6 protocol header
- **icmp6.h**: ICMPv6 definitions
- **nd6.h**: Neighbor Discovery
- **mld6.h**: Multicast Listener Discovery

### Wireless Networking (net80211/)
IEEE 802.11 wireless definitions:
- **ieee80211.h**: 802.11 protocol definitions
- **ieee80211_ioctl.h**: 802.11 ioctl commands
- **ieee80211_radiotap.h**: Radiotap header definitions

### Memory Management (uvm/)
Virtual memory system definitions:
- **uvm.h**: UVM (Unified Virtual Memory) interface
- **uvm_extern.h**: UVM external interfaces
- **uvm_param.h**: UVM parameters and limits
- **uvm_prot.h**: Memory protection definitions

### Compatibility Layer (compat/)
Compatibility definitions for different systems:
- **sys/**: System call compatibility
- **net/**: Network compatibility
- **netinet/**: Internet protocol compatibility

### Alternative Queuing (altq/)
Network traffic queuing definitions:
- **altq.h**: Alternative queuing interface
- Various queuing discipline headers

### AppleTalk Support (netatalk/)
AppleTalk protocol definitions:
- **at.h**: AppleTalk address definitions
- **ddp.h**: Datagram Delivery Protocol

### MPLS Support (netmpls/)
Multiprotocol Label Switching:
- **mpls.h**: MPLS definitions

### UFS File System (ufs/)
Unix File System definitions:
- **ffs/**: Fast File System specific headers
- **ufs/**: Generic UFS definitions
- **dinode.h**: Disk inode structure
- **dir.h**: Directory entry format

## RISC-V 64-bit Specific Headers

### Architecture Definitions (arch/evbriscv64/)
RISC-V 64-bit evaluation board support:
- **include/**: Board-specific header files
- **Makefile**: Build configuration for RISC-V headers
- Platform-specific definitions for QEMU virt platform
- Memory map and device configuration

### Key RISC-V Headers
- **riscvreg.h**: RISC-V register definitions
- **cpu.h**: CPU-specific definitions
- **param.h**: Architecture parameters
- **limits.h**: Architecture limits
- **endian.h**: Endianness definitions
- **setjmp.h**: Setjmp/longjmp definitions
- **signal.h**: Signal context definitions
- **proc.h**: Process context definitions

## System Interface Categories

### System Calls
Headers defining system call interfaces:
- **unistd.h**: POSIX system calls
- **sys/syscall.h**: MINIX-specific system calls
- **minix/syscall.h**: MINIX system call numbers
- **minix/com.h**: MINIX system call messages

### Process Management
Process control and management:
- **sys/wait.h**: Process wait status
- **sys/proc.h**: Process information
- **sys/ptrace.h**: Process tracing
- **sys/sched.h**: Process scheduling

### Memory Management
Memory and virtual memory:
- **sys/mman.h**: Memory mapping
- **sys/shm.h**: Shared memory
- **sys/msg.h**: Message queues
- **sys/sem.h**: Semaphores

### File System Interface
File system operations:
- **sys/statvfs.h**: File system statistics
- **sys/dirent.h**: Directory entries
- **sys/vfs.h**: Virtual file system interface
- **sys/mount.h**: Mount operations

### Device I/O
Device input/output:
- **sys/ioccom.h**: I/O control commands
- **sys/disk.h**: Disk device interface
- **sys/audioio.h**: Audio device interface
- **sys/videoio.h**: Video device interface

### Network Interface
Network system calls:
- **sys/socket.h**: Socket interface
- **sys/sockio.h**: Socket I/O controls
- **sys/if_dl.h**: Data link interface
- **sys/ttycom.h**: Terminal I/O controls

## Build System Integration

### Makefile Structure
- **Makefile**: Top-level header build configuration
- **arch/Makefile.inc**: Architecture-specific build rules
- Platform-specific header generation
- Conditional compilation support

### Header Dependencies
- **Cross-references**: Inter-header dependencies
- **Architecture dependencies**: Platform-specific headers
- **Feature dependencies**: Optional feature headers
- **Version dependencies**: API version management

## Development Guidelines

### Header Design
- **Consistency**: Uniform naming conventions
- **Documentation**: Comprehensive header comments
- **Portability**: Cross-platform compatibility
- **Stability**: Stable ABI definitions

### Version Management
- **API versioning**: Backward compatibility
- **Feature detection**: Compile-time checks
- **Deprecation**: Graceful API evolution
- **Migration**: Upgrade path documentation

This header architecture provides a clean, well-organized interface between user applications and the MINIX kernel, with specific support for the RISC-V 64-bit architecture and comprehensive coverage of all system interfaces.
