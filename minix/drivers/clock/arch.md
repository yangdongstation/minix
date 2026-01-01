# MINIX Clock and Timer Drivers Architecture

## Overview

Clock and timer drivers in MINIX provide system timekeeping, timer services, and real-time clock functionality. These drivers are essential for process scheduling, timeout management, and maintaining accurate system time across different hardware platforms.

## Clock Architecture

### System Clock Hierarchy
```
Hardware Clock Sources
├── High-Resolution Timer (HPET, ARM Generic Timer)
├── Programmable Interval Timer (PIT, 8254)
├── Real-Time Clock (RTC, MC146818)
├── ACPI Power Management Timer
├── CPU Timestamp Counter (TSC)
└── Platform-Specific Timers (RISC-V Timer, OMAP Timer)

Software Clock Layers
├── Hardware Abstraction Layer
├── Clock Driver Interface
├── Timer Management
├── Timekeeping Services
└── User Interface (/dev/rtc, /dev/hpet)
```

### Clock Types
- **System Timer**: Regular interrupts for scheduling
- **High-Resolution Timer**: Precise timing for multimedia
- **Real-Time Clock**: Battery-backed timekeeping
- **CPU Timer**: Per-CPU timing facilities
- **Power Management Timer**: Low-power timing

## Directory Structure

```
minix/drivers/clock/
├── Makefile           # Build configuration
├── Makefile.inc       # Common make rules
├── readclock/        # Real-time clock driver
│   ├── arch/         # Architecture-specific RTC code
│   ├── readclock.c   # Main RTC driver
│   ├── forward.c     # Forward declarations
│   └── readclock.h   # RTC interface definitions
└── [platform-specific]/ # Platform timer drivers
```

## Real-Time Clock Driver (readclock)

### Purpose and Functionality
The readclock driver provides battery-backed timekeeping and system time initialization:
- **Time reading**: Get current time from RTC
- **Time setting**: Update RTC with new time
- **Alarm support**: Wake-on-alarm functionality
- **Power management**: Maintain time during system sleep
- **Time zone support**: Handle local time conversions

### Architecture Interface
```c
/* RTC hardware abstraction interface */
struct rtc {
    int (*init)(void);                    // Initialize RTC hardware
    int (*get_time)(struct tm *t, int flags); // Get current time
    int (*set_time)(struct tm *t, int flags); // Set current time
    int (*pwr_off)(void);                 // Prepare for power off
    void (*exit)(void);                   // Cleanup
};

/* Architecture setup function */
int arch_setup(struct rtc *r);
```

### Key Source Files

#### `readclock.c` - Main RTC Driver
```c
/* RTC driver state */
struct readclock_state {
    int initialized;                // Driver initialized
    struct rtc *hw_rtc;             // Hardware RTC interface
    endpoint_t owner;               // Current owner endpoint
    int open_count;                 // Number of open instances
};

Key functions:
- readclock_init()              // Initialize RTC driver
- readclock_open()              // Handle device open
- readclock_close()             // Handle device close
- readclock_read()              // Read time from RTC
- readclock_write()             // Write time to RTC
- readclock_ioctl()             // Handle RTC IOCTLs
- readclock_power_off()         // Prepare for system shutdown
```

#### `forward.h` - Forward Declarations
Contains function prototypes and data structure definitions.

#### `readclock.h` - RTC Interface
```c
/* RTC IOCTL commands */
#define RTC_GET_TIME    _IOR('r', 1, struct tm)
#define RTC_SET_TIME    _IOW('r', 2, struct tm)
#define RTC_GET_STATUS  _IOR('r', 3, int)
#define RTC_SET_ALARM   _IOW('r', 4, struct rtc_time)
#define RTC_GET_ALARM   _IOR('r', 5, struct rtc_time)

/* Time structure */
struct rtc_time {
    int tm_sec;                     // Seconds (0-59)
    int tm_min;                     // Minutes (0-59)
    int tm_hour;                    // Hours (0-23)
    int tm_mday;                    // Day of month (1-31)
    int tm_mon;                     // Month (0-11)
    int tm_year;                    // Year (since 1900)
    int tm_wday;                    // Day of week (0-6)
    int tm_yday;                    // Day of year (0-365)
    int tm_isdst;                   // Daylight saving time
};
```

