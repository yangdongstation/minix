# MINIX Internet Protocol (IPv4) Headers Architecture

## Directory Overview

The `/sys/netinet/` directory contains the complete IPv4 protocol stack implementation for MINIX, providing comprehensive network protocol definitions, data structures, and interface specifications. These headers define the core Internet protocols that enable network communication in the MINIX operating system.

## Protocol Stack Architecture

### Layer Integration
- **Link Layer**: Ethernet and ARP integration
- **Network Layer**: IPv4 routing and forwarding
- **Transport Layer**: TCP and UDP protocols
- **Application Layer**: Socket interface integration
- **Cross-Layer**: ICMP for error reporting and diagnostics

### Standards Compliance
- **RFC 791**: Internet Protocol (IP) specification
- **RFC 792**: Internet Control Message Protocol (ICMP)
- **RFC 793**: Transmission Control Protocol (TCP)
- **RFC 768**: User Datagram Protocol (UDP)
- **RFC 826**: Address Resolution Protocol (ARP)
- **RFC 1112**: Internet Group Management Protocol (IGMP)

## Core Protocol Headers

### Internet Protocol (IP)

#### `in.h` - Internet Address Definitions
**Purpose**: Defines IPv4 address structures, constants, and utility functions

**Core Address Structures**:
```c
// IPv4 address structure
struct in_addr {
    uint32_t s_addr;                // 32-bit network byte order address
};

// Socket address structure for IPv4
struct sockaddr_in {
    uint8_t         sin_len;        // Structure length
    sa_family_t     sin_family;     // Address family (AF_INET)
    uint16_t        sin_port;       // Port number (network byte order)
    struct in_addr  sin_addr;       // IPv4 address
    char            sin_zero[8];    // Padding to 16 bytes
};

// IPv4 multicast request structure
struct ip_mreq {
    struct in_addr imr_multiaddr;   // Multicast group address
    struct in_addr imr_interface;   // Local interface address
};
```

**Protocol Constants**:
```c
// IP protocol versions
#define IPVERSION       4               // IPv4 version number
#define IPV4_VERSION    0x40            // IP header version field
#define IPV4_HDRLEN     0x05            // IP header length (5 * 4 = 20 bytes)

// IP header length and options
#define IP_MAXPACKET    65535           // Maximum IP packet size
#define IP_MINPACKET    20              // Minimum IP header size
#define IP_MAXOPTS      40              // Maximum IP options length

// Special IP addresses
#define INADDR_ANY      0x00000000      // Any address (0.0.0.0)
#define INADDR_BROADCAST 0xffffffff     // Broadcast address (255.255.255.255)
#define INADDR_LOOPBACK 0x7f000001      // Loopback address (127.0.0.1)
#define INADDR_NONE     0xffffffff      // Invalid address
```

**RISC-V 64-bit Alignment**:
```c
// Optimized for RISC-V 64-bit alignment
struct in_addr {
    uint32_t s_addr __attribute__((aligned(4)));  // 4-byte aligned
};

struct sockaddr_in {
    uint8_t         sin_len;                      // 1 byte
    sa_family_t     sin_family;                   // 2 bytes
    uint16_t        sin_port;                     // 2 bytes (aligned)
    struct in_addr  sin_addr;                     // 4 bytes (aligned)
    char            sin_zero[8];                  // 8 bytes (aligned)
} __attribute__((aligned(8)));                   // 16-byte total
```

#### `ip.h` - IP Protocol Header
**Purpose**: Defines the IPv4 packet header structure and protocol constants

**IP Header Structure**:
```c
struct ip {
    uint8_t  ip_vhl;                // Version (4 bits) + Header length (4 bits)
    uint8_t  ip_tos;                // Type of service
    uint16_t ip_len;                // Total length
    uint16_t ip_id;                 // Identification
    uint16_t ip_off;                // Fragment offset + flags
    uint8_t  ip_ttl;                // Time to live
    uint8_t  ip_p;                  // Protocol
    uint16_t ip_sum;                // Header checksum
    struct   in_addr ip_src;        // Source address
    struct   in_addr ip_dst;        // Destination address
};
```

**IP Fragmentation Flags**:
```c
#define IP_RF           0x8000          // Reserved fragment flag
#define IP_DF           0x4000          // Don't fragment
#define IP_MF           0x2000          // More fragments
#define IP_OFFMASK      0x1fff          // Mask for fragment offset
```

