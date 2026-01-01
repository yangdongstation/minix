# MINIX Device Driver Interface System

## Overview

The `sys/dev/` directory contains the device driver interface system for MINIX 3, providing essential definitions, utilities, and interfaces for hardware device management. This directory serves as the central hub for device driver development, offering standardized interfaces for clock management, device verbosity control, and virtual disk device operations that bridge the gap between MINIX's microkernel architecture and diverse hardware peripherals.

## Purpose and Responsibilities

### Core Functions
- **Device Driver Interface Standardization**: Provides standardized interfaces for device driver development
- **Clock Subsystem Management**: Manages system clock and timing-related operations
- **Device Verbose Output Control**: Controls device driver diagnostic and logging output
- **Virtual Disk Device Support**: Provides virtual disk device interfaces and definitions
- **Hardware Abstraction**: Abstracts hardware-specific details from higher-level system components

### Key Features
- **Clock Abstraction** - Unified clock management across different hardware platforms
- **Device Verbosity Framework** - Configurable device driver logging and debugging
- **Virtual Device Support** - Virtual disk and device emulation interfaces
- **Hardware Portability** - Platform-independent device driver interfaces
- **Performance Monitoring** - Device performance and statistics tracking

## Directory Structure

```
sys/dev/
├── Makefile               # Main device interface makefile
├── clock_subr.c          # Clock subsystem utilities
├── clock_subr.h          # Clock subsystem header
├── dev_verbose.c         # Device verbosity control
├── dev_verbose.h         # Device verbosity header
├── vndvar.h              # Virtual disk device definitions
├── i2c/                  # I2C bus interface
│   ├── i2c_bitbang.c     # Bit-banged I2C implementation
│   ├── i2c_bitbang.h     # Bit-banged I2C header
│   ├── i2c.c             # I2C core implementation
│   ├── i2c.h             # I2C interface header
│   ├── i2c_io.c          # I2C I/O operations
│   ├── i2cvar.h          # I2C device variables
│   └── Makefile          # I2C makefile
├── ic/                   # Integrated circuit support
│   ├── ds1672.c          # DS1672 real-time clock
│   ├── ds1672reg.h       # DS1672 register definitions
│   ├── ds17x85.c         # DS17x85 RTC family
│   ├── ds17x85reg.h      # DS17x85 register definitions
│   ├── hd64770.c         # HD64770 controller
│   ├── hd64770reg.h      # HD64770 register definitions
│   ├── i82365.c          # i82365 PCMCIA controller
│   ├── i82365reg.h       # i82365 register definitions
│   ├── Makefile          # IC makefile
│   ├── mk48txx.c         # MK48TXX RTC family
│   ├── mk48txxreg.h      # MK48TXX register definitions
│   ├── pc87307.c         # PC87307 super I/O
│   ├── pc87307reg.h      # PC87307 register definitions
│   ├── rp5c01.c          # RP5C01 RTC
│   ├── rp5c01reg.h       # RP5C01 register definitions
│   └── stc.c             # STC timer
├── pci/                  # PCI bus interface
│   ├── devs_data.h       # PCI device data
│   ├── devs.h            # PCI device definitions
│   ├── if_de.c           # DEC network interface
│   ├── if_devar.h        # DEC network variables
│   ├── if_fxp.c          # Intel fxp network interface
│   ├── if_fxpvar.h       # Intel fxp variables
│   ├── Makefile          # PCI makefile
│   ├── pci.c             # PCI core implementation
│   ├── pci.h             # PCI interface header
│   ├── pciid.h           # PCI ID definitions
│   ├── pci_machdep.c     /* Machine-dependent PCI support
│   ├── pcivar.h          # PCI device variables
│   ├── ppb.c             # PCI-PCI bridge
│   ├── ppbvar.h          # PCI-PCI bridge variables
│   ├── if_rl.c           # Realtek network interface
│   ├── if_rlreg.h        # Realtek register definitions
│   ├── if_rlvar.h        # Realtek variables
│   ├── if_tlp.c          # TLP network interface
│   ├── if_tlpreg.h       # TLP register definitions
│   ├── if_tlpvar.h       # TLP variables
│   └── if_wm.c           # Intel wm network interface
├── stbi/                 # STB image support
│   ├── stbi.c            # STB image implementation
│   ├── stbi.h            # STB image header
│   └── Makefile          # STBI makefile
└── videomode/            # Video mode support
    ├── db_videomodes.c   # Video mode database
    ├── edid.c            # EDID parsing
    ├── edid.h            # EDID header
    ├── edidvar.h         # EDID variables
    ├── Makefile          # Videomode makefile
    ├── videomode.c       # Video mode implementation
    └── videomode.h       # Video mode header
```

