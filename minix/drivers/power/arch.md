# MINIX Power Management Drivers Architecture

## Overview

Power management drivers in MINIX provide comprehensive system power control, battery monitoring, and energy optimization capabilities. These drivers implement platform-specific power management features including CPU frequency scaling, device power states, system suspend/resume, and thermal management.

## Power Management Architecture

### Power Management Hierarchy
```
System Power Management
├── CPU Power Management
│   ├── Frequency scaling (DVFS)
│   ├── Idle state management (C-states)
│   ├── Turbo/boost modes
│   └── Thermal throttling
├── Device Power Management
│   ├── Device power states (D-states)
│   ├── Bus power management
│   ├── Peripheral power control
│   └── Wake-on-LAN/device
├── Platform Power Management
│   ├── System sleep states (S-states)
│   ├── Platform idle states
│   ├── Power domain control
│   └── Voltage regulation
└── Thermal Management
    ├── Temperature monitoring
    ├── Fan control
    ├── Thermal throttling
    └── Emergency shutdown
```

### Power States
```c
/* CPU power states (C-states) */
enum cpu_power_state {
    CPU_C0,                         // Operating
    CPU_C1,                         // Halt
    CPU_C2,                         // Stop-Clock
    CPU_C3,                         // Sleep
    CPU_C4,                         // Deep Sleep
    CPU_C5,                         // Deeper Sleep
    CPU_C6,                         // Deep Power Down
};

/* Device power states (D-states) */
enum device_power_state {
    DEVICE_D0,                      // Fully On
    DEVICE_D1,                      // Light Sleep
    DEVICE_D2,                      // Deep Sleep
    DEVICE_D3hot,                   // Software Off
    DEVICE_D3cold,                  // Hardware Off
};

/* System sleep states (S-states) */
enum system_sleep_state {
    SYSTEM_S0,                      // Working
    SYSTEM_S1,                      // Power On Suspend
    SYSTEM_S2,                      // CPU Off
    SYSTEM_S3,                      // Suspend to RAM
    SYSTEM_S4,                      // Suspend to Disk
    SYSTEM_S5,                      // Soft Off
};
```

## Directory Structure

```
minix/drivers/power/
├── Makefile           # Build configuration
├── Makefile.inc       # Common make rules
├── acpi/             # ACPI power management
│   ├── acpi.c        # ACPI core implementation
│   ├── dispatcher/   # ACPI dispatcher
│   ├── events/       # ACPI event handling
│   ├── executer/     # ACPI execution engine
│   ├── hardware/     # Hardware abstraction
│   ├── include/      # ACPI headers
│   ├── namespace/    # ACPI namespace
│   ├── parser/       # ACPI table parser
│   ├── resources/    # Resource management
│   ├── tables/       # ACPI table handling
│   └── utilities/    # ACPI utilities
├── tps65217/         # TI TPS65217 PMIC
├── tps65950/         # TI TPS65950 PMIC
└── [platform-specific]/ # Platform power drivers
```

## ACPI Power Management

### ACPI Overview
ACPI (Advanced Configuration and Power Interface) provides standardized power management:
- **System power states**: S0-S5 sleep states
- **Device power management**: D0-D3 device states
- **Processor power states**: C0-C6 CPU states
- **Thermal management**: Temperature-based throttling
- **Battery management**: Battery status and charging
- **Wake events**: System wake from sleep states

### ACPI Implementation Structure
```
ACPI Subsystem
├── Table Parser
│   ├── RSDP (Root System Description Pointer)
   ├── XSDT (Extended System Description Table)
│   ├── FADT (Fixed ACPI Description Table)
│   ├── DSDT (Differentiated System Description Table)
│   └── SSDT (Secondary System Description Table)
├── Namespace Manager
│   ├── Object tree traversal
│   ├── Method execution
│   ├── Control method parsing
│   └── Object evaluation
├── Event Handler
│   ├── Fixed events (power button, sleep button)
│   ├── General purpose events (GPEs)
│   ├── Embedded controller events
│   └── Wake events
├── Power Resource Manager
│   ├── Power resource objects
│   ├── Reference counting
│   ├── State transitions
│   └── Dependency management
└── Thermal Manager
    ├── Thermal zone management
    ├── Cooling device control
    ├── Passive cooling
    └── Active cooling
```

### Key ACPI Source Files