**IP Protocol Numbers**:
```c
#define IPPROTO_IP      0               // Dummy protocol for IP
#define IPPROTO_ICMP    1               // Internet Control Message Protocol
#define IPPROTO_IGMP    2               // Internet Group Management Protocol
#define IPPROTO_TCP     6               // Transmission Control Protocol
#define IPPROTO_UDP     17              // User Datagram Protocol
#define IPPROTO_ROUTING 43              // Routing Header for IPv6
#define IPPROTO_FRAGMENT 44             // Fragment Header for IPv6
#define IPPROTO_ESP     50              // Encapsulating Security Payload
#define IPPROTO_AH      51              // Authentication Header
#define IPPROTO_ICMPV6  58              // Internet Control Message Protocol for IPv6
#define IPPROTO_NONE    59              // No Next Header for IPv6
#define IPPROTO_DSTOPTS 60              // Destination Options for IPv6
```

#### `ip_icmp.h` - Internet Control Message Protocol
**Purpose**: Defines ICMP protocol for network error reporting and diagnostics

**ICMP Message Types**:
```c
// ICMP error messages
#define ICMP_ECHOREPLY      0           // Echo reply
#define ICMP_UNREACH        3           // Destination unreachable
#define ICMP_SOURCEQUENCH   4           // Source quench
#define ICMP_REDIRECT       5           // Redirect (change route)
#define ICMP_ECHO           8           // Echo request
#define ICMP_TIMXCEED       11          // Time exceeded
#define ICMP_PARAMPROB      12          // Parameter problem
#define ICMP_TSTAMP         13          // Timestamp request
#define ICMP_TSTAMPREPLY    14          // Timestamp reply
#define ICMP_IREQ           15          // Information request
#define ICMP_IREQREPLY      16          // Information reply
#define ICMP_MASKREQ        17          // Address mask request
#define ICMP_MASKREPLY      18          // Address mask reply

// ICMP unreachable codes
#define ICMP_UNREACH_NET    0           // Network unreachable
#define ICMP_UNREACH_HOST   1           // Host unreachable
#define ICMP_UNREACH_PROTOCOL 2         // Protocol unreachable
#define ICMP_UNREACH_PORT   3           // Port unreachable
#define ICMP_UNREACH_NEEDFRAG 4         // Fragmentation needed and DF set
#define ICMP_UNREACH_SRCFAIL 5          // Source route failed
```

**ICMP Header Structure**:
```c
struct icmp {
    uint8_t  icmp_type;             // ICMP message type
    uint8_t  icmp_code;             // ICMP message code
    uint16_t icmp_cksum;            // ICMP checksum
    union {
        uint8_t  ih_pptr;           // Parameter problem pointer
        struct in_addr ih_gwaddr;   // Gateway address (redirect)
        struct ih_idseq {
            uint16_t icd_id;        // Echo request/reply ID
            uint16_t icd_seq;       // Echo request/reply sequence
        } ih_idseq;
        uint32_t ih_void;           // Unused field
    } icmp_hun;
#define icmp_pptr   icmp_hun.ih_pptr
#define icmp_gwaddr icmp_hun.ih_gwaddr
#define icmp_id     icmp_hun.ih_idseq.icd_id
#define icmp_seq    icmp_hun.ih_idseq.icd_seq
#define icmp_void   icmp_hun.ih_void
    union {
        struct id_ts {
            uint32_t its_otime;     // Originate timestamp
            uint32_t its_rtime;     // Receive timestamp
            uint32_t its_ttime;     // Transmit timestamp
        } id_ts;
        struct id_ip  {
            struct ip idi_ip;       // Original IP header
            // Options and first 8 bytes of protocol header
        } id_ip;
        uint32_t id_mask;           // Address mask
        char    id_data[1];         // Start of data
    } icmp_dun;
#define icmp_otime  icmp_dun.id_ts.its_otime
#define icmp_rtime  icmp_dun.id_ts.its_rtime
#define icmp_ttime  icmp_dun.id_ts.its_ttime
#define icmp_ip     icmp_dun.id_ip.idi_ip
#define icmp_mask   icmp_dun.id_mask
#define icmp_data   icmp_dun.id_data
};
```

### Transmission Control Protocol (TCP)

#### `tcp.h` - TCP Protocol Header
**Purpose**: Defines TCP protocol header and connection management structures

**TCP Header Structure**:
```c
struct tcphdr {
    uint16_t th_sport;              // Source port
    uint16_t th_dport;              // Destination port
    uint32_t th_seq;                // Sequence number
    uint32_t th_ack;                // Acknowledgment number
    uint8_t  th_off;                // Data offset (header length)
    uint8_t  th_flags;              // Control flags
    uint16_t th_win;                // Window size
    uint16_t th_sum;                // Checksum
    uint16_t th_urp;                // Urgent pointer
};
```