### Architecture-Specific Implementations

#### x86/PC Architecture
Located in `arch/i386/`:
```c
/* Standard PC RTC (MC146818) */
#define RTC_ADDR    0x70        // RTC address register
#define RTC_DATA    0x71        // RTC data register

/* RTC registers */
#define RTC_SEC     0x00        // Seconds
#define RTC_MIN     0x02        // Minutes
#define RTC_HOUR    0x04        // Hours
#define RTC_DOW     0x06        // Day of week
#define RTC_DOM     0x07        // Day of month
#define RTC_MON     0x08        // Month
#define RTC_YEAR    0x09        // Year
#define RTC_STATUSA 0x0A        // Status register A
#define RTC_STATUSB 0x0B        // Status register B
#define RTC_STATUSC 0x0C        // Status register C

/* PC RTC implementation */
int pc_rtc_init(void);
int pc_rtc_get_time(struct tm *t, int flags);
int pc_rtc_set_time(struct tm *t, int flags);
int pc_rtc_power_off(void);
void pc_rtc_exit(void);
```

#### ARM Architecture
Located in `arch/earm/`:
```c
/* ARM platform RTC */
struct arm_rtc {
    vir_bytes base;                 // Register base address
    int irq;                        // Interrupt number
    int uses_bcd;                   // BCD format flag
};

/* ARM RTC functions */
int arm_rtc_init(struct arm_rtc *rtc);
int arm_rtc_get_time(struct arm_rtc *rtc, struct tm *t);
int arm_rtc_set_time(struct arm_rtc *rtc, struct tm *t);
```

#### RISC-V Architecture
Located in `arch/riscv64/`:
```c
/* RISC-V platform RTC */
struct riscv_rtc {
    vir_bytes base;                 // MMIO register base
    int irq;                        // PLIC interrupt
    u32_t clock_freq;               // Reference clock frequency
};

/* RISC-V RTC implementation */
int riscv_rtc_init(struct riscv_rtc *rtc);
int riscv_rtc_get_time(struct riscv_rtc *rtc, struct tm *t);
int riscv_rtc_set_time(struct riscv_rtc *rtc, struct tm *t);
```

## Timer Driver Architecture

### System Timer Requirements
- **Regular interrupts**: Typically 100-1000 Hz
- **High resolution**: Microsecond precision
- **CPU affinity**: Per-CPU timer support
- **Power efficiency**: Low-power modes
- **Synchronization**: Multi-CPU time sync

### Timer Types
```c
/* Timer types */
enum timer_type {
    TIMER_ONESHOT,                  // One-shot timer
    TIMER_PERIODIC,                 // Periodic timer
    TIMER_ABSOLUTE,                 // Absolute time timer
    TIMER_RELATIVE                  // Relative time timer
};

/* Timer structure */
struct timer {
    struct timer *next;             // Next timer in list
    clock_t expiry;                 // Expiration time
    int (*function)(struct timer *); // Timer function
    void *arg;                      // Argument
    int active;                     // Timer active
};
```

### Platform Timer Implementations

#### PIT (Programmable Interval Timer)
Classic x86 timer using Intel 8254:
```c
/* PIT configuration */
#define PIT_CHANNEL0    0x40        // Timer channel 0
#define PIT_CHANNEL1    0x41        // Timer channel 1
#define PIT_CHANNEL2    0x42        // Timer channel 2
#define PIT_COMMAND     0x43        // Command register

/* PIT timer implementation */
int pit_timer_init(int frequency);
void pit_timer_set_frequency(int frequency);
void pit_timer_handler(int irq);
clock_t pit_timer_read(void);
```

