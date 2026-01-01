# MINIX Unified Virtual Memory Interface System

## Overview

The `sys/uvm/` directory contains the Unified Virtual Memory (UVM) interface system for MINIX 3, providing the essential framework for memory management across the operating system. This directory serves as the central hub for virtual memory operations, offering sophisticated memory management capabilities including page mapping, address space management, memory allocation, and virtual memory policies while maintaining compatibility with MINIX's microkernel architecture.

## Purpose and Responsibilities

### Core Functions
- **Virtual Memory Management**: Provides comprehensive virtual memory management services
- **Page Mapping and Translation**: Handles page table management and address translation
- **Memory Allocation**: Manages kernel and user memory allocation
- **Address Space Management**: Controls process address spaces and memory regions
- **Memory Protection**: Implements memory protection and access control
- **Paging and Swapping**: Supports demand paging and memory swapping

### Key Features
- **Unified Memory Interface** - Single interface for all memory operations
- **Advanced Memory Policies** - Sophisticated memory allocation and replacement policies
- **Page Cache Management** - Efficient page caching and buffer management
- **Memory Mapped Files** - Support for memory-mapped file I/O
- **Copy-on-Write Optimization** - Efficient memory sharing through COW
- **Physical Memory Mapping** - Direct physical memory access support

## Directory Structure

```
sys/uvm/
├── Makefile               # Main UVM makefile
├── files.uvm              # UVM file list
├── uvm_amap.h            # Anonymous memory map definitions
├── uvm_anon.h            # Anonymous memory definitions
├── uvm_aobj.h            # Anonymous object definitions
├── uvm_ddb.h             # Kernel debugger support
├── uvm_device.h          # Device memory interface
├── uvm_extern.h          # External UVM interfaces
├── uvm_fault.h           # Page fault handling
├── uvm_fault_i.h         # Internal fault handling
├── uvm_glue.h            # UVM glue interfaces
├── uvm.h                 # Main UVM header
├── uvm_km.h              # Kernel memory interface
├── uvm_loan.h            # Memory loan interface
├── uvm_map.h             # Memory mapping interface
├── uvm_object.h          # Memory object interface
├── uvm_page.h            # Physical page management
├── uvm_pager.h           # Pager interface
├── uvm_param.h           # UVM parameters
├── uvm_pdaemon.h         # Page daemon interface
├── uvm_pdpolicy.h        # Page daemon policy
├── uvm_pdpolicy_impl.h   # Policy implementation
├── uvm_pglist.h          # Page list management
├── uvm_pmap.h            # Physical mapping interface
├── uvm_prot.h            # Memory protection
├── uvm_readahead.h       # Read-ahead interface
├── uvm_stat.h            # UVM statistics
├── uvm_swap.h            # Swap management
└── pmap/                 # Physical mapping implementations
    ├── pmap.h            # Physical mapping header
    └── Makefile          # Physical mapping makefile
```

## Core UVM Interfaces

### Main UVM Header (`uvm.h`)
Central UVM interface definitions and system-wide constants:

```c
/* UVM version and constants */
#define UVM_VERSION "3.0"
#define UVM_MAXVERSION 0x03

/* UVM page size constants */
#define UVM_PAGE_SIZE       PAGE_SIZE
#define UVM_PAGE_SHIFT      PAGE_SHIFT
#define UVM_PAGE_MASK       PAGE_MASK

/* UVM object types */
#define UVM_OBJTYPE_NONE    0
#define UVM_OBJTYPE_ANON    1
#define UVM_OBJTYPE_VNODE   2
#define UVM_OBJTYPE_DEVICE  3
#define UVM_OBJTYPE_PHYS    4

/* UVM fault types */
#define UVM_FAULT_READ      0   /* Read fault */
#define UVM_FAULT_WRITE     1   /* Write fault */
#define UVM_FAULT_PROTECT   2   /* Protection fault */

/* UVM map entry flags */
#define UVM_MAP_NOMERGE     0x00000001  /* Don't merge entries */
#define UVM_MAP_OVERLAP     0x00000002  /* Allow overlap */
#define UVM_MAP_FIXED       0x00000004  /* Fixed mapping */
#define UVM_MAP_REMAP       0x00000008  /* Remap existing */
#define UVM_MAP_COPYONWRITE 0x00000010  /* Copy-on-write */
#define UVM_MAP_WIREFUTURE  0x00000020  /* Wire future pages */
```

