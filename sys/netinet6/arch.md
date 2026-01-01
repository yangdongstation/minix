# MINIX Internet Protocol Version 6 (IPv6) Headers Architecture

## Directory Overview

The `/sys/netinet6/` directory contains the comprehensive IPv6 protocol stack implementation for MINIX, providing next-generation Internet protocol support with enhanced addressing, security, and mobility features. These headers define the core IPv6 protocols that enable modern network communication in the MINIX operating system.

## IPv6 Architecture Foundation

### Protocol Evolution
- **Extended Address Space**: 128-bit addresses vs. 32-bit IPv4
- **Simplified Header**: Fixed 40-byte header with extension headers
- **Built-in Security**: IPsec integration as fundamental requirement
- **Auto-configuration**: Stateless and stateful address configuration
- **Mobility Support**: Built-in mobile IP capabilities
- **Multicast Enhancement**: Improved multicast group management

### Standards Compliance
- **RFC 2460**: Internet Protocol, Version 6 (IPv6) Specification
- **RFC 4291**: IPv6 Addressing Architecture
- **RFC 4443**: Internet Control Message Protocol for IPv6 (ICMPv6)
- **RFC 4861**: Neighbor Discovery for IPv6 (NDP)
- **RFC 4862**: IPv6 Stateless Address Autoconfiguration (SLAAC)
- **RFC 8200**: Internet Protocol, Version 6 (IPv6) Specification (Updated)

## Core IPv6 Protocol Headers

### IPv6 Address Architecture

#### `in6.h` - IPv6 Address Definitions
**Purpose**: Defines IPv6 address structures, constants, and utility functions

**IPv6 Address Structure**:
```c
// IPv6 address structure (128-bit)
struct in6_addr {
    union {
        uint8_t  __u6_addr8[16];    // 16 bytes as uint8_t array
        uint16_t __u6_addr16[8];    // 8 16-bit words
        uint32_t __u6_addr32[4];    // 4 32-bit words
    } __u6_addr;
};
#define s6_addr   __u6_addr.__u6_addr8
#define s6_addr16 __u6_addr.__u6_addr16
#define s6_addr32 __u6_addr.__u6_addr32
```

**IPv6 Socket Address**:
```c
// IPv6 socket address structure
struct sockaddr_in6 {
    uint8_t         sin6_len;       // Structure length
    sa_family_t     sin6_family;    // Address family (AF_INET6)
    uint16_t        sin6_port;      // Port number (network byte order)
    uint32_t        sin6_flowinfo;  // IPv6 flow information
    struct in6_addr sin6_addr;      // IPv6 address
    uint32_t        sin6_scope_id;  // Scope ID (for link-local)
};
```

**IPv6 Address Constants**:
```c
// IPv6 address sizes
#define INET6_ADDRSTRLEN 46           // Max length of IPv6 address string
#define IN6ADDRSZ       16            // Size of IPv6 address in bytes

// Special IPv6 addresses
extern const struct in6_addr in6addr_any;        // :: (any address)
extern const struct in6_addr in6addr_loopback;   // ::1 (loopback)
extern const struct in6_addr in6addr_nodelocal_allnodes;   // FF01::1
extern const struct in6_addr in6addr_linklocal_allnodes;   // FF02::1
extern const struct in6_addr in6addr_linklocal_allrouters; // FF02::2
```

**RISC-V 64-bit Alignment**:
```c
// Optimized for RISC-V 64-bit alignment
struct in6_addr {
    union {
        uint8_t  __u6_addr8[16]  __attribute__((aligned(16)));
        uint16_t __u6_addr16[8]  __attribute__((aligned(16)));
        uint32_t __u6_addr32[4]  __attribute__((aligned(16)));
    } __u6_addr;
} __attribute__((aligned(16)));

struct sockaddr_in6 {
    uint8_t         sin6_len;       // 1 byte
    sa_family_t     sin6_family;    // 2 bytes
    uint16_t        sin6_port;      // 2 bytes (aligned)
    uint32_t        sin6_flowinfo;  // 4 bytes (aligned)
    struct in6_addr sin6_addr;      // 16 bytes (aligned)
    uint32_t        sin6_scope_id;  // 4 bytes (aligned)
} __attribute__((aligned(8)));
```

