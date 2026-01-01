# MINIX Audio Drivers Architecture

## Overview

The MINIX audio driver framework provides a unified interface for audio hardware support across different architectures. Audio drivers in MINIX are implemented as userspace processes that communicate with the kernel and other system components through message passing, following the microkernel philosophy.

## Architecture Design

### Message-Based Communication
Audio drivers use MINIX's synchronous message passing for all communication:
- **Request messages**: From applications to audio drivers
- **Response messages**: From drivers back to applications
- **Notification messages**: For asynchronous events (buffer underrun, device removal)

### Driver Categories
The audio framework supports several audio device categories:
- **ISA bus devices**: Sound Blaster 16, ESS AudioDrive
- **PCI bus devices**: Ensoniq AudioPCI (ES1370/ES1371), C-Media CMI8738
- **USB audio devices**: Supported through USB stack integration
- **Platform-specific**: SoC-integrated audio controllers

## Directory Structure

```
minix/drivers/audio/
├── Makefile           # Build configuration for all audio drivers
├── Makefile.inc       # Common make rules
├── README            # Overview of available drivers
├── sb16/             # Sound Blaster 16 ISA driver
├── es1370/           # Ensoniq AudioPCI ES1370 driver
├── es1371/           # Ensoniq AudioPCI ES1371 driver
├── als4000/          # Avance Logic ALS4000 driver
├── cmi8738/          # C-Media CMI8738 driver
├── cs4281/           # Cirrus Logic CS4281 driver
└── maestro3/         # ESS Maestro3 driver
```

## Key Components

### Audio Framework Interface
All audio drivers implement the MINIX audio framework interface defined in `<minix/audio_fw.h>`:

```c
struct audio_driver {
    const char *name;
    int (*open)(devminor_t minor, int access, endpoint_t user_endpt);
    int (*close)(devminor_t minor);
    int (*ioctl)(devminor_t minor, unsigned long request, endpoint_t endpt,
                 cp_grant_id_t grant, int flags, endpoint_t user_endpt);
    void (*other)(message *m, int ipc_status);
};
```

### Device Registration
Audio drivers register with the device manager through:
1. **Service declaration** in `/etc/system.conf`
2. **Device node creation** via `MAKEDEV`
3. **Driver initialization** with `minix-service`

### Mixer Support
Most audio drivers provide separate mixer devices for volume control:
- `/dev/audio` - Main audio device for playback/recording
- `/dev/mixer` - Mixer device for volume and settings control

## Sound Blaster 16 Driver (sb16)

### Hardware Support
The SB16 driver supports Creative Sound Blaster 16 and compatible ISA sound cards:
- **I/O Ports**: Configurable base address (0x220 default)
- **IRQ**: Configurable interrupt line (7 default)
- **DMA**: 8-bit and 16-bit DMA channels
- **Features**: 8/16-bit audio playback, mixer control

### Key Source Files

#### `sb16.c` - Main Driver Implementation
```c
Key functions:
- sb16_init()          // Initialize hardware and driver state
- sb16_open()          // Handle device open requests
- sb16_close()         // Handle device close requests
- sb16_ioctl()         // Process IOCTL commands
- sb16_interrupt()     // Handle hardware interrupts
- sb16_playback()      // Audio playback implementation
```

#### `mixer.c` - Audio Mixer Implementation
```c
Key functions:
- mixer_init()         // Initialize mixer hardware
- mixer_get()          // Get mixer settings
- mixer_set()          // Set mixer parameters
- mixer_ioctl()        // Process mixer IOCTLs
```

#### `sb16.h` - Driver Definitions
Contains hardware register definitions, DMA configuration, and driver constants.

### Interrupt Handling
The SB16 driver uses interrupt-driven I/O for efficient audio playback:
1. **DMA setup**: Configure DMA controller for audio data transfer
2. **Interrupt service**: Handle buffer completion interrupts
3. **Buffer management**: Maintain circular buffer for continuous playback
4. **Error recovery**: Handle underrun and hardware errors

### Configuration
Hardware parameters are defined in `sb16.h`:
```c
#define SB_IRQ          7       // Interrupt line
#define SB_BASE_ADDR    0x220   // I/O base address
#define SB_DMA_8        1       // 8-bit DMA channel
#define SB_DMA_16       5       // 16-bit DMA channel
```