**TCP Control Flags**:
```c
#define TH_FIN  0x01                    // Finish connection
#define TH_SYN  0x02                    // Synchronize sequence numbers
#define TH_RST  0x04                    // Reset connection
#define TH_PUSH 0x08                    // Push data
#define TH_ACK  0x10                    // Acknowledgment
#define TH_URG  0x20                    // Urgent pointer
#define TH_ECE  0x40                    // ECN echo
#define TH_CWR  0x80                    // Congestion window reduced
```

**TCP Options**:
```c
#define TCPOPT_EOL      0               // End of options
#define TCPOPT_NOP      1               // No operation
#define TCPOPT_MAXSEG   2               // Maximum segment size
#define TCPOPT_WINDOW   3               // Window scaling
#define TCPOPT_SACK     4               // Selective acknowledgment
#define TCPOPT_TIMESTAMP 8              // Timestamp option
```

#### `tcp_fsm.h` - TCP Finite State Machine
**Purpose**: Defines TCP connection states and state transitions

**TCP States**:
```c
#define TCPS_CLOSED     0               // Closed
#define TCPS_LISTEN     1               // Listening for connection
#define TCPS_SYN_SENT   2               // Active, have sent SYN
#define TCPS_SYN_RECEIVED 3             // Have sent and received SYN
#define TCPS_ESTABLISHED 4              // Established
#define TCPS_CLOSE_WAIT 5               // Received FIN, waiting for close
#define TCPS_FIN_WAIT_1 6               // Have closed, sent FIN
#define TCPS_CLOSING    7               // Closed, exchanged FINs
#define TCPS_LAST_ACK   8               // Had FIN and close, awaiting FIN ACK
#define TCPS_FIN_WAIT_2 9               // Have closed, FIN is acknowledged
#define TCPS_TIME_WAIT  10              // In 2*MSL quiet wait after close
```

#### `tcp_seq.h` - TCP Sequence Number Management
**Purpose**: TCP sequence number arithmetic and comparison utilities

**Sequence Number Operations**:
```c
typedef uint32_t tcp_seq;           // TCP sequence number type

// Sequence number comparison macros
#define SEQ_LT(a,b)     ((int32_t)((a)-(b)) < 0)
#define SEQ_LEQ(a,b)    ((int32_t)((a)-(b)) <= 0)
#define SEQ_GT(a,b)     ((int32_t)((a)-(b)) > 0)
#define SEQ_GEQ(a,b)    ((int32_t)((a)-(b)) >= 0)
#define SEQ_MIN(a,b)    (SEQ_LT(a,b) ? (a) : (b))
#define SEQ_MAX(a,b)    (SEQ_GT(a,b) ? (a) : (b))
#define SEQ_DIFF(a,b)   ((a) - (b))
```

#### `tcp_timer.h` - TCP Timer Management
**Purpose**: TCP retransmission and timing parameters

**TCP Timer Constants**:
```c
#define TCPTV_MSL       (30*PR_SLOWHZ)          // Maximum segment lifetime
#define TCPTV_SRTTBASE  0                       // Base smoothed round-trip time
#define TCPTV_SRTTDFLT  (3*PR_SLOWHZ)           // Default SRTT
#define TCPTV_PERSMIN   (5*PR_SLOWHZ)           // Minimum persistence time
#define TCPTV_PERSMAX   (60*PR_SLOWHZ)          // Maximum persistence time
#define TCPTV_KEEP_INIT (75*PR_SLOWHZ)          // Keepalive interval
#define TCPTV_KEEP_IDLE (120*60*PR_SLOWHZ)     // Keepalive idle time
#define TCPTV_KEEPINTVL (75*PR_SLOWHZ)          // Keepalive probe interval
#define TCPTV_KEEPCNT   8                       // Keepalive probe count
#define TCPTV_MAXIDLE   (TCPTV_KEEP_IDLE + TCPTV_KEEPINTVL * TCPTV_KEEPCNT)
```

### User Datagram Protocol (UDP)

#### `udp.h` - UDP Protocol Header
**Purpose**: Defines UDP protocol header and datagram structures

**UDP Header Structure**:
```c
struct udphdr {
    uint16_t uh_sport;              // Source port
    uint16_t uh_dport;              // Destination port
    uint16_t uh_ulen;               // UDP length (header + data)
    uint16_t uh_sum;                // UDP checksum
};
```