### IPv6 Protocol Header

#### `ip6.h` - IPv6 Packet Header
**Purpose**: Defines the IPv6 packet header structure and extension headers

**IPv6 Header Structure**:
```c
struct ip6_hdr {
    union {
        struct ip6_hdrctl {
            uint32_t ip6_un1_flow;  // 20-bit flow + 4-bit version
            uint16_t ip6_un1_plen;  // Payload length
            uint8_t  ip6_un1_nxt;   // Next header
            uint8_t  ip6_un1_hlim;  // Hop limit
        } ip6_un1;
        uint8_t ip6_un2_vfc;        // 4-bit version + 4-bit class
    } ip6_ctlun;
    struct in6_addr ip6_src;        // Source address
    struct in6_addr ip6_dst;        // Destination address
};

#define ip6_vfc     ip6_ctlun.ip6_un2_vfc
#define ip6_flow    ip6_ctlun.ip6_un1.ip6_un1_flow
#define ip6_plen    ip6_ctlun.ip6_un1.ip6_un1_plen
#define ip6_nxt     ip6_ctlun.ip6_un1.ip6_un1_nxt
#define ip6_hlim    ip6_ctlun.ip6_un1.ip6_un1_hlim
#define ip6_hops    ip6_ctlun.ip6_un1.ip6_un1_hlim
```

**IPv6 Header Fields**:
```c
// Version field (always 6 for IPv6)
#define IPV6_VERSION            0x60
#define IPV6_VERSION_MASK       0xf0

// Traffic class (formerly priority)
#define IPV6_CLASS_MASK         0x0f
#define IPV6_CLASS_SHIFT        4

// Flow label
#define IPV6_FLOWLABEL_MASK     0x000fffff
#define IPV6_FLOWINFO_MASK      0x0fffffff

// Next header values (same as IPv4 protocol numbers)
#define IPPROTO_IPV6            41      // IPv6-in-IPv6 encapsulation
#define IPPROTO_ROUTING         43      // Routing header
#define IPPROTO_FRAGMENT        44      // Fragment header
#define IPPROTO_ESP             50      // Encapsulating Security Payload
#define IPPROTO_AH              51      // Authentication Header
#define IPPROTO_ICMPV6          58      // Internet Control Message Protocol for IPv6
#define IPPROTO_NONE            59      // No next header
#define IPPROTO_DSTOPTS         60      // Destination options header
```

### Internet Control Message Protocol for IPv6 (ICMPv6)

#### `icmp6.h` - ICMPv6 Protocol Definitions
**Purpose**: Defines ICMPv6 for IPv6 error reporting, diagnostics, and neighbor discovery

**ICMPv6 Message Types**:
```c
// ICMPv6 error messages
#define ICMP6_DST_UNREACH       1       // Destination unreachable
#define ICMP6_PACKET_TOO_BIG    2       // Packet too big
#define ICMP6_TIME_EXCEEDED     3       // Time exceeded
#define ICMP6_PARAM_PROB        4       // Parameter problem

// ICMPv6 informational messages
#define ICMP6_ECHO_REQUEST      128     // Echo request
#define ICMP6_ECHO_REPLY        129     // Echo reply
#define ICMP6_MEMBERSHIP_QUERY  130     // Group membership query
#define ICMP6_MEMBERSHIP_REPORT 131     // Group membership report
#define ICMP6_MEMBERSHIP_REDUCTION 132  // Group membership reduction
#define ICMP6_ROUTER_SOLICITATION 133   // Router solicitation
#define ICMP6_ROUTER_ADVERTISEMENT 134  // Router advertisement
#define ICMP6_NEIGHBOR_SOLICITATION 135 // Neighbor solicitation
#define ICMP6_NEIGHBOR_ADVERTISEMENT 136 // Neighbor advertisement
#define ICMP6_REDIRECT          137     // Redirect message
#define ICMP6_ROUTER_RENUMBERING 138    // Router renumbering
```

