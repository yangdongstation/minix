# MINIX Remote Procedure Call (RPC) Headers Architecture

## Directory Overview

The `/include/rpc/` directory contains the complete Sun RPC (Remote Procedure Call) implementation, providing a comprehensive framework for distributed computing in MINIX. These headers define the interface for building client-server applications that can communicate across network boundaries using a standardized procedure call mechanism.

## RPC Architecture Foundation

### Core RPC Concepts
- **XDR (External Data Representation)**: Platform-independent data serialization
- **Client Stub**: Local proxy for remote procedures
- **Server Stub**: Remote procedure dispatcher
- **RPC Protocol**: Standardized request-response message format
- **Port Mapper**: Service location and binding mechanism

### Standards Compliance
- **Sun RPC Specification**: Original Sun Microsystems RPC protocol
- **ONC RPC**: Open Network Computing Remote Procedure Call
- **RFC 1831**: RPC Version 2 protocol specification
- **RFC 1832**: XDR external data representation standard

## Header Files and Components

### Core RPC Interface Headers

#### `rpc.h` - Master RPC Header
**Purpose**: Primary include file that consolidates all RPC functionality

**Included Components**:
```c
#include <rpc/types.h>      // Basic type definitions
#include <rpc/xdr.h>        // Data serialization
#include <rpc/auth.h>       // Authentication mechanisms
#include <rpc/clnt.h>       // Client-side RPC functions
#include <rpc/rpc_msg.h>    // RPC message structures
#include <rpc/svc.h>        // Server-side RPC functions
#include <rpc/nettype.h>    // Network transport types
```

**Key Features**:
- Unified RPC programming interface
- Automatic header dependency resolution
- Cross-platform compatibility macros
- Version and compatibility checking

#### `types.h` - RPC Type Definitions
**Purpose**: Defines fundamental data types used throughout the RPC system

**Core Types**:
```c
// Basic RPC types
typedef int32_t rpc_inline_t;      // Inline integer type
typedef int32_t bool_t;            // Boolean type
typedef int32_t enum_t;            // Enumeration type

// RPC status codes
typedef enum {
    RPC_SUCCESS = 0,               // Call succeeded
    RPC_CANTENCODEARGS = 1,        // Can't encode arguments
    RPC_CANTDECODERES = 2,         // Can't decode results
    RPC_CANTSEND = 3,              // Failure in sending call
    RPC_CANTRECV = 4,              // Failure in receiving result
    RPC_TIMEDOUT = 5,              // Call timed out
    RPC_VERSMISMATCH = 6,          // RPC version mismatch
    RPC_AUTHERROR = 7,             // Authentication error
    RPC_PROGUNAVAIL = 8,           // Program unavailable
    RPC_PROGVERSMISMATCH = 9,      // Program version mismatch
    RPC_PROCUNAVAIL = 10,          // Procedure unavailable
    RPC_CANTDECODEARGS = 11,       // Can't decode arguments
    RPC_SYSTEMERROR = 12,          // System error
    RPC_UNKNOWNHOST = 13,          // Unknown host
    RPC_UNKNOWNPROTO = 17,         // Unknown protocol
    RPC_UNKNOWNADDR = 19,          // Unknown address
    RPC_NOBROADCAST = 21,          // Broadcasting not supported
    RPC_UDERROR = 23,              // UDP error
    RPC_TCPERROR = 27,             // TCP error
    RPC_VERSMISMATCH = 6,          // Version mismatch
    RPC_PROGUNAVAIL = 8,           // Program unavailable
    RPC_PROGVERSMISMATCH = 9,      // Program version mismatch
    RPC_PROCUNAVAIL = 10,          // Procedure unavailable
    RPC_CANTDECODEARGS = 11,       // Can't decode arguments
    RPC_SYSTEMERROR = 12,          // System error
    RPC_UNKNOWNHOST = 13,          // Unknown host
    RPC_PMAPFAILURE = 14,          // Port mapper failure
    RPC_PROGNOTREGISTERED = 15,    // Program not registered
    RPC_FAILED = 16                // Unspecified failure
} clnt_stat;
```

