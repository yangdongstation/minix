# MINIX Hardware Sensor Drivers Architecture

## Overview

Hardware sensor drivers in MINIX provide monitoring and measurement capabilities for various environmental and system parameters. These drivers interface with temperature sensors, pressure sensors, light sensors, motion detectors, and other monitoring devices commonly found in embedded systems and IoT applications.

## Sensor Architecture

### Sensor Categories
```
Environmental Sensors
├── Temperature Sensors
│   ├── Digital temperature sensors (TMP102, DS18B20)
│   ├── Thermistors and RTDs
│   ├── Infrared temperature sensors
│   └── Thermal imaging sensors
├── Pressure Sensors
│   ├── Barometric pressure (BMP085, BMP180)
│   ├── Differential pressure
│   ├── Absolute pressure
│   └── Vacuum pressure
├── Humidity Sensors
│   ├── Relative humidity (SHT21, DHT22)
   ├── Absolute humidity
│   └── Dew point calculation
├── Light Sensors
│   ├── Ambient light (TSL2550, BH1750)
│   ├── UV index sensors
│   ├── Color sensors
│   └── Proximity sensors
└── Air Quality Sensors
    ├── Gas sensors (CO2, CO, VOC)
    ├── Particulate matter (PM2.5, PM10)
    └── Air quality index calculation

Motion and Position Sensors
├── Accelerometers
│   ├── 3-axis accelerometers (ADXL345)
│   ├── Tilt sensors
│   └── Vibration sensors
├── Gyroscopes
│   ├── 3-axis gyroscopes (L3G4200D)
│   ├── Angular rate sensors
│   └── Orientation sensors
├── Magnetometers
│   ├── 3-axis magnetometers (HMC5883L)
│   ├── Digital compasses
│   └── Magnetic field sensors
└── Inertial Measurement Units
    ├── 6-DOF IMU (accelerometer + gyroscope)
    ├── 9-DOF IMU (accelerometer + gyroscope + magnetometer)
    └── Sensor fusion algorithms

System Monitoring Sensors
├── Voltage Sensors
│   ├── ADC voltage measurement
│   ├── Battery voltage monitoring
│   └── Power supply monitoring
├── Current Sensors
│   ├── Hall effect sensors
│   ├── Shunt resistors
│   └── Current transformers
└── Power Sensors
    ├── Power consumption measurement
    ├── Energy monitoring
    └── Power factor calculation
```

### Sensor Interface Types
```
Digital Interfaces
├── I2C Sensors
│   ├── Standard I2C (100 kHz)
│   ├── Fast I2C (400 kHz)
│   ├── Fast+ I2C (1 MHz)
│   └── High-speed I2C (3.4 MHz)
├── SPI Sensors
│   ├── Standard SPI
│   ├── Dual SPI
│   ├── Quad SPI
│   └── 3-wire SPI
├── UART Sensors
│   ├── Serial communication
│   ├── Modbus protocol
│   └── Custom protocols
└── 1-Wire Sensors
    ├── Dallas 1-Wire protocol
    ├── Parasitic power mode
    └── Multiple device support

Analog Interfaces
├── ADC-based Sensors
│   ├── Voltage measurement
│   ├── Current measurement
│   └── Resistance measurement
├── PWM Sensors
│   ├── Duty cycle measurement
│   ├── Frequency measurement
│   └── Pulse counting
└── GPIO-based Sensors
    ├── Digital input/output
    ├── Pulse width measurement
    └── Event counting
```

## Directory Structure

```
minix/drivers/sensors/
├── Makefile           # Build configuration
├── Makefile.inc       # Common make rules
├── bmp085/           # BMP085 pressure sensor
│   ├── bmp085.c      # Main sensor driver
│   ├── bmp085.h      # Sensor definitions
│   ├── README.txt    # Driver documentation
│   └── Makefile
├── sht21/            # SHT21 humidity sensor
│   ├── sht21.c       # Main sensor driver
│   ├── sht21.h       # Sensor definitions
│   └── Makefile
├── tsl2550/          # TSL2550 light sensor
│   ├── tsl2550.c     # Main sensor driver
│   ├── tsl2550.h     # Sensor definitions
│   └── Makefile
└── [platform-specific]/ # Platform sensor drivers
```

## BMP085 Pressure Sensor Driver