**ICMPv6 Header Structure**:
```c
struct icmp6_hdr {
    uint8_t  icmp6_type;            // ICMPv6 message type
    uint8_t  icmp6_code;            // ICMPv6 message code
    uint16_t icmp6_cksum;           // ICMPv6 checksum
    union {
        uint32_t icmp6_un_data32[1]; // Type-specific data
        uint16_t icmp6_un_data16[2];
        uint8_t  icmp6_un_data8[4];
    } icmp6_dataun;
};

#define icmp6_data32    icmp6_dataun.icmp6_un_data32
#define icmp6_data16    icmp6_dataun.icmp6_un_data16
#define icmp6_data8     icmp6_dataun.icmp6_un_data8
#define icmp6_pptr      icmp6_data32[0]  // Parameter problem pointer
#define icmp6_mtu       icmp6_data32[0]  // Packet too big MTU
#define icmp6_id        icmp6_data16[0]  // Echo request/reply ID
#define icmp6_seq       icmp6_data16[1]  // Echo request/reply sequence
```

**ICMPv6 Destination Unreachable Codes**:
```c
#define ICMP6_DST_UNREACH_NOROUTE   0   // No route to destination
#define ICMP6_DST_UNREACH_ADMIN     1   // Communication administratively prohibited
#define ICMP6_DST_UNREACH_NOTNEIGHBOR 2 // Beyond scope of source address
#define ICMP6_DST_UNREACH_ADDR      3   // Address unreachable
#define ICMP6_DST_UNREACH_NOPORT    4   // Port unreachable
```

### Neighbor Discovery Protocol (NDP)

#### `nd6.h` - Neighbor Discovery for IPv6
**Purpose**: Defines neighbor discovery protocol for IPv6 address resolution and router discovery

**NDP Message Types** (same as ICMPv6 types):
```c
// NDP message types
#define ND_ROUTER_SOLICIT       ICMP6_ROUTER_SOLICITATION
#define ND_ROUTER_ADVERT        ICMP6_ROUTER_ADVERTISEMENT
#define ND_NEIGHBOR_SOLICIT     ICMP6_NEIGHBOR_SOLICITATION
#define ND_NEIGHBOR_ADVERT      ICMP6_NEIGHBOR_ADVERTISEMENT
#define ND_REDIRECT             ICMP6_REDIRECT
```

**NDP Router Solicitation**:
```c
struct nd_router_solicit {
    struct icmp6_hdr nd_rs_hdr;     // ICMPv6 header
    // Options follow
};

#define nd_rs_type      nd_rs_hdr.icmp6_type
#define nd_rs_code      nd_rs_hdr.icmp6_code
#define nd_rs_cksum     nd_rs_hdr.icmp6_cksum
#define nd_rs_reserved  nd_rs_hdr.icmp6_data32[0]
```

**NDP Router Advertisement**:
```c
struct nd_router_advert {
    struct icmp6_hdr nd_ra_hdr;     // ICMPv6 header
    uint32_t nd_ra_reachable;       // Reachable time
    uint32_t nd_ra_retransmit;      // Retransmission timer
    // Options follow
};

#define nd_ra_type          nd_ra_hdr.icmp6_type
#define nd_ra_code          nd_ra_hdr.icmp6_code
#define nd_ra_cksum         nd_ra_hdr.icmp6_cksum
#define nd_ra_flags         nd_ra_hdr.icmp6_data8[0]
#define nd_ra_pref          nd_ra_hdr.icmp6_data8[1]
#define nd_ra_lifetime      nd_ra_hdr.icmp6_data16[1]
```

**NDP Neighbor Solicitation**:
```c
struct nd_neighbor_solicit {
    struct icmp6_hdr nd_ns_hdr;     // ICMPv6 header
    struct in6_addr nd_ns_target;   // Target address
    // Options follow
};

#define nd_ns_type      nd_ns_hdr.icmp6_type
#define nd_ns_code      nd_ns_hdr.icmp6_code
#define nd_ns_cksum     nd_ns_hdr.icmp6_cksum
#define nd_ns_reserved  nd_ns_hdr.icmp6_data32[0]
```

