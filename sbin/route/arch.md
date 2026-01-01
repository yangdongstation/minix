# MINIX Routing Table Management Utility (route)

## Overview

The `route` utility is a fundamental network administration tool for managing the kernel routing table in MINIX systems. It provides comprehensive routing configuration capabilities, including static route management, routing policy configuration, and network path optimization. This utility is essential for establishing network connectivity between different network segments and managing complex network topologies.

## Architecture

### Core Functionality
- **Route Management**: Add, delete, modify, and display routing entries
- **Protocol Support**: IPv4 and IPv6 routing table management
- **Policy Routing**: Support for multiple routing tables and rules
- **Gateway Configuration**: Next-hop router management
- **Interface Association**: Route-to-interface mapping
- **Metric Management**: Route priority and cost configuration

### MINIX Integration
- Communicates with INET server for routing table operations
- Uses MINIX message-passing for kernel routing updates
- Integrates with VFS for persistent route configuration
- Supports MINIX-specific routing features

## Source Files

### Main Implementation
- **`route.c`** (42,946 lines)
  - Main program entry point and command dispatcher
  - Routing table manipulation logic
  - Command-line argument processing and validation
  - Route entry creation, modification, and deletion
  - Error handling and user feedback
  - Integration with MINIX networking subsystem

### Utility Functions
- **`rtutil.c`** (18,978 lines)
  - Routing table utility functions
  - Route entry comparison and sorting
  - Routing metric calculations
  - Address family handling
  - Route filtering and selection
  - Network address manipulation

- **`rtutil.h`** (2,798 lines)
  - Route utility function declarations
  - Routing table structure definitions
  - Route manipulation macros
  - Network constant definitions
  - Route flag and option enumerations

### Display and Output
- **`show.c`** (3,760 lines)
  - Routing table display formatting
  - Human-readable route presentation
  - Machine-parseable output formats
  - Route statistics and counters
  - Customizable display options

### Protocol Operations
- **`route_hostops.c`** (1,915 lines)
  - Host-specific routing operations
  - Direct kernel routing table access
  - System call wrappers for routing

- **`route_rumpops.c`** (2,052 lines)
  - Rump kernel routing operations
  - Virtualized routing environment support
  - Testing and development mode operations

### Program Operations Interface
- **`prog_ops.h`** (2,550 lines)
  - Operation dispatch interface
  - Backend abstraction definitions
  - Routing operation function prototypes
  - Multi-backend support framework

### Command Processing
- **`keywords.c`** (1,402 lines)
  - Command keyword definitions
  - Routing action verb mappings
  - Syntax validation support

- **`keywords.h`** (1,250 lines)
  - Keyword enumeration definitions
  - Command parsing constants
  - Action type declarations

- **`keywords.sh`** (1,918 lines, executable)
  - Keyword generation script
  - Automated keyword table creation
  - Build-time keyword processing

### External Interface
- **`extern.h`** (1,788 lines)
  - Global variable declarations
  - External function prototypes
  - Shared data structure definitions
  - Cross-module interface definitions

## Build Configuration

### Makefile Structure
- **`Makefile`** (719 lines)
  - Standard MINIX build configuration
  - Source file compilation rules
  - Library dependencies and linking
  - Installation and packaging targets

### Dependencies
- **System Libraries**: libc, libutil, libkvm
- **Network Libraries**: libinet, libresolv
- **MINIX Libraries**: libminix, libsys
- **Header Files**: net/route.h, netinet/in.h, net/if.h

### Build Requirements
- C compiler with C99 support
- MINIX cross-compilation toolchain
- Network protocol headers
- Routing table definitions

## MINIX System Integration

### Server Communication
```c
// Example: Route addition message
message m;
m.m_type = INET_ADD_ROUTE;
m.INET_ROUTE_DST = destination;
m.INET_ROUTE_GATEWAY = gateway;
m.INET_ROUTE_NETMASK = netmask;
m.INET_ROUTE_IFINDEX = interface;
m.INET_ROUTE_METRIC = metric;
_sendrec(INET_PROC_NR, &m);
```

### Service Dependencies
- **INET Server**: Primary routing table interface
- **VFS Server**: Configuration file persistence
- **PM Server**: Process privileges and capabilities
- **DS Server**: Service discovery and registration

### Routing Table Structure
```c
struct route_entry {
    struct sockaddr dest;      // Destination network
    struct sockaddr gateway;   // Next hop gateway
    struct sockaddr netmask;   // Network mask
    int interface;             // Output interface
    int metric;                // Route cost/priority
    int flags;                 // Route properties
    int proto;                 // Routing protocol
};
```

## Routing Table Management

### Route Types
- **Host Routes**: /32 (IPv4) or /128 (IPv6) destinations
- **Network Routes**: Variable prefix lengths
- **Default Route**: 0.0.0.0/0 or ::/0 gateway
- **Interface Routes**: Directly connected networks
- **Reject Routes**: Blackhole and unreachable destinations

### Route Operations
- **Add**: Create new routing entries
- **Delete**: Remove existing routes
- **Modify**: Change route parameters
- **Flush**: Remove multiple routes
- **Monitor**: Track routing changes
- **Get**: Retrieve specific route information