### BMP085 Overview
The BMP085 is a high-precision barometric pressure sensor with:
- **Pressure range**: 300-1100 hPa (9000m to -500m altitude)
- **Temperature measurement**: -40 to +85°C
- **I2C interface**: Up to 3.4 MHz
- **Low power**: 5 μA at 1 sample/sec
- **High resolution**: 0.03 hPa pressure, 0.1°C temperature

### Hardware Interface
```c
/* BMP085 I2C address */
#define BMP085_I2C_ADDR         0x77

/* BMP085 registers */
#define BMP085_REG_AC1_MSB      0xAA
#define BMP085_REG_AC1_LSB      0xAB
#define BMP085_REG_AC2_MSB      0xAC
#define BMP085_REG_AC2_LSB      0xAD
#define BMP085_REG_AC3_MSB      0xAE
#define BMP085_REG_AC3_LSB      0xAF
#define BMP085_REG_AC4_MSB      0xB0
#define BMP085_REG_AC4_LSB      0xB1
#define BMP085_REG_AC5_MSB      0xB2
#define BMP085_REG_AC5_LSB      0xB3
#define BMP085_REG_AC6_MSB      0xB4
#define BMP085_REG_AC6_LSB      0xB5
#define BMP085_REG_B1_MSB       0xB6
#define BMP085_REG_B1_LSB       0xB7
#define BMP085_REG_B2_MSB       0xB8
#define BMP085_REG_B2_LSB       0xB9
#define BMP085_REG_MB_MSB       0xBA
#define BMP085_REG_MB_LSB       0xBB
#define BMP085_REG_MC_MSB       0xBC
#define BMP085_REG_MC_LSB       0xBD
#define BMP085_REG_MD_MSB       0xBE
#define BMP085_REG_MD_LSB       0xBF
#define BMP085_REG_CONTROL      0xF4
#define BMP085_REG_DATA_MSB     0xF6
#define BMP085_REG_DATA_LSB     0xF7
#define BMP085_REG_DATA_XLSB    0xF8
```

### BMP085 Driver Implementation

#### `bmp085.c` - Main Driver
```c
/* BMP085 device structure */
struct bmp085 {
    int i2c_bus;                    // I2C bus number
    int i2c_addr;                   // I2C address
    int mode;                       // Measurement mode
    
    /* Calibration data */
    s16_t ac1, ac2, ac3;            // Temperature calibration
    u16_t ac4, ac5, ac6;            // Pressure calibration
    s16_t b1, b2;                   // Pressure calibration
    s16_t mb, mc, md;               // Temperature calibration
    
    /* Measurement data */
    s32_t temperature;              // Last temperature (0.1°C)
    s32_t pressure;                 // Last pressure (Pa)
    s32_t altitude;                 // Calculated altitude (m)
    
    /* State */
    int initialized;                // Driver initialized
    mutex_t lock;                   // Device mutex
};

Key functions:
- bmp085_init()                   // Initialize BMP085
- bmp085_read_calibration()       // Read calibration data
- bmp085_read_temperature()       // Read temperature
- bmp085_read_pressure()          // Read pressure
- bmp085_calculate_altitude()     // Calculate altitude
- bmp085_get_measurement()        // Get sensor measurement
```

#### Calibration Data Reading
```c
/* Read BMP085 calibration data */
static int bmp085_read_calibration(struct bmp085 *sensor)
{
    u8_t data[22];
    int r;
    
    /* Read all calibration data at once */
    r = i2c_read_bytes(sensor->i2c_bus, sensor->i2c_addr,
                      BMP085_REG_AC1_MSB, data, sizeof(data));
    if (r != OK) return r;
    
    /* Parse calibration coefficients */
    sensor->ac1 = (data[0] << 8) | data[1];
    sensor->ac2 = (data[2] << 8) | data[3];
    sensor->ac3 = (data[4] << 8) | data[5];
    sensor->ac4 = (data[6] << 8) | data[7];
    sensor->ac5 = (data[8] << 8) | data[9];
    sensor->ac6 = (data[10] << 8) | data[11];
    sensor->b1 = (data[12] << 8) | data[13];
    sensor->b2 = (data[14] << 8) | data[15];
    sensor->mb = (data[16] << 8) | data[17];
    sensor->mc = (data[18] << 8) | data[19];
    sensor->md = (data[20] << 8) | data[21];
    
    return OK;
}
```