#### `rpc_msg.h` - RPC Message Format
**Purpose**: Defines the structure of RPC request and reply messages

**Message Structures**:
```c
// RPC message header
struct rpc_msg {
    uint32_t rm_xid;              // Transaction ID
    enum msg_type rm_direction;   // Call or reply
    union {
        struct call_body RM_cmb;  // Call message body
        struct reply_body RM_rmb; // Reply message body
    } ru;
};

// Call message body
struct call_body {
    uint32_t cb_rpcvers;          // RPC protocol version
    uint32_t cb_prog;             // Program number
    uint32_t cb_vers;             // Program version
    uint32_t cb_proc;             // Procedure number
    struct opaque_auth cb_cred;   // Authentication credentials
    struct opaque_auth cb_verf;   // Authentication verifier
};
```

### Data Serialization Headers

#### `xdr.h` - External Data Representation
**Purpose**: Provides platform-independent data serialization and deserialization

**Core XDR Functions**:
```c
// Basic XDR operations
bool_t xdr_void(XDR *, void *);           // No data
bool_t xdr_int(XDR *, int *);             // Integer
bool_t xdr_u_int(XDR *, u_int *);         // Unsigned integer
bool_t xdr_long(XDR *, long *);           // Long integer
bool_t xdr_u_long(XDR *, u_long *);       // Unsigned long
bool_t xdr_short(XDR *, short *);         // Short integer
bool_t xdr_u_short(XDR *, u_short *);     // Unsigned short
bool_t xdr_char(XDR *, char *);           // Character
bool_t xdr_u_char(XDR *, u_char *);       // Unsigned character
bool_t xdr_bool(XDR *, bool_t *);         // Boolean
bool_t xdr_enum(XDR *, enum_t *);         // Enumeration
bool_t xdr_string(XDR *, char **, u_int); // String
bool_t xdr_bytes(XDR *, char **, u_int *, u_int); // Byte array
bool_t xdr_array(XDR *, char **, u_int *, u_int, u_int, xdrproc_t); // Array
bool_t xdr_vector(XDR *, char *, u_int, u_int, xdrproc_t); // Vector
```

**XDR Stream Types**:
```c
// XDR stream directions
enum xdr_op {
    XDR_ENCODE = 0,    // Encode data
    XDR_DECODE = 1,    // Decode data
    XDR_FREE = 2       // Free allocated data
};

// XDR stream structure
typedef struct XDR {
    enum xdr_op x_op;              // Stream operation
    struct xdr_ops {
        bool_t (*x_getlong)(struct XDR *, long *);    // Get long
        bool_t (*x_putlong)(struct XDR *, long *);    // Put long
        bool_t (*x_getbytes)(struct XDR *, char *, u_int); // Get bytes
        bool_t (*x_putbytes)(struct XDR *, char *, u_int); // Put bytes
        u_int (*x_getpostn)(struct XDR *);            // Get position
        bool_t (*x_setpostn)(struct XDR *, u_int);     // Set position
        int32_t *(*x_inline)(struct XDR *, u_int);     // Inline buffer
        void (*x_destroy)(struct XDR *);               // Destroy stream
    } *x_ops;
    char *x_public;                // Public data
    char *x_private;               // Private data
    char *x_base;                  // Base of stream
    int x_handy;                   // Extra buffer space
} XDR;
```

**RISC-V 64-bit Considerations**:
- All XDR operations maintain 32-bit alignment for compatibility
- Network byte order preserved regardless of host architecture
- Efficient handling of both 32-bit and 64-bit data types
- Optimized for RISC-V memory access patterns

### Client-Side RPC Headers

#### `clnt.h` - Client RPC Interface
**Purpose**: Provides client-side RPC functionality and connection management