**NDP Neighbor Advertisement**:
```c
struct nd_neighbor_advert {
    struct icmp6_hdr nd_na_hdr;     // ICMPv6 header
    struct in6_addr nd_na_target;   // Target address
    // Options follow
};

#define nd_na_type      nd_na_hdr.icmp6_type
#define nd_na_code      nd_na_hdr.icmp6_code
#define nd_na_cksum     nd_na_hdr.icmp6_cksum
#define nd_na_flags     nd_na_hdr.icmp6_data8[0]
#define nd_na_reserved  nd_na_hdr.icmp6_data32[0]
```

**NDP Flags**:
```c
#define ND_RA_FLAG_MANAGED  0x80        // Managed address configuration
#define ND_RA_FLAG_OTHER    0x40        // Other configuration
#define ND_RA_FLAG_HOME_AGENT 0x20      // Mobile IPv6 home agent

#define ND_NA_FLAG_ROUTER   0x80        // Sender is a router
#define ND_NA_FLAG_SOLICITED 0x40       // Solicited advertisement
#define ND_NA_FLAG_OVERRIDE 0x20        // Override existing cache
```

### IPv6 Extension Headers

#### Extension Header Types
**Purpose**: Defines various IPv6 extension headers for options, routing, and fragmentation

**Extension Header Structure**:
```c
// Generic extension header
struct ip6_ext {
    uint8_t ip6e_nxt;               // Next header
    uint8_t ip6e_len;               // Header length (in 8-byte units)
};

// Hop-by-hop options header
struct ip6_hbh {
    uint8_t ip6h_nxt;               // Next header
    uint8_t ip6h_len;               // Header length (in 8-byte units)
    // Options follow
};

// Destination options header
struct ip6_dest {
    uint8_t ip6d_nxt;               // Next header
    uint8_t ip6d_len;               // Header length (in 8-byte units)
    // Options follow
};

// Routing header
struct ip6_rthdr {
    uint8_t ip6r_nxt;               // Next header
    uint8_t ip6r_len;               // Header length (in 8-byte units)
    uint8_t ip6r_type;              // Routing type
    uint8_t ip6r_segleft;           // Segments left
    // Type-specific data follows
};

// Fragment header
struct ip6_frag {
    uint8_t ip6f_nxt;               // Next header
    uint8_t ip6f_reserved;          // Reserved field
    uint16_t ip6f_offlg;            // Offset and flags
    uint32_t ip6f_ident;            // Identification
};
```

**Fragmentation Flags**:
```c
#define IP6F_OFF_MASK   0xfff8          // Offset mask (13 bits)
#define IP6F_RESERVED_MASK 0x0006       // Reserved bits
#define IP6F_MORE_FRAG  0x0001          // More fragments flag
```

### IPv6 Protocol Control Blocks

#### `in6_pcb.h` - IPv6 Protocol Control Block
**Purpose**: Defines PCB structure for managing IPv6 network connections

**IPv6 PCB Structure**:
```c
struct in6pcb {
    struct in6pcb *in6p_next;       // Next PCB in list
    struct in6pcb *in6p_prev;       // Previous PCB in list
    struct in6pcb *in6p_head;       // List head
    
    // Socket and connection information
    struct socket *in6p_socket;     // Back pointer to socket
    struct in6_addr in6p_faddr;     // Foreign IPv6 address
    struct in6_addr in6p_laddr;     // Local IPv6 address
    uint16_t in6p_fport;            // Foreign port
    uint16_t in6p_lport;            // Local port
    uint32_t in6p_flowinfo;         // IPv6 flow information
    uint32_t in6p_flags;            // IPv6 PCB flags
    
    // IPv6-specific information
    uint8_t in6p_hops;              // Default hop limit
    uint8_t in6p_vflag;             // IP version flags
    struct ip6_pktopts *in6p_options; // IPv6 packet options
    struct in6_addr *in6p_moptions; // IPv6 multicast options
    struct in6_addr *in6p_laddrsa;  // Local address storage
    struct in6_addr *in6p_faddrsa;  // Foreign address storage
};
```