## Clock Subsystem Interface

### Clock Management Framework
The clock subsystem provides unified time management across different hardware platforms:

```c
/* Clock subsystem interface (clock_subr.h) */
struct clock_ymdhms {
    int     dt_year;            /* Year (AD) */
    int     dt_mon;             /* Month (1-12) */
    int     dt_day;             /* Day (1-31) */
    int     dt_hour;            /* Hour (0-23) */
    int     dt_min;             /* Minute (0-59) */
    int     dt_sec;             /* Second (0-59) */
    int     dt_wday;            /* Weekday (0-6, Sunday = 0) */
    int     dt_yday;            /* Yearday (0-365) */
};

/* Clock conversion functions */
void clock_gmt_to_ymdhms(const time_t *gmt, struct clock_ymdhms *ymdhms);
time_t clock_ymdhms_to_gmt(const struct clock_ymdhms *ymdhms);
int clock_leap_year(int year);
int clock_days_in_month(int year, int month);
int clock_ymdhms_to_secs(const struct clock_ymdhms *ymdhms);
```

### Hardware Clock Integration
Integration with hardware real-time clocks:

```c
/* Hardware clock interface */
struct clock_hw_if {
    int (*get)(struct device *dev, struct clock_ymdhms *ymdhms);
    int (*set)(struct device *dev, const struct clock_ymdhms *ymdhms);
    int (*get_secs)(struct device *dev, time_t *secs);
    int (*set_secs)(struct device *dev, time_t secs);
    bool (*ok_to_set)(struct device *dev);
};

/* Clock hardware registration */
void clock_hw_register(struct device *dev, const struct clock_hw_if *hw_if);
void clock_hw_unregister(struct device *dev);
```

## Device Verbosity Framework

### Verbose Output Control
Comprehensive framework for controlling device driver diagnostic output:

```c
/* Device verbosity levels (dev_verbose.h) */
#define DEV_VERBOSE_OFF     0   /* No verbose output */
#define DEV_VERBOSE_ERROR   1   /* Error messages only */
#define DEV_VERBOSE_WARN    2   /* Warnings and errors */
#define DEV_VERBOSE_INFO    3   /* Informational messages */
#define DEV_VERBOSE_DEBUG   4   /* Debug messages */
#define DEV_VERBOSE_TRACE   5   /* Full tracing */

/* Device verbosity control structure */
struct dev_verbose {
    const char *dv_name;        /* Device name */
    int         dv_level;       /* Current verbosity level */
    int         dv_flags;       /* Control flags */
};

/* Global verbosity control */
extern int dev_verbose_global;

/* Verbose output macros */
#define DEV_VERBOSE(dev, level, fmt, ...) do { \
    if (dev_verbose_enabled(dev, level)) { \
        printf("%s: " fmt "\n", device_xname(dev), ##__VA_ARGS__); \
    } \
} while (0)

#define DEV_VERBOSE_ERROR(dev, fmt, ...) \
    DEV_VERBOSE(dev, DEV_VERBOSE_ERROR, fmt, ##__VA_ARGS__)

#define DEV_VERBOSE_WARN(dev, fmt, ...) \
    DEV_VERBOSE(dev, DEV_VERBOSE_WARN, fmt, ##__VA_ARGS__)

#define DEV_VERBOSE_INFO(dev, fmt, ...) \
    DEV_VERBOSE(dev, DEV_VERBOSE_INFO, fmt, ##__VA_ARGS__)

#define DEV_VERBOSE_DEBUG(dev, fmt, ...) \
    DEV_VERBOSE(dev, DEV_VERBOSE_DEBUG, fmt, ##__VA_ARGS__)
```

### Runtime Verbosity Control
Dynamic control of device verbosity levels:

```c
/* Runtime verbosity management */
int dev_verbose_set(const char *name, int level);
int dev_verbose_get(const char *name);
void dev_verbose_list(void);
bool dev_verbose_enabled(struct device *dev, int level);

/* Boot-time verbosity configuration */
void dev_verbose_init(void);
int dev_verbose_parse(const char *spec);

/* Example usage in device driver */
static int
mydevice_attach(struct device *parent, struct device *self, void *aux)
{
    struct mydevice_softc *sc = device_private(self);
    
    DEV_VERBOSE_INFO(self, "attaching device");
    
    if (mydevice_hw_init(sc) != 0) {
        DEV_VERBOSE_ERROR(self, "hardware initialization failed");
        return -1;
    }
    
    DEV_VERBOSE_DEBUG(self, "device attached successfully");
    return 0;
}
```