**UDP Constants**:
```c
#define UDP_TTL         64              // Default UDP TTL
#define UDP_PORT_MAX    65535           // Maximum port number
#define UDP_PORT_MIN    1024            // Minimum ephemeral port
#define UDP_HDR_SIZE    8               // UDP header size
```

### Internet Group Management Protocol (IGMP)

#### `igmp.h` - IGMP Protocol Definitions
**Purpose**: Defines IGMP for multicast group management

**IGMP Message Types**:
```c
#define IGMP_MEMBERSHIP_QUERY       0x11    // Membership query
#define IGMP_V1_MEMBERSHIP_REPORT   0x12    // V1 membership report
#define IGMP_V2_MEMBERSHIP_REPORT   0x16    // V2 membership report
#define IGMP_V2_LEAVE_GROUP         0x17    // Leave group message
#define IGMP_V3_MEMBERSHIP_REPORT   0x22    // V3 membership report
```

**IGMP Header Structure**:
```c
struct igmp {
    uint8_t  igmp_type;             // IGMP message type
    uint8_t  igmp_code;             // Max response time (V2) / Reserved (V1)
    uint16_t igmp_cksum;            // Checksum
    struct in_addr igmp_group;      // Multicast group address
};
```

### Address Resolution Protocol (ARP)

#### `if_ether.h` - Ethernet and ARP Definitions
**Purpose**: Defines Ethernet frame format and ARP protocol

**Ethernet Header**:
```c
struct ether_header {
    uint8_t  ether_dhost[6];        // Destination MAC address
    uint8_t  ether_shost[6];        // Source MAC address
    uint16_t ether_type;            // Ethernet type field
};
```

**ARP Packet Format**:
```c
struct arphdr {
    uint16_t ar_hrd;                // Hardware address format
    uint16_t ar_pro;                // Protocol address format
    uint8_t  ar_hln;                // Hardware address length
    uint8_t  ar_pln;                // Protocol address length
    uint16_t ar_op;                 // ARP operation
};

// Ethernet ARP packet
struct ether_arp {
    struct arphdr ea_hdr;           // Fixed-size header
    uint8_t arp_sha[6];             // Sender hardware address
    uint8_t arp_spa[4];             // Sender protocol address
    uint8_t arp_tha[6];             // Target hardware address
    uint8_t arp_tpa[4];             // Target protocol address
};
```

**ARP Operations**:
```c
#define ARPOP_REQUEST   1               // ARP request
#define ARPOP_REPLY     2               // ARP reply
#define ARPOP_RREQUEST  3               // RARP request
#define ARPOP_RREPLY    4               // RARP reply
#define ARPOP_InREQUEST 8               // InARP request
#define ARPOP_InREPLY   9               // InARP reply
#define ARPOP_NAK       10              // ARP NAK
```

### Protocol Control Blocks

#### `in_pcb.h` - Internet Protocol Control Block
**Purpose**: Defines the PCB structure for managing network connections

**PCB Structure**:
```c
struct inpcb {
    struct inpcb *inp_next;         // Next PCB in list
    struct inpcb *inp_prev;         // Previous PCB in list
    struct inpcb *inp_head;         // List head
    
    // Socket and connection information
    struct socket *inp_socket;      // Back pointer to socket
    struct in_addr inp_faddr;       // Foreign address
    struct in_addr inp_laddr;       // Local address
    uint16_t inp_fport;             // Foreign port
    uint16_t inp_lport;             // Local port
    
    // Protocol-specific information
    uint32_t inp_flow;              // IPv6 flow information
    uint8_t inp_vflag;              // IP version flags
    uint8_t inp_ip_ttl;             // Time to live
    uint8_t inp_ip_p;               // Protocol number
    
    // Options and flags
    uint32_t inp_flags;             // PCB flags
    struct ip_options *inp_options; // IP options
    struct ip_moptions *inp_moptions; // Multicast options
};
```

#### `in_var.h` - Internet Protocol Variables
**Purpose**: Global variables and statistics for IP protocol implementation

### Multicast Support

#### `ip_mroute.h` - IP Multicast Routing
**Purpose**: Defines multicast routing support and IGMP integration