### IPv6 Variables and Statistics

#### `ip6_var.h` - IPv6 Protocol Variables
**Purpose**: Global variables and statistics for IPv6 protocol implementation

**IPv6 Statistics Structure**:
```c
struct ip6stat {
    uint32_t ip6s_total;            // Total packets received
    uint32_t ip6s_tooshort;         // Too short packets
    uint32_t ip6s_toosmall;         // Too small packets
    uint32_t ip6s_fragments;        // Fragmented packets
    uint32_t ip6s_fragdropped;      // Dropped fragments
    uint32_t ip6s_fragtimeout;      // Fragment timeouts
    uint32_t ip6s_forward;          // Packets forwarded
    uint32_t ip6s_cantforward;      // Packets that couldn't be forwarded
    uint32_t ip6s_redirectsent;     // Redirect messages sent
    uint32_t ip6s_delivered;        // Packets delivered
    uint32_t ip6s_localout;         // Packets sent locally
    uint32_t ip6s_odropped;         // Output drops
    uint32_t ip6s_reassembled;      // Packets reassembled
    uint32_t ip6s_fragmented;       // Packets fragmented
    uint32_t ip6s_ofragments;       // Output fragments
    uint32_t ip6s_cantfrag;         // Packets that couldn't be fragmented
};
```

#### `icmp6_var.h` - ICMPv6 Variables
**Purpose**: ICMPv6 protocol statistics and configuration

#### `nd6_var.h` - Neighbor Discovery Variables
**Purpose**: NDP statistics and neighbor cache management

**Neighbor Cache Entry**:
```c
struct nd6_cache {
    struct in6_addr nc_addr;        // IPv6 address
    uint8_t nc_lladdr[8];           // Link-layer address
    uint8_t nc_flags;               // Cache flags
    uint8_t nc_state;               // Cache state
    uint16_t nc_pmtu;               // Path MTU
    uint32_t nc_expire;             // Expiration time
    struct ifnet *nc_ifp;           // Interface pointer
    struct nd6_cache *nc_next;      // Next cache entry
};

// Cache states
#define ND6_LLINFO_INCOMPLETE   0   // Address resolution in progress
#define ND6_LLINFO_REACHABLE    1   // Neighbor is reachable
#define ND6_LLINFO_STALE        2   // Neighbor is stale
#define ND6_LLINFO_DELAY        3   // Neighbor is in delay state
#define ND6_LLINFO_PROBE        4   // Neighbor is being probed
```

### IPv6 Multicast Support

#### `ip6_mroute.h` - IPv6 Multicast Routing
**Purpose**: Defines multicast routing support for IPv6

**IPv6 Multicast Route Entry**:
```c
struct mif6 {
    uint8_t mif6_flags;             // MIF flags
    uint8_t mif6_rate_limit;        // Rate limit
    uint16_t mif6_pmtu;             // Path MTU
    struct in6_addr mif6_lcl_addr;  // Local address
    struct ifnet *mif6_ifp;         // Interface pointer
    uint32_t mif6_pkt_in;           // Packets in
    uint32_t mif6_pkt_out;          // Packets out
    uint64_t mif6_bytes_in;         // Bytes in
    uint64_t mif6_bytes_out;        // Bytes out
};

struct mf6c {
    struct in6_addr mf6c_origin;    // Source address
    struct in6_addr mf6c_mcastgrp;  // Multicast group
    uint8_t mf6c_parent;            // Incoming interface
    uint8_t mf6c_ifset[MAXMIFS];    // Outgoing interfaces
    uint32_t mf6c_pkt_cnt;          // Packet count
    uint64_t mf6c_byte_cnt;         // Byte count
    uint32_t mf6c_wrong_if;         // Wrong interface count
    uint32_t mf6c_expire;           // Expiration time
};
```