**Client Creation Functions**:
```c
// Create UDP-based RPC client
CLIENT *clntudp_create(struct sockaddr_in *raddr, u_long program, 
                       u_long version, struct timeval wait, int *sockp);

// Create TCP-based RPC client
CLIENT *clnttcp_create(struct sockaddr_in *raddr, u_long program, 
                       u_long version, int *sockp, u_int sendsz, u_int recvsz);

// Generic client creation
CLIENT *clnt_create(const char *host, u_long prog, u_long vers, const char *proto);
```

**Client Operations**:
```c
// Make RPC call
enum clnt_stat clnt_call(CLIENT *clnt, u_long proc, xdrproc_t xargs, 
                         char *argsp, xdrproc_t xres, char *resp, 
                         struct timeval timeout);

// Client control operations
bool_t clnt_control(CLIENT *cl, u_int req, char *info);

// Destroy client
void clnt_destroy(CLIENT *clnt);

// Get error information
void clnt_geterr(CLIENT *clnt, struct rpc_err *errp);

// Print error message
void clnt_perror(CLIENT *clnt, const char *s);
void clnt_perrno(enum clnt_stat stat);
```

#### `clnt_soc.h` - Socket-based Client Interface
**Purpose**: Socket-specific client creation and management functions

**Socket Client Functions**:
```c
// Create connected UDP client
CLIENT *clntudp_bufcreate(struct sockaddr_in *raddr, u_long program, 
                          u_long version, struct timeval wait, int *sockp,
                          u_int sendsz, u_int recvsz);

// Create broadcast RPC client
CLIENT *clntudp_create_vers(struct sockaddr_in *raddr, u_long program, 
                            u_long *version_out, u_int version_min, 
                            u_int version_max, struct timeval wait, int *sockp);
```

### Server-Side RPC Headers

#### `svc.h` - Server RPC Interface
**Purpose**: Provides server-side RPC functionality and service registration

**Service Registration Functions**:
```c
// Register RPC service
SVCXPRT *svcudp_create(int sock);
SVCXPRT *svctcp_create(int sock, u_int sendsize, u_int recvsize);
SVCXPRT *svcfd_create(int fd, u_int sendsize, u_int recvsize);

// Register service with port mapper
bool_t pmap_set(u_long prognum, u_long versnum, int protocol, u_short port);
bool_t pmap_unset(u_long prognum, u_long versnum);

// Get service port
u_short pmap_getport(struct sockaddr_in *address, u_long program, 
                     u_long version, u_int protocol);
```

**Service Dispatch Functions**:
```c
// Register service procedure
bool_t svc_register(SVCXPRT *xprt, u_long prognum, u_long versnum, 
                    void (*dispatch)(struct svc_req *, SVCXPRT *), int protocol);

// Unregister service
void svc_unregister(u_long prognum, u_long versnum);

// Service request dispatch
void svc_getreq(int rdfds);
void svc_getreqset(fd_set *rdfds);
void svc_getreq_common(int fd);

// Run service loop
void svc_run(void);

// Enable asynchronous service
bool_t svc_sendreply(SVCXPRT *xprt, xdrproc_t xdr_results, char *xdr_location);
```

#### `svc_soc.h` - Socket-based Server Interface
**Purpose**: Socket-specific server creation and management

### Authentication Headers

#### `auth.h` - RPC Authentication Framework
**Purpose**: Defines authentication mechanisms for RPC calls

**Authentication Types**:
```c
// Authentication flavors
#define AUTH_NONE       0       // No authentication
#define AUTH_SYS        1       // UNIX-style authentication
#define AUTH_SHORT      2       // Short form authentication
#define AUTH_DES        3       // DES encryption authentication
#define AUTH_KERB       4       // Kerberos authentication

// Authentication structure
struct opaque_auth {
    enum_t oa_flavor;           // Authentication flavor
    caddr_t oa_base;            // Authentication data
    u_int oa_length;            // Length of authentication data
};
```