**Multicast Routing Structures**:
```c
// Multicast route entry
struct mfc {
    struct in_addr mfc_origin;      // Source address
    struct in_addr mfc_mcastgrp;    // Multicast group address
    uint8_t mfc_parent;             // Incoming interface index
    uint8_t mfc_ttls[MAXVIFS];      // Outgoing interface TTLs
    uint32_t mfc_pkt_cnt;           // Packet count
    uint32_t mfc_byte_cnt;          // Byte count
    uint32_t mfc_wrong_if;          // Wrong interface count
};
```

### Protocol Variables and Statistics

#### `icmp_var.h` - ICMP Variables
**Purpose**: ICMP protocol statistics and configuration variables

#### `igmp_var.h` - IGMP Variables
**Purpose**: IGMP protocol statistics and multicast group management

#### `tcp_var.h` - TCP Variables
**Purpose**: Comprehensive TCP protocol statistics and configuration

**TCP Statistics Structure**:
```c
struct tcpstat {
    uint32_t tcps_connattempt;      // Connection attempts
    uint32_t tcps_accepts;          // Connection accepts
    uint32_t tcps_connects;         // Connections established
    uint32_t tcps_drops;            // Connections dropped
    uint32_t tcps_conndrops;        // Embryonic connections dropped
    uint32_t tcps_closedrop;        // Connections closed
    uint32_t tcps_segstimed;        // Segments with RTT calculated
    uint32_t tcps_rttupdated;       // RTT calculations updated
    uint32_t tcps_delack;           // Delayed acknowledgments sent
    uint32_t tcps_timeoutdrop;      // Connections dropped due to timeout
    uint32_t tcps_rexmttimeo;       // Retransmission timeouts
    uint32_t tcps_persisttimeo;     // Persist timeouts
    uint32_t tcps_keeptimeo;        // Keepalive timeouts
    uint32_t tcps_keepprobe;        // Keepalive probes sent
    uint32_t tcps_keepdrops;        // Connections dropped in keepalive
};
```

#### `udp_var.h` - UDP Variables
**Purpose**: UDP protocol statistics and configuration

## MINIX Integration Architecture

### Microkernel Network Server
MINIX implements network protocols in userspace servers:

```c
// Network server message interface
message m;
m.m_type = NET_INET_SEND;
m.NET_PACKET = packet_buffer;
m.NET_LENGTH = packet_length;
m.NET_PROTOCOL = IPPROTO_TCP;
_sendrec(NET_PROC_NR, &m);

// Protocol control through PM
m.m_type = PM_INET_CONTROL;
m.PM_INET_CMD = INET_SET_ROUTE;
m.PM_INET_DATA = route_entry;
_sendrec(PM_PROC_NR, &m);
```

### Driver Framework Integration
Network drivers provide link-layer services:
```c
// Ethernet driver interface
struct ethernet_driver {
    int (*ed_send)(struct mbuf *m, struct sockaddr *dst);
    int (*ed_ioctl)(u_long cmd, caddr_t data);
    void (*ed_init)(void);
    void (*ed_stop)(void);
};
```

### Socket Interface
Integration with BSD socket API:
```c
// Socket creation with protocol selection
int socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);    // TCP socket
int socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);     // UDP socket
int socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);      // ICMP socket
```

## RISC-V 64-bit Architecture Support

### Memory Alignment Optimization
```c
// Aligned structures for RISC-V 64-bit
struct ip {
    uint8_t  ip_vhl;                // 1 byte
    uint8_t  ip_tos;                // 1 byte
    uint16_t ip_len;                // 2 bytes (aligned)
    uint16_t ip_id;                 // 2 bytes (aligned)
    uint16_t ip_off;                // 2 bytes (aligned)
    uint8_t  ip_ttl;                // 1 byte
    uint8_t  ip_p;                  // 1 byte
    uint16_t ip_sum;                // 2 bytes (aligned)
    struct   in_addr ip_src;        // 4 bytes (aligned)
    struct   in_addr ip_dst;        // 4 bytes (aligned)
} __attribute__((packed, aligned(4)));

struct tcphdr {
    uint16_t th_sport;              // 2 bytes (aligned)
    uint16_t th_dport;              // 2 bytes (aligned)
    uint32_t th_seq;                // 4 bytes (aligned)
    uint32_t th_ack;                // 4 bytes (aligned)
    uint8_t  th_off;                // 1 byte
    uint8_t  th_flags;              // 1 byte
    uint16_t th_win;                // 2 bytes (aligned)
    uint16_t th_sum;                // 2 bytes (aligned)
    uint16_t th_urp;                // 2 bytes (aligned)
} __attribute__((packed, aligned(4)));
```

