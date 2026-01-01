# MINIX 3 Router Advertisement Daemon (rtadvd)

## Overview

`rtadvd` is the Router Advertisement Daemon that implements IPv6 router advertisement functionality for MINIX 3 systems. As a critical component of IPv6 network infrastructure, `rtadvd` sends Router Advertisement (RA) messages to inform IPv6 hosts about network configuration parameters, prefix information, and routing capabilities, enabling stateless address autoconfiguration (SLAAC) and network parameter discovery.

In MINIX 3's microkernel architecture, `rtadvd` operates as a userspace daemon that communicates with the INET server for IPv6 network operations, utilizing MINIX's message-passing system for inter-process communication and system service coordination.

## Architecture

### IPv6 Router Advertisement

`rtadvd` implements comprehensive IPv6 router functionality that:
- Sends periodic Router Advertisement messages on configured interfaces
- Provides network prefix information for address autoconfiguration
- Advertises routing parameters and link-layer addresses
- Supports multiple network interfaces and prefixes
- Implements router discovery and neighbor discovery protocols
- Manages prefix delegation and renumbering events
- Provides DNS configuration and other network parameters
- Supports mobile IPv6 and advanced routing features

### MINIX Integration

As an IPv6 network service in MINIX 3:
- Communicates with INET server for IPv6 socket operations
- Uses MINIX message passing for IPC with network services
- Integrates with system logging for network event recording
- Supports MINIX's security and privilege model
- Provides IPv6 network configuration for system administration

## Source Files

### Core Implementation

**`rtadvd.c`** (47,871 bytes)
- Main daemon implementation with comprehensive IPv6 router advertisement
- Router Advertisement message construction and transmission
- Network interface management and monitoring
- Prefix information handling and lifecycle management
- Neighbor Discovery protocol implementation
- Router configuration and parameter management
- Multi-interface support with interface-specific settings
- Prefix delegation and dynamic renumbering support
- DNS configuration advertisement (RDNSS and DNSSL)
- Mobile IPv6 support and advanced routing features
- Comprehensive error handling and network diagnostics
- Signal handling and daemon lifecycle management

**`rtadvd.h`** (6,348 bytes)
- Core data structures and type definitions
- Router Advertisement message formats
- Configuration structure definitions
- Interface and prefix management structures
- Protocol constants and timer definitions
- Function declarations and APIs

### Configuration Management

**`config.c`** (36,407 bytes)
- Configuration file parsing and validation
- Interface-specific parameter handling
- Prefix configuration and management
- Router option processing and storage
- Default parameter initialization
- Configuration validation and error checking
- Runtime configuration updates and reloading
- Advanced feature configuration support

**`config.h`** (2,264 bytes)
- Configuration structure definitions
- Configuration parsing function declarations
- Default parameter constants
- Configuration validation interfaces

**`rtadvd.conf`** (955 bytes)
- Default configuration file template
- Example interface configurations
- Common parameter settings
- Feature enable/disable options

**`rtadvd.conf.5`** (14,304 bytes)
- Comprehensive configuration file documentation
- All configuration parameters and options
- Interface-specific settings
- Advanced feature configurations
- Examples and best practices

### Network Interface Management

**`if.c`** (9,363 bytes)
- Network interface enumeration and management
- Interface capability detection and validation
- Link-state monitoring and event handling
- Interface-specific parameter application
- Multi-interface coordination and management
- Interface metric and preference handling
- Hardware address management and advertisement

**`if.h`** (2,406 bytes)
- Interface management data structures
- Interface capability definitions
- Link-state event declarations
- Interface management function prototypes

### Advanced Features

**`advcap.c`** (9,657 bytes)
- Router advertisement capability management
- Advanced feature negotiation and advertisement
- Capability detection and validation
- Feature-specific parameter handling
- Compatibility and interoperability support

**`advcap.h`** (1,817 bytes)
- Capability management structures
- Feature advertisement definitions
- Capability function declarations

**`rrenum.c`** (14,072 bytes)
- Router renumbering protocol support
- Dynamic prefix changes and updates
- Prefix deprecation and invalidation
- Smooth network renumbering procedures
- Router advertisement sequence management
- Network continuity maintenance during changes

**`rrenum.h`** (1,795 bytes)
- Renumbering protocol structures
- Prefix change management definitions
- Renumbering function declarations

### Diagnostic and Monitoring