## Virtual Disk Device Support

### Virtual Disk Interface
Virtual disk device definitions for disk image and virtual storage support:

```c
/* Virtual disk device structure (vndvar.h) */
struct vnd_softc {
    device_t        sc_dev;         /* Device structure */
    struct disk     sc_disk;        /* Disk structure */
    struct vnode    *sc_vp;         /* Backing vnode */
    struct pathbuf  *sc_pathbuf;    /* Path buffer */
    
    /* Device parameters */
    uint64_t        sc_size;        /* Device size in bytes */
    uint32_t        sc_sector_size; /* Sector size */
    uint32_t        sc_flags;       /* Device flags */
    
    /* I/O statistics */
    uint64_t        sc_reads;       /* Number of reads */
    uint64_t        sc_writes;      /* Number of writes */
    uint64_t        sc_read_bytes;  /* Bytes read */
    uint64_t        sc_write_bytes; /* Bytes written */
    
    /* Synchronization */
    kmutex_t        sc_lock;        /* Device lock */
    kcondvar_t      sc_cv;          /* Condition variable */
    
    /* Worker thread */
    kthread_t       *sc_thread;     /* Worker thread */
    struct workqueue *sc_wq;        /* Work queue */
};

/* Virtual disk flags */
#define VNF_READONLY    0x0001      /* Read-only device */
#define VNF_VALID       0x0002      /* Device is valid */
#define VNF_WANTCLOSE   0x0004      /* Close requested */
#define VNF_WANTED      0x0008      /* Device wanted */
```

### Virtual Disk Operations
Standard disk operations for virtual devices:

```c
/* Virtual disk I/O operations */
int vndopen(dev_t dev, int flags, int fmt, struct lwp *l);
int vndclose(dev_t dev, int flags, int fmt, struct lwp *l);
int vndstrategy(struct buf *bp);
int vndioctl(dev_t dev, u_long cmd, void *data, int flags, struct lwp *l);
int vndsize(dev_t dev);

/* Virtual disk configuration */
int vndconfig(struct vnd_ioctl *vio, struct lwp *l);
int vndunconfig(dev_t dev, int flags, struct lwp *l);

/* Virtual disk utility functions */
int vndgetdisklabel(dev_t dev, struct disklabel *lp, struct cpu_disklabel *clp);
void vndiodone(struct buf *bp);
```

## I2C Bus Interface

### I2C Core Interface
Inter-Integrated Circuit bus support for embedded systems:

```c
/* I2C interface definitions (i2c.h) */
struct i2c_controller {
    const struct i2c_methods *ic_methods;
    void *ic_cookie;
    int ic_flags;
};

struct i2c_methods {
    /* Basic I2C operations */
    int (*im_send)(void *cookie, uint16_t addr, uint8_t *buf, size_t len, int flags);
    int (*im_recv)(void *cookie, uint16_t addr, uint8_t *buf, size_t len, int flags);
    int (*im_exec)(void *cookie, i2c_op_t op, void *cmdbuf, size_t cmdlen,
                   void *buf, size_t len, int flags);
    
    /* Advanced features */
    int (*im_poll)(void *cookie, int events, int *revents);
    int (*im_set_freq)(void *cookie, uint32_t freq);
    uint32_t (*im_get_freq)(void *cookie);
};

/* I2C operation flags */
#define I2C_F_READ      0x0001  /* Read operation */
#define I2C_F_WRITE     0x0002  /* Write operation */
#define I2C_F_STOP      0x0004  /* Generate stop condition */
#define I2C_F_RESTART   0x0008  /* Generate restart condition */
#define I2C_F_POLL      0x0010  /* Poll for completion */
#define I2C_F_NOERR     0x0020  /* Ignore errors */
```

### I2C Device Support
Support for I2C-based devices and sensors:

```c
/* I2C device attachment */
struct i2c_attach_args {
    uint16_t ia_addr;           /* Device address */
    uint32_t ia_size;           /* Device size */
    const char *ia_name;        /* Device name */
    int ia_flags;               /* Device flags */
};

/* I2C device registration */
void i2c_attach(struct i2c_controller *ic, struct device *parent);
int i2c_scan(struct i2c_controller *ic, struct device *parent);

/* I2C utility functions */
int i2c_read(struct i2c_controller *ic, uint16_t addr, uint8_t *buf, size_t len, int flags);
int i2c_write(struct i2c_controller *ic, uint16_t addr, const uint8_t *buf, size_t len, int flags);
int i2c_exec(struct i2c_controller *ic, i2c_op_t op, void *cmdbuf, size_t cmdlen,
             void *buf, size_t len, int flags);
```

## PCI Bus Interface

### PCI Core Interface
Peripheral Component Interconnect bus support:

```c
/* PCI device structure (pcivar.h) */
struct pci_attach_args {
    struct pci_controller *pa_pc;       /* PCI controller */
    pcitag_t pa_tag;                    /* PCI device tag */
    int pa_bus;                         /* Bus number */
    int pa_device;                      /* Device number */
    int pa_function;                    /* Function number */
    pcireg_t pa_id;                     /* Device ID */
    pcireg_t pa_class;                  /* Device class */
    
    /* Resource information */
    pcireg_t pa_map[PCI_MAPREG_COUNT];  /* Base address registers */
    pcireg_t pa_map_type[PCI_MAPREG_COUNT];
    pcireg_t pa_map_flags[PCI_MAPREG_COUNT];
    
    /* Interrupt information */
    int pa_intrline;                    /* Interrupt line */
    int pa_intrpin;                     /* Interrupt pin */
    int pa_irq;                         /* IRQ number */
};

/* PCI configuration space access */
pcireg_t pci_conf_read(struct pci_controller *pc, pcitag_t tag, int reg);
void pci_conf_write(struct pci_controller *pc, pcitag_t tag, int reg, pcireg_t data);

/* PCI device enumeration */
int pci_find_device(struct pci_controller *pc, int vendor, int product, int *busp, int *devp, int *funcp);
int pci_find_device_by_class(struct pci_controller *pc, int class, int *busp, int *devp, int *funcp);
```

### PCI Device Support
Standard PCI device drivers and interfaces:

```c
/* Network interface drivers */
extern struct cfattach de_ca;       /* DEC network interface */
extern struct cfattach fxp_ca;      /* Intel fxp network interface */
extern struct cfattach rl_ca;       /* Realtek network interface */
extern struct cfattach tlp_ca;      /* TLP network interface */
extern struct cfattach wm_ca;       /* Intel wm network interface */

/* PCI bridge support */
extern struct cfattach ppb_ca;      /* PCI-PCI bridge */
```

## Video Mode Support

### Video Mode Interface
Display and video mode management:

```c
/* Video mode structure (videomode.h) */
struct videomode {
    const char *name;           /* Mode name */
    uint32_t type;              /* Mode type */
    uint32_t pixel_clock;       /* Pixel clock in kHz */
    uint32_t hdisplay;          /* Horizontal display */
    uint32_t hsync_start;       /* Horizontal sync start */
    uint32_t hsync_end;         /* Horizontal sync end */
    uint32_t htotal;            /* Horizontal total */
    uint32_t vdisplay;          /* Vertical display */
    uint32_t vsync_start;       /* Vertical sync start */
    uint32_t vsync_end;         /* Vertical sync end */
    uint32_t vtotal;            /* Vertical total */
    uint32_t flags;             /* Mode flags */
    uint32_t privsize;          /* Private data size */
};

/* Video mode database */
extern const struct videomode videomode_db[];
extern const int videomode_db_size;

/* Video mode lookup */
const struct videomode *videomode_find(const char *name);
const struct videomode *videomode_find_by_mode(const struct videomode *mode);
int videomode_from_edid(const uint8_t *edid, struct videomode *mode);
```

### EDID Support
Extended Display Identification Data parsing:

```c
/* EDID parsing (edid.h) */
struct edid_info {
    char vendor[4];             /* Vendor ID */
    char product[16];           /* Product name */
    char serial[14];            /* Serial number */
    uint32_t serial_number;     /* Serial number (numeric) */
    uint8_t week;               /* Manufacturing week */
    uint8_t year;               /* Manufacturing year */
    
    /* Display parameters */
    uint32_t width_mm;          /* Width in mm */
    uint32_t height_mm;         /* Height in mm */
    uint8_t gamma;              /* Gamma value */
    uint8_t dpms_flags;         /* DPMS flags */
    
    /* Supported modes */
    uint32_t nmodes;            /* Number of supported modes */
    struct videomode *modes;    /* Supported modes */
};

/* EDID parsing functions */
int edid_parse(const uint8_t *edid, struct edid_info *info);
int edid_is_valid(const uint8_t *edid);
void edid_print(const struct edid_info *info);
```

