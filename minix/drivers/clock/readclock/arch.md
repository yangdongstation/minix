# MINIX Real-Time Clock Driver Architecture

## Overview

The readclock driver provides battery-backed real-time clock (RTC) functionality for MINIX systems. This driver is essential for maintaining accurate system time, providing persistent timekeeping across reboots, and supporting alarm-based wake functionality. The driver implements a hardware abstraction layer that supports various RTC implementations across different architectures.

## Real-Time Clock Architecture

### RTC Functions
- **Time keeping**: Maintain current date and time
- **Persistent storage**: Battery-backed time across power cycles
- **Alarm support**: Programmable wake-up alarms
- **Time zone handling**: Support for local time conversions
- **Power management**: Low-power operation modes
- **Synchronization**: Coordinate with system time

### Hardware Implementations
```
RTC Hardware Types
├── MC146818 (PC/AT Standard)
├── Dallas DS12887/DS12885
├── STM32 RTC
├── OMAP RTC
├── Allwinner RTC
├── RISC-V Platform RTC
└── ACPI RTC (via ACPI tables)
```

## Driver Architecture

### Hardware Abstraction Layer
The driver uses a pluggable architecture for different RTC hardware:
```c
/* RTC hardware interface */
struct rtc {
    int (*init)(void);                    // Initialize hardware
    int (*get_time)(struct tm *t, int flags); // Get current time
    int (*set_time)(struct tm *t, int flags); // Set current time
    int (*pwr_off)(void);                 // Prepare for power off
    void (*exit)(void);                   // Cleanup
};

/* Architecture setup function */
int arch_setup(struct rtc *r);
```

### Driver State Management
```c
/* Readclock driver state */
struct readclock_state {
    int initialized;                // Driver initialized
    struct rtc *hw_rtc;             // Hardware RTC interface
    endpoint_t owner;               // Current owner endpoint
    int open_count;                 // Number of open instances
    mutex_t lock;                   // Driver mutex
};
```

## Source Code Organization

### Main Driver Files

#### `readclock.c` - Core Driver Implementation
```c
/* Driver entry points */
static int readclock_open(devminor_t minor, int access, endpoint_t user_endpt);
static int readclock_close(devminor_t minor);
static ssize_t readclock_read(devminor_t minor, u64_t position, endpoint_t endpt,
                             cp_grant_id_t grant, size_t size, int flags, cdev_id_t id);
static ssize_t readclock_write(devminor_t minor, u64_t position, endpoint_t endpt,
                              cp_grant_id_t grant, size_t size, int flags, cdev_id_t id);
static int readclock_ioctl(devminor_t minor, unsigned long request, endpoint_t endpt,
                          cp_grant_id_t grant, int flags, endpoint_t user_endpt, cdev_id_t id);

/* Helper functions */
static int readclock_get_time_raw(struct tm *tm);
static int readclock_set_time_raw(struct tm *tm);
static int readclock_bcd_to_dec(int n);
static int readclock_dec_to_bcd(int n);
static int readclock_validate_time(struct tm *tm);
static void readclock_normalize_time(struct tm *tm);
```

#### `readclock.h` - Driver Interface
```c
/* RTC IOCTL commands */
#define RTC_GET_TIME    _IOR('r', 1, struct tm)
#define RTC_SET_TIME    _IOW('r', 2, struct tm)
#define RTC_GET_STATUS  _IOR('r', 3, int)
#define RTC_SET_ALARM   _IOW('r', 4, struct rtc_time)
#define RTC_GET_ALARM   _IOR('r', 5, struct rtc_time)
#define RTC_CLEAR_ALARM _IO('r', 6)

/* Time validation flags */
#define RTC_TIME_LOCAL  0x01        // Local time (not UTC)
#define RTC_TIME_BCD    0x02        // BCD format
#define RTC_TIME_24H    0x04        // 24-hour format
```

#### `forward.c` and `forward.h` - Forward Declarations
Contains function prototypes and shared definitions.

## Architecture-Specific Implementations

### x86/PC Architecture (arch/i386/)