### IPv6 Extension Headers

#### Extension Header Processing
**Purpose**: Defines processing logic for IPv6 extension headers

**Extension Header Processing Structure**:
```c
// Extension header processing function
typedef int (*ext6_process_t)(struct mbuf **, int *, int);

// Extension header registry
struct ip6_ext_hdr {
    uint8_t ext_type;               // Extension header type
    ext6_process_t ext_process;     // Processing function
    const char *ext_name;           // Extension header name
};

// Extension header types
#define EXT6_HOPBYHOP   IPPROTO_HOPOPTS     // Hop-by-hop options
#define EXT6_ROUTING    IPPROTO_ROUTING     // Routing header
#define EXT6_FRAGMENT   IPPROTO_FRAGMENT    // Fragment header
#define EXT6_AH         IPPROTO_AH          // Authentication header
#define EXT6_ESP        IPPROTO_ESP         // Encapsulating security payload
#define EXT6_DSTOPTS    IPPROTO_DSTOPTS     // Destination options
```

## MINIX Microkernel Integration

### IPv6 Network Server Architecture
MINIX implements IPv6 protocols in userspace servers:

```c
// IPv6 network server message interface
message m;
m.m_type = NET_INET6_SEND;
m.NET6_PACKET = packet_buffer;
m.NET6_LENGTH = packet_length;
m.NET6_NEXT_HDR = IPPROTO_TCP;
m.NET6_FLOWINFO = flow_information;
_sendrec(NET6_PROC_NR, &m);

// IPv6 control operations
m.m_type = PM_INET6_CONTROL;
m.PM_INET6_CMD = INET6_SET_ROUTE;
m.PM_INET6_PREFIX = prefix_entry;
m.PM_INET6_PREFIX_LEN = prefix_length;
_sendrec(PM_PROC_NR, &m);
```

### Neighbor Discovery Integration
```c
// NDP message processing
int nd6_input(struct mbuf *m, int off, int icmp6off) {
    struct icmp6_hdr *icmp6;
    struct in6_addr src, dst;
    
    // Extract ICMPv6 header
    icmp6 = (struct icmp6_hdr *)(mtod(m, caddr_t) + icmp6off);
    
    // Process based on type
    switch (icmp6->icmp6_type) {
    case ND_NEIGHBOR_SOLICIT:
        return nd6_ns_input(m, src, dst);
    case ND_NEIGHBOR_ADVERT:
        return nd6_na_input(m, src, dst);
    case ND_ROUTER_SOLICIT:
        return nd6_rs_input(m, src, dst);
    case ND_ROUTER_ADVERT:
        return nd6_ra_input(m, src, dst);
    default:
        return -1;
    }
}
```

### Extension Header Processing
```c
// IPv6 extension header chain processing
int ip6_process_ext_hdrs(struct mbuf *m, int *offp, int *nxtp) {
    struct ip6_ext *ext;
    int off = *offp;
    int nxt = *nxtp;
    int ext_len;
    
    while (1) {
        switch (nxt) {
        case IPPROTO_HOPOPTS:
        case IPPROTO_DSTOPTS:
            // Process options header
            ext = (struct ip6_ext *)(mtod(m, caddr_t) + off);
            ext_len = (ext->ip6e_len + 1) * 8;
            nxt = ext->ip6e_nxt;
            off += ext_len;
            break;
            
        case IPPROTO_ROUTING:
            // Process routing header
            return ip6_process_rthdr(m, off, nxt);
            
        case IPPROTO_FRAGMENT:
            // Process fragment header
            return ip6_process_frag(m, off, nxt);
            
        case IPPROTO_AH:
        case IPPROTO_ESP:
            // Process security headers
            return ip6_process_sec(m, off, nxt);
            
        default:
            // Upper layer protocol
            *offp = off;
            *nxtp = nxt;
            return 0;
        }
    }
}
```

## RISC-V 64-bit Architecture Support