## Audio Driver Integration

### System Service Configuration
Audio drivers are configured in `/etc/system.conf`:
```
service sb16_dsp
{
    system
    {
        DEVMAN_MAP     /* Device manager mapping */
        IRQCTL         /* Interrupt control */
        IRQ_7          /* Specific IRQ access */
        IO_PORT_RANGE  /* I/O port access */
        { 0x220, 0x230 }
        DMA_1          /* DMA channel access */
        DMA_5
    }
}
```

### Device Manager Integration
Audio drivers integrate with the MINIX device manager:
1. **Device registration**: Register audio and mixer devices
2. **Access control**: Manage device access permissions
3. **Hot-plug support**: Handle device insertion/removal (for USB audio)
4. **Power management**: Suspend/resume functionality

### Message Flow
```
Application → VFS → Audio Driver → Hardware
    ↓           ↓         ↓           ↑
  System Call  Message   IOCTL    Interrupt
```

## RISC-V Platform Support

### Current Status
- **ISA audio devices**: Not supported (no ISA bus on RISC-V)
- **PCI audio devices**: Supported through PCI bus enumeration
- **Platform audio**: Requires SoC-specific drivers
- **USB audio**: Supported through USB host controller

### RISC-Specific Considerations
- **Memory mapping**: Use MMIO for register access
- **Interrupt handling**: Platform-level interrupt controller (PLIC)
- **DMA support**: Platform DMA controller or CPU-based transfer
- **Device tree**: Audio device discovery via FDT

### QEMU virt Platform
The QEMU virt machine provides:
- **AC97 audio**: Intel AC97 audio controller
- **PCI interface**: Standard PCI audio devices
- **MMIO access**: Memory-mapped I/O registers
- **Interrupt support**: Through PLIC

## Power Management

### Suspend/Resume Support
Audio drivers implement power management callbacks:
```c
static int
audio_suspend(void)
{
    // Save hardware state
    // Disable audio hardware
    // Release system resources
}

static int
audio_resume(void)
{
    // Reinitialize hardware
    // Restore saved state
    // Resume audio operations
}
```

### Runtime Power Management
- **Dynamic clock gating**: Disable clocks when idle
- **Power state transitions**: Multiple low-power states
- **Wake-on-sound**: Resume on audio activity

## Error Handling

### Hardware Error Recovery
- **DMA errors**: Restart transfer with new buffer
- **Interrupt storms**: Disable and re-enable interrupts
- **Buffer underrun**: Adjust buffer size and timing
- **Hardware failure**: Graceful degradation or device removal

### Software Error Handling
- **Invalid parameters**: Return appropriate error codes
- **Resource exhaustion**: Memory and DMA allocation failures
- **Permission violations**: Access control enforcement
- **Timeout handling**: Operation completion timeouts

## Testing and Debugging

### Test Infrastructure
- **Audio loopback**: Internal testing mode
- **Tone generation**: Sine wave test patterns
- **Buffer analysis**: Underrun/overrun detection
- **Performance metrics**: Latency and throughput measurement

### Debug Features
- **Verbose logging**: Detailed operation traces
- **Register dumps**: Hardware state inspection
- **Interrupt statistics**: Interrupt frequency analysis
- **DMA monitoring**: Transfer completion tracking

## Future Enhancements

### Planned Features
- **HD Audio support**: Intel High Definition Audio
- **Bluetooth audio**: A2DP profile support
- **Audio routing**: Advanced mixer capabilities
- **Low-latency mode**: Real-time audio applications

### Architecture Improvements
- **Unified audio framework**: Common driver infrastructure
- **Plugin architecture**: Modular codec support
- **Hardware abstraction**: Platform-independent interfaces
- **Advanced power management**: Aggressive power saving

## References

- **MINIX Audio Framework**: `<minix/audio_fw.h>`
- **Sound Driver Interface**: `<minix/sound.h>`
- **Device Manager API**: `<minix/ds.h>`
- **Character Driver Interface**: `<minix/chardriver.h>`
- **PCI Bus Interface**: `<dev/pci/pciio.h>`