#### `acpi.c` - ACPI Core Implementation
```c
/* ACPI global state */
struct acpi_state {
    int initialized;                // ACPI initialized
    struct acpi_table_header *rsdp; // RSDP table
    struct acpi_fadt *fadt;         // FADT table
    struct acpi_namespace *root;    // Namespace root
    
    /* Power management */
    struct acpi_sleep_state sleep_states[8]; // Sleep states
    struct acpi_power_resource *power_resources; // Power resources
    
    /* Thermal management */
    struct acpi_thermal_zone *thermal_zones; // Thermal zones
    
    /* Event handling */
    struct acpi_event_handler *event_handlers; // Event handlers
    int gpe_count;                  // Number of GPEs
};

Key functions:
- acpi_init()                     // Initialize ACPI subsystem
- acpi_parse_tables()             // Parse ACPI tables
- acpi_evaluate_object()          // Evaluate ACPI objects
- acpi_enter_sleep_state()        // Enter sleep state
- acpi_handle_power_button()      // Handle power button
- acpi_thermal_zone_check()       // Check thermal zones
```

#### `dispatcher/dispatcher.c` - ACPI Method Dispatcher
```c
/* ACPI execution context */
struct acpi_execute_state {
    struct acpi_namespace *scope;   // Current scope
    struct acpi_operand *operands;  // Operand stack
    int operand_count;              // Operand count
    struct acpi_walk_state *walk_state; // Walk state
};

/* Method execution functions */
int acpi_execute_method(struct acpi_namespace *method,
                       struct acpi_operand *args,
                       struct acpi_operand **result);
int acpi_walk_namespace(struct acpi_namespace *root,
                       struct acpi_walk_state *state);
```

#### `events/events.c` - ACPI Event Handling
```c
/* ACPI event types */
#define ACPI_EVENT_FIXED_POWER_BUTTON   0
#define ACPI_EVENT_FIXED_SLEEP_BUTTON   1
#define ACPI_EVENT_FIXED_WAKE           2
#define ACPI_EVENT_GPE_BASE             16

/* Event handling functions */
int acpi_enable_event(int event);
int acpi_disable_event(int event);
int acpi_clear_event(int event);
int acpi_handle_event(int event);
```

### Power Resource Management
```c
/* ACPI power resource */
struct acpi_power_resource {
    char name[ACPI_NAME_SIZE];      // Resource name
    int system_level;               // System power level
    int order;                      // Power sequence order
    int reference_count;            // Reference count
    int state;                      // Current state
    
    /* Resource methods */
    struct acpi_namespace *on_method;   // _ON method
    struct acpi_namespace *off_method;  // _OFF method
    struct acpi_namespace *sta_method;  // _STA method
};

/* Power resource functions */
int acpi_power_resource_on(struct acpi_power_resource *resource);
int acpi_power_resource_off(struct acpi_power_resource *resource);
int acpi_power_resource_get_state(struct acpi_power_resource *resource);
```

## Platform-Specific PMIC Drivers

### TI TPS65217 Power Management IC
Located in `tps65217/`:
```c
/* TPS65217 PMIC structure */
struct tps65217 {
    int i2c_bus;                    // I2C bus number
    int i2c_addr;                   // I2C address (0x24)
    int irq;                        // Interrupt line
    
    /* Power rails */
    struct tps65217_rail {
        int enabled;                // Rail enabled
        int voltage;                // Current voltage (mV)
        int min_voltage;            // Minimum voltage (mV)
        int max_voltage;            // Maximum voltage (mV)
    } rails[TPS65217_NUM_RAILS];
    
    /* Status and monitoring */
    int temperature;                // Die temperature
    int power_good;                 // Power good status
    int fault_status;               // Fault status
};

/* TPS65217 registers */
#define TPS65217_STATUS         0x00
#define TPS65217_ENABLE1        0x01
#define TPS65217_ENABLE2        0x02
#define TPS65217_ENABLE3        0x03
#define TPS65217_VCORE_CTRL     0x05
#define TPS65217_DCDC1_CTRL     0x06
#define TPS65217_DCDC2_CTRL     0x07
#define TPS65217_DCDC3_CTRL     0x08
#define TPS65217_SLEW_CTRL      0x09
#define TPS65217_LDO_CTRL       0x0A
#define TPS65217_SEQ1           0x0D
#define TPS65217_SEQ2           0x0E
#define TPS65217_SEQ3           0x0F
#define TPS65217_SEQ4           0x10
#define TPS65217_SEQ5           0x11
#define TPS65217_SEQ6           0x12
#define TPS65217_DEFPG          0x13
#define TPS65217_DEFSLEW        0x14
#define TPS65217_PGOOD          0x15
#define TPS65217_FAULT          0x16
#define TPS65217_TS             0x17
```