### Memory Object Interface (`uvm_object.h`)
Abstract memory object interface for different backing stores:

```c
/* Memory object structure */
struct uvm_object {
    struct uvm_pagerops *pgops;     /* Pager operations */
    struct pglist memq;             /* Memory queue */
    u_int uo_npages;                /* Number of pages */
    u_int uo_refs;                  /* Reference count */
    int uo_flags;                   /* Object flags */
    void *uo_data;                  /* Private data */
};

/* Memory object operations */
struct uvm_objectops {
    void (*uop_reference)(struct uvm_object *);
    void (*uop_detach)(struct uvm_object *);
    int (*uop_fault)(struct uvm_object *, vaddr_t, vm_page_t *, int, int, vm_prot_t, vm_prot_t);
    void (*uop_putpages)(struct uvm_object *, vm_page_t *, int, int);
    vaddr_t (*uop_unmap)(struct uvm_object *, vaddr_t, vaddr_t, vm_page_t *);
    boolean_t (*uop_releasepg)(struct uvm_object *, vm_page_t *, vaddr_t);
};

/* Memory object types */
struct uvm_vnode {
    struct uvm_object u_obj;        /* Base object */
    struct vnode *u_vnode;          /* Associated vnode */
    struct vm_page *u_pages;        /* Page array */
};

struct uvm_device {
    struct uvm_object u_obj;        /* Base object */
    struct device *u_device;        /* Associated device */
    vaddr_t u_offset;               /* Device offset */
    vsize_t u_size;                 /* Device size */
};
```

### Virtual Memory Mapping (`uvm_map.h`)
Virtual address space management and mapping:

```c
/* Virtual memory map structure */
struct vm_map {
    struct lock vm_lock;            /* Map lock */
    struct vm_map_entry header;     /* Map entry list header */
    int nentries;                   /* Number of entries */
    vsize_t size;                   /* Total map size */
    vm_map_entry_t first_free;      /* First free entry */
    vm_map_entry_t hint;            /* Hint for allocation */
    u_int timestamp;                /* Map timestamp */
    u_int flags;                    /* Map flags */
    pmap_t pmap;                    /* Physical map */
};

/* Map entry structure */
struct vm_map_entry {
    struct vm_map_entry *next;      /* Next entry */
    struct vm_map_entry *prev;      /* Previous entry */
    vaddr_t start;                  /* Start address */
    vaddr_t end;                    /* End address */
    vm_map_entry_t next_free;       /* Next free entry */
    struct uvm_object *object;      /* Associated object */
    vaddr_t offset;                 /* Object offset */
    vm_prot_t protection;           /* Protection */
    vm_prot_t max_protection;       /* Maximum protection */
    vm_inherit_t inheritance;       /* Inheritance */
    int wired_count;                /* Wired page count */
    u_short flags;                  /* Entry flags */
    u_short store_entry;            /* Store entry */
};

/* Map operations */
int uvm_map(struct vm_map *, vaddr_t *, vsize_t, struct uvm_object *, vaddr_t, vaddr_t, vm_prot_t, vm_prot_t, vm_inherit_t, u_int);
int uvm_unmap(struct vm_map *, vaddr_t, vaddr_t);
int uvm_map_protect(struct vm_map *, vaddr_t, vaddr_t, vm_prot_t, boolean_t);
int uvm_map_inherit(struct vm_map *, vaddr_t, vaddr_t, vm_inherit_t);
```

## Physical Memory Management

### Page Management (`uvm_page.h`)
Physical page allocation and management:

```c
/* Physical page structure */
struct vm_page {
    struct pglist pageq;          /* Page queues */
    struct uvm_object *object;    /* Owning object */
    vaddr_t offset;               /* Offset within object */
    u_short flags;                /* Page flags */
    u_short loan_count;           /* Loan count */
    u_int wire_count;             /* Wire count */
    u_int dirty;                  /* Dirty counter */
    u_int valid;                  /* Valid counter */
    struct vm_page *p_next;       /* Next in hash */
    struct vm_anon *anon;         /* Anonymous memory */
    paddr_t phys_addr;            /* Physical address */
};

/* Page flags */
#define PG_BUSY         0x0001      /* Page is busy */
#define PG_WANTED       0x0002      /* Page is wanted */
#define PG_TABLED       0x0004      /* Page is in page table */
#define PG_CLEAN        0x0008      /* Page is clean */
#define PG_FAKE         0x0010      /* Page is fake */
#define PG_PAGER1       0x0020      /* Pager private flag 1 */
#define PG_PAGER2       0x0040      /* Pager private flag 2 */
#define PG_ANON         0x0080      /* Page is anonymous */
#define PG_ZERO         0x0100      /* Page is zeroed */
#define PG_RDONLY       0x0200      /* Page is read-only */
#define PG_NOCACHE      0x0400      /* Don't cache page */
#define PG_PDFREEWAIT   0x0800      /* Page daemon is waiting */
#define PG_MARKER       0x1000      /* Queue marker */

/* Page queues */
extern struct pglist uvm_page_queues[];
#define UVM_PQ_ACTIVE   0           /* Active pages */
#define UVM_PQ_INACTIVE 1           /* Inactive pages */
#define UVM_PQ_FREE     2           /* Free pages */
#define UVM_PQ_ANON     3           /* Anonymous pages */

/* Page allocation */
vm_page_t uvm_pagealloc(struct uvm_object *, vaddr_t, vm_page_t, int);
void uvm_pagefree(vm_page_t);
vm_page_t uvm_pagelookup(struct uvm_object *, vaddr_t);
void uvm_pageactivate(vm_page_t);
void uvm_pagedeactivate(vm_page_t);
```

### Physical Mapping (`uvm_pmap.h`)
Physical-to-virtual address mapping interface:

```c
/* Physical map structure */
struct pmap {
    struct pmap_statistics stats;   /* Statistics */
    struct lock pmap_lock;          /* Map lock */
    void *pm_pdir;                  /* Page directory */
    u_int pm_refcount;              /* Reference count */
    u_int pm_flags;                 /* Flags */
    struct vm_page *pm_ptphint;     /* Page table hint */
    struct pv_entry *pm_pvhint;     /* PV hint */
    struct pmap *pm_list;           /* Global list */
};

/* Physical map operations */
void pmap_enter(pmap_t, vaddr_t, paddr_t, vm_prot_t, vm_prot_t, int);
void pmap_remove(pmap_t, vaddr_t, vaddr_t);
void pmap_protect(pmap_t, vaddr_t, vaddr_t, vm_prot_t);
void pmap_update(pmap_t);
boolean_t pmap_extract(pmap_t, vaddr_t, paddr_t *);
void pmap_copy(pmap_t, pmap_t, vaddr_t, vsize_t, vaddr_t);
void pmap_activate(struct lwp *);
void pmap_deactivate(struct lwp *);
void pmap_zero_page(vm_page_t);
void pmap_copy_page(vm_page_t, vm_page_t);
```

## Memory Allocation and Kernel Memory

### Kernel Memory Interface (`uvm_km.h`)
Kernel memory allocation interfaces:

```c
/* Kernel memory allocation types */
#define UVM_KMF_NOWAIT      0x0001      /* Don't wait */
#define UVM_KMF_WAITVA      0x0002      /* Wait for VA */
#define UVM_KMF_CANFAIL     0x0004      /* Can fail */
#define UVM_KMF_ZERO        0x0008      /* Zero memory */
#define UVM_KMF_WIRED       0x0010      /* Wire pages */
#define UVM_KMF_VALLOC      0x0020      /* VA allocation only */
#define UVM_KMF_NOOBJ       0x0040      /* No object */

/* Kernel memory allocation */
vaddr_t uvm_km_alloc(struct vm_map *, vsize_t, vsize_t, u_int);
vaddr_t uvm_km_alloc1(struct vm_map *, vsize_t, vsize_t, u_int);
vaddr_t uvm_km_zalloc(struct vm_map *, vsize_t);
void uvm_km_free(struct vm_map *, vaddr_t, vsize_t);
void uvm_km_free_wakeup(struct vm_map *, vaddr_t, vsize_t);

/* Kernel map operations */
struct vm_map *uvm_km_suballoc(struct vm_map *, vaddr_t *, vaddr_t *, vsize_t, boolean_t, boolean_t, struct vm_map *);
void uvm_km_guard(struct vm_map *, vaddr_t, vsize_t, boolean_t);
```

### Anonymous Memory (`uvm_anon.h`)
Anonymous memory management for process data:

```c
/* Anonymous memory structure */
struct vm_anon {
    struct vm_page *an_page;        /* Associated page */
    u_int an_ref;                   /* Reference count */
    u_short an_flags;               /* Flags */
    swblk_t an_swslot;              /* Swap slot */
    struct vm_anon *an_next;        /* Next in hash */
};

/* Anonymous memory flags */
#define ANON_PRIVATE    0x0001      /* Private anonymous */
#define ANON_SHARED     0x0002      /* Shared anonymous */
#define ANON_SWAPPED    0x0004      /* Swapped to disk */
#define ANON_BUSY       0x0008      /* Busy */
#define ANON_WANTED     0x0010      /* Wanted */

/* Anonymous memory operations */
struct vm_anon *uvm_anon_alloc(void);
void uvm_anon_free(struct vm_anon *);
struct vm_anon *uvm_anon_lookup(union vm_map_object *, vaddr_t);
void uvm_anon_reference(struct vm_anon *);
void uvm_anon_dereference(struct vm_anon *);
int uvm_anon_try_upgrade(struct vm_anon *);
```

## Page Fault Handling

### Fault Handling Interface (`uvm_fault.h`)
Page fault processing and management:

```c
/* Fault information structure */
struct uvm_faultinfo {
    vaddr_t f_faultaddr;            /* Fault address */
    vm_prot_t f_access_type;        /* Access type */
    vm_prot_t f_prot;               /* Protection */
    int f_flags;                    /* Flags */
    struct vm_map *f_map;           /* Faulting map */
    struct vm_map_entry *f_entry;   /* Faulting entry */
    struct uvm_object *f_object;    /* Faulting object */
    vaddr_t f_offset;               /* Object offset */
    struct lwp *f_lwp;              /* Faulting LWP */
};

/* Fault flags */
#define UVM_FAULT_WIRE      0x0001      /* Wire page */
#define UVM_FAULT_NOWIRE    0x0002      /* Don't wire */
#define UVM_FAULT_COPYONWRITE 0x0004    /* Copy-on-write */
#define UVM_FAULT_OVERLAY   0x0008      /* Overlay */
#define UVM_FAULT_LOCKMAP   0x0010      /* Lock map */
#define UVM_FAULT_UNLOCKMAP 0x0020      /* Unlock map */
#define UVM_FAULT_NOPAGEOUT 0x0040      /* Don't page out */
#define UVM_FAULT_NOMERGE   0x0080      /* Don't merge */

/* Fault handling */
int uvm_fault(struct vm_map *, vaddr_t, vm_prot_t, int);
int uvm_fault_wire(struct vm_map *, vaddr_t, vaddr_t);
void uvm_fault_unwire(struct vm_map *, vaddr_t, vaddr_t);
```

### Internal Fault Processing (`uvm_fault_i.h`)
Internal fault handling structures and functions:

```c
/* Fault state machine states */
enum uvm_fault_state {
    UVM_FAULT_START,                /* Initial state */
    UVM_FAULT_OBJECT,               /* Object lookup */
    UVM_FAULT_ANON,                 /* Anonymous memory */
    UVM_FAULT_PAGEIN,               /* Page in */
    UVM_FAULT_PAGEOUT,              /* Page out */
    UVM_FAULT_COPYONWRITE,          /* Copy-on-write */
    UVM_FAULT_WIRE,                 /* Wire page */
    UVM_FAULT_DONE                  /* Done */
};

/* Internal fault context */
struct uvm_fault_context {
    struct uvm_faultinfo *fi;       /* Fault info */
    enum uvm_fault_state state;     /* Current state */
    vm_page_t *pages;               /* Page array */
    int npages;                     /* Number of pages */
    int centeridx;                  /* Center page index */
    int loavail;                    /* Low available */
    int hiavail;                    /* High available */
    boolean_t fault_handled;        /* Fault handled */
    int error;                      /* Error code */
};

/* Internal fault functions */
int uvm_fault_internal(struct uvm_fault_context *);
void uvm_fault_promote_pagemap(struct vm_map_entry *, struct uvm_object *, vaddr_t);
```

## Page Daemon and Policies

### Page Daemon Interface (`uvm_pdaemon.h`)
Background memory management daemon:

```c
/* Page daemon control */
extern struct lwp *uvm_pdaemon_lwp;
extern int uvm_pageout_pending;
extern int uvm_swapout_pending;

/* Page daemon operations */
void uvm_page_daemon_init(void);
void uvm_page_daemon_start(void);
void uvm_page_daemon_wakeup(void);
void uvm_page_daemon_idle(void);

/* Page daemon parameters */
#define UVM_PAGE_DAEMON_INTERVAL    10      /* Check interval (ticks) */
#define UVM_PAGE_DAEMON_IDLE_TIME   100     /* Idle time (ticks) */
#define UVM_PAGE_DAEMON_MAX_LOOPS   100     /* Maximum loops per cycle */
```

### Page Daemon Policies (`uvm_pdpolicy.h`)
Memory management policies and algorithms:

```c
/* Page daemon policy structure */
struct uvm_pdpolicy {
    const char *name;               /* Policy name */
    void (*init)(void);             /* Initialize policy */
    void (*cleanup)(void);          /* Cleanup policy */
    int (*select_page)(struct pglist *); /* Select page for reclaim */
    void (*page_accessed)(vm_page_t);   /* Page accessed */
    void (*page_dirtied)(vm_page_t);    /* Page dirtied */
    int (*balance_needed)(void);    /* Rebalance needed */
};

/* Page replacement policies */
extern struct uvm_pdpolicy uvm_pdpolicy_fifo;
extern struct uvm_pdpolicy uvm_pdpolicy_lru;
extern struct uvm_pdpolicy uvm_pdpolicy_clock;
extern struct uvm_pdpolicy uvm_pdpolicy_lfu;

/* Current policy */
extern struct uvm_pdpolicy *uvm_pdpolicy_current;

/* Policy management */
void uvm_pdpolicy_init(void);
int uvm_pdpolicy_set(const char *name);
const char *uvm_pdpolicy_get(void);
```

## Memory Protection and Statistics

### Memory Protection (`uvm_prot.h`)
Memory access protection definitions:

```c
/* Protection codes */
#define UVM_PROT_NONE   VM_PROT_NONE    /* No access */
#define UVM_PROT_READ   VM_PROT_READ    /* Read access */
#define UVM_PROT_WRITE  VM_PROT_WRITE   /* Write access */
#define UVM_PROT_EXEC   VM_PROT_EXEC    /* Execute access */
#define UVM_PROT_RW     (VM_PROT_READ|VM_PROT_WRITE) /* Read/write */
#define UVM_PROT_RX     (VM_PROT_READ|VM_PROT_EXEC)  /* Read/execute */
#define UVM_PROT_RWX    (VM_PROT_READ|VM_PROT_WRITE|VM_PROT_EXEC) /* All */

/* Protection checking */
#define UVM_PROT_ISVALID(prot)  (((prot) & ~VM_PROT_ALL) == 0)
#define UVM_PROT_ISREADABLE(prot)  (((prot) & VM_PROT_READ) != 0)
#define UVM_PROT_ISWRITABLE(prot)  (((prot) & VM_PROT_WRITE) != 0)
#define UVM_PROT_ISEXECUTABLE(prot)  (((prot) & VM_PROT_EXEC) != 0)
#define UVM_PROT_ISREADWRITE(prot)  (((prot) & VM_PROT_RW) == VM_PROT_RW)
#define UVM_PROT_ISREADEXEC(prot)  (((prot) & VM_PROT_RX) == VM_PROT_RX)
#define UVM_PROT_ISREADWRITEEXEC(prot)  (((prot) & VM_PROT_RWX) == VM_PROT_RWX)
```

### UVM Statistics (`uvm_stat.h`)
Memory management statistics and monitoring:

```c
/* UVM statistics structure */
struct uvm_statistics {
    /* Page statistics */
    uint64_t    page_allocs;        /* Page allocations */
    uint64_t    page_frees;         /* Page frees */
    uint64_t    page_faults;        /* Page faults */
    uint64_t    page_faults_fixed;  /* Fixed page faults */
    uint64_t    page_ins;           /* Page ins */
    uint64_t    page_outs;          /* Page outs */
    uint64_t    page_zeros;         /* Zero page allocations */
    
    /* Map statistics */
    uint64_t    map_allocs;         /* Map allocations */
    uint64_t    map_frees;          /* Map frees */
    uint64_t    map_collisions;     /* Map collisions */
    uint64_t    map_nocluster;      /* No cluster allocations */
    
    /* Object statistics */
    uint64_t    object_creates;     /* Object creates */
    uint64_t    object_destroys;    /* Object destroys */
    uint64_t    object_collapses;   /* Object collapses */
    
    /* Anonymous memory statistics */
    uint64_t    anon_allocs;        /* Anonymous allocations */
    uint64_t    anon_frees;         /* Anonymous frees */
    uint64_t    anon_coalesces;     /* Anonymous coalesces */
    
    /* Swap statistics */
    uint64_t    swap_ins;           /* Swap ins */
    uint64_t    swap_outs;          /* Swap outs */
    uint64_t    swap_alloc_failures; /* Swap allocation failures */
    uint64_t    swap_io_errors;     /* Swap I/O errors */
};

/* Statistics access */
extern struct uvm_statistics uvm_stats;

#define UVM_STAT_ADD(stat, val)     (uvm_stats.stat += (val))
#define UVM_STAT_INC(stat)          UVM_STAT_ADD(stat, 1)
#define UVM_STAT_DEC(stat)          UVM_STAT_ADD(stat, -1)
#define UVM_STAT_GET(stat)          (uvm_stats.stat)
#define UVM_STAT_ZERO(stat)         (uvm_stats.stat = 0)

/* Statistics functions */
void uvm_stat_init(void);
void uvm_stat_print(void);
void uvm_stat_reset(void);
```

## Swap Management

### Swap Interface (`uvm_swap.h`)
Virtual memory swapping support:

```c
/* Swap slot structure */
struct swslot {
    TAILQ_ENTRY(swslot) ssw_next;   /* Next swap slot */
    swblk_t ssw_blk;                /* Swap block */
    u_int ssw_refcnt;               /* Reference count */
    u_int ssw_flags;                /* Flags */
    struct vm_page *ssw_page;       /* Associated page */
};

/* Swap flags */
#define SWF_INUSE       0x0001      /* Slot in use */
#define SWF_BUSY        0x0002      /* Slot busy */
#define SWF_WANTED      0x0004      /* Slot wanted */
#define SWF_DIRTY       0x0008      /* Slot dirty */

/* Swap device */
struct swdevt {
    TAILQ_ENTRY(swdevt) sw_next;    /* Next swap device */
    struct vnode *sw_vp;            /* Swap vnode */
    swblk_t sw_nblks;               /* Number of blocks */
    swblk_t sw_used;                /* Blocks used */
    u_int sw_flags;                 /* Flags */
    char *sw_name;                  /* Device name */
};

/* Swap operations */
void uvm_swap_init(void);
swblk_t uvm_swap_alloc(int *, int);
void uvm_swap_free(swblk_t, int);
int uvm_swap_io(struct vm_page *, swblk_t, int, int);
void uvm_swap_stats(void);
```

## Physical Memory Mapping

### Architecture-Specific Mapping (`pmap/`)
Hardware-specific physical memory mapping:

```c
/* Physical mapping interface (pmap.h) */
struct pmap {
    struct pmap_statistics stats;   /* Statistics */
    struct lock pmap_lock;          /* Lock */
    void *pm_pdir;                  /* Page directory */
    u_int pm_refcount;              /* Reference count */
    u_int pm_flags;                 /* Flags */
    struct vm_page *pm_ptphint;     /* Page table hint */
    struct pv_entry *pm_pvhint;     /* PV hint */
    struct pmap *pm_list;           /* Global list */
};

/* Physical mapping operations */
void pmap_bootstrap(vaddr_t, vaddr_t);
pmap_t pmap_create(void);
void pmap_destroy(pmap_t);
void pmap_reference(pmap_t);
void pmap_enter(pmap_t, vaddr_t, paddr_t, vm_prot_t, vm_prot_t, int);
void pmap_remove(pmap_t, vaddr_t, vaddr_t);
void pmap_protect(pmap_t, vaddr_t, vaddr_t, vm_prot_t);
void pmap_update(pmap_t);
boolean_t pmap_extract(pmap_t, vaddr_t, paddr_t *);
void pmap_copy(pmap_t, pmap_t, vaddr_t, vsize_t, vaddr_t);
void pmap_activate(struct lwp *);
void pmap_deactivate(struct lwp *);
void pmap_zero_page(vm_page_t);
void pmap_copy_page(vm_page_t, vm_page_t);
void pmap_page_protect(vm_page_t, vm_prot_t);
```

## Build System Integration

### UVM Build Configuration
Integration with MINIX build system:

```makefile
# sys/uvm/Makefile
# UVM build configuration

# Core UVM sources
SRCS += uvm_amap.c
SRCS += uvm_anon.c
SRCS += uvm_aobj.c
SRCS += uvm_fault.c
SRCS += uvm_glue.c
SRCS += uvm_km.c
SRCS += uvm_loan.c
SRCS += uvm_map.c
SRCS += uvm_object.c
SRCS += uvm_page.c
SRCS += uvm_pager.c
SRCS += uvm_pdaemon.c
SRCS += uvm_pdpolicy.c
SRCS += uvm_pglist.c
SRCS += uvm_pmap.c
SRCS += uvm_prot.c
SRCS += uvm_readahead.c
SRCS += uvm_stat.c
SRCS += uvm_swap.c

# Physical mapping
SUBDIR += pmap

# Installation
includes:
	install -m 444 uvm*.h ${DESTDIR}/usr/include/uvm/
```

### Architecture-Specific Configuration
Platform-specific memory management:

```makefile
# Architecture-specific UVM configuration
.if ${MACHINE_ARCH} == "evbriscv64"
# RISC-V 64-bit specific
CFLAGS += -DRISCV64_MMU
CFLAGS += -DSV39_VIRTUAL_MEMORY
.elif ${MACHINE_ARCH} == "i386"
# x86 specific
CFLAGS += -DI386_MMU
CFLAGS += -DI386_PAE_SUPPORT
.elif ${MACHINE_ARCH} == "arm"
# ARM specific
CFLAGS += -DARM_MMU
CFLAGS += -DARM_L1_L2_TABLES
.endif
```