### Route Selection Algorithm
1. **Longest Prefix Match**: Most specific route wins
2. **Administrative Distance**: Protocol preference
3. **Metric Comparison**: Lower metric preferred
4. **Interface Priority**: Configured interface preferences
5. **Fallback**: Default route as last resort

## Network Configuration Management

### Static Routing
- Manual route configuration
- Persistent route storage
- Boot-time route installation
- Administrative route management

### Dynamic Routing Integration
- Route redistribution support
- Metric adjustment for dynamic protocols
- Route filtering and policy
- Next-hop tracking

### Policy Routing
- Multiple routing tables
- Routing rule configuration
- Source-based routing
- Policy-based forwarding

## Protocol Support Matrix

| Feature | IPv4 | IPv6 | Notes |
|---------|------|------|-------|
| Static Routes | ✓ Full | ✓ Full | Complete support |
| Host Routes | ✓ Full | ✓ Full | Individual host routing |
| Network Routes | ✓ Full | ✓ Full | Prefix-based routing |
| Default Routes | ✓ Full | ✓ Full | Gateway of last resort |
| Interface Routes | ✓ Full | ✓ Full | Directly connected |
| Policy Routing | ✓ Partial | ✓ Partial | Basic support |
| Route Metrics | ✓ Full | ✓ Full | Cost-based selection |

## RISC-V 64-bit Platform Considerations

### Architecture Independence
- Portable C implementation
- No architecture-specific code
- Network byte order handling
- Cross-platform compatibility

### RISC-V Optimizations
- Efficient memory alignment
- Optimized data structures
- Fast address comparisons
- Atomic operations for counters

### Network Stack Integration
- MINIX INET server compatibility
- Message-passing interface
- Zero-copy optimizations where possible
- Efficient buffer management

### Platform-Specific Features
- RISC-V memory model compliance
- Atomic route table updates
- Memory barrier usage
- Cache-friendly data layout

## Error Handling and Diagnostics

### Error Categories
- **Configuration Errors**: Invalid addresses, malformed routes
- **System Errors**: Permission denied, resource exhaustion
- **Network Errors**: Unreachable networks, gateway failures
- **Protocol Errors**: Unsupported features, version mismatches

### Diagnostic Features
- Verbose mode for detailed output
- Debug information display
- Route lookup tracing
- Error code interpretation
- Suggested corrective actions

### Validation and Safety
- Input parameter validation
- Address family consistency
- Network parameter verification
- Route loop detection
- Conflict resolution

## Security Considerations

### Access Control
- Privilege checking for route modifications
- Capability-based access control
- Audit logging for changes
- Secure configuration file handling

### Network Security
- Route validation and filtering
- Anti-spoofing protection
- Secure route redistribution
- Authentication for routing updates

### Input Validation
- Buffer overflow protection
- Command injection prevention
- Address validation
- Path traversal prevention

## Performance Optimization

### Route Lookup Efficiency
- Optimized data structures
- Caching mechanisms
- Fast prefix matching
- Minimal memory allocation

### Scalability Features
- Large routing table support
- Efficient memory usage
- Fast route updates
- Minimal CPU overhead

### Benchmarking
- Route installation performance
- Lookup speed optimization
- Memory footprint analysis
- CPU utilization metrics

## Usage Examples

### Basic Route Management
```bash
# Add static route
route add -net 192.168.2.0/24 192.168.1.1

# Add default gateway
route add default 10.0.0.1

# Delete specific route
route delete -net 192.168.2.0/24

# Display routing table
route show
```

### Advanced Configuration
```bash
# Add host route
route add -host 10.1.1.1 192.168.1.254

# Add route with metric
route add -net 172.16.0.0/16 10.0.0.1 -metric 5

# Flush all routes
route flush

# Monitor routing changes
route monitor
```

### IPv6 Routing
```bash
# Add IPv6 route
route add -inet6 2001:db8::/64 fe80::1

# Add IPv6 default route
route add -inet6 default fe80::254

# Display IPv6 routes
route show -inet6
```

## Testing and Validation

### Functional Testing
- Route addition and deletion
- Route lookup verification
- Metric-based selection
- Interface association

### Protocol Compliance
- RFC compliance testing
- Protocol interoperability
- Standards validation
- Compatibility verification

### Performance Testing
- Large-scale routing tables
- Concurrent route updates
- Lookup performance benchmarks
- Memory usage analysis

### RISC-Specific Testing
- QEMU virt platform validation
- Network stack integration
- Message-passing performance
- Cross-compilation verification

## Maintenance and Updates

### NetBSD Synchronization
- Regular updates from NetBSD-8
- Security patches and fixes
- Protocol enhancements
- Performance improvements

### MINIX-Specific Enhancements
- Message-based system integration
- Service-oriented architecture
- Enhanced reliability features
- Microkernel optimization

### Future Development
- Advanced policy routing
- MPLS support enhancement
- BGP integration preparation
- Network virtualization support

The `route` utility provides comprehensive routing table management capabilities essential for MINIX network administration. Its robust feature set, combined with tight integration with MINIX system services and platform-specific optimizations, makes it a critical component for network configuration and management in the microkernel environment.