#### `auth_unix.h` - UNIX Authentication
**Purpose**: Implements traditional UNIX-style authentication using UID/GID

**UNIX Credentials**:
```c
struct authunix_parms {
    u_long aup_time;            // Creation time
    char *aup_machname;         // Machine name
    uid_t aup_uid;              // Effective user ID
    gid_t aup_gid;              // Effective group ID
    u_int aup_len;              // Group list length
    gid_t *aup_gids;            // Group list
};
```

### Port Mapping and Service Discovery

#### `pmap_prot.h` - Port Mapper Protocol
**Purpose**: Defines the port mapper service protocol for RPC service location

**Port Mapper Procedures**:
```c
#define PMAPPROG        100000  // Port mapper program number
#define PMAPVERS        2       // Port mapper version
#define PMAPVERS_ORIG   2       // Original version
#define PMAPVERS_PROTO  3       // Protocol version

// Port mapper procedures
#define PMAPPROC_NULL       0   // Null procedure
#define PMAPPROC_SET        1   // Set port mapping
#define PMAPPROC_UNSET      2   // Unset port mapping
#define PMAPPROC_GETPORT    3   // Get port number
#define PMAPPROC_DUMP       4   // Dump all mappings
#define PMAPPROC_CALLIT     5   // Call remote procedure
```

#### `pmap_clnt.h` - Port Mapper Client Interface
**Purpose**: Client-side interface to port mapper service

#### `pmap_rmt.h` - Remote Port Mapper Functions
**Purpose**: Remote port mapper operations and utilities

#### `rpcb_prot.x` - RPCBIND Protocol Definitions
**Purpose**: Extended RPC binding protocol for modern RPC implementations

### Network Transport Headers

#### `nettype.h` - Network Transport Types
**Purpose**: Defines network transport mechanisms for RPC

**Transport Types**:
```c
// Network transport types
#define NC_LOOPBACK     "loopback"      // Loopback transport
#define NC_TPI_CLTS     "tpi_clts"      // TPI connectionless
#define NC_TPI_COTS     "tpi_cots"      // TPI connection-oriented
#define NC_TPI_COTS_ORD "tpi_cots_ord"  // TPI ordered connection
#define NC_TPI_RAW      "tpi_raw"       // TPI raw transport

// Transport selection
#define NETPATH         "NETPATH"       // Environment variable
#define NETCONFIG       "/etc/netconfig" // Configuration file
```

#### `raw.h` - Raw RPC Interface
**Purpose**: Low-level RPC interface for specialized applications

### RPC Service Definitions

#### `rpcent.h` - RPC Service Database
**Purpose**: RPC service name-to-number mappings and database interface

**Service Database Functions**:
```c
// Get RPC service entry
struct rpcent *getrpcbyname(const char *name);
struct rpcent *getrpcbynumber(int number);
struct rpcent *getrpcent(void);

// Database iteration
void setrpcent(int stayopen);
void endrpcent(void);
```

#### `rpcb_clnt.h` - RPCBIND Client Interface
**Purpose**: Client interface to RPCBIND service for service location

#### `svc_auth.h` - Server Authentication
**Purpose**: Server-side authentication handling and verification

#### `rpc_com.h` - RPC Common Definitions
**Purpose**: Shared definitions and utilities for RPC implementation

## MINIX Microkernel Integration

### Message-Passing Architecture
RPC integrates with MINIX's message-passing IPC system:

```c
// RPC server registration through PM
message m;
m.m_type = PM_RPC_REGISTER;
m.PM_RPC_PROG = program_number;
m.PM_RPC_VERS = version_number;
m.PM_RPC_PROC = procedure_array;
_sendrec(PM_PROC_NR, &m);
```

### Userspace Server Model
- RPC services implemented as userspace servers
- Port mapper runs as dedicated service process
- Authentication handled by separate auth server
- Network transport managed by network server

### Driver Integration
- Network drivers provide transport layer
- Character drivers for local RPC transport
- Block drivers for RPC-based storage services