#### TPS65217 Key Functions
```c
/* TPS65217 initialization */
int tps65217_init(struct tps65217 *pmic);
int tps65217_probe(int bus);

/* Power rail control */
int tps65217_rail_enable(struct tps65217 *pmic, int rail);
int tps65217_rail_disable(struct tps65217 *pmic, int rail);
int tps65217_rail_set_voltage(struct tps65217 *pmic, int rail, int voltage);
int tps65217_rail_get_voltage(struct tps65217 *pmic, int rail);

/* Power sequencing */
int tps65217_set_power_sequence(struct tps65217 *pmic, int sequence);
int tps65217_get_power_sequence(struct tps65217 *pmic);

/* Monitoring and status */
int tps65217_get_temperature(struct tps65217 *pmic);
int tps65217_get_power_good(struct tps65217 *pmic);
int tps65217_get_fault_status(struct tps65217 *pmic);
```

### TI TPS65950 Power Management
Located in `tps65950/`:
```c
/* TPS65950 PMIC structure */
struct tps65950 {
    int i2c_bus;                    // I2C bus number
    int i2c_addr;                   // I2C address (0x48)
    int irq;                        // Interrupt line
    
    /* Power resources */
    struct tps65950_resource {
        int type;                   // Resource type
        int state;                  // Current state
        int reference_count;        // Reference count
        int constraints;            // Power constraints
    } resources[TPS65950_NUM_RESOURCES];
    
    /* SmartReflex */
    struct tps65950_smartreflex {
        int enabled;                // SmartReflex enabled
        int voltage;                // Current voltage
        int target_voltage;         // Target voltage
    } smartreflex;
};
```

## CPU Power Management

### Dynamic Voltage and Frequency Scaling (DVFS)
```c
/* DVFS governor types */
enum dvfs_governor {
    DVFS_GOVERNOR_PERFORMANCE,      // Maximum performance
    DVFS_GOVERNOR_POWERSAVE,        // Maximum power saving
    DVFS_GOVERNOR_USERSPACE,        // User-controlled
    DVFS_GOVERNOR_ONDEMAND,         // On-demand scaling
    DVFS_GOVERNOR_CONSERVATIVE,     // Conservative scaling
};

/* DVFS policy */
struct dvfs_policy {
    enum dvfs_governor governor;    // DVFS governor
    int min_freq;                   // Minimum frequency
    int max_freq;                   // Maximum frequency
    int target_freq;                // Target frequency
    int sampling_rate;              // Sampling rate (ms)
};

/* DVFS operations */
int dvfs_set_frequency(int cpu, int frequency);
int dvfs_get_frequency(int cpu);
int dvfs_set_governor(int cpu, enum dvfs_governor governor);
int dvfs_get_available_frequencies(int cpu, int *frequencies, int count);
```

### CPU Idle Management
```c
/* CPU idle states */
struct cpuidle_state {
    char name[CPUIDLE_NAME_LEN];    // State name
    int exit_latency;               // Exit latency (us)
    int power_usage;                // Power usage (mW)
    int target_residency;           // Target residency (us)
    int disabled;                   // State disabled
    
    /* State statistics */
    u64_t usage;                    // Usage count
    u64_t time;                     // Total time in state
    u64_t rejected;                 // Rejected count
};

/* Idle management functions */
int cpuidle_register_state(int cpu, struct cpuidle_state *state);
int cpuidle_select_state(int cpu, int predicted_idle_us);
int cpuidle_enter_state(int cpu, int state);
int cpuidle_reflect_state(int cpu, int state);
```

## Thermal Management

### Thermal Zone Management
```c
/* Thermal zone */
struct thermal_zone {
    char name[THERMAL_NAME_LEN];    // Zone name
    int temperature;                // Current temperature (milliCelsius)
    int trip_points[NUM_TRIP_POINTS]; // Trip point temperatures
    enum thermal_trip_type trip_types[NUM_TRIP_POINTS];
    
    /* Cooling devices */
    struct thermal_cooling_device *cooling_devices[MAX_COOLING_DEVICES];
    int cooling_device_count;
    
    /* Thermal governor */
    struct thermal_governor *governor;
    
    /* Notification */
    void (*notify)(struct thermal_zone *tz, int trip);
};

/* Trip point types */
enum thermal_trip_type {
    THERMAL_TRIP_ACTIVE,            // Active cooling
    THERMAL_TRIP_PASSIVE,           // Passive cooling
    THERMAL_TRIP_HOT,               // Hot trip point
    THERMAL_TRIP_CRITICAL,          // Critical trip point
};
```