## Build System Integration

### Device Interface Build Configuration
Integration with the MINIX build system:

```makefile
# sys/dev/Makefile
# Device interface build configuration

# Core device interfaces
SRCS += clock_subr.c
SRCS += dev_verbose.c

# I2C support
SUBDIR += i2c

# IC support
SUBDIR += ic

# PCI support
SUBDIR += pci

# STBI support
SUBDIR += stbi

# Video mode support
SUBDIR += videomode

# Installation
includes:
	install -m 444 clock_subr.h ${DESTDIR}/usr/include/sys/
	install -m 444 dev_verbose.h ${DESTDIR}/usr/include/sys/
	install -m 444 vndvar.h ${DESTDIR}/usr/include/sys/
```

### Cross-Architecture Support
Device interfaces support multiple architectures:

```makefile
# Architecture-specific device support
.if ${MACHINE_ARCH} == "evbriscv64"
# RISC-V specific devices
SRCS += riscv_timer.c
SRCS += riscv_clint.c
.elif ${MACHINE_ARCH} == "i386"
# x86 specific devices
SRCS += i386_timer.c
SRCS += i386_pit.c
.elif ${MACHINE_ARCH} == "arm"
# ARM specific devices
SRCS += arm_timer.c
SRCS += arm_pl011.c
.endif
```

## Integration with MINIX Microkernel

### Message-Based Device Interface
Integration with MINIX's message-passing architecture:

```c
/* Device driver message interface */
struct device_message {
    int     dm_type;            /* Message type */
    int     dm_device;          /* Device number */
    int     dm_operation;       /* Operation code */
    void    *dm_data;           /* Operation data */
    size_t  dm_size;            /* Data size */
    int     dm_result;          /* Operation result */
};

/* Device driver message handlers */
int device_handle_read(struct device_message *msg);
int device_handle_write(struct device_message *msg);
int device_handle_ioctl(struct device_message *msg);
```

### Device Manager Integration
Integration with MINIX device manager:

```c
/* Device registration with device manager */
int device_register(const char *name, int device_id, 
                   const struct device_ops *ops, void *private);
int device_unregister(int device_id);

/* Device operation structure */
struct device_ops {
    int (*do_open)(void *private, int flags);
    int (*do_close)(void *private, int flags);
    int (*do_read)(void *private, void *buf, size_t len, off_t offset);
    int (*do_write)(void *private, const void *buf, size_t len, off_t offset);
    int (*do_ioctl)(void *private, u_long cmd, void *data, int flags);
};
```

## Testing and Validation

### Device Interface Testing
Comprehensive testing framework for device interfaces:

```c
/* Device interface test framework */
struct device_test_case {
    const char *name;
    int (*test_func)(void);
    int expected_result;
};

/* Clock subsystem tests */
static struct device_test_case clock_tests[] = {
    { "clock_gmt_to_ymdhms", test_clock_gmt_to_ymdhms, 0 },
    { "clock_ymdhms_to_gmt", test_clock_ymdhms_to_gmt, 0 },
    { "clock_leap_year", test_clock_leap_year, 0 },
    /* ... additional tests ... */
};

/* Device verbosity tests */
static struct device_test_case verbose_tests[] = {
    { "dev_verbose_set", test_dev_verbose_set, 0 },
    { "dev_verbose_get", test_dev_verbose_get, 0 },
    { "dev_verbose_enabled", test_dev_verbose_enabled, 0 },
    /* ... additional tests ... */
};
```

### Hardware-Specific Testing
Platform-specific device testing:

```c
/* Platform-specific device tests */
int test_platform_devices(void) {
    int result = 0;
    
    /* Test I2C interface */
    result |= test_i2c_interface();
    
    /* Test PCI interface */
    result |= test_pci_interface();
    
    /* Test clock hardware */
    result |= test_clock_hardware();
    
    /* Test video modes */
    result |= test_videomodes();
    
    return result;
}
```

The device driver interface system provides the essential foundation that enables MINIX 3 to support diverse hardware peripherals while maintaining the modularity and reliability principles of the microkernel architecture. This comprehensive device interface framework ensures consistent hardware abstraction across different platforms and device types.