#### Temperature and Pressure Calculation
```c
/* Calculate compensated temperature */
static s32_t bmp085_calc_temperature(struct bmp085 *sensor, s32_t ut)
{
    s32_t x1, x2;
    
    x1 = ((ut - sensor->ac6) * sensor->ac5) >> 15;
    x2 = (sensor->mc << 11) / (x1 + sensor->md);
    sensor->b5 = x1 + x2;
    
    return (sensor->b5 + 8) >> 4;   // Temperature in 0.1°C
}

/* Calculate compensated pressure */
static s32_t bmp085_calc_pressure(struct bmp085 *sensor, s32_t up)
{
    s32_t x1, x2, x3, b3, b6, p;
    u32_t b4, b7;
    
    b6 = sensor->b5 - 4000;
    x1 = (sensor->b2 * ((b6 * b6) >> 12)) >> 11;
    x2 = (sensor->ac2 * b6) >> 11;
    x3 = x1 + x2;
    b3 = (((sensor->ac1 * 4 + x3) << sensor->mode) + 2) >> 2;
    
    x1 = (sensor->ac3 * b6) >> 13;
    x2 = (sensor->b1 * ((b6 * b6) >> 12)) >> 16;
    x3 = ((x1 + x2) + 2) >> 2;
    b4 = (sensor->ac4 * (u32_t)(x3 + 32768)) >> 15;
    b7 = ((u32_t)(up - b3) * (50000 >> sensor->mode));
    
    if (b7 < 0x80000000)
        p = (b7 << 1) / b4;
    else
        p = (b7 / b4) << 1;
    
    x1 = (p >> 8) * (p >> 8);
    x1 = (x1 * 3038) >> 16;
    x2 = (-7357 * p) >> 16;
    p += (x1 + x2 + 3791) >> 4;
    
    return p;                       // Pressure in Pa
}
```

#### Altitude Calculation
```c
/* Calculate altitude from pressure */
static s32_t bmp085_calc_altitude(s32_t pressure)
{
    /* Sea level pressure: 101325 Pa */
    /* Barometric formula for altitude */
    s64_t altitude;
    
    altitude = 44330 * (100000 - (s64_t)pressure);
    altitude = altitude / (100000 - (altitude * 100) / 82999);
    
    return (s32_t)altitude;
}
```

## SHT21 Humidity Sensor Driver

### SHT21 Overview
The SHT21 is a digital humidity and temperature sensor with:
- **Humidity range**: 0-100% RH
- **Temperature range**: -40 to +125°C
- **I2C interface**: Up to 400 kHz
- **Low power**: 3 μW at 1 measurement/second
- **High accuracy**: ±2% RH humidity, ±0.3°C temperature

### SHT21 Implementation
```c
/* SHT21 device structure */
struct sht21 {
    int i2c_bus;                    // I2C bus number
    int i2c_addr;                   // I2C address (0x40)
    
    /* Measurement data */
    u16_t humidity_raw;             // Raw humidity data
    u16_t temperature_raw;          // Raw temperature data
    s32_t humidity;                 // Compensated humidity (%RH)
    s32_t temperature;              // Compensated temperature (°C)
    
    /* State */
    int initialized;                // Driver initialized
    mutex_t lock;                   // Device mutex
};

/* SHT21 commands */
#define SHT21_CMD_TEMP_HOLD         0xE3
#define SHT21_CMD_HUMIDITY_HOLD     0xE5
#define SHT21_CMD_TEMP_NOHOLD       0xF3
#define SHT21_CMD_HUMIDITY_NOHOLD   0xF5
#define SHT21_CMD_WRITE_USER_REG    0xE6
#define SHT21_CMD_READ_USER_REG     0xE7
#define SHT21_CMD_SOFT_RESET        0xFE
```

## TSL2550 Light Sensor Driver

### TSL2550 Overview
The TSL2550 is a digital ambient light sensor with:
- **Light range**: 0.1 to 40,000 lux
- **Dual photodiodes**: Visible and infrared light
- **I2C interface**: Up to 400 kHz
- **Low power**: 500 μA active, 3 μA standby
- **High resolution**: 16-bit ADC