### Cooling Device Management
```c
/* Cooling device */
struct thermal_cooling_device {
    char name[THERMAL_NAME_LEN];    // Device name
    int max_state;                  // Maximum cooling state
    int cur_state;                  // Current cooling state
    
    /* Cooling operations */
    int (*get_max_state)(struct thermal_cooling_device *cdev, int *state);
    int (*get_cur_state)(struct thermal_cooling_device *cdev, int *state);
    int (*set_cur_state)(struct thermal_cooling_device *cdev, int state);
};

/* Cooling device types */
#define THERMAL_COOLING_DEVICE_FAN      1
#define THERMAL_COOLING_DEVICE_CPU_FREQ 2
#define THERMAL_COOLING_DEVICE_DEV_POWER 3
```

## Battery Management

### Battery Information
```c
/* Battery information */
struct battery_info {
    char name[BATTERY_NAME_LEN];    // Battery name
    int present;                    // Battery present
    int technology;                 // Battery technology
    int voltage_now;                // Current voltage (uV)
    int voltage_min;                // Minimum voltage (uV)
    int voltage_max;                // Maximum voltage (uV)
    int current_now;                // Current current (uA)
    int charge_now;                 // Current charge (uAh)
    int charge_full;                // Full charge (uAh)
    int charge_full_design;         // Design full charge (uAh)
    int energy_now;                 // Current energy (uWh)
    int energy_full;                // Full energy (uWh)
    int energy_full_design;         // Design full energy (uWh)
    int capacity;                   // Capacity percentage
    int capacity_level;             // Capacity level
    int temperature;                // Temperature (decidegrees C)
    int time_to_empty;              // Time to empty (minutes)
    int time_to_full;               // Time to full (minutes)
    int status;                     // Charging status
};

/* Battery status */
#define BATTERY_STATUS_UNKNOWN      0
#define BATTERY_STATUS_CHARGING     1
#define BATTERY_STATUS_DISCHARGING  2
#define BATTERY_STATUS_NOT_CHARGING 3
#define BATTERY_STATUS_FULL         4
```

### Battery Monitoring
```c
/* Battery monitoring functions */
int battery_get_info(struct battery_info *info);
int battery_get_status(void);
int battery_get_capacity(void);
int battery_get_voltage(void);
int battery_get_current(void);
int battery_get_temperature(void);

/* Battery event notification */
int battery_register_notifier(void (*notify)(struct battery_info *));
int battery_unregister_notifier(void (*notify)(struct battery_info *));
```

## System Integration

### Device Registration and Configuration
Power management drivers are configured in `/etc/system.conf`:
```
service acpi
{
    system
    {
        DEVMAN_MAP     /* Device manager integration */
        MEMORY_MAPPING /* ACPI table access */
        IRQCTL         /* Interrupt control */
        PRIVCTL        /* System privilege control */
    }
}

service tps65217
{
    system
    {
        DEVMAN_MAP     /* Device manager integration */
        I2C_BUS        /* I2C access */
        IRQCTL         /* Interrupt control */
    }
}
```

### Power Management Interface
```c
/* Power management IOCTLs */
#define POWER_GET_STATE     _IOR('p', 1, int)
#define POWER_SET_STATE     _IOW('p', 2, int)
#define POWER_GET_BATTERY   _IOR('p', 3, struct battery_info)
#define POWER_GET_THERMAL   _IOR('p', 4, struct thermal_info)
#define POWER_SET_GOVERNOR  _IOW('p', 5, int)
#define POWER_GET_GOVERNOR  _IOR('p', 6, int)
```

### System Power Events
```c
/* Power event types */
#define POWER_EVENT_BATTERY_LOW     1
#define POWER_EVENT_BATTERY_CRITICAL 2
#define POWER_EVENT_THERMAL_CRITICAL 3
#define POWER_EVENT_POWER_BUTTON    4
#define POWER_EVENT_SLEEP_BUTTON    5
#define POWER_EVENT_WAKE            6
#define POWER_EVENT_AC_CONNECTED    7
#define POWER_EVENT_AC_DISCONNECTED 8

/* Event handling */
int power_register_event_handler(int (*handler)(int event, void *data));
int power_unregister_event_handler(int (*handler)(int event, void *data));
int power_notify_event(int event, void *data);
```

