# MINIX RPC Service Definitions Architecture

## Directory Overview

The `/include/rpcsvc/` directory contains RPC service definitions for standard network services built on top of the Sun RPC framework. These headers define the protocol specifications, data structures, and procedure interfaces for distributed services commonly used in UNIX network environments.

## Service Architecture

### RPC Service Model
- **Client-Server Architecture**: Standard RPC request-response pattern
- **Service Registration**: Services register with port mapper/rpcbind
- **Protocol Versioning**: Support for multiple service versions
- **Authentication Integration**: Various authentication mechanisms
- **Network Transport**: UDP and TCP transport support

### Standards Compliance
- **Sun RPC Services**: Traditional UNIX network services
- **RFC Specifications**: Internet standard service protocols
- **BSD Compatibility**: Berkeley Software Distribution services
- **POSIX Extensions**: Portable operating system interfaces

## Service Definitions

### Network Information Service (NIS/YP)

#### `yp_prot.h` - NIS Protocol Definitions
**Purpose**: Defines the Network Information Service (NIS/YP) protocol for distributed system administration

**NIS Domain Management**:
```c
// NIS program and version numbers
#define YPPROG          100004          // NIS program number
#define YPVERS          2               // NIS version number
#define YPVERS_ORIG     1               // Original version

// NIS procedure numbers
#define YPPROC_NULL     0               // Null procedure
#define YPPROC_DOMAIN   1               // Get domain name
#define YPPROC_DOMAIN_NONACK 2          // Get domain (non-acknowledging)
#define YPPROC_MATCH    3               // Lookup key-value pair
#define YPPROC_FIRST    4               // Get first key-value pair
#define YPPROC_NEXT     5               // Get next key-value pair
#define YPPROC_XFR      6               // Transfer map
#define YPPROC_CLEAR    7               // Clear map
#define YPPROC_ALL      8               // Get all values
#define YPPROC_MASTER   9               // Get master server
#define YPPROC_ORDERNUM 10              // Get map order number
#define YPPROC_MAPLIST  11              // Get map list
```

**NIS Data Structures**:
```c
// NIS map entry
struct ypentry {
    char *key;                      // Key string
    char *val;                      // Value string
    int keylen;                     // Key length
    int vallen;                     // Value length
};

// NIS response status
enum ypstat {
    YP_TRUE = 1,                    // Success
    YP_FALSE = 0,                   // General failure
    YP_NOMORE = 2,                  // No more entries
    YP_FAILED = 3,                  // Operation failed
    YP_BADDB = 4,                   // Bad database
    YP_YPSERR = 5,                  // NIS server error
    YP_BADARGS = 6,                 // Invalid arguments
    YP_VERS = 7                     // Version mismatch
};

// NIS binding information
struct ypbind_binding {
    char *ypbind_binding_domain;    // Domain name
    char *ypbind_binding_server;    // Server address
    unsigned int ypbind_binding_port; // Server port
    unsigned int ypbind_binding_order; // Binding order
};
```

#### `ypclnt.h` - NIS Client Interface
**Purpose**: Client-side interface for NIS operations

**Client Functions**:
```c
// Domain operations
int yp_get_default_domain(char **domain);
int yp_bind(const char *domain);
int yp_unbind(const char *domain);

// Map lookup operations
int yp_match(const char *domain, const char *map, 
             const char *key, int keylen, char **val, int *vallen);

int yp_first(const char *domain, const char *map,
             char **key, int *keylen, char **val, int *vallen);

int yp_next(const char *domain, const char *map,
            const char *oldkey, int oldkeylen,
            char **newkey, int *newkeylen, char **val, int *vallen);

// Map enumeration
int yp_all(const char *domain, const char *map,
           struct ypall_callback *callback);

// Server information
int yp_master(const char *domain, const char *map, char **master);
int yp_order(const char *domain, const char *map, int *order);
int yp_maplist(const char *domain, struct ypmaplist **maplist);
```

**NIS Map Types**:
```c
// Standard NIS maps
#define YP_MAP_PASSWD   "passwd.byname"     // User accounts
#define YP_MAP_GROUP    "group.byname"      // Group information
#define YP_MAP_HOSTS    "hosts.byname"      // Host names
#define YP_MAP_SERVICES "services.byname"   // Network services
#define YP_MAP_PROTOCOLS "protocols.bynumber" // Protocol numbers
#define YP_MAP_RPC      "rpc.bynumber"      // RPC programs
#define YP_MAP_NETGROUP "netgroup"          // Netgroup definitions
#define YP_MAP_NETID    "netid.byname"      // Network IDs
#define YP_MAP_ALIASES  "mail.aliases"      // Mail aliases
#define YP_MAP_ETHERS   "ethers.byname"     // Ethernet addresses
```

### Service Integration Architecture