### TSL2550 Implementation
```c
/* TSL2550 device structure */
struct tsl2550 {
    int i2c_bus;                    // I2C bus number
    int i2c_addr;                   // I2C address (0x39)
    
    /* Measurement data */
    u16_t ch0_data;                 // Channel 0 (visible + IR)
    u16_t ch1_data;                 // Channel 1 (IR only)
    u32_t lux;                      // Calculated lux
    
    /* Configuration */
    int gain;                       // Amplifier gain
    int integration_time;           // Integration time
    
    /* State */
    int initialized;                // Driver initialized
    mutex_t lock;                   // Device mutex
};

/* TSL2550 commands */
#define TSL2550_CMD_POWER_DOWN      0x00
#define TSL2550_CMD_POWER_UP        0x03
#define TSL2550_CMD_MEASURE         0x04
#define TSL2550_CMD_READ_ADC        0x05
#define TSL2550_CMD_READ_CH0        0x06
#define TSL2550_CMD_READ_CH1        0x07
#define TSL2550_CMD_WRITE_CONFIG    0x08
#define TSL2550_CMD_READ_CONFIG     0x09
```

## Generic Sensor Framework

### Sensor Interface
```c
/* Generic sensor interface */
struct sensor {
    char name[SENSOR_NAME_LEN];     // Sensor name
    int type;                       // Sensor type
    int units;                      // Measurement units
    
    /* Sensor operations */
    int (*init)(struct sensor *sensor);
    int (*read_raw)(struct sensor *sensor, s32_t *value);
    int (*read_compensated)(struct sensor *sensor, s32_t *value);
    int (*calibrate)(struct sensor *sensor, s32_t reference);
    
    /* Sensor state */
    int enabled;                    // Sensor enabled
    int calibrated;                 // Sensor calibrated
    s32_t offset;                   // Calibration offset
    s32_t scale;                    // Scaling factor
    
    /* Device-specific data */
    void *device_data;              // Device-specific information
};

/* Sensor types */
#define SENSOR_TYPE_TEMPERATURE     1
#define SENSOR_TYPE_PRESSURE        2
#define SENSOR_TYPE_HUMIDITY        3
#define SENSOR_TYPE_LIGHT           4
#define SENSOR_TYPE_ACCELERATION    5
#define SENSOR_TYPE_GYROSCOPE       6
#define SENSOR_TYPE_MAGNETIC_FIELD  7
#define SENSOR_TYPE_VOLTAGE         8
#define SENSOR_TYPE_CURRENT         9
#define SENSOR_TYPE_POWER           10
```

### Sensor Registration
```c
/* Sensor registration functions */
int sensor_register(struct sensor *sensor);
int sensor_unregister(struct sensor *sensor);
int sensor_enable(struct sensor *sensor);
int sensor_disable(struct sensor *sensor);

/* Sensor access functions */
int sensor_read(struct sensor *sensor, s32_t *value);
int sensor_read_multiple(struct sensor *sensors[], s32_t values[], int count);
int sensor_get_info(struct sensor *sensor, struct sensor_info *info);
```

## Device Interface

### Character Device Access
Sensors provide character device interfaces:
```bash
# Sensor device nodes
/dev/bmp085b3s77    # BMP085 on bus 3, address 0x77
/dev/sht21b1s40     # SHT21 on bus 1, address 0x40
/dev/tsl2550b2s39   # TSL2550 on bus 2, address 0x39
```

### Sensor Data Format
```c
/* Sensor data format */
#define SENSOR_FORMAT_RAW       1       // Raw sensor data
#define SENSOR_FORMAT_COMPENSATED 2     // Compensated data
#define SENSOR_FORMAT_PHYSICAL  3       // Physical units
#define SENSOR_FORMAT_NORMALIZED 4      // Normalized 0-100%

/* Sensor output format */
struct sensor_data {
    s32_t value;                    // Sensor value
    int format;                     // Data format
    int units;                      // Physical units
    struct timeval timestamp;       // Measurement timestamp
    int status;                     // Measurement status
};
```

### IOCTL Commands
```c
/* Sensor IOCTL commands */
#define SENSOR_GET_INFO         _IOR('s', 1, struct sensor_info)
#define SENSOR_GET_CONFIG       _IOR('s', 2, struct sensor_config)
#define SENSOR_SET_CONFIG       _IOW('s', 3, struct sensor_config)
#define SENSOR_CALIBRATE        _IOW('s', 4, s32_t)
#define SENSOR_GET_RAW          _IOR('s', 5, s32_t)
#define SENSOR_GET_COMPENSATED  _IOR('s', 6, s32_t)
#define SENSOR_START_MEASUREMENT _IO('s', 7)
#define SENSOR_STOP_MEASUREMENT  _IO('s', 8)
```

## Calibration and Compensation