## RISC-V Platform Support

### RISC-V Power Management Features
- **No ACPI**: Platform-specific power management
- **SBI calls**: Power management via SBI
- **Platform-specific**: Vendor-dependent implementation
- **Device tree**: Power state descriptions

### RISC-V SBI Power Management
```c
/* RISC-V SBI power management calls */
#define SBI_EXT_ID_TIME             0x54494D45
#define SBI_EXT_ID_IPI              0x735049
#define SBI_EXT_ID_RFENCE           0x52464E43
#define SBI_EXT_ID_HSM              0x48534D

/* Hart state management */
#define SBI_HSM_HART_START          0
#define SBI_HSM_HART_STOP           1
#define SBI_HSM_HART_GET_STATUS     2
#define SBI_HSM_HART_SUSPEND        3

/* RISC-V power management functions */
int riscv_sbi_hart_suspend(uint32_t suspend_type, uintptr_t resume_addr);
int riscv_sbi_hart_start(uint32_t hart_id, uintptr_t start_addr);
int riscv_sbi_hart_stop(void);
int riscv_sbi_hart_get_status(uint32_t hart_id);
```

### Platform-Specific Implementation
```c
/* RISC-V platform power management */
struct riscv_power_mgmt {
    int sbi_extension;              // SBI extension available
    int hart_count;                 // Number of harts
    int sleep_states;               // Available sleep states
    
    /* Platform callbacks */
    int (*suspend)(int state);
    int (*resume)(int state);
    int (*power_off)(void);
    int (*reboot)(void);
};

/* RISC-V power functions */
int riscv_power_init(struct riscv_power_mgmt *pm);
int riscv_power_suspend(int state);
int riscv_power_resume(int state);
int riscv_power_off(void);
```

## Testing and Debugging

### Test Utilities
```bash
# Power management testing
power_test -s                  // Test sleep states
power_test -f                  // Test frequency scaling
power_test -t                  // Test thermal management
power_test -b                  // Test battery monitoring

# Thermal testing
thermal_test -z                // List thermal zones
thermal_test -s 0 80           // Set trip point
thermal_test -m                // Monitor temperatures

# Battery testing
battery_test -i                // Show battery info
battery_test -s                // Show battery status
battery_test -e                // Test battery events
```

### Debug Features
```c
/* Debug levels */
#define POWER_DEBUG_NONE    0
#define POWER_DEBUG_ERROR   1
#define POWER_DEBUG_WARN    2
#define POWER_DEBUG_INFO    3
#define POWER_DEBUG_TRACE   4

/* Debug functions */
void power_debug_state(struct power_state *state);
void power_dump_registers(void);
void power_trace_transitions(void);
void power_profile_consumption(void);
```

## Performance Optimization

### Power Optimization
- **Aggressive power saving**: Minimize power consumption
- **Dynamic scaling**: Adjust frequency/voltage dynamically
- **Idle optimization**: Optimize idle state selection
- **Thermal-aware**: Balance performance and temperature

### Efficiency Improvements
- **Predictive algorithms**: Predict usage patterns
- **Machine learning**: Learn optimal settings
- **Hardware acceleration**: Use power management hardware
- **Coordinated control**: Coordinate multiple domains

## Future Enhancements

### Planned Features
- **Advanced thermal management**: Machine learning thermal control
- **Predictive power management**: AI-based power prediction
- **Energy harvesting**: Support for energy harvesting devices
- **Carbon footprint**: Track and optimize carbon footprint

### Architecture Improvements
- **Unified power framework**: Common power management infrastructure
- **Plugin architecture**: Modular power management
- **Advanced debugging**: Enhanced debugging and profiling
- **Virtualization support**: Power management virtualization

## References

- **Advanced Configuration and Power Interface Specification**: UEFI Forum
- **Intel Power Management**: Intel Developer Manuals
- **ARM Power Management**: ARM Technical Reference Manuals
- **RISC-V SBI Specification**: RISC-V International
- **MINIX System Services**: `<minix/system.h>`
- **I2C Interface**: `<minix/i2c.h>`
- **Thermal Framework**: Linux Thermal Framework Documentation