#### MINIX Microkernel Integration
RPC services integrate with MINIX's message-passing architecture:

```c
// NIS service registration
message m;
m.m_type = VFS_NIS_REGISTER;
m.VFS_NIS_DOMAIN = domain_name;
m.VFS_NIS_MAPS = map_array;
m.VFS_NIS_SERVER = server_address;
_sendrec(VFS_PROC_NR, &m);

// NIS lookup through message passing
m.m_type = VFS_NIS_LOOKUP;
m.VFS_NIS_DOMAIN = domain;
m.VFS_NIS_MAP = map_name;
m.VFS_NIS_KEY = lookup_key;
_sendrec(VFS_PROC_NR, &m);
```

#### Userspace Service Model
- **NIS Server**: Userspace daemon managing NIS maps
- **YPBIND Service**: Binding service for NIS server discovery
- **YPXFR Service**: Map transfer service for replication
- **Database Backend**: File-based or database storage for maps

### Protocol Specifications

#### NIS Protocol Flow
```
1. Client Discovery: YPBIND finds NIS server for domain
2. Domain Binding: Client establishes domain association
3. Map Operations: Client performs map lookups/updates
4. Cache Management: Client maintains local cache
5. Server Failover: Automatic failover to backup servers
```

#### Authentication and Security
```c
// NIS authentication levels
enum yp_security {
    YP_SEC_NONE = 0,                // No authentication
    YP_SEC_PORT = 1,                // Port-based authentication
    YP_SEC_DES = 2,                 // DES authentication
    YP_SEC_KERBEROS = 3             // Kerberos authentication
};

// Secure NIS operations
struct yp_secure_request {
    struct opaque_auth auth;        // Authentication credentials
    struct timestamp timestamp;     // Request timestamp
    u_int32_t nonce;                // Anti-replay nonce
    // Original request data
};
```

## RISC-V 64-bit Architecture Support

### Memory Alignment
```c
// Proper alignment for RISC-V 64-bit
struct ypbind_binding {
    char *ypbind_binding_domain;    // 8-byte pointer on RV64
    char *ypbind_binding_server;    // 8-byte pointer on RV64
    unsigned int ypbind_binding_port; // 4-byte aligned
    unsigned int ypbind_binding_order; // 4-byte aligned
} __attribute__((aligned(8)));

// NIS entry structure optimization
struct ypentry {
    char *key;                      // 8-byte aligned pointer
    char *val;                      // 8-byte aligned pointer
    int keylen;                     // 4-byte aligned
    int vallen;                     // 4-byte aligned
    // Padding for 8-byte alignment
    int _pad;
} __attribute__((aligned(8)));
```

### Performance Optimizations
- **64-bit Pointers**: Efficient pointer operations on RV64
- **Memory Access**: Aligned memory access for optimal performance
- **Protocol Parsing**: Optimized for RISC-V instruction set
- **Network Byte Order**: Efficient endianness conversion

### Atomic Operations
```c
// Atomic NIS binding updates
static __inline void ypbind_update_binding(struct ypbind_binding *binding) {
    // Atomic update of binding information
    __atomic_store_n(&binding->ypbind_binding_order, new_order, 
                     __ATOMIC_SEQ_CST);
}
```

## Service Database Integration

### Map Storage Format
```c
// NIS map file format
struct yp_map_header {
    uint32_t magic;                 // File magic number
    uint32_t version;               // Map version
    uint32_t entries;               // Number of entries
    uint32_t order;                 // Map order number
    time_t timestamp;               // Last modification time
    char master[MAXHOSTNAMELEN];    // Master server name
};

// Hash table for fast lookups
struct yp_map_entry {
    uint32_t hash;                  // Key hash value
    uint32_t key_offset;            // Key string offset
    uint32_t val_offset;            // Value string offset
    uint32_t next;                  // Next entry in chain
};
```

### Database Backend Options
- **DBM Database**: Traditional UNIX database format
- **Hash Tables**: In-memory hash tables for performance
- **B-trees**: Balanced tree structures for ordered access
- **Flat Files**: Simple text-based map files
- **SQL Database**: Relational database backend

## Network Transport Integration

### UDP Transport
```c
// UDP-based NIS operations
#define YP_UDP_TIMEOUT  5               // UDP timeout in seconds
#define YP_UDP_RETRIES  3               // Number of retries
#define YP_UDP_MAXSIZE  8192            // Maximum UDP packet size

// UDP packet format
struct yp_udp_packet {
    uint32_t transaction_id;        // Transaction identifier
    uint32_t procedure;             // Procedure number
    uint32_t status;                // Response status
    // Procedure-specific data
};
```

### TCP Transport
```c
// TCP-based NIS for large transfers
#define YP_TCP_BACKLOG  5               // TCP connection backlog
#define YP_TCP_TIMEOUT  30              // TCP connection timeout

// TCP stream protocol
struct yp_tcp_stream {
    uint32_t length;                // Message length
    uint32_t type;                  // Message type
    // Message data follows
};
```