### Memory Alignment Optimization
```c
// Aligned structures for RISC-V 64-bit
struct ip6_hdr {
    union {
        struct ip6_hdrctl {
            uint32_t ip6_un1_flow;  // 4 bytes (aligned)
            uint16_t ip6_un1_plen;  // 2 bytes (aligned)
            uint8_t  ip6_un1_nxt;   // 1 byte
            uint8_t  ip6_un1_hlim;  // 1 byte
        } ip6_un1;
        uint8_t ip6_un2_vfc;        // 1 byte
    } ip6_ctlun;
    struct in6_addr ip6_src;        // 16 bytes (aligned)
    struct in6_addr ip6_dst;        // 16 bytes (aligned)
} __attribute__((packed, aligned(4)));

struct in6pcb {
    struct in6pcb *in6p_next;       // 8-byte pointer on RV64
    struct in6pcb *in6p_prev;       // 8-byte pointer on RV64
    struct in6pcb *in6p_head;       // 8-byte pointer on RV64
    struct socket *in6p_socket;     // 8-byte pointer on RV64
    struct in6_addr in6p_faddr;     // 16 bytes (aligned)
    struct in6_addr in6p_laddr;     // 16 bytes (aligned)
    uint16_t in6p_fport;            // 2 bytes
    uint16_t in6p_lport;            // 2 bytes
    uint32_t in6p_flowinfo;         // 4 bytes (aligned)
    uint32_t in6p_flags;            // 4 bytes (aligned)
    uint8_t in6p_hops;              // 1 byte
    uint8_t in6p_vflag;             // 1 byte
    // Padding for alignment
    uint16_t _in6p_pad;
} __attribute__((aligned(8)));
```

### Performance Optimizations
- **128-bit Operations**: Efficient handling of IPv6 addresses
- **Memory Access**: Aligned protocol header access
- **Network Byte Order**: Optimized endianness conversion for 128-bit values
- **Cache Efficiency**: Structure layout optimized for cache lines

### Atomic Operations
```c
// Atomic IPv6 statistics updates
static __inline void ip6stat_inc(uint32_t *stat) {
    __atomic_fetch_add(stat, 1, __ATOMIC_RELAXED);
}

// Atomic neighbor cache updates
static __inline bool nd6_cache_update(struct nd6_cache *nc,
                                     struct in6_addr *addr) {
    return __atomic_compare_exchange_strong(
        &nc->nc_addr, addr, new_addr, 0, __ATOMIC_SEQ_CST);
}
```

## Security Framework

### IPsec Integration
```c
// IPsec security associations
struct ipsec_sa {
    uint32_t sa_spi;                // Security Parameter Index
    struct in6_addr sa_src;         // Source address
    struct in6_addr sa_dst;         // Destination address
    uint8_t sa_protocol;            // Protocol (ESP or AH)
    uint8_t sa_state;               // SA state
    uint32_t sa_lifetime;           // Lifetime in seconds
    uint64_t sa_bytes;              // Byte counter
    uint32_t sa_flags;              // SA flags
};

// IPsec policy
struct ipsec_policy {
    struct in6_addr policy_src;     // Source address range
    struct in6_addr policy_dst;     // Destination address range
    uint8_t policy_sport;           // Source port range
    uint8_t policy_dport;           // Destination port range
    uint8_t policy_proto;           // Protocol
    uint8_t policy_dir;             // Direction (in/out)
    uint32_t policy_action;         // Action (allow/discard/ipsec)
};
```

### Cryptographic Acceleration
```c
// Cryptographic operations for IPsec
#define IPSEC_CRYPTO_AES        1       // AES encryption
#define IPSEC_CRYPTO_3DES       2       // 3DES encryption
#define IPSEC_CRYPTO_SHA1       3       // SHA-1 authentication
#define IPSEC_CRYPTO_MD5        4       // MD5 authentication

// Hardware acceleration support
#ifdef RISCV_CRYPTO_EXT
#define IPSEC_HW_ACCEL          1       // Use crypto extensions
#endif
```

