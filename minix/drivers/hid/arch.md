# MINIX Human Interface Device Drivers Architecture

## Overview

Human Interface Device (HID) drivers in MINIX provide support for user input devices including keyboards, mice, touchpads, game controllers, and other interactive peripherals. These drivers implement the USB HID protocol and legacy PS/2 interfaces, translating low-level hardware events into standardized input events for the system.

## HID Architecture

### HID Device Categories
```
HID Device Types
├── Keyboards
│   ├── Standard 104-key keyboards
│   ├── Multimedia keyboards
│   ├── Virtual keyboards
│   └── Special function keys
├── Pointing Devices
│   ├── Standard mice
│   ├── Trackballs
│   ├── Touchpads
│   ├── Graphics tablets
│   └── Touch screens
├── Game Controllers
│   ├── Gamepads
│   ├── Joysticks
│   ├── Steering wheels
│   └── Flight sticks
└── Miscellaneous
    ├── Barcode scanners
    ├── RFID readers
    ├── Remote controls
    └── Custom HID devices
```

### Protocol Layers
```
Application Layer
├── TTY subsystem (keyboards)
├── Input event interface (mice)
├── Game controller API
└── Raw HID access

HID Protocol Layer
├── HID report parsing
├── Report descriptor processing
├── Feature report handling
└── HID report generation

Transport Layer
├── USB HID protocol
├── PS/2 keyboard protocol
├── PS/2 mouse protocol
├── I2C HID
└── Bluetooth HID

Hardware Layer
├── USB endpoints
├── PS/2 ports
├── I2C interfaces
└── GPIO pins
```

## Directory Structure

```
minix/drivers/hid/
├── Makefile           # Build configuration
├── Makefile.inc       # Common make rules
├── pckbd/            # PC AT keyboard driver
│   ├── pckbd.c       # Main keyboard driver
│   ├── pckbd.h       # Keyboard definitions
│   ├── table.c       # Keycode translation tables
│   └── Makefile
└── usbhid/           # USB HID driver framework
    ├── usbhid.c      # USB HID core
    ├── keyboard/     # USB keyboard support
    ├── mouse/        # USB mouse support
    └── gamepad/      # USB gamepad support
```

## PC AT Keyboard Driver (pckbd)

### Hardware Interface
The PC AT keyboard uses a bidirectional serial protocol over PS/2 interface:
- **Data port**: 0x60 (keyboard data)
- **Command/status port**: 0x64 (keyboard controller)
- **Interrupt**: IRQ1 (keyboard interrupt)
- **Protocol**: 11-bit serial frame (start, data, parity, stop, ack)

### Key Scan Codes
```c
/* Keyboard scan code sets */
#define SCAN_CODE_SET1      1       // PC/XT scan codes
#define SCAN_CODE_SET2      2       // AT scan codes (default)
#define SCAN_CODE_SET3      3       // PS/2 scan codes

/* Special scan codes */
#define SCAN_BREAK_BIT      0x80    // Break code flag
#define SCAN_EXT_PREFIX     0xE0    // Extended key prefix
#define SCAN_EXT_PREFIX2    0xE1    // Extended key prefix 2

/* Key states */
#define KEY_PRESSED         0       // Key pressed
#define KEY_RELEASED        1       // Key released
#define KEY_EXTENDED        2       // Extended key
```

### Source Code Organization

#### `pckbd.c` - Main Keyboard Driver
```c
/* Keyboard driver state */
struct pckbd_state {
    int initialized;                // Driver initialized
    int irq_hook;                   // IRQ hook ID
    endpoint_t owner;               // Current owner
    int open_count;                 // Number of open instances
    int scan_code_set;              // Current scan code set
    int leds;                       // Current LED state
    
    /* Key state tracking */
    int extended;                   // Extended key sequence
    int release;                    // Key release
    int pause;                      // Pause key sequence
    
    /* Input buffer */
    u8_t buffer[KBD_IN_BUFSZ];      // Input buffer
    int buf_count;                  // Buffer count
    int buf_head;                   // Buffer head
    int buf_tail;                   // Buffer tail
};

Key functions:
- pckbd_init()                  // Initialize keyboard driver
- pckbd_open()                  // Handle device open
- pckbd_close()                 // Handle device close
- pckbd_read()                  // Read keyboard data
- pckbd_ioctl()                 // Handle IOCTL commands
- pckbd_interrupt()             // Handle keyboard interrupts
- pckbd_process_scancode()      // Process scan codes
```