#### HPET (High Precision Event Timer)
Modern high-resolution timer:
```c
/* HPET configuration */
#define HPET_BASE       0xFED00000  // Default base address
#define HPET_CAPABILITIES 0x000      // Capabilities register
#define HPET_CONFIG     0x010        // Configuration register
#define HPET_COUNTER    0x0F0        // Main counter register

/* HPET timer implementation */
int hpet_timer_init(void);
int hpet_timer_setup_channel(int channel, int comparator);
void hpet_timer_handler(int irq);
u64_t hpet_timer_read(void);
```

#### RISC-V Timer
RISC-V platform timer:
```c
/* RISC-V timer registers */
#define RISCV_TIMER_MTIME    0x0200BFF8  // Machine time
#define RISCVTIMER_MTIMECMP  0x02004000  // Machine time compare

/* RISC-V timer implementation */
int riscv_timer_init(void);
void riscv_timer_set_compare(u64_t compare);
u64_t riscv_timer_read(void);
void riscv_timer_handler(int irq);
```

## Device Registration and Configuration

### System Service Configuration
Clock drivers are configured in `/etc/system.conf`:
```
service readclock
{
    system
    {
        DEVMAN_MAP     /* Device manager integration */
        IRQCTL         /* Interrupt control */
        IO_PORT_RANGE  /* RTC register access */
        { 0x70, 0x73 }
        TIME_SERVICE   /* Time service privileges */
    }
}

service timer
{
    system
    {
        DEVMAN_MAP     /* Device manager integration */
        IRQCTL         /* Timer interrupt control */
        IRQ_0          /* PIT timer interrupt */
        TIME_SERVICE   /* Timer service privileges */
    }
}
```

### Device Node Creation
```bash
# Create RTC device
MAKEDEV /dev/rtc

# Create timer devices
MAKEDEV /dev/hpet
MAKEDEV /dev/pit
```

### Service Initialization
```bash
# Start RTC service
minix-service up /service/readclock -dev /dev/rtc

# Start timer service
minix-service up /service/timer -label timer.0
```

## Time Management

### System Timekeeping
```c
/* System time structure */
struct system_time {
    time_t seconds;                 // Seconds since epoch
    suseconds_t microseconds;       // Microseconds
    int timezone;                   // Timezone offset
    int dst;                        // Daylight saving time
};

/* Time functions */
int clock_get_time(struct system_time *time);
int clock_set_time(struct system_time *time);
int clock_get_resolution(struct timespec *res);
int clock_get_monotonic(struct timespec *mono);
```

### Timer Management
```c
/* Timer operations */
int timer_set(struct timer *timer, clock_t expiry);
int timer_cancel(struct timer *timer);
int timer_pending(struct timer *timer);
void timer_process(clock_t now);

/* Timer list management */
void timer_add(struct timer *timer);
void timer_remove(struct timer *timer);
struct timer *timer_get_next(void);
```

## Power Management

### Clock Power States
```c
/* Clock power states */
enum clock_power_state {
    CLOCK_POWER_ON,                 // Full power
    CLOCK_POWER_STANDBY,            // Standby mode
    CLOCK_POWER_SUSPEND,            // Suspend mode
    CLOCK_POWER_OFF                 // Powered off
};

/* Power management functions */
int clock_set_power_state(enum clock_power_state state);
enum clock_power_state clock_get_power_state(void);
int clock_save_state(void);
int clock_restore_state(void);
```

### RTC Power Management
- **Battery backup**: Maintain time during power loss
- **Low-power mode**: Reduce RTC power consumption
- **Wake-on-alarm**: System wake from RTC alarm
- **Time compensation**: Account for power-off periods

## Error Handling

### Clock Error Types
```c
/* Clock error codes */
#define CLOCK_ERROR_INVALID_TIME    1   // Invalid time value
#define CLOCK_ERROR_HARDWARE_FAULT  2   // Hardware failure
#define CLOCK_ERROR_TIMEOUT         3   // Operation timeout
#define CLOCK_ERROR_NO_PERMISSION   4   // Permission denied
#define CLOCK_ERROR_NOT_SUPPORTED   5   // Operation not supported
```