#### Standard PC RTC (MC146818)
```c
/* PC RTC hardware registers */
#define RTC_ADDR        0x70        // RTC address port
#define RTC_DATA        0x71        // RTC data port

/* RTC register offsets */
#define RTC_SEC         0x00        // Seconds (0-59)
#define RTC_SEC_ALARM   0x01        // Seconds alarm
#define RTC_MIN         0x02        // Minutes (0-59)
#define RTC_MIN_ALARM   0x03        // Minutes alarm
#define RTC_HOUR        0x04        // Hours (0-23)
#define RTC_HOUR_ALARM  0x05        // Hours alarm
#define RTC_DOW         0x06        // Day of week (1-7)
#define RTC_DOM         0x07        // Day of month (1-31)
#define RTC_MON         0x08        // Month (1-12)
#define RTC_YEAR        0x09        // Year (0-99)

/* Control registers */
#define RTC_STATUSA     0x0A        // Status register A
#define RTC_STATUSB     0x0B        // Status register B
#define RTC_STATUSC     0x0C        // Status register C
#define RTC_STATUSD     0x0D        // Status register D

/* Status A bits */
#define RTC_A_UIP       0x80        // Update in progress
#define RTC_A_DV_MASK   0x70        // Divider bits
#define RTC_A_RS_MASK   0x0F        // Rate selection bits

/* Status B bits */
#define RTC_B_SET       0x80        // Set time mode
#define RTC_B_PIE       0x40        // Periodic interrupt enable
#define RTC_B_AIE       0x20        // Alarm interrupt enable
#define RTC_B_UIE       0x10        // Update interrupt enable
#define RTC_B_SQWE      0x08        // Square wave enable
#define RTC_B_DM        0x04        // Data mode (BCD/binary)
#define RTC_B_24H       0x02        // 24-hour mode
#define RTC_B_DSE       0x01        // Daylight saving enable
```

#### PC RTC Implementation
```c
/* PC RTC hardware interface */
static int pc_rtc_init(void)
{
    u8_t reg_b;
    
    /* Read status register B */
    outb(RTC_ADDR, RTC_STATUSB);
    reg_b = inb(RTC_DATA);
    
    /* Configure for binary mode, 24-hour format */
    reg_b |= RTC_B_DM;              // Binary data mode
    reg_b |= RTC_B_24H;             // 24-hour format
    outb(RTC_ADDR, RTC_STATUSB);
    outb(RTC_DATA, reg_b);
    
    return OK;
}

static int pc_rtc_get_time(struct tm *t, int flags)
{
    u8_t status;
    
    /* Wait for update to complete */
    do {
        outb(RTC_ADDR, RTC_STATUSA);
        status = inb(RTC_DATA);
    } while (status & RTC_A_UIP);
    
    /* Read time registers */
    outb(RTC_ADDR, RTC_SEC);
    t->tm_sec = inb(RTC_DATA);
    outb(RTC_ADDR, RTC_MIN);
    t->tm_min = inb(RTC_DATA);
    outb(RTC_ADDR, RTC_HOUR);
    t->tm_hour = inb(RTC_DATA);
    outb(RTC_ADDR, RTC_DOM);
    t->tm_mday = inb(RTC_DATA);
    outb(RTC_ADDR, RTC_MON);
    t->tm_mon = inb(RTC_DATA);
    outb(RTC_ADDR, RTC_YEAR);
    t->tm_year = inb(RTC_DATA);
    outb(RTC_ADDR, RTC_DOW);
    t->tm_wday = inb(RTC_DATA);
    
    /* Convert BCD to decimal if necessary */
    if (!(flags & RTC_TIME_BCD)) {
        t->tm_sec = bcd_to_dec(t->tm_sec);
        t->tm_min = bcd_to_dec(t->tm_min);
        t->tm_hour = bcd_to_dec(t->tm_hour);
        t->tm_mday = bcd_to_dec(t->tm_mday);
        t->tm_mon = bcd_to_dec(t->tm_mon);
        t->tm_year = bcd_to_dec(t->tm_year);
        t->tm_wday = bcd_to_dec(t->tm_wday);
    }
    
    /* Adjust for century */
    t->tm_year += 100;  // Assume 2000+ for now
    
    /* Adjust month to 0-based */
    t->tm_mon--;
    
    /* Adjust day of week to 0-based */
    t->tm_wday--;
    
    return OK;
}
```

### ARM Architecture (arch/earm/)

#### OMAP RTC Implementation
```c
/* OMAP RTC hardware interface */
struct omap_rtc {
    vir_bytes base;                 // Register base address
    int irq;                        // Interrupt number
    int uses_bcd;                   // BCD format flag
};

/* OMAP RTC registers */
#define OMAP_RTC_SECONDS            0x00
#define OMAP_RTC_MINUTES            0x04
#define OMAP_RTC_HOURS              0x08
#define OMAP_RTC_DAYS               0x0C
#define OMAPRTC_MONTHS              0x10
#define OMAP_RTC_YEARS              0x14
#define OMAP_RTC_WEEKS              0x18
#define OMAP_RTC_CONTROL            0x40
#define OMAP_RTC_STATUS             0x44
#define OMAPRTC_INTERRUPTS          0x48
#define OMAP_RTC_COMPL_SECONDS      0x50
#define OMAP_RTC_COMPL_MINUTES      0x54
#define OMAP_RTC_COMPL_HOURS        0x58
```

### RISC-V Architecture (arch/riscv64/)