## Security Framework

### Authentication Mechanisms
```c
// NIS authentication integration
enum yp_auth_type {
    YP_AUTH_NONE = 0,               // No authentication
    YP_AUTH_UNIX = 1,               // UNIX authentication
    YP_AUTH_DES = 2,                // DES authentication
    YP_AUTH_KERBEROS = 3,           // Kerberos authentication
    YP_AUTH_TLS = 4                 // TLS/SSL authentication
};

// Authenticated NIS request
struct yp_auth_request {
    enum yp_auth_type auth_type;    // Authentication type
    union {
        struct authunix_parms unix; // UNIX credentials
        struct authdes_cred des;    // DES credentials
        struct authkerb_cred kerb;  // Kerberos credentials
    } auth_data;
    // Original request follows
};
```

### Access Control
```c
// NIS access control lists
struct yp_acl_entry {
    char *principal;                // Principal name
    uint32_t permissions;           // Permission bits
    struct yp_acl_entry *next;      // Next ACL entry
};

// Permission bits
#define YP_PERM_READ    0x01        // Read permission
#define YP_PERM_WRITE   0x02        // Write permission
#define YP_PERM_CREATE  0x04        // Create permission
#define YP_PERM_DELETE  0x08        // Delete permission
```

## Development Guidelines

### Service Development Pattern
```c
// NIS service implementation example
#include <rpc/rpc.h>
#include <rpcsvc/yp_prot.h>

// Service procedure implementation
bool_t ypproc_match_2_svc(yprequest *req, ypresponse *resp, 
                          struct svc_req *rqstp) {
    // Validate request
    if (req->domain == NULL || req->map == NULL || req->key == NULL) {
        resp->status = YP_BADARGS;
        return TRUE;
    }
    
    // Perform lookup
    resp->status = yp_lookup(req->domain, req->map, req->key, 
                            req->keylen, &resp->value, &resp->vallen);
    
    return TRUE;
}

// Service registration
void yp_service_init(void) {
    // Register with port mapper
    pmap_unset(YPPROG, YPVERS);
    
    // Create service transports
    SVCXPRT *udp_transp = svcudp_create(RPC_ANYSOCK);
    SVCXPRT *tcp_transp = svctcp_create(RPC_ANYSOCK, 0, 0);
    
    // Register service procedures
    svc_register(udp_transp, YPPROG, YPVERS, ypprog_2, IPPROTO_UDP);
    svc_register(tcp_transp, YPPROG, YPVERS, ypprog_2, IPPROTO_TCP);
}
```

### Client Usage Pattern
```c
// NIS client implementation
#include <rpcsvc/ypclnt.h>

int lookup_user(const char *username, struct passwd *pw) {
    char *domain, *result;
    int result_len;
    int rval;
    
    // Get default domain
    if (yp_get_default_domain(&domain) != 0)
        return -1;
    
    // Lookup user in passwd map
    rval = yp_match(domain, "passwd.byname", username, strlen(username),
                    &result, &result_len);
    if (rval != 0)
        return -1;
    
    // Parse passwd entry
    parse_passwd_entry(result, pw);
    free(result);
    
    return 0;
}
```

### Error Handling
```c
// Comprehensive NIS error handling
const char *yp_err_string(int err) {
    switch (err) {
    case YP_TRUE:       return "Success";
    case YP_FALSE:      return "General failure";
    case YP_NOMORE:     return "No more entries";
    case YP_FAILED:     return "Operation failed";
    case YP_BADDB:      return "Bad database";
    case YP_YPSERR:     return "NIS server error";
    case YP_BADARGS:    return "Invalid arguments";
    case YP_VERS:       return "Version mismatch";
    default:            return "Unknown error";
    }
}
```

## Testing and Validation

### Protocol Compliance Testing
- **RFC Compliance**: Validate protocol implementation against RFCs
- **Sun RPC Compatibility**: Test interoperability with Sun RPC
- **BSD Compatibility**: Verify BSD system compatibility
- **Cross-Platform**: Test with various client implementations

### Performance Testing
- **Lookup Performance**: Measure map lookup response times
- **Scalability**: Test with large map databases
- **Network Performance**: Measure protocol overhead
- **Failover Testing**: Validate server failover mechanisms

### Security Testing
- **Authentication**: Test all authentication mechanisms
- **Authorization**: Validate access control policies
- **Input Validation**: Test malformed request handling
- **Attack Resistance**: Simulate various attack scenarios

This comprehensive RPC service collection provides the foundation for distributed network services in MINIX, maintaining compatibility with standard UNIX network services while integrating seamlessly with the microkernel architecture and optimizing for the RISC-V 64-bit platform.