#### `pckbd.h` - Keyboard Definitions
```c
/* Keyboard controller commands */
#define KBC_RD_RAM_CCB  0x20        // Read controller command byte
#define KBC_WR_RAM_CCB  0x60        // Write controller command byte
#define KBC_SELF_TEST   0xAA        // Self test
#define KBC_KBD_TEST    0xAB        // Keyboard test
#define KBC_KBD_DISABLE 0xAD        // Disable keyboard
#define KBC_KBD_ENABLE  0xAE        // Enable keyboard

/* Keyboard commands */
#define KBD_SET_LEDS    0xED        // Set LEDs
#define KBD_ECHO        0xEE        // Echo command
#define KBD_SET_SCANSET 0xF0        // Set scan code set
#define KBD_GET_ID      0xF2        // Get keyboard ID
#define KBD_SET_RATE    0xF3        // Set typematic rate
#define KBD_ENABLE      0xF4        // Enable keyboard
#define KBD_RESET       0xFF        // Reset keyboard

/* LED bits */
#define LED_SCROLL_LOCK 0x01        // Scroll lock LED
#define LED_NUM_LOCK    0x02        // Num lock LED
#define LED_CAPS_LOCK   0x04        // Caps lock LED
```

#### `table.c` - Keycode Translation
```c
/* Scan code to keycode mapping */
const struct scanmap scanmap_normal[KBD_SCAN_CODES] = {
    /* 0x00 */ { 0, 0 },           // Reserved
    /* 0x01 */ { 0, '1' },         // 1
    /* 0x02 */ { 0, '2' },         // 2
    /* 0x03 */ { 0, '3' },         // 3
    /* 0x04 */ { 0, '4' },         // 4
    /* 0x05 */ { 0, '5' },         // 5
    /* 0x06 */ { 0, '6' },         // 6
    /* 0x07 */ { 0, '7' },         // 7
    /* 0x08 */ { 0, '8' },         // 8
    /* 0x09 */ { 0, '9' },         // 9
    /* 0x0A */ { 0, '0' },         // 0
    /* 0x0B */ { 0, '-' },         // -
    /* 0x0C */ { 0, '=' },         // =
    /* 0x0D */ { 0, '\b' },        // Backspace
    /* 0x0E */ { 0, '\t' },        // Tab
    /* 0x0F */ { 0, 'q' },         // Q
    /* Extended key mappings */
    /* 0xE0, 0x1C */ { 1, '\n' },  // Enter (keypad)
    /* 0xE0, 0x1D */ { 1, KEY_CTRL }, // Right Ctrl
    /* 0xE0, 0x38 */ { 1, KEY_ALT },  // Right Alt (AltGr)
};
```

### Keyboard Protocol Implementation

#### Scan Code Processing
```c
/* Scan code processing */
static void pckbd_process_scancode(u8_t scancode)
{
    struct pckbd_state *state = &pckbd_state;
    int keycode;
    
    /* Handle extended key sequences */
    if (scancode == SCAN_EXT_PREFIX) {
        state->extended = 1;
        return;
    }
    
    /* Handle pause key special sequence */
    if (state->pause) {
        pckbd_process_pause_sequence(scancode);
        return;
    }
    
    /* Handle break codes */
    if (scancode & SCAN_BREAK_BIT) {
        scancode &= ~SCAN_BREAK_BIT;
        state->release = 1;
    }
    
    /* Convert scan code to keycode */
    keycode = pckbd_scancode_to_keycode(scancode, state->extended);
    
    /* Add to input buffer */
    if (keycode != 0) {
        pckbd_add_to_buffer(keycode | (state->release ? SCAN_RELEASE : 0));
    }
    
    /* Reset state */
    state->extended = 0;
    state->release = 0;
}
```

