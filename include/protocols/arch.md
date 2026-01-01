# Network Protocol Definitions Architecture

## Overview

The `include/protocols/` directory contains header files defining network protocol formats and constants for various Internet and UNIX network services. These definitions provide the protocol-specific data structures, constants, and interfaces required for network communication in MINIX 3, supporting both traditional UNIX network services and Internet protocols.

## Purpose and Responsibilities

### Core Functions
- **Protocol Specification**: Defines exact protocol message formats and field layouts
- **Constant Definitions**: Provides protocol-specific constants, flags, and enumeration values
- **Data Structure Definition**: Specifies C structures for protocol message representation
- **Service Interface**: Defines client-server communication interfaces
- **Binary Compatibility**: Ensures binary protocol compatibility across platforms
- **Standards Compliance**: Maintains adherence to official protocol specifications

### Key Features
- Complete protocol header definitions
- Network byte order handling
- Cross-platform binary compatibility
- Standards-compliant implementations
- Historical protocol support
- Service-specific extensions

## Directory Structure

```
include/protocols/
├── dumprestore.h       # Dump/restore protocol definitions
├── routed.h           # Routing daemon protocol definitions
├── rwhod.h            # Remote who daemon protocol definitions
├── talkd.h            # Talk daemon protocol definitions
└── timed.h            # Time daemon protocol definitions
```

## Protocol Definitions

### Dump/Restore Protocol (`dumprestore.h`)
Defines the protocol for system backup and restore operations:

```c
/* Dump/restore protocol header */
struct dumprestore_header {
    int32_t protocol_version;   /* Protocol version number */
    int32_t operation_type;     /* DUMP or RESTORE operation */
    int32_t data_size;          /* Size of following data */
    int32_t checksum;           /* Data checksum */
    time_t timestamp;           /* Operation timestamp */
};

/* Dump/restore operations */
#define DUMP_OP_CREATE      0x01    /* Create new dump */
#define DUMP_OP_APPEND      0x02    /* Append to existing dump */
#define DUMP_OP_VERIFY      0x03    /* Verify dump integrity */
#define RESTORE_OP_FULL     0x10    /* Full system restore */
#define RESTORE_OP_PARTIAL  0x11    /* Partial restore */
```

**Key Features:**
- Version-controlled protocol format
- Checksum-based integrity verification
- Timestamp-based consistency checking
- Support for both full and incremental operations
- Error recovery mechanisms

### Routing Daemon Protocol (`routed.h`)
Defines the Routing Information Protocol (RIP) interface:

```c
/* RIP packet format */
struct rip_packet {
    u_char command;             /* Request or response */
    u_char version;             /* RIP version */
    u_short unused;             /* Must be zero */
    struct rip_entry entries[25]; /* Route entries */
};

/* RIP route entry */
struct rip_entry {
    u_short family;             /* Address family */
    u_short tag;                /* Route tag */
    struct in_addr addr;        /* Network address */
    struct in_addr mask;        /* Subnet mask */
    struct in_addr next_hop;    /* Next hop address */
    u_int metric;               /* Distance metric */
};
```

**RIP Commands:**
```c
#define RIPCMD_REQUEST      1   /* Request for routing information */
#define RIPCMD_RESPONSE     2   /* Response containing routing table */
#define RIPCMD_TRACEON      3   /* Enable packet tracing */
#define RIPCMD_TRACEOFF     4   /* Disable packet tracing */
#define RIPCMD_POLL         5   /* Poll for routing information */
#define RIPCMD_POLLENTRY    6   /* Poll for specific entry */
```

**Protocol Characteristics:**
- Distance-vector routing algorithm
- Maximum hop count of 15
- Periodic route updates every 30 seconds
- Split horizon with poisoned reverse
- Triggered updates for route changes

### Remote Who Daemon Protocol (`rwhod.h`)
Defines the protocol for remote system status reporting:

```c
/* Who daemon packet format */
struct whod_packet {
    char wd_vers;               /* Protocol version */
    char wd_type;               /* Packet type */
    char wd_pad[2];             /* Alignment padding */
    int wd_sendtime;            /* Time packet sent */
    int wd_recvtime;            /* Time packet received */
    char wd_hostname[32];       /* Hostname */
    int wd_loadav[3];           /* Load averages */
    int wd_boottime;            /* System boot time */
    struct whoent wd_we[1024 / sizeof(struct whoent)];
};

/* User entry */
struct whoent {
    struct timeval we_utmp;     /* User session time */
    char we_name[16];           /* Username */
    char we_line[16];           /* Terminal line */
    char we_host[48];           /* Remote hostname */
};
```

**Protocol Features:**
- Broadcast-based information dissemination
- Load average reporting
- User session tracking
- Remote host information
- Timestamp-based synchronization

### Talk Daemon Protocol (`talkd.h`)
Defines the protocol for real-time communication services:

```c
/* Talk daemon message types */
#define MSG_ANNOUNCE        1   /* Announce availability */
#define MSG_LEAVE           2   /* Leave invitation */
#define MSG_ACCEPT          3   /* Accept invitation */
#define MSG_REJECT          4   /* Reject invitation */
#define MSG_DELETE          5   /* Delete invitation */

/* Talk control message */
struct talk_control {
    u_char version;             /* Protocol version */
    u_char type;                /* Message type */
    u_char answer;              /* Response code */
    u_char pad;                 /* Padding */
    u_int32_t id_num;           /* Conversation ID */
    struct sockaddr_in addr;    /* Address information */
    struct process_info proc;
};

/* Process information */
struct process_info {
    char name[16];              /* Process name */
    char line[16];              /* Terminal line */
    char host[128];             /* Hostname */
    time_t time;                /* Timestamp */
};
```

**Service Operations:**
- Invitation-based communication setup
- Real-time message exchange
- Terminal line sharing
- User presence detection
- Graceful connection termination

### Time Daemon Protocol (`timed.h`)
Defines the protocol for network time synchronization:

```c
/* Time daemon message format */
struct timed_message {
    u_char version;             /* Protocol version */
    u_char type;                /* Message type */
    u_char hop_count;           /* Hop count */
    u_char unused;              /* Reserved */
    u_int32_t seq_num;          /* Sequence number */
    struct timeval time;        /* Timestamp */
    char hostname[64];          /* Hostname */
};

/* Time daemon operations */
#define TIMED_OP_REQUEST    1   /* Request time information */
#define TIMED_OP_REPLY      2   /* Reply with time data */
#define TIMED_OP_BROADCAST  3   /* Broadcast time update */
#define TIMED_OP_MASTER     4   /* Announce master status */
```

**Time Synchronization Features:**
- Master-slave time synchronization
- Network time propagation
- Hop count-based distance measurement
- Clock drift compensation
- Multiple time source support

## Protocol Design Principles

### Network Byte Order
All protocol definitions use network byte order (big-endian):
```c
/* Byte order conversion macros */
#define htons(x)        __builtin_bswap16(x)
#define ntohs(x)        __builtin_bswap16(x)
#define htonl(x)        __builtin_bswap32(x)
#define ntohl(x)        __builtin_bswap32(x)
```

### Binary Compatibility
Protocol structures designed for binary compatibility:
- Explicit padding for alignment
- Fixed-size fields where possible
- Version numbers for evolution
- Reserved fields for future use
- Platform-independent data types

### Error Handling
Comprehensive error handling mechanisms:
```c
/* Protocol error codes */
#define PROTO_SUCCESS       0   /* Operation successful */
#define PROTO_ERROR_VERSION 1   /* Version mismatch */
#define PROTO_ERROR_FORMAT  2   /* Invalid message format */
#define PROTO_ERROR_AUTH    3   /* Authentication failed */
#define PROTO_ERROR_RESOURCE 4  /* Resource unavailable */
#define PROTO_ERROR_TIMEOUT 5   /* Operation timed out */
```

## Implementation Guidelines

### Message Validation
Protocol message validation requirements:
- Version number verification
- Message size validation
- Field range checking
- Checksum verification
- Timestamp validation

### Security Considerations
Security measures for network protocols:
- Message authentication
- Source verification
- Replay attack prevention
- Access control enforcement
- Encryption support where applicable

### Performance Optimization
Protocol performance considerations:
- Minimal message overhead
- Efficient binary formats
- Bulk operation support
- Connection pooling
- Caching mechanisms

## Integration with System Services

### Service Registration
Protocol integration with MINIX services:
- Service discovery mechanisms
- Port assignment and management
- Protocol handler registration
- Event notification setup
- Resource allocation coordination

### System Configuration
Protocol-specific configuration:
```c
/* Protocol configuration structure */
struct proto_config {
    int enabled;                /* Protocol enable/disable */
    int port;                   /* Service port number */
    int timeout;                /* Operation timeout */
    int retry_count;            /* Retry attempts */
    int flags;                  /* Configuration flags */
};
```

## Testing and Validation

### Protocol Compliance
Standards compliance validation:
- RFC specification adherence
- Message format validation
- Interoperability testing
- Performance benchmarking
- Error condition testing

### Regression Testing
Protocol evolution support:
- Version compatibility testing
- Backward compatibility validation
- Performance regression detection
- Security vulnerability assessment
- Load testing under stress

## Future Enhancements

### Protocol Evolution
Support for protocol modernization:
- IPv6 address support
- Enhanced security mechanisms
- Improved error reporting
- Performance optimizations
- Extended functionality

### New Protocol Support
Potential additional protocols:
- Modern routing protocols
- Enhanced time synchronization
- Secure communication protocols
- Service discovery mechanisms
- Network management protocols

The network protocol definitions provide the essential interfaces for traditional UNIX network services in MINIX 3, maintaining compatibility with historical implementations while supporting modern networking requirements and ensuring reliable cross-platform communication.