## RISC-V 64-bit Architecture Support

### Memory Alignment
```c
// Proper structure alignment for RV64
struct rpc_msg {
    uint32_t rm_xid;              // 4-byte aligned
    enum msg_type rm_direction;   // 4-byte aligned
    union {
        struct call_body RM_cmb;  // 8-byte aligned
        struct reply_body RM_rmb;
    } ru __attribute__((aligned(8)));
};
```

### Performance Optimizations
- Efficient 64-bit arithmetic for XDR operations
- Optimized byte swapping for network order
- Cache-friendly structure layouts
- Minimal memory footprint for embedded systems

### Atomic Operations
- Atomic XDR stream position updates
- Thread-safe client and server operations
- Concurrent RPC call handling
- Lock-free data structures where applicable

## Security Framework

### Authentication Mechanisms
- **AUTH_NONE**: No authentication (development/debugging)
- **AUTH_SYS**: Traditional UNIX UID/GID authentication
- **AUTH_DES**: Data Encryption Standard authentication
- **AUTH_KERB**: Kerberos v5 authentication support

### Security Considerations
- Credential validation in userspace servers
- Message integrity verification
- Replay attack prevention
- Secure key distribution mechanisms

### Access Control
- Service-specific access policies
- User and group-based authorization
- Network address-based restrictions
- Time-based access controls

## Development Guidelines

### RPC Service Development
```c
// Service definition
#include <rpc/rpc.h>
#include "myservice.h"

// Server implementation
bool_t myservice_proc_1_svc(myargs *argp, myresult *result, 
                           struct svc_req *rqstp) {
    // Service procedure implementation
    return TRUE;
}

// Client usage
int call_myservice(char *host, myargs *args, myresult *result) {
    CLIENT *clnt;
    enum clnt_stat retval;
    
    clnt = clnt_create(host, MYSERVICE_PROG, MYSERVICE_VERS, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(host);
        return -1;
    }
    
    retval = clnt_call(clnt, MYSERVICE_PROC, 
                      (xdrproc_t)xdr_myargs, (char *)args,
                      (xdrproc_t)xdr_myresult, (char *)result,
                      TIMEOUT);
    
    clnt_destroy(clnt);
    return (retval == RPC_SUCCESS) ? 0 : -1;
}
```

### Best Practices
1. Always check return values from RPC calls
2. Implement proper timeout handling
3. Use appropriate authentication mechanisms
4. Handle network failures gracefully
5. Implement service retry logic
6. Monitor RPC performance and errors
7. Use connection pooling for high-performance applications
8. Implement proper resource cleanup

### Error Handling
```c
// Comprehensive error handling
enum clnt_stat stat;
struct rpc_err err;

stat = clnt_call(clnt, procnum, xdr_args, args, xdr_result, result, timeout);
if (stat != RPC_SUCCESS) {
    clnt_geterr(clnt, &err);
    switch (err.re_status) {
    case RPC_TIMEDOUT:
        // Handle timeout
        break;
    case RPC_CANTENCODEARGS:
        // Handle encoding error
        break;
    case RPC_SYSTEMERROR:
        // Handle system error
        break;
    default:
        // Handle other errors
        break;
    }
}
```

## Performance Considerations

### Transport Selection
- **UDP**: Low-latency, connectionless, suitable for idempotent operations
- **TCP**: Reliable, connection-oriented, suitable for large data transfers
- **Local**: High-performance local transport for same-machine communication

### Optimization Strategies
- Connection reuse and pooling
- Batch RPC operations where possible
- Asynchronous RPC for non-blocking operations
- Caching of frequently accessed data
- Compression for large data transfers

### Scalability Features
- Stateless server design
- Load balancing across multiple servers
- Service replication and failover
- Distributed service registration

This comprehensive RPC header collection provides a complete distributed computing framework for MINIX, enabling robust client-server applications while maintaining compatibility with standard Sun RPC implementations and optimizing for the RISC-V 64-bit architecture.