### Sensor Calibration
```c
/* Calibration types */
#define CALIBRATION_OFFSET      1       // Offset calibration
#define CALIBRATION_GAIN        2       // Gain calibration
#define CALIBRATION_LINEAR      3       // Linear calibration
#define CALIBRATION_NONLINEAR   4       // Non-linear calibration

/* Calibration data */
struct sensor_calibration {
    int type;                       // Calibration type
    s32_t offset;                   // Offset correction
    s32_t gain;                     // Gain correction
    s32_t reference;                // Reference value
    s32_t temperature;              // Calibration temperature
    struct timeval timestamp;       // Calibration timestamp
};

/* Calibration functions */
int sensor_apply_calibration(struct sensor *sensor,
                            struct sensor_calibration *cal);
int sensor_save_calibration(struct sensor *sensor,
                           struct sensor_calibration *cal);
int sensor_load_calibration(struct sensor *sensor,
                           struct sensor_calibration *cal);
```

### Temperature Compensation
```c
/* Temperature compensation */
struct temp_compensation {
    s32_t temp_coeff[3];            // Temperature coefficients
    s32_t reference_temp;           // Reference temperature
    int enabled;                    // Compensation enabled
};

/* Temperature compensation functions */
int sensor_compensate_temperature(struct sensor *sensor,
                                 s32_t raw_value, s32_t temperature);
int sensor_update_temp_coeff(struct sensor *sensor,
                            s32_t coeffs[], int count);
```

## Power Management

### Sensor Power States
```c
/* Sensor power states */
enum sensor_power_state {
    SENSOR_POWER_OFF,               // Powered off
    SENSOR_POWER_STANDBY,           // Low power standby
    SENSOR_POWER_NORMAL,            // Normal operation
    SENSOR_POWER_HIGH,              // High performance mode
};

/* Power management functions */
int sensor_set_power_state(struct sensor *sensor,
                          enum sensor_power_state state);
enum sensor_power_state sensor_get_power_state(struct sensor *sensor);
int sensor_get_power_consumption(struct sensor *sensor, u32_t *power_uw);
```

### Measurement Modes
```c
/* Measurement modes */
#define SENSOR_MODE_SINGLE      1       // Single measurement
#define SENSOR_MODE_CONTINUOUS  2       // Continuous measurement
#define SENSOR_MODE_TRIGGERED   3       // Triggered measurement
#define SENSOR_MODE_STREAMING   4       // Streaming data

/* Measurement configuration */
struct sensor_measurement {
    int mode;                       // Measurement mode
    int rate;                       // Measurement rate (Hz)
    int samples;                    // Number of samples
    int averaging;                  // Averaging count
    int resolution;                 // Measurement resolution
};
```

## RISC-V Platform Support

### RISC-V Sensor Integration
- **Device tree support**: Sensor discovery via FDT
- **Platform-specific sensors**: Vendor-dependent implementations
- **GPIO-based sensors**: Bit-banged sensor protocols
- **Interrupt-driven**: GPIO interrupt handling

### Device Tree Configuration
```dts
bmp085: bmp085@77 {
    compatible = "bosch,bmp085";
    reg = <0x77>;
    
    /* Calibration data */
    bosch,ac1 = <0x1234>;
    bosch,ac2 = <0x5678>;
    bosch,ac3 = <0x9ABC>;
    bosch,ac4 = <0xDEF0>;
    bosch,ac5 = <0x1357>;
    bosch,ac6 = <0x2468>;
    bosch,b1 = <0xABCD>;
    bosch,b2 = <0xEF01>;
    bosch,mb = <0x2345>;
    bosch,mc = <0x6789>;
    bosch,md = <0xABCD>;
    
    /* Measurement configuration */
    measurement-mode = <3>;         // High resolution
    temperature-offset = <0>;       // No offset
    pressure-offset = <0>;          // No offset
};

sht21: sht21@40 {
    compatible = "sensirion,sht21";
    reg = <0x40>;
    
    /* Measurement parameters */
    measurement-mode = <0x03>;      // Hold master mode
    resolution = <0x00>;            // 12-bit RH, 14-bit T
    heater-enabled = <0>;           // Heater disabled
};
```