#### Keyboard Controller Communication
```c
/* Keyboard controller communication */
static int kbc_write_command(u8_t command)
{
    int timeout;
    
    /* Wait for input buffer to be empty */
    for (timeout = KBC_WAIT_TIME; timeout > 0; timeout--) {
        if (!(inb(KB_STATUS) & KB_IN_FULL)) {
            outb(KB_COMMAND, command);
            return OK;
        }
        micro_delay(KBC_IN_DELAY);
    }
    
    return ETIME;
}

static int kbd_write_data(u8_t data)
{
    int timeout;
    
    /* Wait for input buffer to be empty */
    for (timeout = KBC_WAIT_TIME; timeout > 0; timeout--) {
        if (!(inb(KB_STATUS) & KB_IN_FULL)) {
            outb(KEYBD, data);
            return OK;
        }
        micro_delay(KBC_IN_DELAY);
    }
    
    return ETIME;
}

static int kbd_read_data(u8_t *data)
{
    int timeout;
    
    /* Wait for output buffer to be full */
    for (timeout = KBC_READ_TIME; timeout > 0; timeout--) {
        if (inb(KB_STATUS) & KB_OUT_FULL) {
            *data = inb(KEYBD);
            return OK;
        }
        micro_delay(KBC_IN_DELAY);
    }
    
    return ETIME;
}
```

## USB HID Driver Framework

### USB HID Architecture
```c
/* USB HID device structure */
struct usbhid_device {
    struct usb_device *udev;        // USB device
    int interface;                  // Interface number
    
    /* HID descriptors */
    struct hid_descriptor *hdesc;   // HID descriptor
    struct hid_report_descriptor *rdesc; // Report descriptor
    
    /* Endpoints */
    struct usb_endpoint *int_in;    // Interrupt IN endpoint
    struct usb_endpoint *int_out;   // Interrupt OUT endpoint
    
    /* HID reports */
    u8_t *in_report;                // Input report buffer
    u8_t *out_report;               // Output report buffer
    u8_t *feature_report;           // Feature report buffer
    
    /* Report parsing */
    struct hid_parser *parser;      // Report parser
    struct hid_usage *usages;       // Usage mappings
    int num_usages;                 // Number of usages
    
    /* Device-specific data */
    void *device_data;              // Device-specific information
};
```

### HID Report Processing
```c
/* HID report types */
#define HID_REPORT_TYPE_INPUT    1
#define HID_REPORT_TYPE_OUTPUT   2
#define HID_REPORT_TYPE_FEATURE  3

/* HID report processing */
int usbhid_process_report(struct usbhid_device *hid, u8_t *report, int len)
{
    struct hid_parser *parser = hid->parser;
    struct hid_usage *usage;
    int i, value;
    
    /* Parse report based on device type */
    switch (hid->type) {
    case HID_TYPE_KEYBOARD:
        return usbhid_process_keyboard_report(hid, report, len);
    case HID_TYPE_MOUSE:
        return usbhid_process_mouse_report(hid, report, len);
    case HID_TYPE_GAMEPAD:
        return usbhid_process_gamepad_report(hid, report, len);
    default:
        return usbhid_process_generic_report(hid, report, len);
    }
}
```

### HID Report Descriptor Parsing
```c
/* HID report descriptor parser */
struct hid_parser {
    u8_t *report_desc;              // Report descriptor
    int report_size;                // Report descriptor size
    
    /* Collection stack */
    struct hid_collection *collections;
    int collection_size;
    int collection_depth;
    
    /* Usage information */
    struct hid_usage *usages;
    int usage_count;
    int usage_size;
    
    /* Report information */
    int report_id;                  // Current report ID
    int report_size;                // Report size in bits
    int report_count;               // Number of reports
};

/* HID item types */
#define HID_ITEM_TYPE_MAIN      0
#define HID_ITEM_TYPE_GLOBAL    1
#define HID_ITEM_TYPE_LOCAL     2
#define HID_ITEM_TYPE_RESERVED  3

/* HID main items */
#define HID_MAIN_ITEM_INPUT     0x80
#define HID_MAIN_ITEM_OUTPUT    0x90
#define HID_MAIN_ITEM_FEATURE   0xB0
#define HID_MAIN_ITEM_COLLECTION 0xA0
#define HID_MAIN_ITEM_END_COLLECTION 0xC0
```