## Integration with MINIX Microkernel

### Message-Based VM Interface
Integration with MINIX's message-passing architecture:

```c
/* VM message interface */
struct vm_message {
    int vm_type;                    /* Message type */
    int vm_operation;               /* VM operation */
    int vm_result;                  /* Result */
    
    union {
        /* Memory allocation */
        struct {
            vsize_t size;           /* Size */
            vaddr_t addr;           /* Address */
            int flags;              /* Flags */
        } alloc;
        
        /* Memory mapping */
        struct {
            vaddr_t addr;           /* Address */
            vsize_t size;           /* Size */
            vm_prot_t prot;         /* Protection */
            int flags;              /* Flags */
        } map;
        
        /* Memory protection */
        struct {
            vaddr_t start;          /* Start address */
            vaddr_t end;            /* End address */
            vm_prot_t prot;         /* Protection */
        } protect;
        
        /* Memory unmapping */
        struct {
            vaddr_t start;          /* Start address */
            vaddr_t end;            /* End address */
        } unmap;
    } u;
};

/* VM operation handlers */
int vm_handle_alloc(struct vm_message *msg);
int vm_handle_map(struct vm_message *msg);
int vm_handle_protect(struct vm_message *msg);
int vm_handle_unmap(struct vm_message *msg);
```

### VM Manager Integration
Integration with MINIX virtual memory manager:

```c
/* VM manager registration */
int vm_manager_register(const char *name, int vm_id,
                       const struct vm_operations *ops, void *private);
int vm_manager_unregister(int vm_id);

/* VM operations structure */
struct vm_operations {
    int (*do_alloc)(void *private, vsize_t size, vaddr_t *addr, int flags);
    int (*do_free)(void *private, vaddr_t addr, vsize_t size);
    int (*do_map)(void *private, vaddr_t addr, vsize_t size, vm_prot_t prot, int flags);
    int (*do_unmap)(void *private, vaddr_t addr, vsize_t size);
    int (*do_protect)(void *private, vaddr_t addr, vsize_t size, vm_prot_t prot);
};
```

## Testing and Validation

### UVM Test Framework
Comprehensive testing for virtual memory operations:

```c
/* UVM test framework */
struct uvm_test_case {
    const char *name;               /* Test name */
    int (*test_func)(void);         /* Test function */
    int expected_result;            /* Expected result */
};

/* Memory management tests */
static struct uvm_test_case uvm_tests[] = {
    { "page_allocation", test_page_allocation, 0 },
    { "memory_mapping", test_memory_mapping, 0 },
    { "fault_handling", test_fault_handling, 0 },
    { "protection_checks", test_protection_checks, 0 },
    { "swap_operations", test_swap_operations, 0 },
    /* ... additional tests ... */
};

/* Memory stress tests */
int test_memory_stress(void) {
    int result = 0;
    
    /* Test high memory pressure */
    result |= test_high_memory_pressure();
    
    /* Test memory fragmentation */
    result |= test_memory_fragmentation();
    
    /* Test concurrent memory operations */
    result |= test_concurrent_memory_ops();
    
    return result;
}
```

### Cross-Architecture Memory Testing
Ensures memory management works across architectures:

```c
/* Cross-architecture memory validation */
int test_cross_arch_memory(void) {
    int result = 0;
    
    /* Test page size compatibility */
    result |= test_page_size_compatibility();
    
    /* Test address space layout */
    result |= test_address_space_layout();
    
    /* Test memory alignment */
    result |= test_memory_alignment();
    
    /* Test cache coherency */
    result |= test_cache_coherency();
    
    return result;
}
```

The Unified Virtual Memory interface system provides the essential foundation that enables MINIX 3 to manage memory efficiently across different hardware platforms while maintaining the modularity and reliability principles of the microkernel architecture. This comprehensive memory management framework ensures consistent and reliable virtual memory operations from boot through runtime execution.