### RISC-V GPIO Sensors
```c
/* RISC-V GPIO sensor interface */
struct riscv_gpio_sensor {
    int gpio_pin;                   // GPIO pin number
    int gpio_flags;                 // GPIO configuration
    int irq;                        // GPIO interrupt
    
    /* Sensor-specific configuration */
    int pulse_width_min;            // Minimum pulse width
    int pulse_width_max;            // Maximum pulse width
    int timeout;                    // Measurement timeout
};

/* RISC-V GPIO sensor functions */
int riscv_gpio_sensor_init(struct riscv_gpio_sensor *sensor);
int riscv_gpio_sensor_read(struct riscv_gpio_sensor *sensor, u32_t *value);
void riscv_gpio_sensor_handler(int irq);
```

## Error Handling

### Sensor Error Types
```c
/* Sensor error codes */
#define SENSOR_ERROR_NO_DEVICE      1   // Device not found
#define SENSOR_ERROR_TIMEOUT        2   // Measurement timeout
#define SENSOR_ERROR_INVALID_DATA   3   // Invalid sensor data
#define SENSOR_ERROR_CALIBRATION    4   // Calibration error
#define SENSOR_ERROR_HARDWARE       5   // Hardware failure
#define SENSOR_ERROR_COMMUNICATION  6   // Communication error
#define SENSOR_ERROR_OUT_OF_RANGE   7   // Value out of range
```

### Error Recovery
```c
/* Error recovery functions */
int sensor_recover_from_error(struct sensor *sensor, int error);
int sensor_reset_hardware(struct sensor *sensor);
int sensor_validate_data(struct sensor *sensor, s32_t data);
int sensor_check_health(struct sensor *sensor);
```

### Fault Detection
```c
/* Fault detection functions */
int sensor_detect_stuck_at_fault(struct sensor *sensor);
int sensor_detect_drift_fault(struct sensor *sensor);
int sensor_detect_communication_fault(struct sensor *sensor);
int sensor_detect_calibration_fault(struct sensor *sensor);
```

## Testing and Debugging

### Test Utilities
```bash
# Sensor testing
sensor_test -d /dev/bmp085b3s77  // Test BMP085 sensor
sensor_test -d /dev/sht21b1s40   // Test SHT21 sensor
sensor_test -d /dev/tsl2550b2s39 // Test TSL2550 sensor

# Sensor calibration
sensor_calibrate -d /dev/bmp085b3s77 -r 101325  // Calibrate pressure
sensor_calibrate -d /dev/sht21b1s40 -r 50.0     // Calibrate humidity

# Sensor monitoring
sensor_monitor -a                    // Monitor all sensors
sensor_monitor -d /dev/bmp085b3s77   // Monitor specific sensor
sensor_monitor -l                    // List available sensors
```

### Debug Features
```c
/* Debug levels */
#define SENSOR_DEBUG_NONE    0
#define SENSOR_DEBUG_ERROR   1
#define SENSOR_DEBUG_WARN    2
#define SENSOR_DEBUG_INFO    3
#define SENSOR_DEBUG_TRACE   4

/* Debug functions */
void sensor_debug_data(struct sensor *sensor, s32_t data);
void sensor_dump_registers(struct sensor *sensor);
void sensor_trace_measurements(struct sensor *sensor);
void sensor_profile_performance(struct sensor *sensor);
```

## Performance Optimization

### Measurement Optimization
- **Burst measurements**: Multiple measurements in sequence
- **Averaging**: Reduce noise through averaging
- **Filtering**: Digital filtering of sensor data
- **Interpolation**: Improve resolution through interpolation

### Communication Optimization
- **Batch reads**: Read multiple registers at once
- **Cached values**: Cache frequently accessed data
- **Interrupt-driven**: Use interrupts instead of polling
- **DMA support**: Use DMA for large data transfers

## Future Enhancements

### Planned Features
- **Sensor fusion**: Combine multiple sensors
- **Machine learning**: AI-based sensor calibration
- **Predictive maintenance**: Predict sensor failures
- **Edge computing**: Process data locally

### Architecture Improvements
- **Unified sensor framework**: Common sensor infrastructure
- **Plugin architecture**: Modular sensor support
- **Advanced calibration**: Sophisticated calibration algorithms
- **Cloud integration**: Sensor data cloud upload

## References

- **BMP085 Datasheet**: Bosch Sensortec
- **SHT21 Datasheet**: Sensirion
- **TSL2550 Datasheet**: TAOS/AMS
- **I2C Sensor Interface**: NXP I2C Specification
- **MINIX I2C Framework**: `<minix/i2c.h>`
- **Character Device Interface**: `<minix/chardriver.h>`
- **Device Tree Specification**: Devicetree.org