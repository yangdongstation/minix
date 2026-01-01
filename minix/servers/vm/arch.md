# MINIX Virtual Memory Manager (VM) Server Architecture

This directory contains the Virtual Memory Manager server implementation, which handles all memory management operations in MINIX. The VM manages process address spaces, memory allocation, page mapping, and coordinates with the kernel's memory management subsystem.

## Core VM Infrastructure

### Server Framework
- **main.c**: VM server main program
  - Server initialization and setup
  - Message loop and dispatch
  - Memory system initialization
  - Service registration
  - Main server state management
  - Error handling and recovery

- **vm.h**: VM server definitions
  - VM-specific constants and macros
  - Server configuration parameters
  - VM service definitions
  - Internal data structure definitions

- **glo.h**: Global variables
  - VM global state
  - Memory allocation tables
  - System-wide counters
  - Configuration parameters
  - Shared state information

### Process Memory Management
- **vmproc.h**: VM process table structure
  - Per-process VM state
  - Memory region information
  - Page table references
  - Memory usage statistics
  - Virtual memory layout

### Memory Allocation
- **alloc.c**: Memory allocation core
  - Virtual memory allocation
  - Memory region management
  - Free list maintenance
  - Allocation policy implementation
  - Memory coalescing
  - Fragmentation management
  - Buddy system implementation
  - Slab allocator support

### Memory Region Management
- **region.c**: Memory region operations
  - Virtual memory region creation
  - Region growth and shrinkage
  - Region merging and splitting
  - Memory mapping coordination
  - Access permission management

### Process Lifecycle Memory Operations
- **fork.c**: Memory handling for fork()
  - Copy-on-write implementation
  - Parent memory space duplication
  - Page sharing setup
  - Memory region inheritance
  - Reference counting

- **exit.c**: Memory cleanup for exit()
  - Process memory deallocation
  - Page table cleanup
  - Memory region removal
  - Reference count management
  - Shared memory handling

- **exec.c**: Memory setup for exec()
  - Memory space reorganization
  - Old memory region cleanup
  - New program layout setup
  - Stack initialization
  - Argument area allocation

### Memory Types
- **mem_anon.c**: Anonymous memory
  - Non-file-backed memory
  - Heap and stack allocation
  - Zero-page optimization
  - Demand allocation
  - Copy-on-write support

- **mem_anon_contig.c**: Contiguous anonymous memory
  - Physically contiguous allocation
  - DMA buffer support
  - Device memory requirements
  - Large page allocation

### Memory Mapping
- **mmap.c**: Memory mapping operations
  - mmap() system call implementation
  - File-backed memory mapping
  - Shared memory regions
  - Memory protection
  - Mapping synchronization

- **munmap.c**: Memory unmapping
  - munmap() system call
  - Memory region removal
  - Page cleanup
  - Reference count updates
  - Resource deallocation

- **mremap.c**: Memory remapping
  - mremap() system call
  - Memory region resizing
  - Address space reorganization
  - Move mapping support

### Memory Protection
- **protect.c**: Memory protection
  - Page permission management
  - Access control implementation
  - Protection violation handling
  - Memory region attributes
  - Permission inheritance

### Memory Information
- **memset.c**: Memory information retrieval
  - Memory usage statistics
  - Available memory reporting
  - Memory mapping information
  - Process memory details

- **getset.c**: Memory parameter management
  - Memory limit retrieval
  - Memory parameter setting
  - Resource limit enforcement
  - Memory policy management

### Signal and Context Management
- **signal.c**: Signal stack management
  - Alternate signal stack support
  - Signal context preservation
  - Stack overflow protection
  - Signal delivery coordination

- **mcontext.c**: Machine context operations
  - Process context switching
  - Memory context preservation
  - Context validation
  - Architecture-specific handling

### Memory Sharing
- **share.c**: Memory sharing
  - Shared memory regions
  - Reference counting
  - Copy-on-write optimization
  - Shared region synchronization
  - Memory deduplication

### Memory Synchronization
- **msync.c**: Memory synchronization
  - msync() system call
  - Memory mapping synchronization
  - File-backed mapping flush
  - Cache coherency
  - Dirty page handling

### Architecture Support
- **arch/**: Architecture-specific VM code
  - Platform-specific memory operations
  - Page table management
  - TLB operations
  - Cache management
  - Memory barrier operations

### Memory Locking
- **mlock.c**: Memory locking
  - mlock() and mlockall() support
  - Memory page locking
  - Physical memory reservation
  - Real-time memory guarantees
  - Lock limit enforcement

### Memory Advice
- **madvise.c**: Memory advice
  - madvise() system call
  - Memory access pattern hints
  - Page prefetching
  - Memory usage optimization
  - Cache behavior control

### Memory Barriers
- **mb.c**: Memory barriers
  - Memory ordering operations
  - Synchronization primitives
  - Cache coherency
  - Multi-processor memory consistency

### File Descriptor Management
- **fdref.c**: File descriptor reference counting
  - VM file descriptor tracking
  - Reference count management
  - File-backed memory coordination
  - Descriptor cleanup

### Access Control
- **acl.c**: Access control lists
  - Memory region ACLs
  - Permission checking
  - Access validation
  - Security policy enforcement

### Cache Management
- **cache.c/cache.h**: Memory cache
  - Virtual memory cache
  - Page cache management
  - Cache coherency
  - Performance optimization

### Data Structures
- **avl_impl.h/cavl_if.h**: AVL tree implementation
  - Memory region indexing
  - Efficient range queries
  - Balanced tree operations
  - Memory-efficient storage

## VM Features

### Virtual Memory Management
- **Demand Paging**: Load pages on access
- **Copy-on-Write**: Efficient memory sharing
- **Memory Mapping**: File-backed memory regions
- **Shared Memory**: Inter-process memory sharing
- **Memory Protection**: Page-level access control

### Memory Allocation Policies
- **First-Fit**: Simple allocation strategy
- **Buddy System**: Power-of-2 allocation
- **Slab Allocation**: Object caching
- **Lazy Allocation**: Allocate on first access
- **Overcommit**: Allow virtual > physical

### Performance Optimizations
- **TLB Efficiency**: Minimize TLB misses
- **Page Coloring**: Cache optimization
- **Large Pages**: Reduce page table overhead
- **Memory Prefetching**: Anticipate access patterns
- **Zero Page Sharing**: Share zero-filled pages

## Integration with MINIX System

### Inter-Server Communication
- **PM Coordination**: Process lifecycle management
- **VFS Collaboration**: File-backed memory mapping
- **Kernel Interface**: Low-level memory operations
- **Driver Communication**: Device memory coordination

### Security Model
- **Memory Isolation**: Process address space separation
- **Access Control**: Page-level permission checking
- **Resource Limits**: Per-process memory quotas
- **Capability System**: Memory access authorization

### Fault Tolerance
- **Memory Allocation Failure**: Graceful degradation
- **Out-of-Memory Handling**: Emergency recovery
- **Memory Leak Detection**: Resource tracking
- **Corruption Detection**: Memory integrity checking

The VM server is essential to MINIX's memory management architecture, providing secure, efficient, and reliable virtual memory services while maintaining process isolation and system stability through comprehensive memory protection and resource management mechanisms.