### Access Control
```c
// IPv6 firewall rules
struct ip6_fw_rule {
    struct in6_addr rule_src;       // Source address
    struct in6_addr rule_dst;       // Destination address
    uint16_t rule_sport;            // Source port
    uint16_t rule_dport;            // Destination port
    uint8_t rule_proto;             // Protocol
    uint8_t rule_action;            // Action (allow/deny)
    uint32_t rule_flags;            // Rule flags
};
```

## Development Guidelines

### IPv6 Application Development
```c
// IPv6 socket programming example
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip6.h>

int create_ipv6_socket(void) {
    int sock;
    struct sockaddr_in6 addr;
    
    // Create IPv6 socket
    sock = socket(AF_INET6, SOCK_STREAM, 0);
    if (sock < 0)
        return -1;
    
    // Configure IPv6 address
    memset(&addr, 0, sizeof(addr));
    addr.sin6_family = AF_INET6;
    addr.sin6_port = htons(8080);
    addr.sin6_flowinfo = 0;
    addr.sin6_scope_id = 0;
    inet_pton(AF_INET6, "2001:db8::1", &addr.sin6_addr);
    
    // Bind socket
    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(sock);
        return -1;
    }
    
    return sock;
}
```

### Protocol Implementation Pattern
```c
// IPv6 protocol processing
int ip6_input(struct mbuf *m) {
    struct ip6_hdr *ip6;
    int off, nxt;
    
    // Validate IPv6 header
    if (m->m_len < sizeof(struct ip6_hdr)) {
        m = m_pullup(m, sizeof(struct ip6_hdr));
        if (m == NULL)
            return -1;
    }
    
    ip6 = mtod(m, struct ip6_hdr *);
    
    // Validate version
    if ((ip6->ip6_vfc & IPV6_VERSION_MASK) != IPV6_VERSION)
        return -1;
    
    // Process extension headers
    off = sizeof(struct ip6_hdr);
    nxt = ip6->ip6_nxt;
    
    if (ip6_process_ext_hdrs(m, &off, &nxt) < 0)
        return -1;
    
    // Dispatch to upper layer
    return ip6_dispatch(m, off, nxt);
}
```

### Error Handling
```c
// Comprehensive IPv6 error handling
const char *icmp6_err_string(int type, int code) {
    switch (type) {
    case ICMP6_DST_UNREACH:
        switch (code) {
        case ICMP6_DST_UNREACH_NOROUTE:
            return "No route to destination";
        case ICMP6_DST_UNREACH_ADMIN:
            return "Administratively prohibited";
        case ICMP6_DST_UNREACH_NOTNEIGHBOR:
            return "Beyond scope of source address";
        case ICMP6_DST_UNREACH_ADDR:
            return "Address unreachable";
        case ICMP6_DST_UNREACH_NOPORT:
            return "Port unreachable";
        }
        break;
        
    case ICMP6_PACKET_TOO_BIG:
        return "Packet too big";
        
    case ICMP6_TIME_EXCEEDED:
        return "Time exceeded";
        
    case ICMP6_PARAM_PROB:
        return "Parameter problem";
        
    default:
        return "Unknown ICMPv6 error";
    }
    return "Unknown error";
}
```

## Testing and Validation

### Protocol Compliance Testing
- **RFC Validation**: Verify implementation against IPv6 RFCs
- **Interoperability**: Test with various IPv6 implementations
- **Conformance**: Validate protocol behavior under stress
- **Compatibility**: Ensure dual-stack operation with IPv4

### Performance Testing
- **Throughput**: Measure IPv6 packet processing rates
- **Latency**: Measure packet processing delays
- **Scalability**: Test with large numbers of connections
- **Extension Headers**: Test processing of complex header chains

### Security Testing
- **IPsec Validation**: Test security association establishment
- **Extension Header Attacks**: Test protection against malicious headers
- **Neighbor Discovery**: Validate NDP security features
- **Input Validation**: Verify bounds checking and validation

This comprehensive IPv6 protocol implementation provides the foundation for next-generation network communication in MINIX, maintaining full compatibility with Internet standards while optimizing for the RISC-V 64-bit microkernel architecture and providing advanced security features.