#### RISC-V Platform RTC
```c
/* RISC-V RTC implementation */
struct riscv_rtc {
    vir_bytes base;                 // MMIO register base
    int irq;                        // PLIC interrupt
    u32_t clock_freq;               // Reference clock
};

/* RISC-V RTC functions */
int riscv_rtc_init(struct riscv_rtc *rtc);
int riscv_rtc_get_time(struct riscv_rtc *rtc, struct tm *t);
int riscv_rtc_set_time(struct riscv_rtc *rtc, struct tm *t);
int riscv_rtc_power_off(struct riscv_rtc *rtc);
```

## Device Interface

### Character Device Access
The RTC provides a character device interface:
```bash
# RTC device access
/dev/rtc          # Main RTC device
/dev/rtc0         # RTC instance 0
```

### IOCTL Commands
```c
/* Standard RTC IOCTLs */
RTC_RD_TIME       // Read time
RTC_SET_TIME      // Set time
RTC_ALM_READ      // Read alarm
RTC_ALM_SET       // Set alarm
RTC_IRQP_READ     // Read interrupt rate
RTC_IRQP_SET      // Set interrupt rate
RTC_PIE_ON        // Periodic interrupt on
RTC_PIE_OFF       // Periodic interrupt off
RTC_AIE_ON        // Alarm interrupt on
RTC_AIE_OFF       // Alarm interrupt off
RTC_UIE_ON        // Update interrupt on
RTC_UIE_OFF       // Update interrupt off
```

### Direct Register Access
For low-level access and debugging:
```c
/* Direct RTC register access */
int rtc_read_register(u8_t reg, u8_t *value);
int rtc_write_register(u8_t reg, u8_t value);
int rtc_dump_registers(void);
```

## Time Format Handling

### BCD to Decimal Conversion
```c
/* BCD conversion utilities */
int bcd_to_dec(int n)
{
    return ((n >> 4) * 10) + (n & 0x0F);
}

int dec_to_bcd(int n)
{
    return ((n / 10) << 4) | (n % 10);
}
```

### Time Validation
```c
/* Time validation function */
int readclock_validate_time(struct tm *tm)
{
    /* Validate seconds */
    if (tm->tm_sec < 0 || tm->tm_sec > 59) return EINVAL;
    
    /* Validate minutes */
    if (tm->tm_min < 0 || tm->tm_min > 59) return EINVAL;
    
    /* Validate hours */
    if (tm->tm_hour < 0 || tm->tm_hour > 23) return EINVAL;
    
    /* Validate day of month */
    if (tm->tm_mday < 1 || tm->tm_mday > 31) return EINVAL;
    
    /* Validate month */
    if (tm->tm_mon < 0 || tm->tm_mon > 11) return EINVAL;
    
    /* Validate year (reasonable range) */
    if (tm->tm_year < 70 || tm->tm_year > 200) return EINVAL;
    
    return OK;
}
```

### Time Normalization
```c
/* Time normalization function */
void readclock_normalize_time(struct tm *tm)
{
    static const int days_in_month[12] = {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
    };
    
    /* Handle leap years */
    int leap_year = (tm->tm_year % 4 == 0 && 
                    (tm->tm_year % 100 != 0 || tm->tm_year % 400 == 0));
    
    if (leap_year) days_in_month[1] = 29;
    
    /* Normalize seconds */
    while (tm->tm_sec >= 60) {
        tm->tm_sec -= 60;
        tm->tm_min++;
    }
    
    /* Normalize minutes */
    while (tm->tm_min >= 60) {
        tm->tm_min -= 60;
        tm->tm_hour++;
    }
    
    /* Normalize hours */
    while (tm->tm_hour >= 24) {
        tm->tm_hour -= 24;
        tm->tm_mday++;
    }
    
    /* Normalize day of month */
    while (tm->tm_mday > days_in_month[tm->tm_mon]) {
        tm->tm_mday -= days_in_month[tm->tm_mon];
        tm->tm_mon++;
        
        if (tm->tm_mon >= 12) {
            tm->tm_mon = 0;
            tm->tm_year++;
        }
    }
}
```

## Power Management

### System Power Off
```c
/* Power off preparation */
int readclock_power_off(void)
{
    struct tm current_time;
    int r;
    
    /* Get current system time */
    if ((r = gettimeofday(&tv, NULL)) != OK) {
        return r;
    }
    
    /* Convert to RTC format */
    localtime_r(&tv.tv_sec, &current_time);
    
    /* Write time to RTC */
    if ((r = readclock_set_time_raw(&current_time)) != OK) {
        return r;
    }
    
    /* Call hardware-specific power off */
    if (rtc_state.hw_rtc && rtc_state.hw_rtc->pwr_off) {
        return rtc_state.hw_rtc->pwr_off();
    }
    
    return OK;
}
```