## Input Event Processing

### Event Types
```c
/* Input event types */
#define EV_SYN          0x00        // Synchronization event
#define EV_KEY          0x01        // Key event
#define EV_REL          0x02        // Relative axis event
#define EV_ABS          0x03        // Absolute axis event
#define EV_MSC          0x04        // Miscellaneous event
#define EV_SW           0x05        // Switch event
#define EV_LED          0x11        // LED event
#define EV_SND          0x12        // Sound event
#define EV_REP          0x14        // Repeat event
#define EV_FF           0x15        // Force feedback event
#define EV_PWR          0x16        // Power event
#define EV_FF_STATUS    0x17        // Force feedback status
```

### Key Event Processing
```c
/* Key event structure */
struct input_event {
    struct timeval time;            // Timestamp
    u16_t type;                     // Event type
    u16_t code;                     // Event code
    s32_t value;                    // Event value
};

/* Key codes (Linux input event codes) */
#define KEY_RESERVED    0
#define KEY_ESC         1
#define KEY_1           2
#define KEY_2           3
#define KEY_A           30
#define KEY_ENTER       28
#define KEY_SPACE       57
#define KEY_LEFTCTRL    29
#define KEY_RIGHTCTRL   97
#define KEY_LEFTSHIFT   42
#define KEY_RIGHTSHIFT  54
```

### Mouse Event Processing
```c
/* Mouse button codes */
#define BTN_LEFT        0x110
#define BTN_RIGHT       0x111
#define BTN_MIDDLE      0x112
#define BTN_SIDE        0x113
#define BTN_EXTRA       0x114

/* Mouse movement reporting */
struct mouse_event {
    s32_t x_rel;                    // Relative X movement
    s32_t y_rel;                    // Relative Y movement
    s32_t wheel_rel;                // Wheel movement
    u32_t buttons;                  // Button state bitmap
};
```

## Device Registration and Configuration

### System Service Configuration
HID drivers are configured in `/etc/system.conf`:
```
service pckbd
{
    system
    {
        DEVMAN_MAP     /* Device manager integration */
        IRQCTL         /* Interrupt control */
        IRQ_1          /* Keyboard interrupt */
        IO_PORT_RANGE  /* Keyboard ports */
        { 0x60, 0x64 }
    }
}

service usbhid
{
    system
    {
        DEVMAN_MAP     /* Device manager integration */
        USB_HID        /* USB HID access */
        MEMORY_MAPPING /* Memory mapping for descriptors */
    }
}
```

### Device Node Creation
```bash
# Create HID devices
MAKEDEV /dev/kbd                 # Keyboard device
MAKEDEV /dev/mouse               # Mouse device
MAKEDEV /dev/input/event0        # Input event device
MAKEDEV /dev/input/event1        # Input event device
```

### Service Initialization
```bash
# Start keyboard service
minix-service up /service/pckbd -dev /dev/kbd

# Start USB HID service
minix-service up /service/usbhid
```

## Power Management

### HID Power States
```c
/* HID power management */
enum hid_power_state {
    HID_POWER_D0,                   // Full power
    HID_POWER_D1,                   // Light sleep
    HID_POWER_D2,                   // Deep sleep
    HID_POWER_D3,                   // Device off
};

/* Power management functions */
int hid_set_power_state(struct hid_device *hid, enum hid_power_state state);
enum hid_power_state hid_get_power_state(struct hid_device *hid);
int hid_suspend(struct hid_device *hid);
int hid_resume(struct hid_device *hid);
```

### Wake-on-HID
```c
/* Wake-on-HID support */
int hid_enable_wake(struct hid_device *hid);
int hid_disable_wake(struct hid_device *hid);
int hid_set_wake_key(struct hid_device *hid, int keycode);
```

## Error Handling