**`dump.c`** (8,095 bytes)
- Router advertisement packet dumping and analysis
- Network debugging and troubleshooting support
- Packet content inspection and validation
- Protocol compliance verification
- Network diagnostic information display

**`dump.h`** (1,718 bytes)
- Packet dumping function declarations
- Diagnostic output format definitions
- Debug information structures

### Timer Management

**`timer.c`** (4,702 bytes)
- Timer management for periodic advertisements
- Randomized advertisement scheduling
- Retransmission and timeout handling
- Interface-specific timer management
- Event-driven timer operations

**`timer.h`** (2,394 bytes)
- Timer management structures
- Timer event definitions
- Timer function declarations

### System Integration

**`pathnames.h`** (175 bytes)
- System path definitions
- Configuration file locations
- PID file and socket paths
- Default directory structures

## IPv6 Protocol Support

### Router Advertisement Protocol

**Core Functionality**
- Periodic Router Advertisement transmission
- Solicited Router Advertisement responses
- Router lifetime and preference management
- Default router selection support
- Router discovery protocol compliance

**Message Components**
- Router lifetime and hop limit settings
- Managed/Other configuration flags
- Prefix information options
- MTU configuration options
- Source link-layer address options

### Prefix Management

**Prefix Information**
- On-link prefix advertisement
- Autonomous address configuration flag
- Preferred and valid lifetime management
- Prefix renumbering and deprecation
- Multiple prefix support per interface

**Address Configuration**
- Stateless Address Autoconfiguration (SLAAC) support
- Prefix delegation to downstream routers
- Address lifetime management
- Network continuity during renumbering

### Advanced IPv6 Features

**DNS Configuration**
- Recursive DNS Server (RDNSS) advertisement
- DNS Search List (DNSSL) provision
- DNS configuration lifetime management
- Multiple DNS server support

**Mobile IPv6 Support**
- Home agent advertisement
- Mobile router functionality
- Care-of-address registration
- Route optimization support

## Network Interface Management

### Multi-Interface Support

**Interface Configuration**
- Per-interface router parameters
- Interface-specific prefix settings
- Individual advertisement intervals
- Interface capability detection
- Link-state monitoring and response

**Interface Events**
- Link-up/link-down detection
- Interface addition and removal
- Address configuration changes
- Network parameter updates
- Dynamic interface management

### Network Topology Integration

**Router Discovery**
- Router solicitation processing
- Router advertisement coordination
- Default router selection
- Router preference handling
- Multiple router environments

**Neighbor Discovery Integration**
- Neighbor solicitation handling
- Neighbor advertisement coordination
- Duplicate address detection support
- Neighbor unreachability detection

## MINIX Server Integration

### INET Server Communication

**IPv6 Socket Operations**
- Raw IPv6 socket creation and management
- ICMPv6 packet transmission
- Router Advertisement message crafting
- Network interface information retrieval
- IPv6 protocol stack integration

**Network Configuration**
- Interface address management
- Routing table coordination
- Network parameter synchronization
- IPv6-specific network functions

### Message Passing Architecture

**Service Communication**
- Registration with device manager
- INET server message exchange
- Asynchronous operation handling
- Error propagation and recovery
- Service discovery integration

**System Integration**
- Communication with reincarnation server
- Fault tolerance and restart support
- Service dependency management
- System-wide configuration coordination

## Security and Privileges

### Access Control

**Privilege Requirements**
- Root privileges for raw IPv6 socket creation
- Network configuration access
- System information retrieval
- Configuration file access

**Security Considerations**
- Router advertisement validation
- Network parameter verification
- Prefix information authentication
- Rogue router detection and prevention

### Network Security

**RA Guard Integration**
- Router advertisement filtering
- Network access control
- Unauthorized router prevention
- Network segment protection

**Cryptographic Security**
- SEND (Secure Neighbor Discovery) preparation
- Cryptographically Generated Addresses (CGA)
- RSA signature support infrastructure
- Certificate-based authentication

## RISC-V 64-bit Platform Support

### Architecture Optimization

**Native RISC-V Support**
- RISC-V 64-bit (RV64GC) compilation
- Optimized instruction selection
- Memory alignment compliance
- Atomic operation utilization

**Performance Features**
- Efficient IPv6 packet processing
- Optimized timer operations
- Memory-conscious implementation
- Platform-specific optimizations

### Platform Integration

**QEMU virt Platform**
- Virtual network interface support
- Platform-specific IPv6 features
- Virtual router functionality
- Hardware abstraction compliance