### Low-Power Operation
- **Battery backup**: Maintain time during power loss
- **Reduced power mode**: Lower power consumption
- **Periodic updates**: Minimize RTC access
- **Wake-on-alarm**: System wake functionality

## Error Handling

### Common Error Conditions
```c
/* RTC error codes */
#define RTC_ERROR_INVALID_TIME   1  // Invalid time value
#define RTC_ERROR_HARDWARE_FAULT 2  // Hardware failure
#define RTC_ERROR_TIMEOUT        3  // Operation timeout
#define RTC_ERROR_NO_PERMISSION  4  // Permission denied
#define RTC_ERROR_BUSY           5  // RTC busy updating
```

### Error Recovery
```c
/* Error recovery functions */
int rtc_recover_from_error(int error);
int rtc_reset_hardware(void);
int rtc_check_hardware(void);
int rtc_wait_for_ready(void);
```

### Hardware Fault Handling
```c
/* Hardware fault detection */
int rtc_detect_hardware_fault(void)
{
    u8_t status;
    
    /* Check RTC status */
    rtc_read_register(RTC_STATUSD, &status);
    
    /* Check for valid RAM and time */
    if (!(status & 0x80)) {
        return RTC_ERROR_HARDWARE_FAULT;
    }
    
    return OK;
}
```

## Testing and Debugging

### Test Utilities
```bash
# Test RTC functionality
rtc_test -r              # Read RTC time
rtc_test -s "2024-01-01 12:00:00"  # Set RTC time
rtc_test -a 60           # Set alarm for 60 seconds
rtc_test -d              # Dump RTC registers

# Time validation
date -s "2024-01-01 12:00:00"  # Set system time
hwclock -w                 # Write to RTC
hwclock -r                 # Read from RTC
hwclock --hctosys         # Set system time from RTC
```

### Debug Features
```c
/* Debug levels */
#define RTC_DEBUG_NONE    0
#define RTC_DEBUG_ERROR   1
#define RTC_DEBUG_WARN    2
#define RTC_DEBUG_INFO    3
#define RTC_DEBUG_TRACE   4

/* Debug functions */
void rtc_debug_time(struct tm *tm);
void rtc_dump_registers(void);
void rtc_trace_operations(void);
void rtc_profile_performance(void);
```

## RISC-V Platform Support

### RISC-V RTC Challenges
- **No standard RTC**: Platform-specific implementations
- **Device tree dependency**: RTC discovery via FDT
- **MMIO access**: Memory-mapped register access
- **PLIC integration**: Timer interrupts through PLIC

### RISC-V RTC Implementation
```c
/* RISC-V RTC initialization */
int riscv_rtc_setup(struct riscv_rtc *rtc)
{
    struct fdt_node *node;
    int r;
    
    /* Find RTC node in device tree */
    node = fdt_find_node("/soc/rtc");
    if (!node) {
        return ENOENT;
    }
    
    /* Get register base address */
    r = fdt_get_reg(node, 0, &rtc->base, NULL);
    if (r != OK) {
        return r;
    }
    
    /* Get interrupt number */
    rtc->irq = fdt_get_interrupt(node, 0);
    
    /* Setup PLIC interrupt */
    plic_enable_interrupt(rtc->irq);
    plic_set_priority(rtc->irq, RTC_IRQ_PRIORITY);
    
    return OK;
}
```

### Platform-Specific Features
- **Multiple RTC sources**: Support for various RTC implementations
- **Clock source selection**: Choose best available RTC
- **Power management**: RISC-V power state handling
- **Timer coordination**: Coordinate with RISC-V timer

## Performance Optimization

### Access Optimization
- **Cached reads**: Cache RTC time values
- **Batch operations**: Combine multiple RTC accesses
- **Interrupt-driven updates**: Update on RTC interrupts
- **Lazy synchronization**: Defer time synchronization

### Power Optimization
- **Minimal access**: Reduce RTC register reads
- **Battery conservation**: Optimize for battery life
- **Efficient algorithms**: Fast time calculations
- **Power state management**: Handle low-power modes

## Future Enhancements

### Planned Features
- **High-precision RTC**: Microsecond precision
- **Network time sync**: NTP synchronization
- **Advanced alarms**: Multiple alarm support
- **Temperature compensation**: Drift correction

### Architecture Improvements
- **Unified RTC framework**: Common RTC infrastructure
- **Plugin architecture**: Modular RTC support
- **Advanced debugging**: Enhanced debugging features
- **Virtualization support**: RTC virtualization

## References

- **MC146818 RTC Datasheet**: Motorola
- **Dallas Semiconductor DS12887**: Maxim Integrated
- **ACPI Specification**: UEFI Forum
- **MINIX Character Driver**: `<minix/chardriver.h>`
- **Time Management**: `<time.h>`
- **RISC-V Platform Specification**: RISC-V International