### Error Recovery
```c
/* Error recovery functions */
int clock_recover_from_error(int error);
int clock_reset_hardware(void);
int clock_validate_time(struct system_time *time);
int clock_synchronize(void);
```

## Testing and Debugging

### Test Utilities
```bash
# Test RTC functionality
rtc_test -r    # Read RTC time
rtc_test -s    # Set RTC time
rtc_test -a    # Test RTC alarm

# Test system timer
timer_test -p  # Test periodic timer
timer_test -o  # Test one-shot timer
timer_test -h  # Test high-resolution timer

# Time synchronization tests
ntpdate -q pool.ntp.org  # Query NTP server
hwclock -r               # Read hardware clock
hwclock -w               # Write to hardware clock
```

### Debug Features
```c
/* Debug levels */
#define CLOCK_DEBUG_NONE    0
#define CLOCK_DEBUG_ERROR   1
#define CLOCK_DEBUG_WARN    2
#define CLOCK_DEBUG_INFO    3
#define CLOCK_DEBUG_TRACE   4

/* Debug functions */
void clock_debug_time(struct system_time *time);
void clock_dump_registers(void);
void clock_trace_interrupts(void);
void clock_profile_performance(void);
```

## RISC-V Platform Support

### RISC-V Timer Features
- **Machine timer**: Standard RISC-V timer
- **Supervisor timer**: S-mode timer support
- **User timer**: U-mode timer access
- **Timer interrupts**: Platform timer interrupts

### RISC-V Timer Implementation
```c
/* RISC-V timer initialization */
int riscv_timer_init(void)
{
    struct riscv_timer *timer = &riscv_timer;
    
    /* Get timer frequency from device tree */
    timer->frequency = riscv_dtb_get_timer_frequency();
    
    /* Setup timer interrupt */
    timer->irq = RISCV_IRQ_TIMER;
    irq_set_handler(timer->irq, riscv_timer_handler);
    
    /* Initialize timer */
    riscv_timer_set_compare(riscv_timer_read() + timer->frequency / HZ);
    
    return OK;
}

/* RISC-V timer interrupt handler */
void riscv_timer_handler(int irq)
{
    struct riscv_timer *timer = &riscv_timer;
    clock_t now;
    
    /* Acknowledge interrupt */
    riscv_clear_timer_interrupt();
    
    /* Get current time */
    now = riscv_timer_read();
    
    /* Process timers */
    timer_process(now);
    
    /* Schedule next interrupt */
    riscv_timer_set_compare(now + timer->frequency / HZ);
}
```

### Platform Timer Integration
RISC-V platforms provide various timer sources:
- **CLINT timer**: Core Local Interruptor timer
- **Platform timers**: Vendor-specific timers
- **ACPI timers**: Power management timers
- **HPET equivalent**: High-precision event timers

## Performance Optimization

### Timer Optimization
- **Tickless operation**: Reduce timer interrupts
- **Adaptive ticks**: Dynamic tick rate adjustment
- **Batch processing**: Process multiple timers together
- **CPU affinity**: Per-CPU timer management

### Timekeeping Optimization
- **TSC usage**: Use CPU timestamp counter
- **Clock source selection**: Choose best available clock
- **Interpolation**: Improve resolution between ticks
- **Synchronization**: Minimize clock drift

## Future Enhancements

### Planned Features
- **High-resolution timers**: Nanosecond precision
- **Dynamic tick rate**: Adaptive tick frequency
- **Timer consolidation**: Merge similar timers
- **Power-aware timing**: Energy-efficient timing

### Architecture Improvements
- **Unified timer framework**: Common timer infrastructure
- **Clock event abstraction**: Platform-independent events
- **Advanced power management**: Sophisticated power states
- **Virtualization support**: Timer virtualization

## References

- **Intel 8254 Programmable Interval Timer**: Intel Datasheet
- **High Precision Event Timer Specification**: Intel
- **RISC-V Timer Specification**: RISC-V International
- **ACPI Specification**: UEFI Forum
- **MINIX Timer Framework**: `<minix/clock.h>`
- **Character Driver Interface**: `<minix/chardriver.h>`