### HID Error Types
```c
/* HID error codes */
#define HID_ERROR_NO_DEVICE     1   // Device not found
#define HID_ERROR_NO_REPORT     2   // Report not found
#define HID_ERROR_INVALID_DESC  3   // Invalid descriptor
#define HID_ERROR_TIMEOUT       4   // Transfer timeout
#define HID_ERROR_STALL         5   // Endpoint stalled
#define HID_ERROR unplugged     6   // Device unplugged
```

### Error Recovery
```c
/* Error recovery functions */
int hid_recover_from_error(struct hid_device *hid, int error);
int hid_reset_device(struct hid_device *hid);
int hid_restore_state(struct hid_device *hid);
```

## Testing and Debugging

### Test Utilities
```bash
# Test keyboard functionality
kbd_test -r                    # Read keyboard events
kbd_test -l                    // Test LED control
kbd_test -s                    // Show scan codes

# Test mouse functionality
mouse_test -r                  // Read mouse events
mouse_test -a                  // Test absolute mode
mouse_test -p                  // Test pressure sensitivity

# Test input events
evtest /dev/input/event0       // Test input device
evtest --grab /dev/input/event0 // Grab device exclusively
```

### Debug Features
```c
/* Debug levels */
#define HID_DEBUG_NONE    0
#define HID_DEBUG_ERROR   1
#define HID_DEBUG_WARN    2
#define HID_DEBUG_INFO    3
#define HID_DEBUG_TRACE   4

/* Debug functions */
void hid_debug_report(struct hid_device *hid, u8_t *report, int len);
void hid_dump_descriptor(struct hid_device *hid);
void hid_trace_events(struct hid_device *hid);
void hid_profile_performance(struct hid_device *hid);
```

## RISC-V Platform Support

### RISC-V HID Challenges
- **No legacy PS/2**: No built-in keyboard controller
- **Platform-specific GPIO**: Custom GPIO implementations
- **USB dependency**: Primary HID through USB
- **Device tree integration**: HID discovery via FDT

### RISC-V GPIO-based HID
```c
/* RISC-V GPIO keyboard matrix */
struct riscv_gpio_kbd {
    int *row_pins;                  // Row GPIO pins
    int *col_pins;                  // Column GPIO pins
    int rows;                       // Number of rows
    int cols;                       // Number of columns
    int irq;                        // GPIO interrupt
};

/* RISC-V GPIO keyboard functions */
int riscv_gpio_kbd_init(struct riscv_gpio_kbd *kbd);
int riscv_gpio_kbd_scan(struct riscv_gpio_kbd *kbd);
void riscv_gpio_kbd_handler(int irq);
```

### Platform Integration
- **GPIO matrix scanning**: Row/column keyboard scanning
- **Interrupt-driven input**: GPIO interrupt handling
- **Debouncing**: Hardware/software debouncing
- **Pull-up configuration**: GPIO pull-up setup

## Performance Optimization

### Input Optimization
- **Interrupt coalescing**: Reduce interrupt frequency
- **Buffer management**: Efficient input buffering
- **Polling optimization**: Optimize polling intervals
- **DMA support**: Use DMA for large transfers

### Event Processing
- **Event batching**: Process multiple events together
- **Priority handling**: Prioritize important events
- **Filtering**: Filter unwanted events
- **Compression**: Compress similar events

## Future Enhancements

### Planned Features
- **Multi-touch support**: Advanced touch interfaces
- **Gesture recognition**: Complex gesture support
- **Force feedback**: Haptic feedback support
- **Biometric integration**: Fingerprint and other biometrics

### Architecture Improvements
- **Unified input framework**: Common input infrastructure
- **Plugin architecture**: Modular device support
- **Advanced debugging**: Enhanced debugging capabilities
- **Virtualization support**: Input device virtualization

## References

- **USB HID Specification**: USB Implementers Forum
- **PS/2 Keyboard and Mouse Interface**: IBM Technical Reference
- **Linux Input Subsystem**: Linux Kernel Documentation
- **MINIX Character Driver**: `<minix/chardriver.h>`
- **USB Framework**: `<minix/usb.h>`
- **RISC-V GPIO Specification**: RISC-V International