### Performance Optimizations
- **64-bit Arithmetic**: Efficient checksum calculations
- **Memory Access**: Aligned protocol header access
- **Network Byte Order**: Optimized endianness conversion
- **Cache Efficiency**: Structure layout optimized for cache lines

### Atomic Operations
```c
// Atomic protocol statistics updates
static __inline void tcpstat_inc(uint32_t *stat) {
    __atomic_fetch_add(stat, 1, __ATOMIC_RELAXED);
}

// Atomic flag operations
static __inline bool tcp_set_flag(uint32_t *flags, uint32_t flag) {
    return __atomic_fetch_or(flags, flag, __ATOMIC_ACQ_REL) & flag;
}
```

## Security Framework

### Protocol Security Features
- **IP Options Validation**: Protection against malicious IP options
- **ICMP Rate Limiting**: Prevention of ICMP flood attacks
- **TCP SYN Cookies**: Protection against SYN flood attacks
- **UDP Validation**: Source validation for UDP packets
- **Checksum Verification**: Mandatory checksum validation

### Network Security Integration
```c
// Packet filtering hooks
#define IP_FW_PASS      0               // Allow packet
#define IP_FW_BLOCK     1               // Block packet
#define IP_FW_REJECT    2               // Reject packet with ICMP

// Security policy enforcement
int ip_check_security(struct ip *ip, struct mbuf *m, int dir);
int tcp_check_policy(struct tcpcb *tp, struct tcp_hdr *th, int dir);
```

### Access Control
```c
// Network access control lists
struct ip_acl_entry {
    struct in_addr acl_src;         // Source address
    struct in_addr acl_dst;         // Destination address
    uint16_t acl_sport;             // Source port
    uint16_t acl_dport;             // Destination port
    uint8_t acl_protocol;           // Protocol number
    uint8_t acl_action;             // Action (allow/deny)
};
```

## Development Guidelines

### Protocol Implementation Pattern
```c
// TCP connection management
struct tcpcb *tcp_newconn(struct socket *so) {
    struct tcpcb *tp;
    
    // Allocate TCP control block
    tp = tcp_cballoc();
    if (tp == NULL)
        return NULL;
    
    // Initialize connection state
    tp->t_state = TCPS_CLOSED;
    tp->t_timer[TCPT_KEEP] = TCPTV_KEEP_INIT;
    tp->t_flags = TF_ACKNOW;
    
    // Attach to socket
    so->so_pcb = (caddr_t)tp;
    tp->t_inpcb = sotoinpcb(so);
    
    return tp;
}
```

### Socket Integration
```c
// Socket to protocol binding
int tcp_attach(struct socket *so, int proto) {
    struct tcpcb *tp;
    int error;
    
    // Create TCP control block
    tp = tcp_newconn(so);
    if (tp == NULL)
        return ENOBUFS;
    
    // Initialize sequence numbers
    tp->iss = tcp_new_iss();
    tcp_sendseqinit(tp);
    
    // Set socket options
    soisconnecting(so);
    tcpstat.tcps_connattempt++;
    
    return 0;
}
```

### Error Handling
```c
// Comprehensive protocol error handling
void tcp_input_error(struct tcpcb *tp, int error) {
    switch (error) {
    case EMSGSIZE:
        // Message too large
        tcpstat.tcps_tooshort++;
        break;
    case EINVAL:
        // Invalid argument
        tcpstat.tcps_badformat++;
        break;
    case ENOBUFS:
        // No buffer space
        tcpstat.tcps_rcvmemdrop++;
        break;
    default:
        // General error
        tcpstat.tcps_badformat++;
        break;
    }
}
```

## Testing and Validation

### Protocol Compliance Testing
- **RFC Validation**: Verify protocol implementation against RFCs
- **Interoperability**: Test with various operating systems
- **Conformance**: Validate protocol behavior under stress
- **Compatibility**: Ensure BSD socket API compatibility

### Performance Testing
- **Throughput**: Measure maximum data transfer rates
- **Latency**: Measure packet processing delays
- **Scalability**: Test with large numbers of connections
- **Resource Usage**: Monitor memory and CPU utilization

### Security Testing
- **Protocol Validation**: Test malformed packet handling
- **Attack Resistance**: Simulate various network attacks
- **Resource Exhaustion**: Test protection against DoS attacks
- **Input Validation**: Verify bounds checking and validation

This comprehensive IPv4 protocol implementation provides the foundation for network communication in MINIX, maintaining full compatibility with Internet standards while optimizing for the RISC-V 64-bit microkernel architecture.