**RISC-V Networking**
- IPv6 network stack integration
- Hardware acceleration support
- Platform-specific network optimizations
- Architecture-specific enhancements

## Configuration Management

### Configuration File Structure

**Interface Definitions**
```
interface em0 {
    # Basic router parameters
    AdvSendAdvert on;
    MaxRtrAdvInterval 600;
    MinRtrAdvInterval 200;
    
    # Prefix advertisement
    prefix 2001:db8:1::/64 {
        AdvOnLink on;
        AdvAutonomous on;
        AdvValidLifetime 86400;
        AdvPreferredLifetime 14400;
    };
    
    # DNS configuration
    RDNSS 2001:db8:1::1 2001:db8:1::2 {
        AdvRDNSSLifetime 86400;
    };
    
    # DNS search list
    DNSSL example.com internal.com {
        AdvDNSSLLifetime 86400;
    };
};
```

### Runtime Management

**Daemon Control**
```bash
# Start rtadvd on all interfaces
rtadvd

# Start on specific interface
rtadvd em0

# Start with debug output
rtadvd -d

# Use custom configuration file
rtadvd -f /etc/rtadvd.custom.conf

# Specify PID file location
rtadvd -p /var/run/rtadvd.pid
```

**Configuration Reloading**
```bash
# Reload configuration without restart
kill -HUP $(cat /var/run/rtadvd.pid)

# Dump current configuration
rtadvd -c

# Display router advertisements
rtadvd -D
```

## Advanced Features

### Router Renumbering

**Dynamic Prefix Changes**
- Smooth prefix transitions
- Network continuity maintenance
- Prefix deprecation and invalidation
- Router advertisement sequencing
- Client notification and updates

**Network Migration**
- Zero-downtime renumbering
- Prefix lifetime management
- Router coordination during changes
- Service continuity preservation

### Mobile IPv6 Support

**Home Agent Functionality**
- Home agent advertisement
- Mobile node registration
- Care-of-address management
- Tunnel establishment and maintenance

**Route Optimization**
- Direct communication facilitation
- Binding cache management
- Return routability procedures
- Security association establishment

## Network Diagnostics and Monitoring

### Diagnostic Capabilities

**Router Advertisement Analysis**
- Packet content inspection
- Protocol compliance verification
- Network debugging support
- Router behavior analysis

**Network Monitoring**
- Router advertisement statistics
- Client response tracking
- Network health assessment
- Performance metrics collection

### Troubleshooting Support

**Common Issues**
- Router advertisement transmission problems
- Prefix configuration errors
- Network connectivity issues
- Client autoconfiguration failures

**Diagnostic Procedures**
```bash
# Display router advertisement packets
rtadvd -D

# Check interface configuration
rtadvd -c

# Enable debug mode for troubleshooting
rtadvd -d -f /etc/rtadvd.conf

# Monitor router advertisements
tcpdump -i em0 icmp6 and ip6[40] = 134
```

## Integration with IPv6 Network Infrastructure

### System Administration

**IPv6 Network Management**
- Router configuration and management
- Prefix allocation and delegation
- Network parameter distribution
- Client autoconfiguration support

**Network Planning**
- Prefix planning and allocation
- Router placement optimization
- Network topology design
- Addressing scheme management

### Service Integration

**DNS Integration**
- DNS server advertisement
- Search domain provision
- DNS configuration management
- Name resolution optimization

**DHCPv6 Coordination**
- Stateless and stateful configuration
- Parameter distribution coordination
- Network management integration
- Service discovery support

## Development and Maintenance

### Code Quality

**Standards Compliance**
- RFC 4861 (Neighbor Discovery) compliance
- RFC 6275 (Mobile IPv6) support
- IPv6 protocol standards adherence
- Interoperability testing

**Security Practices**
- Secure coding standards
- Input validation and sanitization
- Privilege management
- Network security considerations

### Testing and Validation

**Protocol Compliance**
- Router advertisement protocol testing
- Prefix information validation
- Neighbor discovery integration
- Mobile IPv6 functionality

**Network Integration**
- Multi-vendor interoperability
- Complex network topology testing
- Router coordination validation
- Client compatibility testing

The `rtadvd` daemon provides essential IPv6 router advertisement functionality for MINIX 3, enabling comprehensive IPv6 network deployment with advanced features for enterprise environments, mobile networking, and dynamic network reconfiguration.